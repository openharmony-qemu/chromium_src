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

#ifndef OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_SESSIONS_TYPES_H_
#define OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_SESSIONS_TYPES_H_

#include <optional>

#include "nweb_extension_common_types.h"
#include "ohos_nweb/src/capi/web_extension_tab_items.h"
#include "ohos_nweb/src/capi/web_extension_window_items.h"

struct NWebExtensionSession {
  static constexpr int NUM_PROPERTIES = 3;
  int64_t last_modified;
  std::optional<NWebExtensionTab> tab;
  std::optional<WebExtensionWindow> window;
};

struct NWebExtensionSessionsFilter {
  static constexpr int NUM_PROPERTIES = 1;
  std::optional<int32_t> max_results;
};

struct NWebExtensionSessionsGetRecentlyClosedParams {
  static constexpr int NUM_PROPERTIES = 2;
  std::optional<NWebExtensionSessionsFilter> filter;
  std::optional<NWebExtensionFunctionContext> context;
};

struct NWebExtensionSessionsRestoreParams {
  static constexpr int NUM_PROPERTIES = 2;
  std::optional<std::string> session_id;
  std::optional<NWebExtensionFunctionContext> context;
};

struct NWebExtensionSessionsGetRecentlyClosedCallbackParams {
  static constexpr int NUM_PROPERTIES = 2;
  std::vector<NWebExtensionSession> sessions;
  std::optional<std::string> error;
};

struct NWebExtensionSessionsRestoreCallbackParams {
  static constexpr int NUM_PROPERTIES = 2;
  std::optional<NWebExtensionSession> session;
  std::optional<std::string> error;
};

#endif  // OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_SESSIONS_TYPES_H_
