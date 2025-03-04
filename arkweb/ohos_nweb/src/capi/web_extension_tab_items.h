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

#ifndef OHOS_NWEB_EX_BROWSER_WEB_EXTENSION_TAB_ITEMS_H_
#define OHOS_NWEB_EX_BROWSER_WEB_EXTENSION_TAB_ITEMS_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "ohos_nweb/src/capi/web_extension_window_items_type.h"

enum class NWebExtensionTabStatus {
  UNLOADED,
  LOADING,
  COMPLETE,
  UNKOWN_STATUS,
};

struct NWebExtensionTabActiveInfo {
  int32_t tabId;
  int32_t windowId;
};

enum class NWebExtensionTabImageFormat {
  JPEG,
  PNG
};

struct NWebExtensionTabImageDetails {
  std::optional<NWebExtensionTabImageFormat> format;
  std::optional<int32_t> quality;
};

struct NWebExtensionTabCreateProperties {
  std::optional<int32_t> windowId;
};

struct NWebExtensionTabGroupOptions {
  std::optional<NWebExtensionTabCreateProperties> createProperties;
  std::optional<int32_t> groupId;
  std::vector<int32_t> tabIds;
};

struct NWebExtensionTabAttachInfo {
  int32_t newPosition;
  int32_t newWindowId;
};

struct NWebExtensionTabDetachInfo {
  int32_t oldPosition;
  int32_t oldWindowId;
};

struct NWebExtensionTabHighlightInfo {
  std::vector<int32_t> tabIds;
  std::optional<int32_t> windowId;
};

struct NWebExtensionTabMoveProperties {
  int32_t index;
  std::optional<int32_t> windowId;
};

struct NWebExtensionTabQueryInfo {
  std::optional<bool> active;
  std::optional<bool> audible;
  std::optional<bool> autoDiscardable;
  std::optional<bool> currentWindow;
  std::optional<int32_t> currentWindowId;
  std::optional<bool> discarded;
  std::optional<int32_t> groupId;
  std::optional<bool> highlighted;
  std::optional<int32_t> index;
  std::optional<bool> lastFocusedWindow;
  std::optional<bool> muted;
  std::optional<int32_t> windowId;
  std::optional<bool> pinned;
  std::optional<NWebExtensionTabStatus> status;
  std::optional<std::string> title;
  std::optional<std::vector<std::string>> url;
  std::optional<WebExtensionWindowType> windowType;
};

struct NWebExtensionTabReloadProperties {
  std::optional<bool> bypassCache;
};

struct NWebExtensionTabMoveInfo {
  int32_t fromIndex;
  int32_t toIndex;
  int32_t windowId;
};

enum class NWebExtensionTabZoomSettingsMode {
  AUTOMATIC,
  MANUAL,
  DISABLE
};

enum class NWebExtensionTabZoomSettingsScope {
  PER_ORIGIN,
  PER_TAB
};

struct NWebExtensionTabZoomSettings {
  std::optional<int32_t> defaultZoomFactor;
  std::optional<NWebExtensionTabZoomSettingsMode> mode;
  std::optional<NWebExtensionTabZoomSettingsScope> scope;
};

struct NWebExtensionTabZoomChangeInfo {
  int32_t newZoomFactor;
  int32_t oldZoomFactor;
  int32_t tabId;
  NWebExtensionTabZoomSettings zoomSettings;
};

enum class NWebExtensionTabMutedInfoReason {
  USER,
  CAPTURE,
  EXTENSION,
  UNKOWN_REASON,
};

struct NWebExtensionTabMutedInfo {
  std::optional<std::string> extensionId;
  bool muted;
  std::optional<std::string> reason;
};

struct NWebExtensionTabChangeInfo {
  std::optional<bool> audible;
  std::optional<bool> autoDiscardable;
  std::optional<bool> discarded;
  std::optional<std::string> favIconUrl;
  std::optional<int32_t> groupId;
  std::optional<NWebExtensionTabMutedInfo> mutedInfo;
  std::optional<bool> pinned;
  std::optional<NWebExtensionTabStatus> status;
  std::optional<std::string> title;
  std::optional<std::string> url;
};

struct NWebExtensionTabRemoveInfo {
  bool isWindowClosing;
  int32_t windowId;
};

struct NWebExtensionTabUpdateProperties {
  std::optional<bool> active;
  std::optional<bool> autoDiscardable;
  std::optional<bool> highlighted;
  std::optional<bool> muted;
  std::optional<int32_t> openerTabId;
  std::optional<bool> pinned;
  std::optional<std::string> url;
};

inline const char* NWebExtensionTabMutedInfoReasonToString(
    NWebExtensionTabMutedInfoReason enum_param) {
  switch (enum_param) {
    case NWebExtensionTabMutedInfoReason::USER:
      return "user";
    case NWebExtensionTabMutedInfoReason::CAPTURE:
      return "capture";
    case NWebExtensionTabMutedInfoReason::EXTENSION:
      return "extension";
    default:
      return "";
  }
  return "";
}

inline const char* NWebExtensionTabStatusToString(
    NWebExtensionTabStatus enum_param) {
  switch (enum_param) {
    case NWebExtensionTabStatus::UNLOADED:
      return "unloaded";
    case NWebExtensionTabStatus::LOADING:
      return "loading";
    case NWebExtensionTabStatus::COMPLETE:
      return "complete";
    default:
      return "";
  }
  return "";
}

struct NWebExtensionTab {
  int32_t nwebId;
  bool active;
  std::optional<bool> audible;
  bool autoDiscardable;
  bool discarded;
  std::optional<std::string> favIconUrl;
  int32_t groupId;
  std::optional<int32_t> height;
  bool highlighted;
  std::optional<int32_t> id;
  bool incognito;
  int32_t index;
  std::optional<int32_t> lastAccessed;
  std::optional<NWebExtensionTabMutedInfo> mutedInfo;
  std::optional<int32_t> openerTabId;
  std::optional<std::string> pendingUrl;
  bool pinned;
  std::optional<std::string> sessionId;
  std::optional<std::string> status;
  std::optional<std::string> title;
  std::optional<std::string> url;
  std::optional<int32_t> width;
  int32_t windowId;
};

#endif  // OHOS_NWEB_EX_BROWSER_WEB_EXTENSION_TAB_ITEMS_H_