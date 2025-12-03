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

#include <codecvt>
#include <locale>

#include "base/logging.h"
#include "chrome/browser/extensions/api/permissions/permissions_api.h"
#include "chrome/browser/extensions/chrome_extension_function_details.h"
#include "chrome/browser/extensions/extension_install_prompt.h"
#include "chrome/browser/extensions/extension_install_prompt_show_params.h"
#include "chrome/browser/extensions/extension_management.h"
#include "chrome/browser/extensions/api/permissions/permissions_api_helpers.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/extensions/api/permissions.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/common/manifest_handlers/permissions_parser.h"
#include "extensions/common/permissions/permission_message_provider.h"
#include "extensions/common/permissions/permissions_data.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_prompt_cef_delegate.h"

namespace extensions {

const char kBlockedByEnterprisePolicy[] =
    "Permissions are blocked by enterprise policy.";

const char kNotInManifestPermissionsError[] =
    "Only permissions specified in the manifest may be requested.";

using permissions_api_helpers::UnpackPermissionSetResult;

ExtensionFunction::ResponseAction PermissionsRequestFunction::Run() {
  LOG(INFO) << "begin to request permissions";

  gfx::NativeWindow native_window =
      ChromeExtensionFunctionDetails(this).GetNativeWindowForUI();
  std::optional<api::permissions::Request::Params> params =
      api::permissions::Request::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  std::string error;
  std::unique_ptr<UnpackPermissionSetResult> unpack_result =
      permissions_api_helpers::UnpackPermissionSet(
          params->permissions,
          PermissionsParser::GetRequiredPermissions(extension()),
          PermissionsParser::GetOptionalPermissions(extension()),
          ExtensionPrefs::Get(browser_context())
              ->AllowFileAccess(extension_->id()),
          &error);

  if (!unpack_result) {
    LOG(INFO) << "failed to unpack permission";
    return RespondNow(Error(std::move(error)));
  }

  // Don't allow the extension to request any permissions that weren't specified
  // in the manifest.
  if (!unpack_result->unlisted_apis.empty() ||
      !unpack_result->unlisted_hosts.is_empty()) {
    LOG(INFO) << "permission is not in the manifest";
    return RespondNow(Error(kNotInManifestPermissionsError));
  }

  if (!unpack_result->restricted_file_scheme_patterns.is_empty()) {
    LOG(INFO) << "file scheme patterns is not support";
    return RespondNow(Error(
        "Extension must have file access enabled to request '*'.",
        unpack_result->restricted_file_scheme_patterns.begin()->GetAsString()));
  }

  const PermissionSet& active_permissions =
      extension()->permissions_data()->active_permissions();

  // Determine which of the requested permissions are optional permissions that
  // are "new", i.e. aren't already active on the extension.
  requested_optional_ = std::make_unique<const PermissionSet>(
      std::move(unpack_result->optional_apis), ManifestPermissionSet(),
      std::move(unpack_result->optional_explicit_hosts), URLPatternSet());
  requested_optional_ =
      PermissionSet::CreateDifference(*requested_optional_, active_permissions);

  // Determine which of the requested permissions are withheld host permissions.
  // Since hosts are not always exact matches, we cannot take a set difference.
  // Thus we only consider requested permissions that are not already active on
  // the extension.
  URLPatternSet explicit_hosts;
  for (const auto& host : unpack_result->required_explicit_hosts) {
    if (!active_permissions.explicit_hosts().ContainsPattern(host)) {
      explicit_hosts.AddPattern(host);
    }
  }

  URLPatternSet scriptable_hosts;
  for (const auto& host : unpack_result->required_scriptable_hosts) {
    if (!active_permissions.scriptable_hosts().ContainsPattern(host)) {
      scriptable_hosts.AddPattern(host);
    }
  }

  requested_withheld_ = std::make_unique<const PermissionSet>(
      APIPermissionSet(), ManifestPermissionSet(), std::move(explicit_hosts),
      std::move(scriptable_hosts));

  // Determine the total "new" permissions; this is the set of all permissions
  // that aren't currently active on the extension.
  std::unique_ptr<const PermissionSet> total_new_permissions =
      PermissionSet::CreateUnion(*requested_withheld_, *requested_optional_);

  // If all permissions are already active, nothing left to do.
  if (total_new_permissions->IsEmpty()) {
    LOG(INFO) << "no new permission needs to be applied";
    constexpr bool granted = true;
    return RespondNow(WithArguments(granted));
  }

  // Automatically declines api permissions requests, which are blocked by
  // enterprise policy.
  if (!ExtensionManagementFactory::GetForBrowserContext(browser_context())
           ->IsPermissionSetAllowed(extension(), *total_new_permissions)) {
    LOG(INFO) << "permissions are blocked by enterprise policy";
    return RespondNow(Error(kBlockedByEnterprisePolicy));
  }

  // At this point, all permissions in |requested_withheld_| should be within
  // the |withheld_permissions| section of the PermissionsData.
  DCHECK(extension()->permissions_data()->withheld_permissions().Contains(
      *requested_withheld_));

  // Prompt the user for any new permissions that aren't contained within the
  // already-granted permissions. We don't prompt for already-granted
  // permissions since these were either granted to an earlier extension version
  // or removed by the extension itself (using the permissions.remove() method).
  std::unique_ptr<const PermissionSet> granted_permissions =
      ExtensionPrefs::Get(browser_context())
          ->GetRuntimeGrantedPermissions(extension()->id());
  std::unique_ptr<const PermissionSet> already_granted_permissions =
      PermissionSet::CreateIntersection(*granted_permissions,
                                        *requested_optional_);
  total_new_permissions = PermissionSet::CreateDifference(
      *total_new_permissions, *already_granted_permissions);

  // We don't need to show the prompt if there are no new warnings, or if
  // we're skipping the confirmation UI. COMPONENT extensions are allowed to
  // silently increase their permission level.
  const PermissionMessageProvider* message_provider =
      PermissionMessageProvider::Get();
  // TODO(devlin): We should probably use the same logic we do for permissions
  // increases here, where we check if there are *new* warnings (e.g., so we
  // don't warn about the tabs permission if history is already granted).
  bool has_no_warnings =
      message_provider
          ->GetPermissionMessages(message_provider->GetAllPermissionIDs(
              *total_new_permissions, extension()->GetType()))
          .empty();
  if (has_no_warnings ||
      extension_->location() == mojom::ManifestLocation::kComponent) {
    OnInstallPromptDone(ExtensionInstallPrompt::DoneCallbackPayload(
        ExtensionInstallPrompt::Result::ACCEPTED));
    LOG(INFO) << "has no warnings and granted permissions";
    return did_respond() ? AlreadyResponded() : RespondLater();
  }

  LOG(INFO) << "begin to show dialog";
  install_ui_ = std::make_unique<ExtensionInstallPrompt>(
      Profile::FromBrowserContext(browser_context()), native_window);
  install_ui_->ShowDialog(
      base::BindOnce(&PermissionsRequestFunction::OnInstallPromptDone, this),
      extension(), nullptr,
      std::make_unique<ExtensionInstallPrompt::Prompt>(
          ExtensionInstallPrompt::PERMISSIONS_PROMPT),
      std::move(total_new_permissions),
      base::BindRepeating(&PermissionsRequestFunction::ShowPrompt,
                          weak_ptr_factory_.GetWeakPtr()));

  // ExtensionInstallPrompt::ShowDialog() can call the response synchronously.
  return did_respond() ? AlreadyResponded() : RespondLater();
}

void PermissionsRequestFunction::ShowPrompt(
    std::unique_ptr<ExtensionInstallPromptShowParams> show_params,
    ExtensionInstallPrompt::DoneCallback done_callback,
    std::unique_ptr<ExtensionInstallPrompt::Prompt> prompt) {
  prompt_ = std::move(prompt);

  auto showPromptFunc =
      base::BindRepeating(&PermissionsRequestFunction::OnShowPrompt,
                          weak_ptr_factory_.GetWeakPtr());
  auto getPromptDataFunc =
      base::BindRepeating(&PermissionsRequestFunction::OnGetPromptData,
                          weak_ptr_factory_.GetWeakPtr());
  if (OHOS::NWeb::NWebExtensionPromptCefDelegate::GetInstance()
          .ShowExtensionPrompt(PROMPT_PERMISSION, extension()->id(),
                               prompt_->icon(), showPromptFunc,
                               getPromptDataFunc)) {
    AddRef();
    LOG(INFO) << "succeed to notify to show permission prompt";
    return;
  }
}

void PermissionsRequestFunction::OnShowPrompt(int action,
                                              const std::string& error) {
  if (action == static_cast<int>(DialogAction::kAutoConfirm)) {
    OnInstallPromptDone(ExtensionInstallPrompt::DoneCallbackPayload(
        ExtensionInstallPrompt::Result::ACCEPTED));
  } else if (action == static_cast<int>(DialogAction::kAutoReject)) {
    OnInstallPromptDone(ExtensionInstallPrompt::DoneCallbackPayload(
        ExtensionInstallPrompt::Result::USER_CANCELED));
  } else {
    LOG(INFO) << "no support action " << action << ",error is " << error;
  }
}

void PermissionsRequestFunction::OnGetPromptData(
    NWebExtensionPromptData* data) {
  if (!data || !prompt_) {
    LOG(WARNING) << "data or prompt is nullptr";
    return;
  }

  data->free_memory_func =
      OHOS::NWeb::NWebExtensionPromptCefDelegate::FreePromptData;
  std::string title =
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
          .to_bytes(prompt_->GetDialogTitle());
  std::string abortButtonLabel =
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
          .to_bytes(prompt_->GetAbortButtonLabel());
  std::string acceptButtonLabel =
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
          .to_bytes(prompt_->GetAcceptButtonLabel());
  std::string permissionsHeading =
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
          .to_bytes(prompt_->GetPermissionsHeading());
  data->title = strdup(title.c_str());
  data->abortButtonLabel = strdup(abortButtonLabel.c_str());
  data->acceptButtonLabel = strdup(acceptButtonLabel.c_str());
  data->permissionsHeading = strdup(permissionsHeading.c_str());

  data->permissionCount = prompt_->GetPermissionCount();
  if (0 < data->permissionCount) {
    data->permissions = (NWebExtensionPermission*)calloc(
        data->permissionCount, sizeof(NWebExtensionPermission));
    extensions::InstallPromptPermissions install_permissions = prompt_->GetPermissions();
    for (uint32_t i = 0; i < data->permissionCount; i++) {
      std::string permission =
          std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
              .to_bytes(prompt_->GetPermission(i));
      std::string detail = "";
      if (i < install_permissions.details.size()) {
        detail = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
                     .to_bytes(install_permissions.details[i]);
      }
      data->permissions[i].detail = strdup(detail.c_str());
      data->permissions[i].permission = strdup(permission.c_str());
    }
  }
}

}  // namespace extensions
