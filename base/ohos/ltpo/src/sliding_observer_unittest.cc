// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#define private public

#include "build/build_config.h"
#if BUILDFLAG(IS_OHOS)

#include "adapter_base.h"
#include "base/logging.h"
#include "base/ohos/ltpo/include/sliding_observer.h"
#include "base/trace_event/trace_event.h"
#include "ohos_adapter_helper.h"
#include "ohos_nweb/src/sysevent/event_reporter.h"
#include "system_properties_adapter.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/gesture_detection/gesture_configuration.h"
#include <stdint.h>
#include <vector>

namespace {
const float kMicroSecondPerSecond = 1000000.0;
}

namespace base {
namespace ohos {
using OHOS::NWeb::FrameRateSetting;
class SlidingObserverTest : public SlidingObserver {
public:
  SlidingObserverTest() = default;
  ~SlidingObserverTest() = default;
};

TEST(SlidingObserverTest, Init01) {
  SlidingObserver observer;
  observer.Init();
  // Test case:on_screen_setting_
  auto product_device_type = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                 .GetSystemPropertiesInstance()
                                 .GetProductDeviceType();
  if (product_device_type !=
      OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE) {
    EXPECT_TRUE(observer.on_screen_setting_.size() == 0);
    return;
  }
  auto on_screen_setting = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                               .GetSystemPropertiesInstance()
                               .GetLTPOConfig("scroll");
  ASSERT_EQ(observer.on_screen_setting_.size(), on_screen_setting.size());
}

TEST(SlidingObserverTest, Init02) {
  SlidingObserver observer;
  observer.Init();
  // Test case:off_screen_setting_
  auto product_device_type = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                 .GetSystemPropertiesInstance()
                                 .GetProductDeviceType();
  if (product_device_type !=
      OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE) {
    EXPECT_TRUE(observer.off_screen_setting_.size() == 0);
    return;
  }

  auto off_screen_setting = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                .GetSystemPropertiesInstance()
                                .GetLTPOConfig("fling");
  ASSERT_EQ(observer.off_screen_setting_.size(), off_screen_setting.size());
}

TEST(SlidingObserverTest, Init03) {
  SlidingObserver observer;
  observer.Init();
  // Test case:virtual_pixel_ratio_
  auto product_device_type = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                 .GetSystemPropertiesInstance()
                                 .GetProductDeviceType();
  if (product_device_type !=
      OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE) {
    EXPECT_TRUE(observer.virtual_pixel_ratio_ == 0);
    return;
  }

  auto virtual_pixel_ratio =
      ui::GestureConfiguration::GetInstance()->virtual_pixel_ratio();
  ASSERT_EQ(observer.virtual_pixel_ratio_, virtual_pixel_ratio);
}
TEST(SlidingObserverTest, Init04) {
  SlidingObserver observer;
  observer.Init();
  // Test case:dpi_
  auto product_device_type = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                 .GetSystemPropertiesInstance()
                                 .GetProductDeviceType();
  if (product_device_type !=
      OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE) {
    EXPECT_TRUE(observer.dpi_ == 0);
    return;
  }
  auto display_manager_adapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
  if (!display_manager_adapter) {
    EXPECT_TRUE(observer.dpi_ == 0);
    return;
  }

  std::shared_ptr<OHOS::NWeb::DisplayAdapter> display =
      display_manager_adapter->GetDefaultDisplay();
  if (!display) {
    EXPECT_TRUE(observer.dpi_ == 0);
    return;
  }
  auto dpi = display->GetDpi();
  ASSERT_EQ(observer.dpi_, dpi);
}

TEST(SlidingObserverTest, Init05) {
  SlidingObserver observer;
  observer.Init();
  // Test case:is_inited_
  auto product_device_type = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                 .GetSystemPropertiesInstance()
                                 .GetProductDeviceType();
  if (product_device_type !=
      OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE) {
    EXPECT_FALSE(observer.is_inited_);
    return;
  }
  auto display_manager_adapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
  if (!display_manager_adapter) {
    EXPECT_FALSE(observer.is_inited_);
    return;
  }

  std::shared_ptr<OHOS::NWeb::DisplayAdapter> display =
      display_manager_adapter->GetDefaultDisplay();
  if (!display) {
    EXPECT_FALSE(observer.is_inited_);
    return;
  }

  if (observer.dpi_ <= 0 || observer.virtual_pixel_ratio_ <= 0) {
    EXPECT_FALSE(observer.is_inited_);
    return;
  }
  EXPECT_TRUE(observer.is_inited_);
}

TEST(SlidingObserverTest, StartSliding01) {
  SlidingObserver observer;
  // Test case: when is_inited_ is true and is_sliding_ is false
  observer.is_inited_ = true;
  observer.is_sliding_ = false;

  observer.StartSliding();

  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartSliding02) {
  SlidingObserver observer;
  // Test case: when is_inited_ is true and is_sliding_ is true
  observer.is_inited_ = true;
  observer.is_sliding_ = true;

  observer.StartSliding();

  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartSliding03) {
  SlidingObserver observer;
  // Test case: when is_inited_ is false and is_sliding_ is true
  observer.is_inited_ = false;
  observer.is_sliding_ = true;

  observer.StartSliding();

  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartSliding04) {
  SlidingObserver observer;
  // Test case: when is_inited_ is false and is_sliding_ is false
  observer.is_inited_ = false;
  observer.is_sliding_ = false;

  observer.StartSliding();
  if (!observer.is_inited_) {
    EXPECT_FALSE(observer.is_sliding_);
    EXPECT_FALSE(observer.is_off_screen_);
    return;
  }
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StopSlidingTest01) {
  SlidingObserver observer;
  // Test case: when is_inited_ is true and is_sliding_ is false
  observer.is_sliding_ = false;
  observer.is_inited_ = true;

  int32_t result = observer.StopSliding();
  ASSERT_EQ(observer.current_timestamp_, -1);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
  ASSERT_EQ(observer.sliding_frame_rate_, 0);
  ASSERT_EQ(result, -1);
}

TEST(SlidingObserverTest, StopSlidingTest02) {
  SlidingObserver observer;
  // Test case: when is_inited_ is false and is_sliding_ is true
  observer.is_sliding_ = true;
  observer.is_inited_ = false;

  int32_t result = observer.StopSliding();
  ASSERT_EQ(observer.current_timestamp_, -1);
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
  ASSERT_EQ(observer.sliding_frame_rate_, 0);
  ASSERT_EQ(result, -1);
}

TEST(SlidingObserverTest, StopSlidingTest03) {
  SlidingObserver observer;
  // Test case: when is_inited_ is true and is_sliding_ is true
  observer.is_sliding_ = true;
  observer.is_inited_ = true;

  int32_t result = observer.StopSliding();
  ASSERT_EQ(observer.current_timestamp_, -1);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
  ASSERT_EQ(observer.sliding_frame_rate_, 0);
  ASSERT_EQ(result, 0);
}

TEST(SlidingObserverTest, StopSlidingTest04) {
  SlidingObserver observer;
  // Test case: when is_inited_ is false and is_sliding_ is false
  observer.is_sliding_ = false;
  observer.is_inited_ = false;

  int32_t result = observer.StopSliding();
  ASSERT_EQ(observer.current_timestamp_, -1);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
  ASSERT_EQ(observer.sliding_frame_rate_, 0);
  ASSERT_EQ(result, -1);
}

TEST(SlidingObserverTest, StartFlingTest01) {
  SlidingObserver observer;
  // Test case: when is_sliding_ and is_inited_ are true
  observer.is_sliding_ = true;
  observer.is_inited_ = true;

  observer.StartFling();
  EXPECT_TRUE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartFlingTest02) {
  SlidingObserver observer;
  // Test case: when is_sliding_ is false and is_inited_ is true;
  observer.is_sliding_ = false;
  observer.is_inited_ = true;

  observer.StartFling();
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartFlingTest03) {
  SlidingObserver observer;
  // Test case: when is_inited_ is false and is_sliding_ is true
  observer.is_sliding_ = true;
  observer.is_inited_ = false;

  observer.StartFling();
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartFlingTest04) {
  SlidingObserver observer;
  // Test case: when is_inited_ is false and is_sliding_ is true
  observer.is_sliding_ = false;
  observer.is_inited_ = false;

  observer.StartFling();
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, OnScrollUpdateTest01) {
  SlidingObserver observer;
  // Test case: when is_off_screen_ is true and is_sliding_ is false
  observer.is_sliding_ = false;
  observer.is_off_screen_ = true;
  float delta_x = 10.0f;
  float delta_y = 20.0f;

  int32_t result = observer.OnScrollUpdate(delta_x, delta_y);
  ASSERT_EQ(observer.current_timestamp_, -1);
  ASSERT_EQ(observer.sliding_frame_rate_, 0);
  ASSERT_EQ(result, -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest02) {
  SlidingObserver observer;
  // Test case: when is_off_screen_ is true and is_sliding_ is true
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  float delta_x = 10.0f;
  float delta_y = 20.0f;

  int32_t result = observer.OnScrollUpdate(delta_x, delta_y);
  ASSERT_EQ(observer.current_timestamp_, -1);
  ASSERT_EQ(observer.sliding_frame_rate_, 0);
  ASSERT_EQ(result, -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest03) {
  SlidingObserver observer;
  // Test case: when is_off_screen_ is false and is_sliding_ is true
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  float delta_x = 10.0f;
  float delta_y = 20.0f;

  int32_t result = observer.OnScrollUpdate(delta_x, delta_y);
  ASSERT_EQ(result, observer.sliding_frame_rate_);
  auto current_timestamp = observer.GetCurrentTimestamp();

  float velocity_x = delta_x * kMicroSecondPerSecond /
                     (current_timestamp - observer.GetCurrentTimestamp());
  float velocity_y = delta_y * kMicroSecondPerSecond /
                     (current_timestamp - observer.GetCurrentTimestamp());
  float velocity = observer.GetVelocity(velocity_x, velocity_y);
  auto on_screen_setting = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                               .GetSystemPropertiesInstance()
                               .GetLTPOConfig("scroll");
  int32_t preferred_frame_rate =
      observer.GetPreferedFrameRate(velocity, on_screen_setting);
  ASSERT_EQ(result, preferred_frame_rate);
  ASSERT_EQ(observer.sliding_frame_rate_, preferred_frame_rate);
}

TEST(SlidingObserverTest, OnScrollUpdateTest04) {
  SlidingObserver observer;
  // Test case: when is_off_screen_ is false and is_sliding_ is false
  observer.is_sliding_ = false;
  observer.is_off_screen_ = false;
  float delta_x = 10.0f;
  float delta_y = 20.0f;

  int32_t result = observer.OnScrollUpdate(delta_x, delta_y);
  ASSERT_EQ(observer.current_timestamp_, -1);
  ASSERT_EQ(observer.sliding_frame_rate_, 0);
  ASSERT_EQ(result, -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest01) {
  SlidingObserver observer;
  // Test case: when is_off_screen_ is true and is_sliding_ is false
  observer.is_sliding_ = false;
  observer.is_off_screen_ = true;
  float delta_x = 10.0f;
  float delta_y = 20.0f;

  int32_t result = observer.OnFlingUpdate(delta_x, delta_y);
  ASSERT_EQ(result, -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest02) {
  SlidingObserver observer;
  // Test case: when is_off_screen_ is false and is_sliding_ is true
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  float delta_x = 10.0f;
  float delta_y = 20.0f;

  int32_t result = observer.OnFlingUpdate(delta_x, delta_y);
  ASSERT_EQ(result, -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest03) {
  SlidingObserver observer;
  // Test case: when is_off_screen_ is true and is_sliding_ is true
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  float velocity_x = 10.0f;
  float velocity_y = 20.0f;

  int32_t result = observer.OnFlingUpdate(velocity_x, velocity_y);
  ASSERT_EQ(result, observer.sliding_frame_rate_);
}

TEST(SlidingObserverTest, OnFlingUpdateTest04) {
  SlidingObserver observer;
  // Test case: when is_off_screen_ is false and is_sliding_ is false
  observer.is_sliding_ = false;
  observer.is_off_screen_ = false;
  float delta_x = 10.0f;
  float delta_y = 20.0f;

  int32_t result = observer.OnFlingUpdate(delta_x, delta_y);
  ASSERT_EQ(result, -1);
}

} // namespace ohos
} // namespace base
#endif