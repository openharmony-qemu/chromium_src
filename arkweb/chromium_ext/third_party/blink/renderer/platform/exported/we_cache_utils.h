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
#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_EXPORTED_WE_CACHE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_EXPORTED_WE_CACHE_H_

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "third_party/blink/public/platform/web_cache.h"
#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
#include "net/base/ip_endpoint.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#endif

namespace blink {

class WebCache;

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
enum class OfflineResourceType { IMAGE, CSS, CLASSIC_JS, MODULE_JS };
class ResourceResponse;
#endif

class WebCacheUtils {
  public:
  WebCache* web_cache_;
  WebCacheUtils(WebCache* web_cache);
#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
  // Add resource into MemoryCache
  static void AddResourceToCache(
    const std::string& url,
    const std::string& origin,
    const std::vector<uint8_t>& resource,
    const base::flat_map<std::string, std::string>& response_headers,
    const uint64_t type);
#endif

  private:
#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
  static ResourceResponse GetResourceResponse(
      const KURL& kurl,
      const base::flat_map<std::string, std::string>& response_headers);
#endif
};
}

#endif