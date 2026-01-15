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
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "base/test/bind.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_tester.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_testing.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/mojo/mojo_binding_context.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/mojom/frame/lifecycle.mojom-blink.h"

class MockBusinessRiskIntelligentDetection
    : public device::mojom::blink::BusinessRiskIntelligentDetection {
 public:
  MockBusinessRiskIntelligentDetection() = default;
  ~MockBusinessRiskIntelligentDetection() override = default;

  MOCK_METHOD5(DetectSimulatedClickRiskEnhanced,
               void(int32_t request_id,
                    int32_t algorithm,
                    const WTF::Vector<int8_t>& nonce,
                    int32_t version,
                    device::mojom::blink::BusinessRiskIntelligentDetection
                        ::DetectSimulatedClickRiskEnhancedCallback callback));

  void Bind(mojo::PendingReceiver<device::mojom::blink::BusinessRiskIntelligentDetection> receiver) {
    receiver_.Bind(std::move(receiver));
  }

 private:
  mojo::Receiver<device::mojom::blink::BusinessRiskIntelligentDetection> receiver_{this};
};

namespace blink {

class DetectSimulatedClickRiskEnhancedImplTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    dummy_page_holder_ = std::make_unique<DummyPageHolder>();
    window_ = dummy_page_holder_->GetFrame().DomWindow();

    mock_service_ = std::make_unique<MockBusinessRiskIntelligentDetection>();
    mojo::PendingRemote<device::mojom::blink::BusinessRiskIntelligentDetection> remote;
    mock_service_->Bind(remote.InitWithNewPipeAndPassReceiver());

    auto* broker = Platform::Current()->GetBrowserInterfaceBroker();
    broker->SetBinderForTesting(
        device::mojom::blink::BusinessRiskIntelligentDetection::Name_,
        base::BindLambdaForTesting([this](mojo::ScopedMessagePipeHandle handle) {
          mojo::PendingReceiver<device::mojom::blink::BusinessRiskIntelligentDetection>
              receiver(std::move(handle));
          mock_service_->Bind(std::move(receiver));
        }));

    impl_ = MakeGarbageCollected<DetectSimulatedClickRiskEnhancedImpl>(window_);
  }

  void TearDown() override {
    impl_->Dispose();
    mock_service_.reset();
    dummy_page_holder_.reset();
  }

  ScriptState* GetScriptState() {
    return ToScriptStateForMainWorld(&dummy_page_holder_->GetFrame());
  }

 protected:
  frame_test_helpers::WebViewHelper web_view_helper_;
  std::unique_ptr<DummyPageHolder> dummy_page_holder_;
  LocalDOMWindow* window_ = nullptr;
  Persistent<DetectSimulatedClickRiskEnhancedImpl> impl_;
  std::unique_ptr<MockBusinessRiskIntelligentDetection> mock_service_;
};

TEST_F(DetectSimulatedClickRiskEnhancedImplTest, DetectSimulatedClickRiskEnhanced_Success) {
  ScriptState* script_state = GetScriptState();
  DummyExceptionStateForTesting exception_state;
  ScriptState::Scope scope(script_state);

  int32_t algorithm = 0;
  WTF::Vector<int32_t> nonce = {1, 2, 3};
  int32_t version = 1;

  EXPECT_CALL(*mock_service_, DetectSimulatedClickRiskEnhanced)
      .WillOnce(testing::Invoke([](
          int32_t request_id, int32_t, const WTF::Vector<int8_t>&, int32_t,
          device::mojom::blink::BusinessRiskIntelligentDetection
              ::DetectSimulatedClickRiskEnhancedCallback callback) {
        std::move(callback).Run(request_id, 0, R"({"result": "success"})");
      }));

  ScriptPromise<IDLString> promise = impl_->DetectSimulatedClickRiskEnhanced(
      script_state, algorithm, nonce, version, exception_state);

  ScriptPromiseTester tester(script_state, promise);
  tester.WaitUntilSettled();

  EXPECT_TRUE(tester.IsFulfilled());
  EXPECT_EQ(tester.ValueAsString(), R"({"result": "success"})");
}

TEST_F(DetectSimulatedClickRiskEnhancedImplTest, InvalidParameter_NonceOutOfInt8Range) {
  ScriptState* script_state = GetScriptState();
  DummyExceptionStateForTesting exception_state;
  ScriptState::Scope scope(script_state);

  int32_t algorithm = 0;
  WTF::Vector<int32_t> nonce = {1, 256, 3};
  int32_t version = 1;

  ScriptPromise<IDLString> promise = impl_->DetectSimulatedClickRiskEnhanced(
      script_state, algorithm, nonce, version, exception_state);

  ScriptPromiseTester tester(script_state, promise);
  tester.WaitUntilSettled();

  EXPECT_TRUE(tester.IsRejected());
  v8::Local<v8::Value> error = tester.Value().V8Value();
  EXPECT_TRUE(error->IsObject());

  v8::Local<v8::Object> error_obj = error.As<v8::Object>();
  v8::Isolate* isolate = script_state->GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::Value> code_val;
  EXPECT_TRUE(error_obj->Get(context,
      v8::String::NewFromUtf8(isolate, "code").ToLocalChecked()).ToLocal(&code_val));
  EXPECT_EQ(code_val->Int32Value(context).FromJust(),
      static_cast<int32_t>(BusinessError::INVALID_PARAMETERS));

  v8::Local<v8::Value> msg_val;
  EXPECT_TRUE(error_obj->Get(context,
      v8::String::NewFromUtf8(isolate, "message").ToLocalChecked()).ToLocal(&msg_val));
  EXPECT_EQ(v8::String::Utf8Value(isolate, msg_val).operator*(),
      std::string("Invalid parameters."));
}

TEST_F(DetectSimulatedClickRiskEnhancedImplTest, DetectSimulatedClickRiskEnhanced_NetworkUnreachable) {
  ScriptState* script_state = GetScriptState();
  DummyExceptionStateForTesting exception_state;
  ScriptState::Scope scope(script_state);

  int32_t algorithm = 0;
  WTF::Vector<int32_t> nonce = {1, 2, 3};
  int32_t version = 1;

  EXPECT_CALL(*mock_service_, DetectSimulatedClickRiskEnhanced)
      .WillOnce(testing::Invoke([](
          int32_t request_id, int32_t, const WTF::Vector<int8_t>&, int32_t,
          device::mojom::blink::BusinessRiskIntelligentDetection
              ::DetectSimulatedClickRiskEnhancedCallback callback) {
        std::move(callback).Run(request_id,
            static_cast<int32_t>(BusinessError::NETWORK_UNREACHABLE), "");
      }));

  ScriptPromise<IDLString> promise = impl_->DetectSimulatedClickRiskEnhanced(
      script_state, algorithm, nonce, version, exception_state);

  ScriptPromiseTester tester(script_state, promise);
  tester.WaitUntilSettled();

  EXPECT_TRUE(tester.IsRejected());
  v8::Local<v8::Value> error = tester.Value().V8Value();
  v8::Local<v8::Object> error_obj = error.As<v8::Object>();
  v8::Isolate* isolate = script_state->GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::Value> code_val;
  EXPECT_TRUE(error_obj->Get(context,
      v8::String::NewFromUtf8(isolate, "code").ToLocalChecked()).ToLocal(&code_val));
  EXPECT_EQ(code_val->Int32Value(context).FromJust(),
      static_cast<int32_t>(BusinessError::NETWORK_UNREACHABLE));

  v8::Local<v8::Value> msg_val;
  EXPECT_TRUE(error_obj->Get(context,
      v8::String::NewFromUtf8(isolate, "message").ToLocalChecked()).ToLocal(&msg_val));
  EXPECT_EQ(v8::String::Utf8Value(isolate, msg_val).operator*(),
      std::string("The network is unreachable."));
}

TEST_F(DetectSimulatedClickRiskEnhancedImplTest, ServiceUnbound_AfterDispose) {
  impl_->Dispose();

  ScriptState* script_state = GetScriptState();
  DummyExceptionStateForTesting exception_state;
  ScriptState::Scope scope(script_state);

  int32_t algorithm = 0;
  WTF::Vector<int32_t> nonce = {1, 2, 3};
  int32_t version = 1;

  ScriptPromise<IDLString> promise = impl_->DetectSimulatedClickRiskEnhanced(
      script_state, algorithm, nonce, version, exception_state);

  ScriptPromiseTester tester(script_state, promise);
  tester.WaitUntilSettled();

  EXPECT_TRUE(tester.IsRejected());
  v8::Local<v8::Value> error = tester.Value().V8Value();
  v8::Local<v8::Object> error_obj = error.As<v8::Object>();
  v8::Isolate* isolate = script_state->GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::Value> code_val;
  EXPECT_TRUE(error_obj->Get(context,
      v8::String::NewFromUtf8(isolate, "code").ToLocalChecked()).ToLocal(&code_val));
  EXPECT_EQ(code_val->Int32Value(context).FromJust(), -1);

  v8::Local<v8::Value> msg_val;
  EXPECT_TRUE(error_obj->Get(context,
      v8::String::NewFromUtf8(isolate, "message").ToLocalChecked()).ToLocal(&msg_val));
  EXPECT_EQ(v8::String::Utf8Value(isolate, msg_val).operator*(), std::string("Unknown error."));
}

TEST_F(DetectSimulatedClickRiskEnhancedImplTest, Dispose_CleanupAllPendingPromises) {
  ScriptState* script_state = GetScriptState();
  DummyExceptionStateForTesting exception_state;
  ScriptState::Scope scope(script_state);

  WTF::Vector<int32_t> nonce1 = {1, 1, 1};
  WTF::Vector<int32_t> nonce2 = {2, 2, 2};
  ScriptPromise<IDLString> promise1
      = impl_->DetectSimulatedClickRiskEnhanced(script_state, 0, nonce1, 1, exception_state);
  ScriptPromise<IDLString> promise2
      = impl_->DetectSimulatedClickRiskEnhanced(script_state, 0, nonce2, 1, exception_state);

  impl_->Dispose();

  ScriptPromiseTester tester1(script_state, promise1);
  tester1.WaitUntilSettled();
  EXPECT_TRUE(tester1.IsRejected());

  ScriptPromiseTester tester2(script_state, promise2);
  tester2.WaitUntilSettled();
  EXPECT_TRUE(tester2.IsRejected());
}

}  // namespace blink
