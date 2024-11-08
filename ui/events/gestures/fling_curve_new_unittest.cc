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
#define private public
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/system/sys_info.h"
#include "ui/events/gestures/fling_curve.h"
#undef private
#include "base/logging.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace ui {

class FlingCurveTest1 : public ::testing::Test {
 public:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(FlingCurveTest1, FlingCurveTest1) {
  const gfx::Vector2dF velocity(0, 5000);
  base::TimeTicks now = base::TimeTicks::Now();
  float max_start_velocity = std::max(fabs(velocity.x()), fabs(velocity.y()));
  float max_start = max_start_velocity;
  FlingCurve curve(velocity, now);
  gfx::Vector2dF displacement_ratio_ =
      gfx::Vector2dF(velocity.x() / max_start, velocity.y() / max_start);
  EXPECT_EQ(displacement_ratio_, curve.displacement_ratio_);
}

}  // namespace ui
