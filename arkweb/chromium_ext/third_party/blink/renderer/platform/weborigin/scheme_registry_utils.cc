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
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/weborigin/scheme_registry_utils.h"

namespace blink {

SchemeRegistryUtils::SchemeRegistryUtils(SchemeRegistry* scheme_registry)
{
  this->scheme_registry_ = scheme_registry;
}

#if BUILDFLAG(ARKWEB_CUSTOM_SCHEME_CODECACHE)
void SchemeRegistryUtils::RegisterURLSchemeAsSupportingCodeCacheWithResponseTime(
    const String& scheme)
{
  DCHECK_EQ(scheme, scheme.LowerASCII());
  GetMutableURLSchemesRegistry().code_cache_with_response_time_schemes.insert(
      scheme);
}

bool SchemeRegistryUtils::SchemeSupportsCodeCacheWithResponseTime(
    const String& scheme)
{
  if (scheme.empty()) {
    return false;
  }
  DCHECK_EQ(scheme, scheme.LowerASCII());
  return GetURLSchemesRegistry().code_cache_with_response_time_schemes.Contains(
      scheme);
}
#endif
}