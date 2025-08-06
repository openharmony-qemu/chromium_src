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
#include "arkweb/chromium_ext/ui/events/gesture_detection/gesture_detector_ext.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "ui/events/gesture_detection/gesture_listeners.h"
#include "ui/events/gestures/motion_event_aura.h"

using namespace ui;

class MockGestureListener : public GestureListener {
 public:
  bool OnDown(const MotionEvent& e, int tap_down_count) override {
    return false;
  }
  void OnShowPress(const MotionEvent& e) override {}
  bool OnSingleTapUp(const MotionEvent& e, int tap_count) override {
    return false;
  }
  void OnShortPress(const MotionEvent& e) override {}
  void OnLongPress(const MotionEvent& e) override {}
  void OnDragLongPress(const MotionEvent& e) override {}
  void OnCreateOverlay(const MotionEvent& e) override {}
  bool OnScroll(const MotionEvent& e1,
                const MotionEvent& e2,
                const MotionEvent& secondary_pointer_down,
                float distance_x,
                float distance_y) override {
    return false;
  }
  bool OnFling(const MotionEvent& e1,
               const MotionEvent& e2,
               float velocity_x,
               float velocity_y) override {
    return false;
  }
  bool OnSwipe(const MotionEvent& e1,
               const MotionEvent& e2,
               float velocity_x,
               float velocity_y) override {
    return false;
  }
  bool OnTwoFingerTap(const MotionEvent& e1, const MotionEvent& e2) override {
    return false;
  }
  void OnTapCancel(const MotionEvent& e) override {}
};

class MockDoubleTapListener : public DoubleTapListener {
 public:
  bool OnSingleTapConfirmed(const MotionEvent& e) override { return false; }
  bool OnDoubleTap(const MotionEvent& e) override { return false; }
  bool OnDoubleTapEvent(const MotionEvent& e) override { return false; }
};

void GestureDetectorExtFuzzTest(FuzzedDataProvider* fdp) {
  GestureDetector::Config config;
  GestureListener* gesture_listener = new MockGestureListener();
  DoubleTapListener* double_tap_listener = new MockDoubleTapListener();
  bool is_lost_focus = fdp->ConsumeBool();
  MotionEventAura ev;
  GestureDetectorExt impl(config, gesture_listener, double_tap_listener);

  impl.Cancel(is_lost_focus);

  impl.CancelTaps(is_lost_focus);

  impl.ActivateLongPressKeepDragTimeout(ev);

  impl.StopDragLongPressGesture();

  impl.StopCreateOverlayGesture();

  impl.OnAITextSelected();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  GestureDetectorExtFuzzTest(&fdp);

  return 0;
}
