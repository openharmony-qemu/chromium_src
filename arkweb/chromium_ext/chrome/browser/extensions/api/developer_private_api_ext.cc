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
#include "base/datashare_uri_utils.h"
#include "extensions/browser/extension_registry_info_manager.h"
#include "extensions/common/manifest_handlers/options_page_info.h"
#include "extensions/common/permissions/api_permission.h"
#include "extensions/common/permissions/permission_set.h"
#include "extensions/common/permissions/permissions_info.h"
#endif // ARKWEB_ARKWEB_EXTENSIONS

#include "ohos_nweb/src/nweb_common.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_manager_dispatcher.h"
#endif // ARKWEB_NWEB_EX

namespace extensions {

namespace developer = api::developer_private;

namespace {

void UpdateOptionalPermissions(
    content::BrowserContext* browser_context,
    const Extension& extension,
    const developer::OptionalPermissionUpdate& update) {
  if (!browser_context) {
    return;
  }

  const APIPermissionInfo* permission_info =
      PermissionsInfo::GetInstance()->GetByName(update.name);
  if (!permission_info) {
    LOG(ERROR) << "UpdateOptionalPermissions: Unknown optional permission '"
               << update.name << "' for extension " << extension.id();
    return;
  }

  LOG(INFO) << "UpdateOptionalPermissions: "
            << (update.enabled ? "Granted" : "Blocked") << " '" << update.name
            << "' for extension " << extension.id();

  APIPermissionSet apis;
  apis.insert(permission_info->id());
  PermissionSet permission_set(std::move(apis), ManifestPermissionSet(),
                               URLPatternSet(), URLPatternSet());

  PermissionsUpdater(browser_context)
      .SetOptionalPermissionState(extension, permission_set, update.enabled,
                                  base::DoNothing());
}

}  // namespace

namespace api {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
DeveloperPrivateOpenUrlFunction::
    DeveloperPrivateOpenUrlFunction() = default;

DeveloperPrivateOpenUrlFunction::
    ~DeveloperPrivateOpenUrlFunction() = default;

ExtensionFunction::ResponseAction
DeveloperPrivateOpenUrlFunction::Run() {
  std::optional<developer::OpenUrl::Params> params =
      api::developer_private::OpenUrl::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  if (IsNativeApiEnable()) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebExtensionManagerDispatcher::OnExtensionOpenUrlCallBack(params->url,
                                                              params->type);
#endif
  } else {
    ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack(params->url);
  }

  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction DeveloperPrivateShowOptionsFunction::Run() {
  std::optional<developer::ShowOptions::Params> params =
      developer::ShowOptions::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);
  const char kNoSuchExtensionError[] = "No such extension.";
  const char kNoOptionsPageForExtensionError[] =
      "Extension does not have an options page.";
  const char kCouldNotFindWebContentsError[] =
      "Could not find a valid web contents.";

  const Extension* extension = GetEnabledExtensionById(params->extension_id);
  if (!extension)
    return RespondNow(Error(kNoSuchExtensionError));

  if (OptionsPageInfo::GetOptionsPage(extension).is_empty())
    return RespondNow(Error(kNoOptionsPageForExtensionError));

  content::WebContents* web_contents = GetSenderWebContents();
  if (!web_contents)
    return RespondNow(Error(kCouldNotFindWebContentsError));

  GURL url_to_navigate = OptionsPageInfo::GetOptionsPage(extension);
  if (IsNativeApiEnable()) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebExtensionManagerDispatcher::OnExtensionOpenUrlCallBack(
        url_to_navigate.spec(), URL_TYPE_OPTIONS);
#endif
  } else {
    ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack(
        url_to_navigate.spec());
  }

  return RespondNow(NoArguments());
}
#endif // ARKWEB_ARKWEB_EXTENSIONS

}  // namespace api

}  // namespace extensions
