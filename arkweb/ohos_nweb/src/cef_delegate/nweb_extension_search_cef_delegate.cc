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

#include "nweb_extension_search_cef_delegate.h"

#include <map>

#include "nweb_common.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_search_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {
static std::map<int32_t, SearchCallback> g_search_callback_map_;
} // namespace

bool NWebExtensionSearchCefDelegate::Query(SearchCallback callback,
                                           const NWebExtensionSearchQueryInfo& query_info) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int32_t request_id = 0;
  request_id++;
  g_search_callback_map_[request_id] = std::move(callback);
  bool result = NWebExtensionSearchDispatcher::Query(request_id, query_info);
  if (!result) {
    g_search_callback_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionSearchCefDelegate::QueryCallback(int32_t request_id,
    const std::optional<std::string>& error) {
  if (g_search_callback_map_.count(request_id)) {
    std::move(g_search_callback_map_[request_id]).Run(error);
    g_search_callback_map_.erase(request_id);
  }
}

}  // namespace OHOS::NWeb