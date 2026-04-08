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
#include "chrome/browser/extensions/permissions/permissions_updater.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/pref_names.h"
#include "chrome/grit/generated_resources.h"
#include "content/public/browser/browser_context.h"
#include "extensions/browser/extension_icon_aggregator.h"
#include "extensions/browser/extension_icon_image.h"
#include "extensions/browser/extension_icon_placeholder.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_system.h"
#include "extensions/browser/extension_util.h"
#include "extensions/browser/management_policy.h"
#include "extensions/browser/ui_util.h"
#include "extensions/common/extension.h"
#include "extensions/common/extension_set.h"
#include "extensions/common/manifest.h"
#include "extensions/common/manifest_handlers/icons_handler.h"
#include "extensions/common/manifest_handlers/permissions_parser.h"
#include "extensions/common/mojom/manifest.mojom-shared.h"
#include "extensions/common/permissions/api_permission_set.h"
#include "extensions/common/permissions/permission_message.h"
#include "extensions/common/permissions/permission_message_provider.h"
#include "extensions/common/permissions/permission_set.h"
#include "extensions/common/permissions/permissions_data.h"
#include "extensions/common/permissions/permissions_info.h"
#include "ohos_nweb/src/capi/nweb_extension_action_icon.h"
#include "ohos_nweb/src/capi/nweb_extension_manager_callback.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_utils.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "ui/base/l10n/l10n_util.h"

namespace OHOS::NWeb {

namespace {

constexpr float kDefaultIconScale = 1.0f;
constexpr int kExtensionDetailIconWidth = extension_misc::EXTENSION_ICON_LARGE;

NWebExtensionState GetExtensionState(
    const extensions::ExtensionRegistry* extension_registry,
    const std::string& extension_id) {
  if (extension_registry->enabled_extensions().Contains(extension_id)) {
    return NWebExtensionState::ENABLED;
  } else if (extension_registry->disabled_extensions().Contains(extension_id)) {
    return NWebExtensionState::DISABLED;
  } else if (extension_registry->terminated_extensions().Contains(
                 extension_id)) {
    return NWebExtensionState::TERMINATED;
  } else if (extension_registry->blocklisted_extensions().Contains(
                 extension_id)) {
    return NWebExtensionState::BLOCKLISTED;
  } else {
    LOG(WARNING) << "Extension not in any known registry state";
    return NWebExtensionState::DISABLED;
  }
}

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

std::vector<NWebExtensionOptionalPermission>
GetNWebExtensionOptionalPermissionsHelper(
    content::BrowserContext* browser_context,
    const extensions::Extension* extension) {
  std::vector<NWebExtensionOptionalPermission> optional_permissions;

  const extensions::PermissionSet& optional_manifest_set =
      extensions::PermissionsParser::GetOptionalPermissions(extension);
  if (optional_manifest_set.apis().empty()) {
    return optional_permissions;
  }

  extensions::ExtensionPrefs* extension_prefs =
      extensions::ExtensionPrefs::Get(browser_context);
  std::unique_ptr<const extensions::PermissionSet> runtime_granted =
      extension_prefs->GetRuntimeGrantedPermissions(extension->id());
  std::unique_ptr<const extensions::PermissionSet> runtime_blocked =
      extension_prefs->GetRuntimeUserBlockedPermissions(extension->id());

  const extensions::PermissionSet& active_set =
      extension->permissions_data()->active_permissions();
  const extensions::PermissionMessageProvider* message_provider =
      extensions::PermissionMessageProvider::Get();

  for (const auto* api_perm : optional_manifest_set.apis()) {
    const std::string& name = api_perm->name();

    bool is_active = active_set.HasAPIPermission(name);
    bool is_granted =
        runtime_granted && runtime_granted->HasAPIPermission(name);
    bool is_blocked =
        runtime_blocked && runtime_blocked->HasAPIPermission(name);

    if (!((is_granted && is_active) || is_blocked)) {
      continue;
    }

    extensions::APIPermissionSet single_api_set;
    single_api_set.insert(api_perm->id());
    extensions::PermissionSet temp_set(
        std::move(single_api_set), extensions::ManifestPermissionSet(),
        extensions::URLPatternSet(), extensions::URLPatternSet());

    extensions::PermissionMessages messages =
        message_provider->GetPermissionMessages(
            message_provider->GetAllPermissionIDs(temp_set,
                                                  extension->GetType()));

    if (messages.empty()) {
      continue;
    }

    NWebExtensionOptionalPermission optional_permission;
    optional_permission.name = name;
    optional_permission.granted = is_active;

    const extensions::PermissionMessage& main_msg = messages.front();
    optional_permission.message = base::UTF16ToUTF8(main_msg.message());

    for (const auto& submsg : main_msg.submessages()) {
      optional_permission.submessages.push_back(base::UTF16ToUTF8(submsg));
    }

    optional_permissions.push_back(std::move(optional_permission));
  }

  return optional_permissions;
}

bool IsOptionalPermissionActive(content::BrowserContext* browser_context,
                                const std::string& extension_id,
                                const std::string& permission) {
  const extensions::Extension* extension =
      extensions::ExtensionRegistry::Get(browser_context)
          ->GetExtensionById(extension_id,
                             extensions::ExtensionRegistry::EVERYTHING);
  if (!extension) {
    return false;
  }

  const extensions::PermissionSet& active_set =
      extension->permissions_data()->active_permissions();
  return active_set.HasAPIPermission(permission);
}

}  // namespace

// static
NWebExtensionManagerCefDelegate*
NWebExtensionManagerCefDelegate::GetInstance() {
  static NWebExtensionManagerCefDelegate instance;
  return &instance;
}

// Structure to hold async icon loading state
struct NWebExtensionManagerCefDelegate::IconLoadState {
  std::vector<NWebInstalledExtensionInfo> infos;
  std::vector<std::unique_ptr<extensions::IconImage>> icon_images;
  std::unique_ptr<extensions::ExtensionIconAggregator> icon_aggregator;
  size_t pending_count = 0;
};

// Collect basic extension information without icons
std::vector<NWebInstalledExtensionInfo>
NWebExtensionManagerCefDelegate::GetInstalledExtensionsBasicInfo(
    content::BrowserContext* browser_context,
    const extensions::ExtensionRegistry* extension_registry,
    extensions::ExtensionPrefs* extension_prefs) {
  std::vector<NWebInstalledExtensionInfo> infos;
  const extensions::ExtensionSet extensions =
      extension_registry->GenerateInstalledExtensionsSet();

  infos.reserve(extensions.size());

  for (const auto& extension : extensions) {
    if (!extensions::ui_util::ShouldDisplayInExtensionSettings(*extension)) {
      continue;
    }

    LOG(INFO) << "GetInstalledExtensionInfos id: " << extension->id();

    infos.push_back(BuildExtensionInfoItem(
        extension.get(), browser_context, extension_registry, extension_prefs));
  }

  return infos;
}

// Setup icon loading for extensions
size_t NWebExtensionManagerCefDelegate::SetupExtensionIconLoading(
    content::BrowserContext* browser_context,
    const extensions::ExtensionRegistry* extension_registry,
    const std::vector<NWebInstalledExtensionInfo>& infos,
    std::shared_ptr<IconLoadState> state) {
  for (size_t i = 0; i < infos.size(); ++i) {
    const auto& info = infos[i];
    const extensions::Extension* extension =
        extension_registry->GetExtensionById(
            info.id, extensions::ExtensionRegistry::EVERYTHING);

    if (!extension) {
      continue;
    }

    const ExtensionIconSet& icon_set =
        extensions::IconsInfo::GetIcons(extension);
    if (icon_set.empty()) {
      continue;
    }

    // Create placeholder icon
    gfx::Image placeholder_icon =
        extensions::ExtensionIconPlaceholder::CreateImage(
            kExtensionDetailIconWidth, extension->name());
    placeholder_icon.AsImageSkia().GetRepresentation(kDefaultIconScale);

    // Create IconImage for manifest icon
    auto icon_image = std::make_unique<extensions::IconImage>(
        browser_context, extension, icon_set, kExtensionDetailIconWidth,
        placeholder_icon.AsImageSkia(), /*observer=*/nullptr);

    // Add to aggregator with callback to set icon in result
    state->icon_aggregator->AddIcon(
        icon_image.get(), base::BindOnce(
                              [](std::shared_ptr<IconLoadState> state,
                                 size_t index, NWebExtensionActionIconV2 icon) {
                                state->infos[index].icons = std::move(icon);
                              },
                              state, i));

    state->icon_images.push_back(std::move(icon_image));
    state->pending_count++;
  }

  return state->pending_count;
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
    LOG(ERROR) << "Extension can not be incognito enabled";
    callback(std::nullopt);
    return;
  }

  extensions::util::SetIsIncognitoEnabled(extension_id, browser_context,
                                          enabled);
  bool enabled_after_set =
      extensions::util::IsIncognitoEnabled(extension_id, browser_context);
  callback(enabled_after_set);
}

void NWebExtensionManagerCefDelegate::EnableRuntimePermission(
    const std::string& extension_id,
    const std::string& permission,
    bool enabled,
    OnEnableRuntimePermissionCallback callback) {
  LOG(INFO) << "enable permission: " << permission;
  content::BrowserContext* browser_context = GetBrowserContext();
  const extensions::Extension* extension =
      browser_context
          ? extensions::ExtensionRegistry::Get(browser_context)
                ->GetExtensionById(extension_id,
                                   extensions::ExtensionRegistry::EVERYTHING)
          : nullptr;
  if (!extension) {
    LOG(ERROR) << "EnableRuntimePermission failed: invalid extension id";
    callback(std::nullopt);
    return;
  }
  const extensions::APIPermissionInfo* permission_info =
      extensions::PermissionsInfo::GetInstance()->GetByName(permission);
  if (!permission_info) {
    LOG(ERROR) << "EnableRuntimePermission failed: invalid permission name";
    callback(std::nullopt);
    return;
  }

  const extensions::PermissionSet& optional_permissions =
      extensions::PermissionsParser::GetOptionalPermissions(extension);
  if (!optional_permissions.HasAPIPermission(permission)) {
    LOG(ERROR) << "EnableRuntimePermission failed: not an optional permission";
    callback(std::nullopt);
    return;
  }

  extensions::APIPermissionSet apis;
  apis.insert(permission_info->id());
  extensions::PermissionSet permission_set(
      std::move(apis), extensions::ManifestPermissionSet(),
      extensions::URLPatternSet(), extensions::URLPatternSet());

  extensions::PermissionsUpdater(browser_context)
      .SetOptionalPermissionState(
          *extension, permission_set, enabled,
          base::BindOnce(
              [](OnEnableRuntimePermissionCallback callback,
                 const std::string& extension_id, const std::string& permission,
                 content::BrowserContext* browser_context) {
                callback(IsOptionalPermissionActive(browser_context,
                                                    extension_id, permission));
              },
              std::move(callback), extension_id, permission, browser_context));
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

  auto state = std::make_shared<IconLoadState>();
  state->infos =
      NWebExtensionManagerCefDelegate::GetInstalledExtensionsBasicInfo(
          browser_context, extension_registry, extension_prefs);

  // If no extensions, return immediately
  if (state->infos.empty()) {
    callback({});
    return;
  }

  state->icon_aggregator =
      std::make_unique<extensions::ExtensionIconAggregator>(kDefaultIconScale);
  NWebExtensionManagerCefDelegate::SetupExtensionIconLoading(
      browser_context, extension_registry, state->infos, state);

  if (state->pending_count == 0) {
    callback(std::move(state->infos));
    return;
  }

  state->icon_aggregator->SetOnAllReadyCallback(base::BindOnce(
      [](GetInstalledExtensionInfosCallback callback,
         std::shared_ptr<IconLoadState> state) {
        callback(std::move(state->infos));
      },
      std::move(callback), state));

  state->icon_aggregator->StartLoading();
}

void NWebExtensionManagerCefDelegate::GetInstalledExtensionInfo(
    const std::string& extension_id,
    GetInstalledExtensionInfoCallback callback) {
  LOG(INFO) << "GetInstalledExtensionInfo id: " << extension_id;

  content::BrowserContext* browser_context = GetBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "GetInstalledExtensionInfo failed: null browser context";
    callback(std::nullopt);
    return;
  }

  extensions::ExtensionService* extension_service =
      extensions::ExtensionSystem::Get(browser_context)->extension_service();
  if (!extension_service) {
    LOG(ERROR) << "GetInstalledExtensionInfo failed: null extension service";
    callback(std::nullopt);
    return;
  }

  const extensions::ExtensionRegistry* extension_registry =
      extensions::ExtensionRegistry::Get(browser_context);
  extensions::ExtensionPrefs* extension_prefs =
      extensions::ExtensionPrefs::Get(browser_context);

  const extensions::Extension* extension =
      extension_registry->GetInstalledExtension(extension_id);
  if (!extension) {
    LOG(ERROR) << "extension not found: " << extension_id;
    callback(std::nullopt);
    return;
  }

  NWebInstalledExtensionInfo info =
      NWebExtensionManagerCefDelegate::BuildExtensionInfoItem(
          extension, browser_context, extension_registry, extension_prefs);

  auto state = std::make_shared<IconLoadState>();
  state->infos.push_back(info);
  state->icon_aggregator =
      std::make_unique<extensions::ExtensionIconAggregator>(kDefaultIconScale);
  NWebExtensionManagerCefDelegate::SetupExtensionIconLoading(
      browser_context, extension_registry, state->infos, state);

  if (state->pending_count == 0) {
    callback(std::move(info));
    return;
  }

  state->icon_aggregator->SetOnAllReadyCallback(base::BindOnce(
      [](GetInstalledExtensionInfoCallback callback,
         std::shared_ptr<IconLoadState> state) {
        if (!state->infos.empty()) {
          LOG(INFO) << "icon_aggregator OnAllReadyCallback";
          callback(std::move(state->infos[0]));
        } else {
          callback(std::nullopt);
        }
      },
      std::move(callback), state));

  state->icon_aggregator->StartLoading();
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
  item_info.state = GetExtensionState(extension_registry, extension->id());

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

  item_info.optional_permissions =
      GetNWebExtensionOptionalPermissionsHelper(browser_context, extension);

  return item_info;
}

}  // namespace OHOS::NWeb
