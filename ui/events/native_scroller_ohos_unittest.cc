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

#define private public
#include "ui/events/native_scroller_ohos.h"
#undef private

#include <stddef.h>
#include <cmath>
#include <memory>

#include "gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "ui/events/gesture_detection/gesture_configuration.h"
#include "ui/events/native_scroller_ohos.cc"

namespace ui {

const float value_float_1 = -1.0f;
const float value_float_2 = 2.0f;

class NativeScrollerOhosTest : public testing::Test {
 public:
  void SetUp() {
    native_scroller_shared = std::make_shared<NativeScrollerOhos>();
  }

  void TearDown() { native_scroller_shared.reset(); }

  std::shared_ptr<NativeScrollerOhos> native_scroller_shared;
  gfx::Vector2dF offset_;
  gfx::Vector2dF velocity_;
  base::TimeTicks test_time_;
  base::TimeTicks start_time_;
};

TEST_F(NativeScrollerOhosTest, Fling_001) {
  const float start_x = 0.0f;
  const float start_y = 100.0f;
  const float velocity_x = 0.0f;
  const float velocity_y = -10.0f;
  const float min_x = 0.0f;
  const float max_x = 100.0f;
  const float min_y = 0.0f;
  const float max_y = 200.0f;
  float pixel_ratio = 0.0001;
  float value_threshold = kDefaultThreshold * kDefaultMultiplier;
  native_scroller_shared->velocity_threshold_ =
      NearZero(pixel_ratio) ? value_threshold : (value_threshold / pixel_ratio);
  native_scroller_shared->Fling(start_x, start_y, velocity_x, velocity_y, min_x,
                                max_x, min_y, max_y, start_time_);
  ASSERT_FALSE(native_scroller_shared->finisheed_);
}

TEST_F(NativeScrollerOhosTest, Fling_002) {
  const float start_x = 0.0f;
  const float start_y = 100.0f;
  const float velocity_x = 0.0f;
  const float velocity_y = -0.0001f;
  const float min_x = 0.0f;
  const float max_x = 100.0f;
  const float min_y = 0.0f;
  const float max_y = 200.0f;
  float pixel_ratio = 0.0001;
  float value_threshold = kDefaultThreshold * kDefaultMultiplier;
  native_scroller_shared->velocity_threshold_ =
      NearZero(pixel_ratio) ? value_threshold : (value_threshold / pixel_ratio);
  native_scroller_shared->Fling(start_x, start_y, velocity_x, velocity_y, min_x,
                                max_x, min_y, max_y, start_time_);
  ASSERT_FALSE(native_scroller_shared->finisheed_);
}

TEST_F(NativeScrollerOhosTest, Fling_003) {
  const float start_x = 0.0f;
  const float start_y = 100.0f;
  const float velocity_x = 0.0f;
  const float velocity_y = 10.0f;
  const float min_x = 0.0f;
  const float max_x = 100.0f;
  const float min_y = 0.0f;
  const float max_y = 200.0f;
  float pixel_ratio = 0.0001;
  float value_threshold = kDefaultThreshold * kDefaultMultiplier;
  native_scroller_shared->velocity_threshold_ =
      NearZero(pixel_ratio) ? value_threshold : (value_threshold / pixel_ratio);
  native_scroller_shared->Fling(start_x, start_y, velocity_x, velocity_y, min_x,
                                max_x, min_y, max_y, start_time_);
  ASSERT_FALSE(native_scroller_shared->finisheed_);
}

TEST_F(NativeScrollerOhosTest, ComputeScrollOffset001) {
  auto result = native_scroller_shared->ComputeScrollOffset(
      test_time_, &offset_, &velocity_);
  EXPECT_EQ(false, result);
  EXPECT_EQ(true, native_scroller_shared->finisheed_);
}

TEST_F(NativeScrollerOhosTest, ComputeScrollOffset002) {
  native_scroller_shared->finisheed_ = false;
  native_scroller_shared->position_threshold_ = value_float_1;
  native_scroller_shared->velocity_threshold_ = value_float_1;
  auto result = native_scroller_shared->ComputeScrollOffset(
      test_time_, &offset_, &velocity_);
  EXPECT_EQ(true, result);
  EXPECT_EQ(false, native_scroller_shared->finisheed_);
}

TEST_F(NativeScrollerOhosTest, ComputeScrollOffset003) {
  native_scroller_shared->finisheed_ = false;
  native_scroller_shared->position_threshold_ = value_float_2;
  native_scroller_shared->velocity_threshold_ = value_float_2;
  auto result = native_scroller_shared->ComputeScrollOffset(
      test_time_, &offset_, &velocity_);
  EXPECT_EQ(false, result);
  EXPECT_EQ(true, native_scroller_shared->finisheed_);
}

TEST_F(NativeScrollerOhosTest, ShouldAbortAnimation001) {
  native_scroller_shared->position_threshold_ = value_float_2;
  native_scroller_shared->velocity_threshold_ = value_float_2;
  auto result = native_scroller_shared->ShouldAbortAnimation();
  EXPECT_EQ(true, result);
}

TEST_F(NativeScrollerOhosTest, ShouldAbortAnimation002) {
  native_scroller_shared->position_threshold_ = value_float_1;
  native_scroller_shared->velocity_threshold_ = value_float_1;
  auto result = native_scroller_shared->ShouldAbortAnimation();
  EXPECT_EQ(false, result);
}

TEST_F(NativeScrollerOhosTest, ShouldAbortAnimation003) {
  native_scroller_shared->position_threshold_ = value_float_1;
  native_scroller_shared->velocity_threshold_ = value_float_2;
  auto result = native_scroller_shared->ShouldAbortAnimation();
  EXPECT_EQ(true, result);
}

TEST_F(NativeScrollerOhosTest, ShouldAbortAnimation004) {
  native_scroller_shared->position_threshold_ = value_float_2;
  native_scroller_shared->velocity_threshold_ = value_float_1;
  auto result = native_scroller_shared->ShouldAbortAnimation();
  EXPECT_EQ(true, result);
}

}  // namespace ui
