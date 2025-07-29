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
    TabHelper::FromWebContents(out_contents)
        ->active_tab_permission_granter()
        ->GrantIfRequested(extension);
  }
}
 
}

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
#endif  // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

}  // namespace extensions