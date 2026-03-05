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

#include "base/stl_util.h"

namespace extensions {

namespace {

// A boolean preference that indicates whether the extension should be hidden
// in the extensions management UI. Default value is false.
constexpr const char kPrefNotDisplayInSettings[] = "not_display_in_settings";

// The set of optional permissions that the user has manually revoked (disabled)
// via the extension management WebUI. This state is used to persist the
// visibility of the permission in the UI even after it has been hard-removed
// from the active and granted sets.
constexpr const char kPrefRuntimeUserBlockedPermissions[] =
    "runtime_user_blocked_permissions";

}  // namespace

void ExtensionPrefs::SetNotDisplayInSettings(const ExtensionId& extension_id,
                                             bool not_display) {
  UpdateExtensionPref(extension_id, kPrefNotDisplayInSettings,
                      base::Value(not_display));
  prefs_->CommitPendingWrite();
}

bool ExtensionPrefs::IsNotDisplayInSettings(
    const ExtensionId& extension_id) const {
  return ReadPrefAsBooleanAndReturn(extension_id, kPrefNotDisplayInSettings);
}

std::unique_ptr<PermissionSet> ExtensionPrefs::GetRuntimeUserBlockedPermissions(
    const ExtensionId& extension_id) const {
  if (!crx_file::id_util::IdIsValid(extension_id)) {
    LOG(ERROR) << "Invalid extension_id: " << extension_id;
    return nullptr;
  }
  return ReadPrefAsPermissionSet(extension_id,
                                 kPrefRuntimeUserBlockedPermissions);
}

void ExtensionPrefs::AddRuntimeUserBlockedPermissions(
    const ExtensionId& extension_id,
    const PermissionSet& permissions) {
  if (!crx_file::id_util::IdIsValid(extension_id)) {
    LOG(ERROR) << "Invalid extension_id: " << extension_id;
    return;
  }
  AddToPrefPermissionSet(extension_id, permissions,
                         kPrefRuntimeUserBlockedPermissions);
  for (auto& observer : observer_list_) {
    observer.OnExtensionRuntimePermissionsChanged(extension_id);
  }
}

void ExtensionPrefs::RemoveRuntimeUserBlockedPermissions(
    const ExtensionId& extension_id,
    const PermissionSet& permissions) {
  if (!crx_file::id_util::IdIsValid(extension_id)) {
    LOG(ERROR) << "Invalid extension_id: " << extension_id;
    return;
  }
  RemoveFromPrefPermissionSet(extension_id, permissions,
                              kPrefRuntimeUserBlockedPermissions);
  for (auto& observer : observer_list_) {
    observer.OnExtensionRuntimePermissionsChanged(extension_id);
  }
}

}  // namespace extensions
