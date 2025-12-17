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

#include "arkweb/chromium_ext/ui/events/blink/fling_booster_utils.h"
#if BUILDFLAG(ARKWEB_PDF)
#include "base/ohos/ltpo/include/sliding_observer.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_sys_info_util_ext.h"
#endif

namespace ui {
namespace test {

constexpr float kMaxBoostFlingSpeed = 9000;
constexpr float kMaxBoostFlingSpeedPdf = 5000;
using testing::Return;
TEST_F(FlingBoosterTest, LimitVelocityPDF) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  auto& system_properties_mock = ::base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsPcDevice())
      .WillOnce(Return(false))
      .WillRepeatedly(Return(false));
  Vector2dF max_fling_velocity{std::numeric_limits<float>::max(),
                               std::numeric_limits<float>::max()};
  Vector2dF fling_velocity{10000, 10000};
  base::ohos::SlidingObserver::GetInstance().SetIsPdf(true);
  LimitVelocity(fling_velocity, max_fling_velocity);
  EXPECT_LE(fling_velocity.y(), kMaxBoostFlingSpeedPdf);
  fling_velocity.set_x(-10000);
  fling_velocity.set_y(-10000);
  LimitVelocity(fling_velocity, max_fling_velocity);
  EXPECT_GE(fling_velocity.y(), -kMaxBoostFlingSpeedPdf);
  fling_velocity.set_x(kMaxBoostFlingSpeedPdf);
  fling_velocity.set_y(kMaxBoostFlingSpeedPdf);
  LimitVelocity(fling_velocity, max_fling_velocity);
  EXPECT_EQ(fling_velocity.y(), kMaxBoostFlingSpeedPdf);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
}

TEST_F(FlingBoosterTest, LimitVelocityForPC) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  auto& system_properties_mock = ::base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsPcDevice())
      .WillOnce(Return(true))
      .WillRepeatedly(Return(true));
  Vector2dF max_fling_velocity{std::numeric_limits<float>::max(),
                               std::numeric_limits<float>::max()};
  Vector2dF fling_velocity{10000, 10000};
  LimitVelocity(fling_velocity, max_fling_velocity);
  EXPECT_LE(fling_velocity.y(), kMaxBoostFlingSpeed);
  fling_velocity.set_x(-10000);
  fling_velocity.set_y(-10000);
  LimitVelocity(fling_velocity, max_fling_velocity);
  EXPECT_GE(fling_velocity.y(), -kMaxBoostFlingSpeed);
  fling_velocity.set_x(kMaxBoostFlingSpeed);
  fling_velocity.set_y(kMaxBoostFlingSpeed);
  LimitVelocity(fling_velocity, max_fling_velocity);
  EXPECT_EQ(fling_velocity.y(), kMaxBoostFlingSpeed);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
}

TEST_F(FlingBoosterTest, LimitFlingVelocity_PositiveVelocity) {
  Vector2dF max_fling_velocity_{5000, 5000};
  Vector2dF velocity{10000, 10000};
  EXPECT_NE(velocity, max_fling_velocity_);
  ShouldLimitFlingVelocity(velocity, max_fling_velocity_);
  EXPECT_EQ(velocity, max_fling_velocity_);
}

TEST_F(FlingBoosterTest, LimitFlingVelocity_NegativeVelocity) {
  Vector2dF max_fling_velocity_{5000, 5000};
  Vector2dF velocity{-10000, -10000};
  Vector2dF target{-5000, -5000};
  EXPECT_NE(velocity, max_fling_velocity_);
  ShouldLimitFlingVelocity(velocity, max_fling_velocity_);
  EXPECT_EQ(velocity, target);
}

TEST_F(FlingBoosterTest, LimitFlingVelocity_UnderVelocity) {
  Vector2dF max_fling_velocity_{5000, 5000};
  Vector2dF velocity{100, 500};
  EXPECT_NE(velocity, max_fling_velocity_);
  ShouldLimitFlingVelocity(velocity, max_fling_velocity_);
  EXPECT_NE(velocity, max_fling_velocity_);
}
}  // namespace test
}  // namespace ui