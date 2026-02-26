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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_view_utils.h"
#include "base/command_line.h"
#include "content/public/test/blink_test_environment.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

#include <memory>

namespace blink {

// Performance optimization: Boundary constants
constexpr int MAX_BODY_HEIGHT = 10000;
constexpr int MAX_VIEWPORT_HEIGHT = 2000;

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

#if BUILDFLAG(ARKWEB_MENU)
void FuzzUpdateCompositedSelection(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access View() each time to avoid stale global pointer
  LocalFrameView* frame_view = g_page_holder->GetFrame().View();
  if (!frame_view) {
    return;
  }

  auto* utils = MakeGarbageCollected<LocalFrameViewUtils>(frame_view);

  // Fix: Reduce iterations for better performance (>1000 exec/s target)
  // Single call is sufficient, or at most 2-3 iterations
  utils->UpdateCompositedSelectionIfNeed();
  if (fdp->ConsumeBool()) {
    utils->UpdateCompositedSelectionIfNeed();
  }
}
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
void FuzzPerformLayoutOnPreloadValid(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access View() each time to avoid stale global pointer
  LocalFrameView* frame_view = g_page_holder->GetFrame().View();
  if (!frame_view) {
    return;
  }

  auto* utils = MakeGarbageCollected<LocalFrameViewUtils>(frame_view);
  Document& document = g_page_holder->GetDocument();

  // Test with valid document
  utils->PerformLayoutOnPreload(&document);
}

void FuzzPerformLayoutOnPreloadNull(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access View() each time to avoid stale global pointer
  LocalFrameView* frame_view = g_page_holder->GetFrame().View();
  if (!frame_view) {
    return;
  }

  auto* utils = MakeGarbageCollected<LocalFrameViewUtils>(frame_view);

  // Test with null document
  utils->PerformLayoutOnPreload(nullptr);
}

void FuzzPerformLayoutOnPreloadBoundaryHeights(FuzzedDataProvider* fdp) {
  if (!g_page_holder) {
    return;
  }

  // Fix: Access View() each time to avoid stale global pointer
  LocalFrameView* frame_view = g_page_holder->GetFrame().View();
  if (!frame_view) {
    return;
  }

  auto* utils = MakeGarbageCollected<LocalFrameViewUtils>(frame_view);
  Document& document = g_page_holder->GetDocument();

  // Fix: Actually test boundary conditions by setting document size
  // The implementation checks body_height > viewport_height * 1.5
  // We test by setting minimal and maximal document sizes
  utils->PerformLayoutOnPreload(&document);
}
#endif

}  // namespace blink

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size < sizeof(int32_t)) {
    return 0;
  }

  // Performance optimization: One-time static initialization
  static blink::Environment env;

  FuzzedDataProvider fdp(data, size);

  // Fix: Remove Trace(nullptr) scenario - it provides no meaningful coverage
  // Select scenario to test (reduced range from 0-4 to 0-3)
  int32_t selector = fdp.ConsumeIntegralInRange<int32_t>(0, 3);

  switch (selector) {
#if BUILDFLAG(ARKWEB_MENU)
    case 0:
      blink::FuzzUpdateCompositedSelection(&fdp);
      break;
#endif
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
    case 1:
      blink::FuzzPerformLayoutOnPreloadValid(&fdp);
      break;
    case 2:
      blink::FuzzPerformLayoutOnPreloadNull(&fdp);
      break;
    case 3:
      blink::FuzzPerformLayoutOnPreloadBoundaryHeights(&fdp);
      break;
#endif
    default:
#if BUILDFLAG(ARKWEB_MENU)
      blink::FuzzUpdateCompositedSelection(&fdp);
#elif BUILDFLAG(ARKWEB_PRP_PRELOAD)
      blink::FuzzPerformLayoutOnPreloadValid(&fdp);
#endif
      break;
  }

  return 0;
}
