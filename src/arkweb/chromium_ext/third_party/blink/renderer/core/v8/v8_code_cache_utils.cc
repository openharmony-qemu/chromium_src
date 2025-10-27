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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/v8/v8_code_cache_utils.h"
#include "third_party/blink/renderer/platform/bindings/v8_binding_macros.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"

namespace blink {

V8CodeCacheUtils::CacheError V8CodeCacheUtils::GenerateCodeCache(
    ScriptState* script_state,
    const String& url,
    const String& script,
    CacheOptions cache_options)
{
  KURL source_url(url);
  ResourceResponse response(source_url);
  for (auto header : cache_options.response_headers_) {
    response.SetHttpHeaderField(AtomicString(header.key),
                                AtomicString(header.value));
  }

  response.SetResponseTime(base::Time::UnixEpoch());
  const AtomicString type("text/javascript");
  response.SetMimeType(type);

  auto sender = blink::CachedMetadataSender::Create(
      response, mojom::blink::CodeCacheType::kJavascript,
      blink::SecurityOrigin::Create(source_url));

  auto cache_handler = MakeGarbageCollected<blink::ScriptCachedMetadataHandler>(
      UTF8Encoding(), std::move(sender));

  V8CodeCacheUtils::CacheError error = GenerateCodeCacheInternal(
      script_state, script, source_url, cache_handler, cache_options);
  return error;
}

V8CodeCacheUtils::CacheError V8CodeCacheUtils::GenerateCodeCacheInternal(
    ScriptState* script_state,
    const String& script_string,
    const KURL& source_url,
    blink::ScriptCachedMetadataHandler* cache_handler,
    CacheOptions cache_options)
{
  const String file_name = source_url.GetString();

  ScriptState::Scope scope(script_state);
  v8::Isolate* isolate = script_state->GetIsolate();
  v8::TryCatch block(isolate);

  v8::ScriptOrigin origin(V8String(isolate, file_name));

  v8::Local<v8::String> code(V8String(isolate, script_string));
  v8::ScriptCompiler::Source source(code, origin);

  v8::MaybeLocal<v8::UnboundScript> maybe_unbound_script =
      v8::ScriptCompiler::CompileUnboundScript(
          isolate, &source, v8::ScriptCompiler::kEagerCompile);

  v8::Local<v8::UnboundScript> unbound_script;
  if (!maybe_unbound_script.ToLocal(&unbound_script)) {
    return V8CodeCacheUtils::CacheError::kInternalError;
  }

  auto execution_context = ExecutionContext::From(script_state);
  auto code_cache_host =
      ExecutionContext::GetCodeCacheHostFromContext(execution_context);

  WTF::TextPosition text_position(WTF::OrdinalNumber::First(), WTF::OrdinalNumber::First());
  V8CodeCache::ProduceCacheUtils(isolate, code_cache_host, unbound_script, cache_handler,
                                 script_string.length(), source_url, text_position,
                                 "v8.precompile",
                                 V8CodeCache::ProduceCacheOptions::kSetTimeStamp);
  V8CodeCache::ProduceCacheUtils(isolate, code_cache_host, unbound_script, cache_handler,
                                 script_string.length(), source_url, text_position,
                                 "v8.precompile",
                                 V8CodeCache::ProduceCacheOptions::kProduceCodeCache);

  return V8CodeCacheUtils::CacheError::kNoError;
}
}  // namespace blink