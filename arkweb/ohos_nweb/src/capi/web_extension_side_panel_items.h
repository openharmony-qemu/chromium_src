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
 
#ifndef OHOS_NWEB_SRC_WEB_EXTENSION_SIDE_PANEL_ITEMS_H_
#define OHOS_NWEB_SRC_WEB_EXTENSION_SIDE_PANEL_ITEMS_H_
 
#include <optional>
#include <string>
 
struct ExtensionSidePanelOpenOptions {
  static const int NUM_PROPERTIES = 6;
  std::optional<std::string> extensionId;
  std::optional<int32_t> tabId;
  std::optional<int32_t> windowId;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
  std::optional<int32_t> requestId;
};
 
struct ExtensionSidePanelSetOptions {
  static const int NUM_PROPERTIES = 7;
  std::optional<std::string> extensionId;
  std::optional<bool> enabled;
  std::optional<int32_t> tabId;
  std::optional<std::string> path;
  std::optional<std::string> contextType;
  std::optional<bool> includeIncognitoInfo;
  std::optional<int32_t> requestId;
};
 
#endif  // OHOS_NWEB_SRC_WEB_EXTENSION_SIDE_PANEL_ITEMS_H_