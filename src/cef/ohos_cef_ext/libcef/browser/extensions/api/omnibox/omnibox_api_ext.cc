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

#include "chrome/browser/extensions/api/omnibox/omnibox_api.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/extensions/tab_helper.h"
#include "chrome/browser/omnibox/omnibox_input_watcher_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "components/omnibox/browser/omnibox_input_watcher.h"
#include "extensions/browser/event_router.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_omnibox_cef_delegate.h"

namespace extensions {

namespace omnibox = api::omnibox;
namespace SendSuggestions = omnibox::SendSuggestions;

namespace {

const char kCurrentTabDisposition[] = "currentTab";
const char kForegroundTabDisposition[] = "newForegroundTab";
const char kBackgroundTabDisposition[] = "newBackgroundTab";

}  // namespace

// static
bool ExtensionOmniboxEventRouter::OnInputChanged(
    const std::string& input,
    const ExtensionId& extension_id,
    content::BrowserContext* browser_context) {
  base::Value::List args;
  args.Append(input);
  auto event = std::make_unique<Event>(events::OMNIBOX_ON_INPUT_CHANGED,
                                       omnibox::OnInputChanged::kEventName,
                                       std::move(args), browser_context);
  EventRouter::Get(browser_context)
      ->DispatchEventToExtension(extension_id, std::move(event));
  return true;
}

// static
void ExtensionOmniboxEventRouter::OnInputEntered(
    int32_t tab_id,
    const std::string& input,
    const ExtensionId& extension_id,
    WindowOpenDisposition disposition,
    content::BrowserContext* browser_context) {
  Profile* profile = Profile::FromBrowserContext(browser_context);

  const Extension* extension =
      ExtensionRegistry::Get(profile)->enabled_extensions().GetByID(
          extension_id);
  CHECK(extension);

  content::WebContents* web_contents = nullptr;
  if (ExtensionTabUtil::GetTabById(tab_id, browser_context, true,
                                   &web_contents)) {
    extensions::TabHelper::FromWebContents(web_contents)
        ->active_tab_permission_granter()
        ->GrantIfRequested(extension);
  }

  base::Value::List args;
  args.Append(input);
  if (disposition == WindowOpenDisposition::NEW_FOREGROUND_TAB) {
    args.Append(kForegroundTabDisposition);
  } else if (disposition == WindowOpenDisposition::NEW_BACKGROUND_TAB) {
    args.Append(kBackgroundTabDisposition);
  } else {
    args.Append(kCurrentTabDisposition);
  }

  auto event = std::make_unique<Event>(events::OMNIBOX_ON_INPUT_ENTERED,
                                       omnibox::OnInputEntered::kEventName,
                                       std::move(args), profile);
  event->user_gesture = EventRouter::USER_GESTURE_ENABLED;
  EventRouter::Get(profile)->DispatchEventToExtension(extension_id,
                                                      std::move(event));

  OmniboxInputWatcherFactory::GetForBrowserContext(profile)
      ->NotifyInputEntered();
}

ExtensionFunction::ResponseAction OmniboxSendSuggestionsFunction::Run() {
  params_ = SendSuggestions::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params_);

  if (is_from_service_worker() && !params_->suggest_results.empty()) {
    std::vector<OHOS::NWeb::OmniboxSuggestResult> results;
    for (const auto& suggest_result : params_->suggest_results) {
      OHOS::NWeb::OmniboxSuggestResult result;
      result.content = suggest_result.content;
      result.deletable = suggest_result.deletable;
      result.description = suggest_result.description;
      results.push_back(result);
    }

    OHOS::NWeb::NWebExtensionOmniboxCefDelegate::GetInstance()
        .OnInputChangedCallback(results);
  }

  NotifySuggestionsReady();
  return RespondNow(NoArguments());
}

}  // namespace extensions
