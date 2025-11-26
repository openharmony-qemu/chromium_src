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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_DETECT_SIMULATED_CLICK_RISK_ENHANCED_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_DETECT_SIMULATED_CLICK_RISK_ENHANCED_IMPL_H_

#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/heap/collection_support/heap_hash_map.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/mojo/heap_mojo_remote.h"
#include "arkweb/chromium_ext/services/device/public/mojom/business_risk_intelligent_detection.mojom-blink.h"

namespace blink {

class LocalDOMWindow;

enum class BusinessError {
  INTERNAL_ERROR = 1012500001,
  NETWORK_UNREACHABLE = 1012500002,
  ACCESS_CLOUD_SERVER_FAIL = 1012500003,
  VERIFY_CLOUD_CAPABILITY_FAIL = 1012500004,
  INTERFACE_ACCESS_FREQUENCY_EXCEEDS_LIMIT = 1012500005,
  INTERNAL_TIMEOUT = 1012500006,
  INVALID_PARAMETERS = 1012500007
};

class CORE_EXPORT DetectSimulatedClickRiskEnhancedImpl final
    : public GarbageCollected<DetectSimulatedClickRiskEnhancedImpl> {
  USING_PRE_FINALIZER(DetectSimulatedClickRiskEnhancedImpl, Dispose);

 public:
  explicit DetectSimulatedClickRiskEnhancedImpl(LocalDOMWindow*);

  ScriptPromise<IDLString> DetectSimulatedClickRiskEnhanced(
      ScriptState* script_state,
      int32_t algorithm,
      const Vector<int32_t>& nonce,
      int32_t version,
      ExceptionState& exception_state);

  void Trace(Visitor* visitor) const;
  void Dispose();

 private:
  HeapMojoRemote<device::mojom::blink::BusinessRiskIntelligentDetection> detect_service_;

  using RequestMap = HeapHashMap<int32_t, Member<ScriptPromiseResolver<IDLString>>>;
  RequestMap request_map_;
  int32_t next_request_id_ = 1;
  int32_t GenerateRequestId();

  void OnRequestComplete(int32_t request_id,
                         int32_t ans_code,
                         const WTF::String& json_result);

  void HandleBusinessError(int32_t code,
                           Member<ScriptPromiseResolver<IDLString>> resolver);

  const char* GetErrorMessage(BusinessError error);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_DETECT_SIMULATED_CLICK_RISK_ENHANCED_IMPL_H_
