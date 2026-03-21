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
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/extensions/permissions/active_tab_permission_granter.h"
#include "chrome/browser/extensions/tab_helper.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/common/mojom/context_type.mojom.h"
#include "ohos_cef_ext/libcef/browser/extensions/tab_extensions_util.h"
#endif

namespace extensions {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
namespace {
 
void ExtensionActionInvokeActiveTab(
    content::BrowserContext* context,
    int tab_id,
    std::string extension_id) {
  ExtensionRegistry* registry = ExtensionRegistry::Get(context);
  if (!registry) {
    LOG(ERROR) << "ExtensionActionInvokeActiveTab registry is null";
    return;
  }
 
  const Extension* extension =
      registry->enabled_extensions().GetByID(extension_id);
  if (!extension) {
    LOG(ERROR) << "ExtensionActionInvokeActiveTab extension is null";
    return;
  }
 
  content::WebContents* out_contents = nullptr;
  if (ExtensionTabUtil::GetTabById(tab_id, context, true, &out_contents)) {
    auto tab_helper = TabHelper::FromWebContents(out_contents);
    if (tab_helper) {
      tab_helper->active_tab_permission_granter()->GrantIfRequested(extension);
    }
  }
}
 
}

void ExtensionActionDispatcher::DispatchExtensionActionClickedWithCustomArgs(
    content::BrowserContext* context,
    std::string extension_id,
    const NWebExtensionTab* custom_tab) {
  LOG(DEBUG) << "ExtensionActionAPI "
                "DispatchExtensionActionClickedWithCustomArgs called";
  events::HistogramValue histogram_value = events::UNKNOWN;
  const char* event_name = nullptr;

  ExtensionRegistry* registry = ExtensionRegistry::Get(browser_context_);
  const Extension* extension =
      registry ? registry->enabled_extensions().GetByID(extension_id) : nullptr;
  const ExtensionAction* extension_action =
      extension ? ExtensionActionManager::Get(browser_context_)
                      ->GetExtensionAction(*extension)
                : nullptr;
  if (extension_action) {
    switch (extension_action->action_type()) {
      case ActionInfo::Type::kAction:
        histogram_value = events::ACTION_ON_CLICKED;
        event_name = "action.onClicked";
        break;
      case ActionInfo::Type::kBrowser:
        histogram_value = events::BROWSER_ACTION_ON_CLICKED;
        event_name = "browserAction.onClicked";
        break;
      case ActionInfo::Type::kPage:
        histogram_value = events::PAGE_ACTION_ON_CLICKED;
        event_name = "pageAction.onClicked";
        break;
    }
  } else {
    LOG(ERROR) << "onClicked type get failed, seen as action.onClicked";
    histogram_value = events::ACTION_ON_CLICKED;
    event_name = "action.onClicked";
  }

  if (!custom_tab) {
    LOG(ERROR) << "custom_tab is null, cannot dispatch event";
    return;
  }
  base::Value::List args;
  GURL gurl(custom_tab->url.value_or(""));
  constexpr mojom::ContextType context_type =
      mojom::ContextType::kPrivilegedExtension;
  ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
      ExtensionTabUtil::GetScrubTabBehaviorExt(extension, context_type, gurl, custom_tab->id.value_or(-1));
 
  NWebExtensionTab tab_t = *custom_tab;
  args.Append(GetTabValue(tab_t, scrub_tab_behavior));

  if (custom_tab->id.has_value()) {
    ExtensionActionInvokeActiveTab(context, custom_tab->id.value(), extension_id);
  }

  DispatchEventToExtension(context,
                           extension_id, histogram_value,
                           event_name, std::move(args));
}

void ExtensionActionDispatcher::WebExtensionActionShowPopup(
    content::BrowserContext* context,
    int tab_id,
    std::string extension_id) {
  ExtensionActionInvokeActiveTab(context, tab_id, extension_id);
}

void ExtensionActionDispatcher::ClearAllValuesForTab(
    content::WebContents* web_contents) {
  DCHECK(web_contents);
  int tab_id = ExtensionTabUtil::GetTabId(web_contents);
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
