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

#include "base/memory/raw_ref.h"
#include "base/scoped_multi_source_observation.h"
#include "chrome/browser/extensions/api/side_panel/side_panel_service.h"
#include "chrome/browser/extensions/menu_manager.h"
#include "extensions/browser/extension_action.h"
#include "extensions/browser/extension_action_manager.h"
#include "extensions/browser/extension_icon_image.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_registry_observer.h"
#include "ohos_nweb/src/capi/nweb_extension_manager_callback.h"

namespace extensions {

class ExtensionRegistryInfoManager : public MenuManager::LoadObserver,
                                     public ExtensionRegistryObserver,
                                     public IconImage::Observer {
 public:
  class BrowserNotifier {
   public:
    BrowserNotifier(const Extension& extension,
                    content::BrowserContext* browser_context,
                    ExtensionRegistryInfoManager* info_manager);
    ~BrowserNotifier();

    void HandleImageEvent(IconImage* icon_image);
    void NotifyIfReady();
    void PopulateActionIcon(const gfx::Image& image);
    void PopulateManifestIcon(const gfx::Image& image);

    IconImage* action_icon_image() { return action_icon_image_.get(); }
    IconImage* manifest_icon_image() { return manifest_icon_image_.get(); }
    gfx::Image placeholder_icon() { return placeholder_icon_; }

   private:
    void PopulateAllSyncInfo();
    void NotifyManagerExtensionLoaded();
    bool IsExtensionValid() const;

    raw_ref<const Extension> extension_;
    std::string extension_id_;
    raw_ptr<content::BrowserContext> browser_context_;
    raw_ptr<ExtensionRegistryInfoManager> info_manager_;
    gfx::Image placeholder_icon_;
    bool action_icon_is_ready_;
    bool manifest_icon_is_ready_;
    std::unique_ptr<IconImage> action_icon_image_;
    std::unique_ptr<IconImage> manifest_icon_image_;
    WebExtensionInfoV2 loaded_info_;
  };

  static void RegisterWebExtensionManagerListener(
      std::shared_ptr<NWebExtensionManagerCallBack>
          web_extension_manager_listener);

  static void UnRegisterWebExtensionManagerListener();

  static void OnExtensionLoadedCallBack(const WebExtensionInfo& extension);

  static void OnExtensionUnLoadedCallBack(const std::string& eid);

  static void OnExtensionOpenUrlCallBack(const std::string& url);

  static void StartInitialLoad();
 
  static void StopInitialLoad();

  ExtensionRegistryInfoManager(content::BrowserContext* browser_context);

  ~ExtensionRegistryInfoManager() = default;

  void GetExtensionManifestInfo(const Extension& extension,
                                WebExtensionManifestInfo& out_manifest) const;

#if BUILDFLAG(ARKWEB_NWEB_EX)
  ExtensionIncognitoMode GetExtensionIncognitoMode(const Extension* extension) const;
#endif

  WebExtensionActionInfo GetExtensionActionInfo(const Extension& extension,
                                                int32_t tabId) const;

  WebExtensionActionInfoV2 GetExtensionActionInfoV2(const Extension& extension,
                                                    int32_t tabId) const;

  WebExtensionSidePanelInfo GetExtensionSidePanelInfo(
      const Extension& extension,
      std::optional<int32_t> tabId) const;

  std::vector<NWebContextMenusItem> GetAllExtensionContextMenus(
      const std::string& extensionId) const;

  std::vector<NWebContextMenusItemV2> GetAllExtensionContextMenusV2(
      const std::string& extensionId) const;

  // MenuManager::LoadObserver implementation.
  void Loaded(const std::string& extension_id) override;

  // ExtensionRegistryObserver implementation.
  void OnExtensionUnloaded(content::BrowserContext* browser_context,
                           const Extension* extension,
                           UnloadedExtensionReason reason) override;
  void OnExtensionInstalled(content::BrowserContext* browser_context,
                            const Extension* extension,
                            bool is_update) override;
  void OnExtensionUninstalled(content::BrowserContext* browser_context,
                              const Extension* extension,
                              UninstallReason reason) override;
  void OnExtensionLoaded(content::BrowserContext* browser_context,
                         const Extension* extension) override;
  void OnExtensionReady(content::BrowserContext* browser_context,
                        const Extension* extension) override {}
  void OnExtensionWillBeInstalled(content::BrowserContext* browser_context,
                                  const Extension* extension,
                                  bool is_update,
                                  const std::string& old_name) override {}
  void OnExtensionUninstallationDenied(content::BrowserContext* browser_context,
                                       const Extension* extension) override {}
  void OnShutdown(ExtensionRegistry* registry) override {}

  // IconImage::Observer implementation.
  void OnExtensionIconImageChanged(IconImage* image) override;

  void AddIconImageObservation(IconImage* image,
                               std::shared_ptr<BrowserNotifier> notifier);

 private:
  void StartNotifyingExtensionLoaded(const Extension& extension);

  raw_ptr<content::BrowserContext> browser_context_;
  std::unordered_map<IconImage*, std::shared_ptr<BrowserNotifier>>
      pending_notifiers_;
  base::ScopedMultiSourceObservation<IconImage, IconImage::Observer>
      icon_observations_{this};
};

}  // namespace extensions

#endif  // EXTENSIONS_BROWSER_EXTENSION_REGISTRY_INFO_MANAGER_H_
