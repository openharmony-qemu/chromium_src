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

#include <set>
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

// This is an exported struct. Do not modify it or its nested structs.
struct WebExtensionActionInfo {
  std::string extensionId;
  std::optional<bool> isEnabled;
  std::optional<std::array<int32_t, EXT_COLOR_MAX>> badgeBackgroundColor;
  std::optional<std::string> badgeText;
  std::optional<std::array<int32_t, EXT_COLOR_MAX>> badgeTextColor;
  std::optional<std::string> popup;
  std::optional<std::string> title;
  std::optional<NWebExtensionActionIcon*> icon;
};

struct WebExtensionActionInfoV2 {
  std::string extensionId;
  std::optional<bool> isEnabled;
  std::optional<std::array<int32_t, EXT_COLOR_MAX>> badgeBackgroundColor;
  std::optional<std::string> badgeText;
  std::optional<std::array<int32_t, EXT_COLOR_MAX>> badgeTextColor;
  std::optional<std::string> popup;
  std::optional<std::string> title;
  std::optional<NWebExtensionActionIconV2> icon;
};

// This is an exported struct. Do not modify it or its nested structs.
struct WebExtensionSidePanelInfo {
  std::optional<bool> enable;
  std::optional<std::string> path;
  std::optional<bool> openPanelOnActionClick;
};

// This is an exported struct. Do not modify it or its nested structs.
struct WebExtensionInfo {
  std::string extensionId;
  bool mustRemainInstalled;
  WebExtensionActionInfo action;
  WebExtensionSidePanelInfo sidePanel;
  std::vector<NWebContextMenusItem> contextMenus;
  bool isOnToolbar = false;
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

struct WebExtensionManifestUrlOverride {
  std::optional<std::string> newtab;
  std::optional<std::string> bookmarks;
  std::optional<std::string> history;
};

struct WebExtensionManifestOmnibox {
  std::string keyword;
};

struct WebExtensionManifestOptionsPageInfo {
  std::string options_page;
  bool open_in_tab;
};

typedef enum {
  EXT_INCOGNITO_NONE = 0,
  EXT_INCOGNITO_SPLIT = 1,
  EXT_INCOGNITO_SPANNING = 2,
  EXT_INCOGNITO_NOT_ALLOWED = 3,
} ExtensionIncognitoMode;

struct WebExtensionManifestInfo {
  std::optional<std::string> name;
  std::optional<std::string> homepage_url;
  std::optional<WebExtensionManifestSettingsOverrides> settings_overrides;
  std::optional<WebExtensionManifestOptionsPageInfo> options_page;
  std::optional<ExtensionIncognitoMode> incognito_mode;
  std::optional<NWebExtensionActionIconV2> icons;
  std::optional<WebExtensionManifestUrlOverride> url_override;
  std::optional<WebExtensionManifestOmnibox> omnibox;
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
  bool is_incognito_enabled = false;
  std::vector<NWebContextMenusItemV2> contextMenusV2;
  WebExtensionActionInfoV2 action_v2;
  std::optional<double> install_time;
};

struct WebExtensionForbidDisplayParams {
  std::set<std::string> extension_ids;
};

struct NWebExtensionLoadError {
  static constexpr int NUM_PROPERTIES = 3;
  std::optional<std::string> contextType;
  std::string errorMsg;
  bool beNoisy;
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

typedef void (*OnExtensionInstallCallback)(int code,
                                           const char* message,
                                           const char* extension_id);

typedef void (*OnExtensionUninstallCallback)(bool success, const char* message);

#endif  // OHOS_NWEB_SRC_NWEB_EXTENSION_MANAGER_CALLBACK_H_
