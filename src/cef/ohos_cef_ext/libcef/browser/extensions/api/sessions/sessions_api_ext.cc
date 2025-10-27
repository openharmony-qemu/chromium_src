/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "chrome/browser/extensions/api/sessions/sessions_api.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/profiles/profile.h"
#include "libcef/browser/extensions/tab_extensions_util.h"
#include "libcef/browser/extensions/window_extensions_util.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_sessions_types.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_sessions_handler_delegate.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_utils.h"

namespace extensions {

namespace {

namespace GetRecentlyClosed = api::sessions::GetRecentlyClosed;
namespace Restore = api::sessions::Restore;

const char kRestoreInIncognitoError[] =
    "Can not restore sessions in incognito mode.";

ExtensionTabUtil::ScrubTabBehavior GetScrubTabBehavior(
    NWebExtensionTab& tab,
    const ExtensionFunction* function) {
  GURL gurl(tab.url.value_or(""));
  return ExtensionTabUtil::GetScrubTabBehaviorExt(
      function->extension(), function->source_context_type(), gurl,
      tab.id.value_or(-1));
}

base::Value::Dict NWebSessionToValue(NWebExtensionSession& session,
                                     const ExtensionFunction* function) {
  base::Value::Dict to_value_result;

  to_value_result.Set("lastModified", static_cast<int>(session.last_modified));
  if (session.tab) {
    to_value_result.Set(
        "tab", GetTabValue(session.tab.value(),
                           GetScrubTabBehavior(session.tab.value(), function)));
  }
  if (session.window) {
    std::vector<ExtensionTabUtil::ScrubTabBehavior> scrub_tab_behaviors;
    for (NWebExtensionTab tab : session.window->tabs) {
      scrub_tab_behaviors.emplace_back(GetScrubTabBehavior(tab, function));
    }
    to_value_result.Set(
        "window", GetWindowValue(session.window.value(), scrub_tab_behaviors));
  }

  return to_value_result;
}

base::Value::List NWebSessionsToValue(
    std::vector<NWebExtensionSession>& sessions,
    const ExtensionFunction* function) {
  base::Value::List to_value_result;

  to_value_result.reserve(sessions.size());
  for (auto& session : sessions) {
    to_value_result.Append(NWebSessionToValue(session, function));
  }

  return to_value_result;
}

}  // namespace

ExtensionFunction::ResponseAction SessionsGetRecentlyClosedFunction::Run() {
  std::optional<GetRecentlyClosed::Params> params =
      GetRecentlyClosed::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int max_results = api::sessions::MAX_SESSION_RESULTS;
  if (params->filter && params->filter->max_results) {
    max_results = *params->filter->max_results;
  }
  EXTENSION_FUNCTION_VALIDATE(
      max_results >= 0 && max_results <= api::sessions::MAX_SESSION_RESULTS);

  NWebExtensionSessionsGetRecentlyClosedParams get_params;
  if (params->filter) {
    get_params.filter.emplace();
    if (params->filter->max_results) {
      get_params.filter->max_results = *params->filter->max_results;
    }
  }
  get_params.context = OHOS::NWeb::GetExtensionFunctionContext(
      extension_id(), browser_context(), include_incognito_information());

  is_calling_ = true;
  OHOS::NWeb::NWebExtensionSessionsHandlerDelegate::GetRecentlyClosed(
      get_params, base::BindRepeating(&OnRecentlyClosedReceived,
                                      weak_ptr_factory_.GetWeakPtr()));
  is_calling_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  } else {
    AddRef();
    return RespondLater();
  }
}

// static
void SessionsGetRecentlyClosedFunction::OnRecentlyClosedReceived(
    const base::WeakPtr<SessionsGetRecentlyClosedFunction>& function,
    std::vector<NWebExtensionSession>& sessions,
    const std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "empty SessionsGetRecentlyClosedFunction called back";
    return;
  }

  if (error) {
    function->Respond(function->Error(error.value()));
  } else {
    if (sessions.empty()) {
      LOG(ERROR) << "GetRecentlyClosed called back with neither sessions nor "
                    "error msg";
      function->Respond(function->WithArguments(base::Value::List()));
    } else {
      function->Respond(function->WithArguments(
          NWebSessionsToValue(sessions, function.get())));
    }
  }

  if (!function->is_calling_) {
    LOG(INFO) << "SessionsGetRecentlyClosedFunction release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction SessionsRestoreFunction::Run() {
  std::optional<Restore::Params> params = Restore::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  Profile* profile = Profile::FromBrowserContext(browser_context());
  if (profile != profile->GetOriginalProfile()) {
    return RespondNow(Error(kRestoreInIncognitoError));
  }

  NWebExtensionSessionsRestoreParams restore_params;
  restore_params.session_id = params->session_id;
  restore_params.context = OHOS::NWeb::GetExtensionFunctionContext(
      extension_id(), browser_context(), include_incognito_information());

  is_calling_ = true;
  OHOS::NWeb::NWebExtensionSessionsHandlerDelegate::Restore(
      restore_params,
      base::BindRepeating(&OnSessionRestored, weak_ptr_factory_.GetWeakPtr()));
  is_calling_ = false;

  if (did_respond()) {
    return AlreadyResponded();
  } else {
    AddRef();
    return RespondLater();
  }
}

// static
void SessionsRestoreFunction::OnSessionRestored(
    const base::WeakPtr<SessionsRestoreFunction>& function,
    std::optional<NWebExtensionSession>& session,
    const std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "empty SessionsRestoreFunction called back";
    return;
  }

  if (error) {
    function->Respond(function->Error(error.value()));
  } else {
    if (!session.has_value()) {
      LOG(ERROR) << "Restore called back with neither session nor error msg";
      function->Respond(function->NoArguments());
    } else {
      function->Respond(function->WithArguments(
          NWebSessionToValue(*session, function.get())));
    }
  }

  if (!function->is_calling_) {
    LOG(INFO) << "SessionsRestoreFunction release";
    function->Release();
  }
}

SessionsEventRouter::SessionsEventRouter(Profile* profile)
    : profile_(profile) {}

}  // namespace extensions
