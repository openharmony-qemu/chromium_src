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
#ifndef ARKWEB_THIRD_PARTY_BLINK_RENDERER_PLATFORM_WEBORIGIN_SCHEME_REGISTRY_UTILS_H_
#define ARKWEB_THIRD_PARTY_BLINK_RENDERER_PLATFORM_WEBORIGIN_SCHEME_REGISTRY_UTILS_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/platform/weborigin/scheme_registry.h"

namespace blink {
class SchemeRegistry;

class SchemeRegistryUtils {
 public:
  SchemeRegistry* scheme_registry_;

  SchemeRegistryUtils(SchemeRegistry* scheme_registry);

  // Schemes which can use code caching and check whether the script content has
  // changed relying on a response time match from the network cache.
#if BUILDFLAG(ARKWEB_CUSTOM_SCHEME_CODECACHE)
  static void RegisterURLSchemeAsSupportingCodeCacheWithResponseTime(
      const String& scheme);
  static bool SchemeSupportsCodeCacheWithResponseTime(const String& scheme);
#endif
};
}  // namespace blink
#endif
