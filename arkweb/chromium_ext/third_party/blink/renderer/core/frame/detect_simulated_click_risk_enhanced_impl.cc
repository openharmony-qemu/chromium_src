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

#include "third_party/blink/renderer/core/frame/detect_simulated_click_risk_enhanced_impl.h"

#include <climits>

#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"

namespace blink {

DetectSimulatedClickRiskEnhancedImpl::DetectSimulatedClickRiskEnhancedImpl(
    LocalDOMWindow* window) : detect_service_(window) {
  Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
      detect_service_.BindNewPipeAndPassReceiver(
          window->GetTaskRunner(TaskType::kMiscPlatformAPI)));

  detect_service_.set_disconnect_handler(WTF::BindOnce(
      &DetectSimulatedClickRiskEnhancedImpl::Dispose, WrapWeakPersistent(this)));
}

ScriptPromise<IDLString> DetectSimulatedClickRiskEnhancedImpl::
    DetectSimulatedClickRiskEnhanced(ScriptState* script_state,
                                     int32_t algorithm,
                                     const Vector<int32_t>& nonce,
                                     int32_t version,
                                     ExceptionState& exception_state) {
  LOG(INFO) << "DetectSim: DetectSimulatedClickRiskEnhanced client enter.";

  auto* resolver = MakeGarbageCollected<ScriptPromiseResolver<IDLString>>(
      script_state, exception_state.GetContext());
  ScriptPromise<IDLString> promise = resolver->Promise();

  Vector<int8_t> nonce_;
  for (size_t i = 0; i < nonce.size(); i++) {
    int32_t num = nonce[i];
    if (num < INT8_MIN || num > INT8_MAX) {
      HandleBusinessError(static_cast<int32_t>(BusinessError::INVALID_PARAMETERS),
                          resolver);
      return promise;
    }
    nonce_.push_back(static_cast<int8_t>(num));
  }

  int32_t request_id = GenerateRequestId();
  request_map_.Set(request_id, resolver);

  if (detect_service_.is_bound()) {
    LOG(INFO) << "DetectSim: detect_service_ bounded.";

    detect_service_->DetectSimulatedClickRiskEnhanced(
        request_id,
        algorithm,
        nonce_,
        version,
        WTF::BindOnce(&DetectSimulatedClickRiskEnhancedImpl::OnRequestComplete,
                      WrapWeakPersistent(this)));
  } else {
    HandleBusinessError(-1, resolver);
    request_map_.erase(request_id);
  }

  return promise;
}

int32_t DetectSimulatedClickRiskEnhancedImpl::GenerateRequestId() {
  return next_request_id_++;
}

void DetectSimulatedClickRiskEnhancedImpl::OnRequestComplete(
    int32_t request_id,
    int32_t ans_code,
    const WTF::String& json_result) {
  LOG(INFO) << "DetectSim: OnRequestComplete enter.";

  auto it = request_map_.find(request_id);
  if (it == request_map_.end()) {
    return;
  }
  Member<ScriptPromiseResolver<IDLString>> resolver = it->value;
  request_map_.erase(it);

  if (ans_code == 0) {
    LOG(INFO) << "DetectSim: OnRequestComplete success. json_result len:"
               << json_result.length();

    resolver->Resolve(json_result);
  } else {
    HandleBusinessError(ans_code, resolver);
  }
}

void DetectSimulatedClickRiskEnhancedImpl::HandleBusinessError(
    int32_t code,
    Member<ScriptPromiseResolver<IDLString>> resolver) {
  LOG(INFO) << "DetectSim: HandleBusinessError enter. code:" << code;

  const char* error_message = GetErrorMessage(static_cast<BusinessError>(code));

  ScriptState* script_state = resolver->GetScriptState();
  if (!script_state) {
    LOG(WARNING) << "DetectSim: HandleBusinessError failed: ScriptState is null.";
    resolver->RejectWithDOMException(DOMExceptionCode::kUnknownError, "Unknown error.");
    return;
  }
  ScriptState::Scope scope(script_state);

  v8::Isolate* isolate = script_state->GetIsolate();
  if (!isolate || isolate->IsDead()) {
    LOG(WARNING) << "DetectSim: HandleBusinessError failed: Isolate is null or dead.";
    resolver->RejectWithDOMException(DOMExceptionCode::kUnknownError, "Unknown error.");
    return;
  }
  if (!isolate->InContext()) {
    LOG(WARNING) << "DetectSim: HandleBusinessError failed: Isolate not in context.";
    resolver->RejectWithDOMException(DOMExceptionCode::kUnknownError, "Unknown error.");
    return;
  }

  v8::Local<v8::Context> context = script_state->GetContext();

  v8::Local<v8::Object> error_obj = v8::Object::New(isolate);
  error_obj->Set(context,
                 v8::String::NewFromUtf8(isolate, "code").ToLocalChecked(),
                 v8::Number::New(isolate, code))
      .Check();
  error_obj->Set(context,
                 v8::String::NewFromUtf8(isolate, "message").ToLocalChecked(),
                 v8::String::NewFromUtf8(isolate, error_message).ToLocalChecked())
      .Check();

  resolver->Reject(error_obj);
}

const char* DetectSimulatedClickRiskEnhancedImpl::GetErrorMessage(BusinessError error) {
  switch (error) {
    case BusinessError::INTERNAL_ERROR:
      return "Internal error.";
    case BusinessError::NETWORK_UNREACHABLE:
      return "The network is unreachable.";
    case BusinessError::ACCESS_CLOUD_SERVER_FAIL:
      return "Access cloud server fail.";
    case BusinessError::VERIFY_CLOUD_CAPABILITY_FAIL:
      return "Verify cloud capability fail.";
    case BusinessError::INTERFACE_ACCESS_FREQUENCY_EXCEEDS_LIMIT:
      return "The interface access frequency exceeds the limit.";
    case BusinessError::INTERNAL_TIMEOUT:
      return "Internal timeout.";
    case BusinessError::INVALID_PARAMETERS:
      return "Invalid parameters.";
    default:
      return "Unknown error.";
  }
}

void DetectSimulatedClickRiskEnhancedImpl::Dispose() {
  LOG(INFO) << "DetectSim: DetectSimulatedClickRiskEnhancedImpl Dispose.";
  if (detect_service_.is_bound()) {
    LOG(INFO) << "DetectSim: Dispose, remote reset.";
    detect_service_.reset();
  }

  // The promises will be resolved/rejected when the context is destroyed.
  request_map_.clear();
}

void DetectSimulatedClickRiskEnhancedImpl::Trace(Visitor* visitor) const {
  visitor->Trace(request_map_);
  visitor->Trace(detect_service_);
}

}  // namespace blink
