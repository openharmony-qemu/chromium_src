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
static BrowsingDataRemoveCallback g_browsing_data_remove_downloads_callback;
static BrowsingDataRemoveCallback g_browsing_data_remove_history_callback;
} // namespace

void NWebExtensionBrowsingDataCefDelegate::RemoveDownloads(
    NWebExtensionBrowsingDataRemovalOptions& options, BrowsingDataRemoveCallback callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  g_browsing_data_remove_downloads_callback = std::move(callback);
  NWebExtensionBrowsingDataDispatcher::RemoveDownloads(options);
#endif
}

void NWebExtensionBrowsingDataCefDelegate::RemoveHistory(
    NWebExtensionBrowsingDataRemovalOptions& options, BrowsingDataRemoveCallback callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  g_browsing_data_remove_history_callback = std::move(callback);
  NWebExtensionBrowsingDataDispatcher::RemoveHistory(options);
#endif
}

void NWebExtensionBrowsingDataCefDelegate::RemoveDownloadsCallback(std::optional<std::string>& error) {
  std::move(g_browsing_data_remove_downloads_callback).Run(error);
}

void NWebExtensionBrowsingDataCefDelegate::RemoveHistoryCallback(std::optional<std::string>& error) {
  std::move(g_browsing_data_remove_history_callback).Run(error);
}

}  // namespace OHOS::NWeb
