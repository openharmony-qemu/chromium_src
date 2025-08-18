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

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_side_panel_dispatcher.h"
#endif
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "arkweb/ohos_nweb/src/nweb_common.h"
#include "base/logging.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_side_panel_cef_delegate.h"
#endif  // ARKWEB_ARKWEB_EXTENSIONS

namespace extensions {
typedef OHOS::NWeb::NWebExtensionSidePanelCefDelegate NWebExtensionSidePanelCefDelegate;

std::string GetExtensionContextType(content::BrowserContext* browser_context) {
  if (!browser_context)
    return std::string();
 
  if (browser_context->IsOffTheRecord()) {
    return "INCOGNITO";
  } else {
    return "REGULAR";
  }
}

void SidePanelOpenFunction::OnOpen(
      const base::WeakPtr<SidePanelOpenFunction>& function,
      const std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnOpen is empty!!!!";
    return;
  }
  if (error) {
    function->Respond(function->Error(*error));
  } else {
    function->Respond(function->NoArguments());
  }
 
  if (!function->call_on_open_) {
    LOG(INFO) << "SidePanelOpenFunction Release";
    function->Release();
  }
}

void SidePanelSetOptionsFunction::OnSetOptions(
      const base::WeakPtr<SidePanelSetOptionsFunction>& function,
      const std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnSetOptions is empty!!!!";
    return;
  }
  if (error) {
    function->Respond(function->Error(*error));
  } else {
    function->Respond(function->NoArguments());
  }
  if (!function->call_on_set_options_) {
    LOG(INFO) << "SidePanelSetOptionsFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction SidePanelSetOptionsFunction::RunFunctionForInclude(
    std::optional<api::side_panel::SetOptions::Params>& params) {
  LOG(INFO) << "SidePanelSetOptionsFunction::RunFunction";
  std::optional<std::string> absolute_path;
  if (params->options.path.has_value()) {
    absolute_path = extension()->GetResourceURL(*params->options.path).spec();
  }
 
  if (IsNativeApiEnable()) {
    if (NWebExtensionSidePanelCefDelegate::GetInstance()->HasOnSetOptionsByPbCallback()) {
      call_on_set_options_ = true;
      ExtensionSidePanelSetOptions options;
      options.extensionId = extension()->id();
      options.enabled = params->options.enabled;
      options.tabId = params->options.tab_id;
      options.path = absolute_path;
      std::string context_type = GetExtensionContextType(browser_context());
      if (!context_type.empty()) {
        options.contextType = context_type;
      }
      options.includeIncognitoInfo = include_incognito_information();
      bool success = NWebExtensionSidePanelCefDelegate::GetInstance()->OnSetOptionsByPb(
           options, base::BindRepeating(&SidePanelSetOptionsFunction::OnSetOptions,
                                        weak_ptr_factory_.GetWeakPtr()));
      call_on_set_options_ = false;
      if (did_respond()) {
        LOG(INFO) << "SidePanelSetOptionsFunction did_respond";
        return AlreadyResponded();
      }
 
      if (success) {
        AddRef();
        LOG(INFO) << "SidePanelSetOptionsFunction AddRef";
        return RespondLater();
      }
    } else {
#if BUILDFLAG(ARKWEB_NWEB_EX)
      NWebExtensionSidePanelDispatcher::OnSetOptionsNative(
          extension()->id(), params->options.enabled, params->options.tab_id,
          absolute_path);
#endif // ARKWEB_NWEB_EX
    }
  } else {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebExtensionSidePanelDispatcher::OnSetOptions(
        extension()->id(), params->options.enabled, params->options.tab_id,
        absolute_path);
#endif // ARKWEB_NWEB_EX
  }
  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction SidePanelOpenFunction::RunOpenFunctionForInclude(
    std::optional<api::side_panel::Open::Params>& params) {
  LOG(INFO) << "SidePanelOpenFunction::RunFunction";
  if (IsNativeApiEnable()) {
    if (NWebExtensionSidePanelCefDelegate::GetInstance()->HasOnOpenByPbCallback()) {
      call_on_open_ = true;
      ExtensionSidePanelOpenOptions options;
      options.extensionId = extension()->id();
      if (params->options.tab_id.has_value()) {
        options.tabId = params->options.tab_id.value();
      }
      if (params->options.window_id.has_value()) {
        options.windowId = params->options.window_id.value();
      }
      std::string context_type = GetExtensionContextType(browser_context());
      if (!context_type.empty()) {
        options.contextType = context_type;
      }
      options.includeIncognitoInfo = include_incognito_information();
      bool success = NWebExtensionSidePanelCefDelegate::GetInstance()->OnOpenByPb(
          options, base::BindRepeating(&SidePanelOpenFunction::OnOpen,
                                       weak_ptr_factory_.GetWeakPtr()));
      call_on_open_ = false;
      if (did_respond()) {
        LOG(INFO) << "SidePanelOpenFunction did_respond";
        return AlreadyResponded();
      }
 
      if (success) {
        AddRef();
        LOG(INFO) << "SidePanelOpenFunction AddRef";
        return RespondLater();
      }
    } else {
#if BUILDFLAG(ARKWEB_NWEB_EX)
      NWebExtensionSidePanelDispatcher::OnOpenNative(
          extension()->id(),
          params->options.tab_id.value_or(api::tabs::TAB_ID_NONE),
          params->options.window_id.value_or(api::windows::WINDOW_ID_NONE));
#endif // ARKWEB_NWEB_EX
    }
  } else {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebExtensionSidePanelDispatcher::OnOpen(
        extension()->id(),
        params->options.tab_id.value_or(api::tabs::TAB_ID_NONE),
        params->options.window_id.value_or(api::windows::WINDOW_ID_NONE));
#endif // ARKWEB_NWEB_EX
  }
  return RespondNow(NoArguments());
}

}  // namespace extensions
