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
  EM_ZERO = 0,
  EM_ONE = 1,
  EM_TWO = 2,
  EM_THREE = 3,
  EM_MAX = 4,
} ExtensionManagerNum;

struct WebExtensionActionInfo {
  std::string extensionId;
  std::optional<bool> isEnabled;
  std::optional<std::array<int32_t, EM_MAX>> badgeBackgroundColor;
  std::optional<std::string> badgeText;
  std::optional<std::array<int32_t, EM_MAX>> badgeTextColor;
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
