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

#ifndef OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_TAB_GROUPS_TYPES_H_
#define OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_TAB_GROUPS_TYPES_H_

#include <optional>
#include <string>

enum class NWebExtensionTabGroupsColor {
  GREY,
  BLUE,
  RED,
  YELLOW,
  GREEN,
  PINK,
  PURPLE,
  CYAN,
  ORANGE
};

struct NWebExtensionTabGroupsMoveProperties {
  int32_t index;
  std::optional<int32_t> window_id;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionTabGroupsQueryInfo {
  std::optional<bool> collapsed;
  std::optional<NWebExtensionTabGroupsColor> color;
  std::optional<bool> shared;
  std::optional<std::string> title;
  std::optional<int32_t> window_id;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionTabGroupsUpdateProperties {
  std::optional<bool> collapsed;
  std::optional<NWebExtensionTabGroupsColor> color;
  std::optional<std::string> title;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
};

struct NWebExtensionTabGroup {
  bool collapsed;
  std::string color;
  int32_t id;
  std::optional<bool> shared;
  std::optional<std::string> title;
  int32_t window_id;
  std::optional<bool> incognito;
};

#endif  // OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_TAB_GROUPS_TYPES_H_