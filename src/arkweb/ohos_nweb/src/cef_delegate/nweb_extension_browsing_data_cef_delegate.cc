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

#include "nweb_extension_browsing_data_cef_delegate.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "cef/libcef/browser/browser_host_base.h"
#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_ext.h"
#include "content/public/common/content_switches.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "nweb_common.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_browsing_data_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {
static std::map<int32_t, BrowsingDataRemoveCallback> g_browsing_data_remove_downloads_callback_map_;
static std::map<int32_t, BrowsingDataRemoveCallback> g_browsing_data_remove_history_callback_map_;
} // namespace

void NWebExtensionBrowsingDataCefDelegate::RemoveDownloads(
    const NWebExtensionBrowsingDataRemovalOptions& removalOptions,
    BrowsingDataRemoveCallback callback,
    const std::optional<NWebExtensionBrowsingDataQueryOptions>& queryOptions) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  static int32_t request_id = 0;
  request_id++;
  g_browsing_data_remove_downloads_callback_map_[request_id] = std::move(callback);
  NWebExtensionBrowsingDataDispatcher::RemoveDownloads(request_id, removalOptions, queryOptions);
#endif
}

void NWebExtensionBrowsingDataCefDelegate::RemoveHistory(
    const NWebExtensionBrowsingDataRemovalOptions& removalOptions,
    BrowsingDataRemoveCallback callback,
    const std::optional<NWebExtensionBrowsingDataQueryOptions>& queryOptions) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  static int32_t request_id = 0;
  request_id++;
  g_browsing_data_remove_history_callback_map_[request_id] = std::move(callback);
  NWebExtensionBrowsingDataDispatcher::RemoveHistory(request_id, removalOptions, queryOptions);
#endif
}

void NWebExtensionBrowsingDataCefDelegate::RemoveDownloadsCallback(int32_t request_id,
    std::optional<std::string>& error) {
  if (g_browsing_data_remove_downloads_callback_map_.count(request_id)) {
    std::move(g_browsing_data_remove_downloads_callback_map_[request_id]).Run(error);
    g_browsing_data_remove_downloads_callback_map_.erase(request_id);
  }
}

void NWebExtensionBrowsingDataCefDelegate::RemoveHistoryCallback(int32_t request_id,
    std::optional<std::string>& error) {
  if (g_browsing_data_remove_history_callback_map_.count(request_id)) {
    std::move(g_browsing_data_remove_history_callback_map_[request_id]).Run(error);
    g_browsing_data_remove_history_callback_map_.erase(request_id);
  }
}

}  // namespace OHOS::NWeb
