// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/gestures/fling_curve.h"

#include "testing/gtest/include/gtest/gtest.h"

#if BUILDFLAG(IS_OHOS)
#include "base/system/sys_info.h"
#endif // BUILDFLAG(IS_OHOS)

namespace ui {

TEST(FlingCurveTest, Basic) {
  const gfx::Vector2dF velocity(0, 5000);
  base::TimeTicks now = base::TimeTicks::Now();
  FlingCurve curve(velocity, now);

  gfx::Vector2dF delta;
  EXPECT_TRUE(
      curve.ComputeScrollDeltaAtTime(now + base::Milliseconds(20), &delta));
  EXPECT_EQ(0, delta.x());
  EXPECT_NEAR(delta.y(), 96, 1);

  EXPECT_TRUE(
      curve.ComputeScrollDeltaAtTime(now + base::Milliseconds(250), &delta));
  EXPECT_EQ(0, delta.x());
#if BUILDFLAG(IS_OHOS)
  if (base::SysInfo::IsLowEndDevice()) {
    EXPECT_NEAR(delta.y(), 675, 1);
  } else {
    EXPECT_NEAR(delta.y(), 705, 1);
  }
#else
  EXPECT_NEAR(delta.y(), 705, 1);
#endif // BUILDFLAG(IS_OHOS)

  EXPECT_FALSE(curve.ComputeScrollDeltaAtTime(now + base::Seconds(10), &delta));
  EXPECT_EQ(0, delta.x());
#if BUILDFLAG(IS_OHOS)
  if (base::SysInfo::IsLowEndDevice()) {
    EXPECT_NEAR(delta.y(), 250, 1);
  } else {
    EXPECT_NEAR(delta.y(), 392, 1);
  }
#else
  EXPECT_NEAR(delta.y(), 392, 1);
#endif // BUILDFLAG(IS_OHOS)

  EXPECT_FALSE(curve.ComputeScrollDeltaAtTime(now + base::Seconds(20), &delta));
  EXPECT_TRUE(delta.IsZero());
}

}  // namespace ui
