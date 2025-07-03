/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_NWEB_EXTENSION_MANAGER_CALLBACK_H_
#define OHOS_NWEB_SRC_NWEB_EXTENSION_MANAGER_CALLBACK_H_

#include <stddef.h>
#include <string>
#include <map>
#include "ohos_nweb/src/capi/nweb_extension_action_icon.h"
#include "ohos_nweb/src/capi/nweb_context_menus_item.h"

typedef enum {
  EXT_COLOR_RED = 0,
  EXT_COLOR_GREEN = 1,
  EXT_COLOR_BLUE = 2,
  EXT_COLOR_ALPHA = 3,
  EXT_COLOR_MAX = 4,
} ExtensionColorIndex;

struct WebExtensionActionInfo {
  std::string extensionId;
  std::optional<bool> isEnabled;
  std::optional<std::array<int32_t, EXT_COLOR_MAX>> badgeBackgroundColor;
  std::optional<std::string> badgeText;
  std::optional<std::array<int32_t, EXT_COLOR_MAX>> badgeTextColor;
  std::optional<std::string> popup;
  std::optional<std::string> title;
  std::optional<OHOS::NWeb::NWebExtensionActionIcon*> icon;
};

struct WebExtensionSidePanelInfo {
  std::optional<bool> enable;
  std::optional<std::string> path;
  std::optional<bool> openPanelOnActionClick;
};

struct WebExtensionInfo {
  /**
   * extensionId of extension.
   */
  std::string extensionId;

  /**
   * whether extension can be removed.
   */
  bool mustRemainInstalled;

  /**
   * actionInfo of extension.
   */
  WebExtensionActionInfo action;

  /**
   * sidePanel of extension.
   */
  WebExtensionSidePanelInfo sidePanel;

  /**
   * contextMenus of extension.
   */
  std::vector<NWebContextMenusItem> contextMenus;

  /**
   * extension's action icon is visible on toolbar.
   */
  bool isOnToolbar;
};

struct WebExtensionManifestSearchProvider {
  std::optional<std::string> name;
  std::optional<std::string> keyword;
  std::optional<std::string> favicon_url;
  std::string search_url;
  std::optional<std::string> encoding;
  std::optional<std::string> suggest_url;
  std::optional<std::string> image_url;
  std::optional<std::string> search_url_post_params;
  std::optional<std::string> suggest_url_post_params;
  std::optional<std::string> image_url_post_params;
  std::vector<std::string> alternate_urls;
  std::optional<int32_t> prepopulated_id;
  bool is_default;
};
 
struct WebExtensionManifestSettingsOverrides {
  std::optional<std::string> homepage;
  std::vector<std::string> startup_pages;
  std::optional<WebExtensionManifestSearchProvider> search_provider;
};
 
struct WebExtensionManifestOptionsPageInfo {
  std::string options_page;
  bool open_in_tab;
};
 
struct WebExtensionManifestInfo {
  std::optional<std::string> homepage_url;
  std::optional<WebExtensionManifestSettingsOverrides> settings_overrides;
  std::optional<WebExtensionManifestOptionsPageInfo> options_page;
};
 
struct WebExtensionInfoV2 {
  void FreeBuffers() {
    if (buffers_needs_free) {
      if (info.action.icon.has_value()) {
        for (auto& it: info.action.icon.value()->bitmaps) {
          free(it.second->bitmap);
          it.second->bitmap = nullptr;
        }
      }
      buffers_needs_free = false;
    }
  }
 
  bool buffers_needs_free;
  WebExtensionInfo info;
  WebExtensionManifestInfo manifest_info;
};

typedef void (*OnWebExtensionLoadedFun)(const WebExtensionInfo& load_info);
typedef void (*OnWebExtensionUnLoadedFun)(std::string extension_id);
typedef void (*OnWebExtensionOpenUrlFun)(std::string url);

struct NWebExtensionManagerCallBack {
  size_t struct_size = sizeof(NWebExtensionManagerCallBack);
  void (*OnWebExtensionLoaded)(const WebExtensionInfo& loadedinfo);
  void (*OnWebExtensionUnLoaded)(std::string extensionId);
  void (*OnWebExtensionOpenUrlFun)(std::string url);
};

#endif  // OHOS_NWEB_SRC_NWEB_EXTENSION_MANAGER_CALLBACK_H_
