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

namespace extensions {

void InitForRegularProfileForInclude(
    Profile* profile,
    ExtensionRegistryInfoManager* extension_registry_info_manager) {
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  ExtensionRegistry* registry = ExtensionRegistry::Get(profile);
  if (!registry) {
    LOG(ERROR) << "registry is null";
  }
  registry->AddObserver(extension_registry_info_manager);

  MenuManager* menu_manager = MenuManager::Get(profile);
  if (!menu_manager) {
    LOG(ERROR) << "menu_manager is null";
  }
  menu_manager->AddLoadObserver(extension_registry_info_manager);
#endif
}

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
ExtensionRegistryInfoManager*
ExtensionSystemImpl::GetExtensionRegistryInfoManager() {
  return extension_registry_info_manager_.get();
}
#endif

}  // namespace extensions
