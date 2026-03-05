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

namespace developer = api::developer_private;

namespace {

void AddOptionalPermissionsInfo(content::BrowserContext* browser_context,
                                const Extension& extension,
                                developer::Permissions* permissions) {
  const PermissionSet& optional_manifest_set =
      PermissionsParser::GetOptionalPermissions(&extension);
  if (optional_manifest_set.apis().empty()) {
    return;
  }

  ExtensionPrefs* extension_prefs = ExtensionPrefs::Get(browser_context);
  std::unique_ptr<const PermissionSet> runtime_granted =
      extension_prefs->GetRuntimeGrantedPermissions(extension.id());
  std::unique_ptr<const PermissionSet> runtime_blocked =
      extension_prefs->GetRuntimeUserBlockedPermissions(extension.id());

  const PermissionSet& active_set =
      extension.permissions_data()->active_permissions();
  const PermissionMessageProvider* message_provider =
      PermissionMessageProvider::Get();

  permissions->optional_simple_permissions.emplace();

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

    APIPermissionSet single_api_set;
    single_api_set.insert(api_perm->id());
    PermissionSet temp_set(std::move(single_api_set), ManifestPermissionSet(),
                           URLPatternSet(), URLPatternSet());

    PermissionMessages messages = message_provider->GetPermissionMessages(
        message_provider->GetAllPermissionIDs(temp_set, extension.GetType()));

    if (messages.empty()) {
      continue;
    }

    developer::OptionalPermission ui_perm;
    ui_perm.name = name;
    ui_perm.granted = is_active;

    const PermissionMessage& main_msg = messages.front();
    ui_perm.message = base::UTF16ToUTF8(main_msg.message());

    ui_perm.submessages.reserve(main_msg.submessages().size());
    for (const auto& submsg : main_msg.submessages()) {
      ui_perm.submessages.push_back(base::UTF16ToUTF8(submsg));
    }

    permissions->optional_simple_permissions->push_back(std::move(ui_perm));
  }
}

}  // namespace

}  // namespace extensions
