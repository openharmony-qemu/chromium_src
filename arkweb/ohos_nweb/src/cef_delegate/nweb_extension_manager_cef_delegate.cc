/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/cef_delegate/nweb_extension_manager_cef_delegate.h"

#include <optional>
#include <vector>

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/extensions/extension_util.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/pref_names.h"
#include "chrome/grit/generated_resources.h"
#include "content/public/browser/browser_context.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_system.h"
#include "extensions/browser/extension_util.h"
#include "extensions/browser/management_policy.h"
#include "extensions/browser/ui_util.h"
#include "extensions/common/extension.h"
#include "extensions/common/extension_set.h"
#include "extensions/common/manifest.h"
#include "extensions/common/mojom/manifest.mojom-shared.h"
#include "extensions/common/permissions/permission_message.h"
#include "extensions/common/permissions/permission_message_provider.h"
#include "extensions/common/permissions/permission_set.h"
#include "ohos_nweb/src/capi/nweb_extension_manager_callback.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_utils.h"
#include "ui/base/l10n/l10n_util.h"

namespace OHOS::NWeb {

namespace {

std::vector<NWebExtensionPermissionV2> GetNWebExtensionPermissionsHelper(
    const extensions::PermissionSet& permissions_to_display,
    extensions::Manifest::Type type) {
  std::vector<NWebExtensionPermissionV2> permissions;

  const extensions::PermissionMessageProvider* message_provider =
      extensions::PermissionMessageProvider::Get();
  const extensions::PermissionIDSet permission_ids =
      message_provider->GetAllPermissionIDs(permissions_to_display, type);
  const extensions::PermissionMessages& permission_messages =
      message_provider->GetPermissionMessages(permission_ids);

  for (const extensions::PermissionMessage& msg : permission_messages) {
    NWebExtensionPermissionV2 permission_info;
    permission_info.permission = base::UTF16ToUTF8(msg.message());
    if (!msg.submessages().empty()) {
      std::vector<std::u16string> detail_lines;
      for (const auto& detail_line : msg.submessages()) {
        detail_lines.push_back(u"- " + detail_line);
      }
      permission_info.detail =
          base::UTF16ToUTF8(base::JoinString(detail_lines, u"\n"));
    }

    permission_info.show_detail_label =
        l10n_util::GetStringUTF8(IDS_EXTENSIONS_SHOW_DETAILS);
    permission_info.hide_detail_label =
        l10n_util::GetStringUTF8(IDS_EXTENSIONS_HIDE_DETAILS);

    permissions.push_back(std::move(permission_info));
  }
  return permissions;
}

}  // namespace

// static
NWebExtensionManagerCefDelegate*
NWebExtensionManagerCefDelegate::GetInstance() {
  static NWebExtensionManagerCefDelegate instance;
  return &instance;
}

void NWebExtensionManagerCefDelegate::EnableWebExtension(
    const std::string& extension_id,
    OnEnableExtensionCallback callback) {
  LOG(INFO) << "EnableWebExtension " << extension_id;

  content::BrowserContext* browser_context = GetBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "EnableWebExtension failed: null browser context";
    callback(std::nullopt);
    return;
  }

  extensions::ExtensionService* extension_service =
      extensions::ExtensionSystem::Get(browser_context)->extension_service();
  if (!extension_service) {
    LOG(ERROR) << "EnableWebExtension failed: null extension service";
    callback(std::nullopt);
    return;
  }

  const extensions::Extension* extension =
      extensions::ExtensionRegistry::Get(browser_context)
          ->GetExtensionById(extension_id,
                             extensions::ExtensionRegistry::EVERYTHING);
  if (!extension) {
    LOG(ERROR) << "EnableWebExtension failed: invalid extension id";
    callback(std::nullopt);
    return;
  }

  extension_service->EnableExtension(extension_id);
  callback(extension_service->IsExtensionEnabled(extension_id));
}

void NWebExtensionManagerCefDelegate::SetIncognitoEnabled(
    const std::string& extension_id,
    bool enabled,
    OnSetIncognitoEnabledCallback callback) {
  LOG(INFO) << "SetIncognitoEnabled " << extension_id << ": " << enabled;

  content::BrowserContext* browser_context = GetBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "SetIncognitoEnabled failed: null browser context";
    callback(std::nullopt);
    return;
  }

  extensions::ExtensionService* extension_service =
      extensions::ExtensionSystem::Get(browser_context)->extension_service();
  if (!extension_service) {
    LOG(ERROR) << "SetIncognitoEnabled failed: null extension service";
    callback(std::nullopt);
    return;
  }

  const extensions::Extension* extension =
      extensions::ExtensionRegistry::Get(browser_context)
          ->GetExtensionById(extension_id,
                             extensions::ExtensionRegistry::EVERYTHING);
  if (!extension) {
    LOG(ERROR) << "SetIncognitoEnabled failed: invalid extension id";
    callback(std::nullopt);
    return;
  }

  if (!extensions::util::CanBeIncognitoEnabled(extension) && enabled) {
    LOG(INFO) << "Extension can not be incognito enabled";
    callback(std::nullopt);
    return;
  }

  extensions::util::SetIsIncognitoEnabled(extension_id, browser_context,
                                          enabled);
  bool enabled_after_set =
      extensions::util::IsIncognitoEnabled(extension_id, browser_context);
  callback(enabled_after_set);
}

void NWebExtensionManagerCefDelegate::GetInstalledExtensionInfos(
    GetInstalledExtensionInfosCallback callback) {
  LOG(INFO) << "GetInstalledExtensionInfos";

  content::BrowserContext* browser_context = GetBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "GetInstalledExtensionInfos failed: null browser context";
    callback({});
    return;
  }

  extensions::ExtensionService* extension_service =
      extensions::ExtensionSystem::Get(browser_context)->extension_service();
  if (!extension_service) {
    LOG(ERROR) << "GetInstalledExtensionInfos failed: null extension service";
    callback({});
    return;
  }

  const extensions::ExtensionRegistry* extension_registry =
      extensions::ExtensionRegistry::Get(browser_context);
  extensions::ExtensionPrefs* extension_prefs =
      extensions::ExtensionPrefs::Get(browser_context);

  std::vector<NWebInstalledExtensionInfo> result;
  const extensions::ExtensionSet extensions =
      extension_registry->GenerateInstalledExtensionsSet();
  for (const auto& extension : extensions) {
    if (!extensions::ui_util::ShouldDisplayInExtensionSettings(*extension)) {
      continue;
    }

    LOG(INFO) << "GetInstalledExtensionInfos id: " << extension->id();

    result.push_back(BuildExtensionInfoItem(
        extension.get(), browser_context, extension_registry, extension_prefs));
  }
  callback(result);
}

// static
NWebInstalledExtensionInfo
NWebExtensionManagerCefDelegate::BuildExtensionInfoItem(
    const extensions::Extension* extension,
    content::BrowserContext* browser_context,
    const extensions::ExtensionRegistry* extension_registry,
    extensions::ExtensionPrefs* extension_prefs) {
  NWebInstalledExtensionInfo item_info;
  item_info.id = extension->id();
  item_info.name = extension->name();
  item_info.description = extension->description();
  item_info.version = extension->version().GetString();
  bool is_enabled =
      extension_registry->enabled_extensions().Contains(extension->id());
  item_info.is_enabled = is_enabled;
  if (is_enabled) {
    item_info.state = NWebExtensionState::ENABLED;
  } else if (extension_registry->disabled_extensions().Contains(
                 extension->id())) {
    item_info.state = NWebExtensionState::DISABLED;
  } else if (extension_registry->terminated_extensions().Contains(
                 extension->id())) {
    item_info.state = NWebExtensionState::TERMINATED;
  } else if (extension_registry->blocklisted_extensions().Contains(
                 extension->id())) {
    item_info.state = NWebExtensionState::BLOCKLISTED;
  } else {
    LOG(WARNING) << "Extension not in any known registry state";
    item_info.state = NWebExtensionState::DISABLED;
  }

  std::unique_ptr<const extensions::PermissionSet> permissions_to_display =
      extensions::util::GetInstallPromptPermissionSetForExtension(
          extension, Profile::FromBrowserContext(browser_context));
  if (permissions_to_display) {
    item_info.permissions = GetNWebExtensionPermissionsHelper(
        *permissions_to_display, extension->GetType());
  }

  item_info.is_incognito_enabled =
      extensions::util::IsIncognitoEnabled(extension->id(), browser_context);
  base::Time install_time =
      extension_prefs->GetFirstInstallTime(extension->id());
  item_info.install_time = install_time.InMillisecondsFSinceUnixEpoch();

  extensions::mojom::ManifestLocation location = extension->location();
  item_info.location = static_cast<int>(location);

  return item_info;
}

}  // namespace OHOS::NWeb
