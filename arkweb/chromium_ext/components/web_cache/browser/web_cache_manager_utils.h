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

#ifndef COMPONENTS_WEB_CACHE_BROWSER_WEB_CACHE_MANAGER_UTILS_H_
#define COMPONENTS_WEB_CACHE_BROWSER_WEB_CACHE_MANAGER_UTILS_H_

#include <map>
#include <string>
#include <vector>

#include "arkweb/build/features/features.h"

namespace web_cache {
class WebCacheManager;

class WebCacheManagerUtils {
 public:
  WebCacheManager* webCacheManager;
  WebCacheManagerUtils(WebCacheManager* impl);

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
  void AddResourceToCache(
      const std::string& url,
      const std::string& origin,
      const std::vector<uint8_t>& resource,
      const std::map<std::string, std::string>& response_headers,
      const int type);
#endif
};

}  // namespace web_cache

#endif  // COMPONENTS_WEB_CACHE_BROWSER_WEB_CACHE_MANAGER_UTILS_H_
