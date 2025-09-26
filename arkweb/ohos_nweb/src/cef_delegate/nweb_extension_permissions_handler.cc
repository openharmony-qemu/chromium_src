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

#include "nweb_extension_permissions_handler.h"

#include "chrome/browser/extensions/extension_action_runner.h"
#include "chrome/browser/extensions/permissions/site_permissions_helper.h"
#include "extensions/browser/permissions_manager.h"
#include "extensions/common/permissions/permissions_data.h"
#include "libcef/browser/chrome/extensions/arkweb_chrome_extension_util_ext.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_utils.h"

using namespace extensions;

namespace OHOS::NWeb {

// static
int NWebExtensionPermissionsHandler::GetExtensionSiteInteraction(
    const std::string& extension_id,
    int tab_id) {
  content::BrowserContext* global_context = GetBrowserContext();

  const Extension* extension = FindExtensionById(global_context, extension_id);
  if (!extension) {
    LOG(ERROR) << "failed to get extension";
    return static_cast<int>(SitePermissionsHelper::SiteInteraction::kNone);
  }

  content::WebContents* web_contents = cef::GetWebContentByTabId(tab_id);
  if (!web_contents) {
    LOG(ERROR) << "get web_contents failed by tab_id: " << tab_id;
    return static_cast<int>(SitePermissionsHelper::SiteInteraction::kNone);
  }

  const GURL& url = web_contents->GetLastCommittedURL();
  PermissionsData::PageAccess page_access =
      extension->permissions_data()->GetPageAccess(url, tab_id,
                                                   /*error=*/nullptr);
  PermissionsData::PageAccess script_access =
      extension->permissions_data()->GetContentScriptAccess(url, tab_id,
                                                            /*error=*/nullptr);

  if (page_access == PermissionsData::PageAccess::kAllowed ||
      script_access == PermissionsData::PageAccess::kAllowed) {
    return static_cast<int>(SitePermissionsHelper::SiteInteraction::kGranted);
  }

  ExtensionActionRunner* action_runner =
      ExtensionActionRunner::GetForWebContents(web_contents);
  bool has_been_blocked = action_runner && action_runner->WantsToRun(extension);

  if (page_access == PermissionsData::PageAccess::kWithheld ||
      script_access == PermissionsData::PageAccess::kWithheld ||
      has_been_blocked) {
    return static_cast<int>(SitePermissionsHelper::SiteInteraction::kWithheld);
  }

  if (PermissionsManager::Get(global_context)
          ->HasActiveTabAndCanAccess(*extension, url)) {
    return static_cast<int>(SitePermissionsHelper::SiteInteraction::kActiveTab);
  }

  return static_cast<int>(SitePermissionsHelper::SiteInteraction::kNone);
}

}  // namespace OHOS::NWeb
