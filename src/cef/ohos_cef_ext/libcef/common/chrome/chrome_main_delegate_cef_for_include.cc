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

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#endif

#include "services/network/public/cpp/features.h"

#if BUILDFLAG(ARKWEB_BFCACHE)
#include "content/public/common/content_features.h"
#endif

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
CefRefPtr<CefRequestContext>
ChromeMainDelegateCef::GetGlobalOTRRequestContext() {
  auto browser_client = content_browser_client();
  if (browser_client) {
    return browser_client->off_the_record_request_context();
  }
  return nullptr;
}
#endif

static void ManageBackForwardCacheExt(std::vector<std::string>& disable_features) {
  if (features::kBackForwardCache.default_state ==
      base::FEATURE_ENABLED_BY_DEFAULT) {
    // Disable BackForwardCache globally so that
    // blink::RuntimeEnabledFeatures::BackForwardCacheEnabled reports the
    // correct value in the renderer process (see issue #3374).
    disable_features.push_back(features::kBackForwardCache.name);
  }
}
