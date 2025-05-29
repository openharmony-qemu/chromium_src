/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef EXTENSIONS_BROWSER_EXTENSION_REGISTRY_INFO_MANAGER_H_
#define EXTENSIONS_BROWSER_EXTENSION_REGISTRY_INFO_MANAGER_H_

#include <string>

#include "extension_registry.h"
#include "chrome/browser/extensions/menu_manager.h"
#include "ohos_nweb/src/capi/nweb_extension_manager_callback.h"
#include "chrome/browser/extensions/api/side_panel/side_panel_service.h"
#include "extensions/browser/extension_action.h"
#include "extensions/browser/extension_action_manager.h"
#include "extensions/browser/extension_registry_observer.h"

namespace extensions {

class ExtensionRegistryInfoManager : public MenuManager::LoadObserver, public ExtensionRegistryObserver {
 public:
  static void RegisterWebExtensionManagerListener(
  std::shared_ptr<NWebExtensionManagerCallBack> web_extension_manager_listener);

  static void UnRegisterWebExtensionManagerListener();

  static void OnExtensionLoadedCallBack(const WebExtensionInfo& extension);

  static void OnExtensionUnLoadedCallBack(const std::string& eid);

  static void OnExtensionOpenUrlCallBack(const std::string& url);

  ExtensionRegistryInfoManager(content::BrowserContext* browser_context);

  ~ExtensionRegistryInfoManager() = default;

  void NotifyOnExtensionLoaded(const Extension& extension);

  WebExtensionActionInfo GetExtensionActionInfo(const Extension& extension,
                                                int32_t tabId) const;

  WebExtensionSidePanelInfo GetExtensionSidePanelInfo(
      const Extension& extension,
      std::optional<int32_t> tabId) const;

  std::vector<NWebContextMenusItem> GetAllExtensionContextMenus(const std::string& extensionId) const;

  void Loaded(const std::string& extension_id) override;

  void OnExtensionLoaded(
  content::BrowserContext* browser_context,
  const Extension* extension) override;

  void OnExtensionReady(content::BrowserContext* browser_context,
                                const Extension* extension) override;

  void OnExtensionUnloaded(content::BrowserContext* browser_context,
                                  const Extension* extension,
                                  UnloadedExtensionReason reason) override;

  void OnExtensionWillBeInstalled(
      content::BrowserContext* browser_context,
      const Extension* extension,
      bool is_update,
      const std::string& old_name) override;

  void OnExtensionInstalled(content::BrowserContext* browser_context,
                                    const Extension* extension,
                                    bool is_update) override;

  void OnExtensionUninstalled(content::BrowserContext* browser_context,
                                      const Extension* extension,
                                      UninstallReason reason) override;

  void OnExtensionUninstallationDenied(
      content::BrowserContext* browser_context,
      const Extension* extension) override;

  void OnShutdown(ExtensionRegistry* registry) override;
 
 private:
  content::BrowserContext* browser_context_;
};

}  // namespace extensions

#endif  // EXTENSIONS_BROWSER_EXTENSION_REGISTRY_INFO_MANAGER_H_
