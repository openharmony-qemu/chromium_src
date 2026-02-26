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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/ark_web.h"
#include "base/command_line.h"
#include "content/public/test/blink_test_environment.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

#include <memory>

namespace blink {

// Performance optimization: Boundary constants
constexpr double MAX_MARGIN_VALUE = 1000000.0;
constexpr double MIN_MARGIN_VALUE = -1000000.0;

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

void FuzzSetScrollbarAvoidAreaTopValid(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  auto* ark_web = MakeGarbageCollected<ArkWeb>(dom_window);

  // Test with valid margin values (positive only)
  double margin = fdp->ConsumeFloatingPointInRange<double>(
      0.0, MAX_MARGIN_VALUE);

  DummyExceptionStateForTesting exception_state;

  ark_web->setScrollbarAvoidAreaTop(margin, exception_state);
}

void FuzzSetScrollbarAvoidAreaTopInvalid(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  auto* ark_web = MakeGarbageCollected<ArkWeb>(dom_window);

  // Test with negative and extreme values
  double margin = fdp->ConsumeFloatingPointInRange<double>(
      MIN_MARGIN_VALUE, MAX_MARGIN_VALUE);

  DummyExceptionStateForTesting exception_state;

  ark_web->setScrollbarAvoidAreaTop(margin, exception_state);
}

void FuzzSetScrollbarAvoidAreaBottomValid(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  auto* ark_web = MakeGarbageCollected<ArkWeb>(dom_window);

  // Test with valid margin values (positive only)
  double margin = fdp->ConsumeFloatingPointInRange<double>(
      0.0, MAX_MARGIN_VALUE);

  DummyExceptionStateForTesting exception_state;

  ark_web->setScrollbarAvoidAreaBottom(margin, exception_state);
}

void FuzzSetScrollbarAvoidAreaBottomInvalid(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  auto* ark_web = MakeGarbageCollected<ArkWeb>(dom_window);

  // Test with negative and extreme values
  double margin = fdp->ConsumeFloatingPointInRange<double>(
      MIN_MARGIN_VALUE, MAX_MARGIN_VALUE);

  DummyExceptionStateForTesting exception_state;

  ark_web->setScrollbarAvoidAreaBottom(margin, exception_state);
}

void FuzzBothMargins(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  auto* ark_web = MakeGarbageCollected<ArkWeb>(dom_window);

  // Test setting both margins in sequence
  double top_margin = fdp->ConsumeFloatingPointInRange<double>(
      MIN_MARGIN_VALUE, MAX_MARGIN_VALUE);
  double bottom_margin = fdp->ConsumeFloatingPointInRange<double>(
      MIN_MARGIN_VALUE, MAX_MARGIN_VALUE);

  DummyExceptionStateForTesting exception_state;

  ark_web->setScrollbarAvoidAreaTop(top_margin, exception_state);
  ark_web->setScrollbarAvoidAreaBottom(bottom_margin, exception_state);
}

void FuzzSpecialValues(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  auto* ark_web = MakeGarbageCollected<ArkWeb>(dom_window);

  // Test with special double values
  double special_values[] = {
    0.0, -0.0, 1.0, -1.0,
    std::numeric_limits<double>::min(),
    std::numeric_limits<double>::max(),
    std::numeric_limits<double>::lowest(),
    std::numeric_limits<double>::infinity(),
    -std::numeric_limits<double>::infinity(),
    std::numeric_limits<double>::quiet_NaN()
  };

  DummyExceptionStateForTesting exception_state;

  size_t index = fdp->ConsumeIntegralInRange<size_t>(0, 9);
  double margin = special_values[index];

  if (fdp->ConsumeBool()) {
    ark_web->setScrollbarAvoidAreaTop(margin, exception_state);
  } else {
    ark_web->setScrollbarAvoidAreaBottom(margin, exception_state);
  }
}

void FuzzNegativeMargins(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access DomWindow() each time to avoid stale global pointer
  LocalDOMWindow* dom_window = g_page_holder->GetFrame().DomWindow();
  if (!dom_window) {
    return;
  }

  auto* ark_web = MakeGarbageCollected<ArkWeb>(dom_window);

  // Fix: Add explicit negative margin testing as suggested in review
  // Test specifically with negative values which should be rejected
  double negative_margin = fdp->ConsumeFloatingPointInRange<double>(
      -10000.0, -1.0);

  DummyExceptionStateForTesting exception_state;

  // Test both top and bottom with negative margins
  ark_web->setScrollbarAvoidAreaTop(negative_margin, exception_state);
  ark_web->setScrollbarAvoidAreaBottom(negative_margin, exception_state);
}

}  // namespace blink

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size < sizeof(int32_t)) {
    return 0;
  }

  // Performance optimization: One-time static initialization
  static blink::Environment env;

  FuzzedDataProvider fdp(data, size);

  // Fix: Remove Trace(nullptr) scenario - it provides no meaningful coverage
  // Select scenario to test (reduced range from 0-6 to 0-6 with new negative test)
  int32_t selector = fdp.ConsumeIntegralInRange<int32_t>(0, 6);

  switch (selector) {
    case 0:
      blink::FuzzSetScrollbarAvoidAreaTopValid(&fdp);
      break;
    case 1:
      blink::FuzzSetScrollbarAvoidAreaTopInvalid(&fdp);
      break;
    case 2:
      blink::FuzzSetScrollbarAvoidAreaBottomValid(&fdp);
      break;
    case 3:
      blink::FuzzSetScrollbarAvoidAreaBottomInvalid(&fdp);
      break;
    case 4:
      blink::FuzzBothMargins(&fdp);
      break;
    case 5:
      blink::FuzzSpecialValues(&fdp);
      break;
    case 6:
      blink::FuzzNegativeMargins(&fdp);
      break;
    default:
      blink::FuzzBothMargins(&fdp);
      break;
  }

  return 0;
}
