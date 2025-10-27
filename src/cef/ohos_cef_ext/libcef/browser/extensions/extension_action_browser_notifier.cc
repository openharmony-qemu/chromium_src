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

#include "extension_action_browser_notifier.h"

#include "chrome/browser/extensions/extension_action_dispatcher.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_action.h"
#include "extensions/common/constants.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_action_cef_delegate.h"

namespace extensions {

// static
ExtensionActionBrowserNotifier* ExtensionActionBrowserNotifier::GetInstance() {
  static base::NoDestructor<ExtensionActionBrowserNotifier> instance;
  return instance.get();
}

void ExtensionActionBrowserNotifier::StartObservingActionDispatcher(
    content::BrowserContext* browser_context) {
  ExtensionActionDispatcher* action_dispatcher =
      ExtensionActionDispatcher::Get(browser_context);
  if (!action_dispatcher) {
    LOG(ERROR) << "Failed to get ExtensionActionDispatcher in initialization, "
                  "related extensions APIs may not work.";
    return;
  }
  extension_action_dispatcher_observation_.Observe(action_dispatcher);
}

void ExtensionActionBrowserNotifier::OnExtensionActionUpdated(
    ExtensionAction* extension_action,
    content::WebContents* web_contents,
    content::BrowserContext* browser_context) {
  if (!extension_action) {
    LOG(ERROR) << "OnExtensionActionUpdated called without extension_action";
    return;
  }

  std::string extension_id = extension_action->extension_id();
  int tab_id = -1;
  if (web_contents) {
    tab_id = ExtensionTabUtil::GetTabId(web_contents);
  }

  NWebExtensionActionSetBadgeTextDetails details;
  if (tab_id != -1) {
    details.tabId = tab_id;
  }

  bool show_default_text = !extension_action->HasBadgeText(tab_id) &&
                           extension_action->GetDNRActionCount(tab_id) <= 0;
  if (!show_default_text) {
    details.text = extension_action->GetDisplayBadgeText(tab_id);
  }

  LOG(INFO) << "notifying browser badge text updated, extension: "
            << extension_id << ", tab_id: " << details.tabId.value_or(-1)
            << ", text: " << details.text.value_or("NA");
  OHOS::NWeb::NWebExtensionActionCefDelegate::GetInstance()->OnSetBadgeText(
      extension_id, details,
      OHOS::NWeb::GetExtensionContextType(browser_context),
      OHOS::NWeb::GetIncludeIncognitoInformation(extension_id,
                                                 browser_context));
}

}  // namespace extensions
