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

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "libcef/browser/chrome/extensions/arkweb_chrome_extension_util_ext.h"
#include "ohos_cef_ext/libcef/browser/extensions/tab_extensions_util.h"
#endif

namespace extensions {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void ExtensionActionDispatcher::DispatchExtensionActionClickedWithCustomArgs(
    content::BrowserContext* context,
    std::string extension_id,
    const NWebExtensionTab* custom_tab) {
  LOG(DEBUG) << "ExtensionActionAPI "
                "DispatchExtensionActionClickedWithCustomArgs called";
  events::HistogramValue histogram_value = events::ACTION_ON_CLICKED;
  const char* event_name = "action.onClicked";
  base::Value::List args;

  args.Append(GetTabValue(*custom_tab));

  DispatchEventToExtension(context,
                           extension_id, histogram_value,
                           event_name, std::move(args));
}

void ExtensionActionDispatcher::ClearAllValuesForTab(
    content::WebContents* web_contents) {
  DCHECK(web_contents);
  int tab_id = cef::GetTabIdForWebContents(web_contents);
  if (tab_id < 0) {
    LOG(ERROR) << "invalid tab_id for ClearAllValuesForTab";
    return;
  }

  content::BrowserContext* browser_context = web_contents->GetBrowserContext();
  const ExtensionSet& enabled_extensions =
      ExtensionRegistry::Get(browser_context_)->enabled_extensions();
  ExtensionActionManager* action_manager =
      ExtensionActionManager::Get(browser_context_);

  for (const auto& extension : enabled_extensions) {
    ExtensionAction* extension_action =
        action_manager->GetExtensionAction(*extension);
    if (extension_action) {
      extension_action->ClearAllValuesForTab(tab_id);
      LOG(INFO) << "clearing all action values for extension "
                << extension_action->extension_id() << ", tab_id: " << tab_id;
      NotifyChange(extension_action, web_contents, browser_context);
    }
  }
}
#endif  // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

}  // namespace extensions