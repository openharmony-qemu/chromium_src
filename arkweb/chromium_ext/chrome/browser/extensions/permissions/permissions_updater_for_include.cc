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

namespace extensions {

void PermissionsUpdater::SetOptionalPermissionState(
    const Extension& extension,
    const PermissionSet& permissions,
    bool should_be_enabled,
    base::OnceClosure completion_callback) {
  if (should_be_enabled) {
    GrantOptionalPermissions(extension, permissions,
                             std::move(completion_callback));
  } else {
    ExtensionPrefs::Get(browser_context_)
        ->AddRuntimeUserBlockedPermissions(extension.id(), permissions);
    RevokeOptionalPermissions(extension, permissions, REMOVE_HARD,
                              std::move(completion_callback));
  }
}

}  // namespace extensions
