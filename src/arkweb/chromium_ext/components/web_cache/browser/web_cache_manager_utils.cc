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

#include "components/web_cache/browser/web_cache_manager.h"
#include "arkweb/chromium_ext/components/web_cache/browser/web_cache_manager_utils.h"

#include "base/logging.h"

namespace web_cache {

WebCacheManagerUtils::WebCacheManagerUtils(WebCacheManager* impl)
{
  this->webCacheManager = impl;
}

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
void WebCacheManagerUtils::AddResourceToCache(
    const std::string& url,
    const std::string& origin,
    const std::vector<uint8_t>& resource,
    const std::map<std::string, std::string>& response_headers,
    const int type)
{
  if (webCacheManager->web_cache_services_.size() != 1) {
    LOG(ERROR) << "Add resource to MemoryCache failed. No render service or in "
                  "multiple render services mode.";
    return;
  }

  auto service = webCacheManager->web_cache_services_.begin();
  if (service != webCacheManager->web_cache_services_.end() && service->second.is_bound()) {
    base::flat_map<std::string, std::string> headers_flat_map(
        response_headers.begin(), response_headers.end());
    service->second->AddResourceToCache(url, origin, resource, headers_flat_map,
                                        type);
  }
}
#endif
}  // namespace web_cache
