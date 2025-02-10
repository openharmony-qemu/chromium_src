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

#ifndef OHOS_NWEB_SRC_WEB_EXTENSION_WINDOW_ITEMS_H_
#define OHOS_NWEB_SRC_WEB_EXTENSION_WINDOW_ITEMS_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "web_extension_tab_items.h"
#include "web_extension_window_items_type.h"

enum WebExtensionWindowId {
  WINDOW_ID_CURRENT = -2,
  WINDOW_ID_NONE = -1,
};

enum class WebExtensionWindowState {
  NORMAL = 0,
  MINIMIZED,
  MAXIMIZED,
  FULLSCREEN,
  LOCKED_FULLSCREEN,
  UNKNOWN_STATE,
};

enum class WebExtensionWindowCreateType {
  NORMAL,
  POPUP,
  UNKNOWN_TYPE
};

struct WebExtensionWindowQueryOptions {
  static const int32_t NUM_PROPERTIES = 2;
  std::optional<bool> populate;
  std::optional<std::vector<std::string>> windowTypes;
};

struct WebExtensionWindow {
  int32_t nwebId;
  bool alwaysOnTop;
  bool focused;
  std::optional<int32_t> height;
  std::optional<int32_t> id;
  bool incognito;
  std::optional<int32_t> left;
  std::optional<std::string> sessionId;
  std::optional<std::string> state;
  std::vector<NWebExtensionTab> tabs;
  std::optional<int32_t> top;
  std::optional<std::string> type;
  std::optional<int32_t> width;
};

#endif  // OHOS_NWEB_SRC_WEB_EXTENSION_WINDOW_ITEMS_H_
