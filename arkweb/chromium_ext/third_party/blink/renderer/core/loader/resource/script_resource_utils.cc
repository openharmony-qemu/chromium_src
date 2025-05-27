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
#include "third_party/blink/renderer/core/loader/resource/script_resource.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/loader/resource/script_resource_utils.h"

#if BUILDFLAG(ARKWEB_CUSTOM_SCHEME_CODECACHE)
#include "base/logging.h"
#endif

namespace blink {
ScriptResourceUtils::ScriptResourceUtils(ScriptResource* impl)
{
	this->scriptResource = impl;
}

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
ScriptResource* ScriptResourceUtils::CreateForOfflineResource(
    const KURL& kurl,
    const KURL& origin_url,
    ResourceResponse response,
    const bool is_module)
{
  ResourceRequest request(kurl);
  if (is_module) {
    request.SetMode(network::mojom::RequestMode::kCors);
    request.SetCredentialsMode(network::mojom::CredentialsMode::kSameOrigin);
  }

  AtomicString cross_origin =
      response.HttpHeaderField(AtomicString("Cross-Origin"));
  if (!cross_origin.empty()) {
    request.SetMode(network::mojom::RequestMode::kCors);
    if (cross_origin != "use-credentials") {
      request.SetCredentialsMode(network::mojom::CredentialsMode::kSameOrigin);
    }
  }

  request.SetRequestorOrigin(SecurityOrigin::Create(origin_url));

  ResourceLoaderOptions options(nullptr);

  TextResourceDecoderOptions decoder_options(
      TextResourceDecoderOptions::kPlainTextContent, UTF8Encoding());

  auto script_type = is_module ? mojom::blink::ScriptType::kModule
                               : mojom::blink::ScriptType::kClassic;
  return MakeGarbageCollected<ScriptResource>(
      request, options, decoder_options, nullptr, ScriptResource::kAllowStreaming,
      nullptr, /*v8_compile_hints_producer=*/
      nullptr, /*v8_compile_hints_consumer=*/
      v8_compile_hints::MagicCommentMode::kNever, script_type);
}
#endif
}