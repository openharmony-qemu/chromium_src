/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <memory>
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "ui/events/gesture_detection/gesture_detector.h"
#include "ui/events/gesture_detection/gesture_listeners.h"
#undef private

namespace ui {
namespace {
class MockGestureListener : public GestureListener {
 public:
  MOCK_METHOD(bool, OnDown, (const MotionEvent&, int), (override));
  MOCK_METHOD(void, OnShowPress, (const MotionEvent&), (override));
  MOCK_METHOD(bool, OnSingleTapUp, (const MotionEvent&, int), (override));
  MOCK_METHOD(void, OnShortPress, (const MotionEvent&), (override));
  MOCK_METHOD(void, OnLongPress, (const MotionEvent&), (override));
#ifdef OHOS_DRAG_DROP
  MOCK_METHOD(void, OnDragLongPress, (const MotionEvent&), (override));
#endif
#ifdef OHOS_AI
  MOCK_METHOD(void, OnCreateOverlay, (const MotionEvent&), (override));
#endif
  MOCK_METHOD(bool,
              OnScroll,
              (const MotionEvent&,
               const MotionEvent&,
               const MotionEvent&,
               float,
               float),
              (override));
  MOCK_METHOD(bool,
              OnFling,
              (const MotionEvent&, const MotionEvent&, float, float),
              (override));
  MOCK_METHOD(bool,
              OnSwipe,
              (const MotionEvent&, const MotionEvent&, float, float),
              (override));
  MOCK_METHOD(bool,
              OnTwoFingerTap,
              (const MotionEvent&, const MotionEvent&),
              (override));
  MOCK_METHOD(void, OnTapCancel, (const MotionEvent&), (override));
};

class MockDoubleTapListener : public DoubleTapListener {
 public:
  MOCK_METHOD(bool, OnSingleTapConfirmed, (const MotionEvent&), (override));
  MOCK_METHOD(bool, OnDoubleTap, (const MotionEvent&), (override));
  MOCK_METHOD(bool, OnDoubleTapEvent, (const MotionEvent&), (override));
};

class GestureDetectorTest : public testing::Test {
 public:
  void SetUp() override {
    GestureDetector::Config config;
    auto mock_listener = std::make_unique<MockGestureListener>();
    auto mock_doubleTapListener = std::make_unique<MockDoubleTapListener>();
    g_detector = std::make_unique<GestureDetector>(
        config, mock_listener.get(), mock_doubleTapListener.get());
  }
  void TearDown() override { g_detector.reset(); }
  std::unique_ptr<GestureDetector> g_detector = nullptr;
};

TEST_F(GestureDetectorTest, Cancel001) {
#ifdef OHOS_DRAG_DROP
  g_detector->Cancel(true);
  EXPECT_FALSE(g_detector->all_pointers_within_slop_regions_);
  EXPECT_FALSE(g_detector->still_down_);
#endif
}
}  // namespace
}  // namespace ui
