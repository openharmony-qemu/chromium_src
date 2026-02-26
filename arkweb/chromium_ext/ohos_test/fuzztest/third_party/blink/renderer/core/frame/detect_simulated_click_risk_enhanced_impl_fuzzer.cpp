/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <fuzzer/FuzzedDataProvider.h>

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/detect_simulated_click_risk_enhanced_impl.h"
#include "base/command_line.h"
#include "content/public/test/blink_test_environment.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"

#include <climits>
#include <memory>

namespace blink {

// Performance optimization: Boundary constants to control input size
constexpr uint8_t MAX_NONCE_SIZE = 256;  // Increased from 32 for better coverage
constexpr int32_t MAX_ALGORITHM_VALUE = 1000;
constexpr int32_t MAX_VERSION_VALUE = 100;

// Performance optimization: Global shared test environment
static std::unique_ptr<DummyPageHolder> g_page_holder = nullptr;

struct Environment {
  Environment() {
    // Initialize Blink environment before creating DummyPageHolder
    base::CommandLine::Init(0, nullptr);
    blink_environment_.SetUp();
    g_page_holder = std::make_unique<DummyPageHolder>();
  }

  ~Environment() {
    // Destroy DummyPageHolder before TearDown to ensure Blink environment is still valid
    g_page_holder.reset();
  }

  content::BlinkTestEnvironmentWithIsolate blink_environment_;
};

void FuzzValidParams(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  // Create implementation instance
  auto* impl = MakeGarbageCollected<DetectSimulatedClickRiskEnhancedImpl>(
      dom_window);

  // Generate valid parameters
  int32_t algorithm = fdp->ConsumeIntegralInRange<int32_t>(0, MAX_ALGORITHM_VALUE);
  int32_t version = fdp->ConsumeIntegralInRange<int32_t>(0, MAX_VERSION_VALUE);

  // Generate nonce vector with controlled size
  size_t nonce_size = fdp->ConsumeIntegralInRange<size_t>(0, MAX_NONCE_SIZE);
  Vector<int32_t> nonce;
  nonce.reserve(nonce_size);
  for (size_t i = 0; i < nonce_size; ++i) {
    nonce.push_back(fdp->ConsumeIntegralInRange<int32_t>(
        INT8_MIN, INT8_MAX));
  }

  // Create script state and exception state
  ScriptState* script_state = ToScriptStateForMainWorld(dom_window);
  if (!script_state) {
    return;
  }

  // Enter V8 context before calling APIs that need it
  ScriptState::Scope scope(script_state);

  DummyExceptionStateForTesting exception_state;

  // Call the main API
  impl->DetectSimulatedClickRiskEnhanced(
      script_state, algorithm, nonce, version, exception_state);
}

void FuzzBoundaryValues(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  auto* impl = MakeGarbageCollected<DetectSimulatedClickRiskEnhancedImpl>(
      dom_window);

  // Test with boundary values
  int32_t algorithm = fdp->ConsumeBool() ? INT32_MIN : INT32_MAX;
  int32_t version = fdp->ConsumeBool() ? INT32_MIN : INT32_MAX;

  // Test with nonce values at boundaries
  Vector<int32_t> nonce;
  size_t nonce_count = fdp->ConsumeIntegralInRange<size_t>(0, 3);
  for (size_t i = 0; i < nonce_count; ++i) {
    if (fdp->ConsumeBool()) {
      nonce.push_back(INT8_MIN);
    } else {
      nonce.push_back(INT8_MAX);
    }
  }

  // Fix: Check null pointer before calling ToScriptState
  ScriptState* script_state = ToScriptStateForMainWorld(dom_window);
  if (!script_state) {
    return;
  }

  // Enter V8 context before calling APIs that need it
  ScriptState::Scope scope(script_state);

  DummyExceptionStateForTesting exception_state;

  impl->DetectSimulatedClickRiskEnhanced(
      script_state, algorithm, nonce, version, exception_state);
}

void FuzzInvalidParameters(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  auto* impl = MakeGarbageCollected<DetectSimulatedClickRiskEnhancedImpl>(
      dom_window);

  // Test with invalid nonce values (outside INT8 range)
  // Fix: Use unsigned arithmetic to avoid integer overflow
  Vector<int32_t> invalid_nonce;
  size_t nonce_count = fdp->ConsumeIntegralInRange<size_t>(0, 5);
  for (size_t i = 0; i < nonce_count; ++i) {
    // Generate values outside valid INT8 range using safe arithmetic
    if (fdp->ConsumeBool()) {
      // Use ConsumeIntegralInRange to avoid overflow
      invalid_nonce.push_back(fdp->ConsumeIntegralInRange<int32_t>(
          INT32_MIN, INT8_MIN - 1));
    } else {
      invalid_nonce.push_back(fdp->ConsumeIntegralInRange<int32_t>(
          INT8_MAX + 1, INT32_MAX));
    }
  }

  int32_t algorithm = fdp->ConsumeIntegral<int32_t>();
  int32_t version = fdp->ConsumeIntegral<int32_t>();

  // Fix: Check null pointer before calling ToScriptState
  ScriptState* script_state = ToScriptStateForMainWorld(dom_window);
  if (!script_state) {
    return;
  }

  // Enter V8 context before calling APIs that need it
  ScriptState::Scope scope(script_state);

  DummyExceptionStateForTesting exception_state;

  impl->DetectSimulatedClickRiskEnhanced(
      script_state, algorithm, invalid_nonce, version, exception_state);
}

}  // namespace blink

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size < sizeof(int32_t)) {
    return 0;
  }

  // Performance optimization: One-time static initialization
  static blink::Environment env;

  FuzzedDataProvider fdp(data, size);

  // Select scenario to test
  int32_t selector = fdp.ConsumeIntegralInRange<int32_t>(0, 2);

  switch (selector) {
    case 0:
      blink::FuzzValidParams(&fdp);
      break;
    case 1:
      blink::FuzzBoundaryValues(&fdp);
      break;
    case 2:
      blink::FuzzInvalidParameters(&fdp);
      break;
    default:
      blink::FuzzValidParams(&fdp);
      break;
  }

  return 0;
}
