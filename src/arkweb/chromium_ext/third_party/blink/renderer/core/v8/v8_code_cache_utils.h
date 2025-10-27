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

#ifndef V8_CODE_CACHE_UTILS_H_
#define V8_CODE_CACHE_UTILS_H_

#include "third_party/blink/renderer/bindings/core/v8/v8_code_cache.h"
#include "arkweb/build/features/features.h"
#include "third_party/blink/public/mojom/v8_cache_options.mojom-blink.h"
#include "third_party/blink/renderer/bindings/core/v8/script_source_location_type.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/loader/fetch/url_loader/cached_metadata_handler.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/core/script/classic_script.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/loader/fetch/code_cache_host.h"
#include "third_party/blink/renderer/platform/wtf/text/text_encoding.h"
#include "v8/include/v8.h"


#if BUILDFLAG(ARKWEB_PRECOMPILE)
namespace WTF {
  class TextEncoding;
  class TextPosition;
}  // namespace WTF

namespace blink {
class CachedMetadata;
class CodeCacheHost;
class KURL;
class ScriptState;

class CORE_EXPORT V8CodeCacheUtils {
 public:
  enum class CacheError {
    kNoError = 0,
    kInternalError = -1,
  };

  class CacheOptions {
   public:
    explicit CacheOptions(
        const WTF::HashMap<WTF::String, WTF::String> response_headers,
        const bool is_module,
        const bool is_top_level)
        : response_headers_(response_headers),
          is_module_(is_module),
          is_top_level_(is_top_level) {}
    WTF::HashMap<WTF::String, WTF::String> response_headers_;
    bool is_module_;
    bool is_top_level_;
  };

  static CacheError GenerateCodeCache(ScriptState* script_state,
                                      const String& url,
                                      const String& script,
                                      CacheOptions cache_options);

 private:
  static CacheError GenerateCodeCacheInternal(ScriptState* script_state,
                                              const String& script_string,
                                              const KURL& source_url,
                                              ScriptCachedMetadataHandler* cache_handler,
                                              CacheOptions cache_options);

};
} // namespace blink
#endif
#endif // V8_CODE_CACHE_UTILS_H_