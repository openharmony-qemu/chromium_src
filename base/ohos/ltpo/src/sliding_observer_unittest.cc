// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdint>
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

TEST(SlidingObserverTest, GetVelocity001) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = 1.0;
  float test_velocity_y = 2.0;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit = kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = std::sqrt(text_velocity_x * text_velocity_x + test_velocity_y * test_velocity_y);
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit*velocity);
}

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

TEST(SlidingObserverTest, Init06) {
    SlidingObserver observer;
    auto type_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetSystemPropertiesInstance().GetProductDeviceType();
    EXPECT_EQ(type_, OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE);
    observer.Init();
    EXPECT_TRUE(observer.is_inited_);
}

TEST(SlidingObserverTest, Init007) {
    SlidingObserver observer;
    auto type_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetSystemPropertiesInstance().GetProductDeviceType();
    EXPECT_EQ(type_, OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE);
    observer.Init();
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

TEST(SlidingObserverTest, StartSliding05) {
  SlidingObserver observer;
  observer.is_inited_ = false;
  observer.is_sliding_ = true;
  observer.StartSliding();
  EXPECT_TRUE(observer.is_sliding_);
}

TEST(SlidingObserverTest, StartSliding06) {
  SlidingObserver observer;
  observer.is_inited_ = false;
  observer.is_sliding_ = true;
  observer.StartSliding();
  EXPECT_TRUE(observer.is_sliding_);
}

TEST(SlidingObserverTest, StartSliding07) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  observer.StartSliding();
  EXPECT_TRUE(observer.is_inited_);
  EXPECT_TRUE(observer.is_sliding_);
}

TEST(SlidingObserverTest, StartSliding08) {
  SlidingObserver observer;
  observer.is_sliding_ = false;
  observer.is_inited_ = true;
  observer.StartSliding();
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_TRUE(observer.is_inited_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartSliding09) {
  SlidingObserver observer;
  observer.strategy_ = LTPOStrategy::APS_FLING;
  observer.StartSliding();
  EXPECT_FALSE(observer.is_sliding_);
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

TEST(SlidingObserverTest, StopSlidingTest05) {
  SlidingObserver observer;
  observer.is_sliding_ = false;
  observer.is_inited_ = false;
  int32_t result = observer.StopSliding();
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_inited_);
  EXPECT_EQ(result, -1);
}

TEST(SlidingObserverTest, StopSlidingTest06) {
  SlidingObserver observer;
  observer.is_sliding_ = false;
  observer.is_inited_ = true;
  int32_t result = observer.StopSliding();
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_TRUE(observer.is_inited_);
  EXPECT_EQ(result, -1);
}

TEST(SlidingObserverTest, StopSlidingTest07) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_inited_ = false;
  int32_t result = observer.StopSliding();
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_inited_);
  EXPECT_EQ(result, -1);
}

TEST(SlidingObserverTest, StopSlidingTest08) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_inited_ = true;
  int32_t result = observer.StopSliding();
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_TRUE(observer.is_inited_);
  EXPECT_EQ(observer.current_timestamp_, -1);
  EXPECT_EQ(result, 0);
}

TEST(SlidingObserverTest, StopSlidingTest09) {
  SlidingObserver observer;
  observer.strategy_ = LTPOStrategy::APS_FLING;
  observer.is_off_screen_ = false;
  int32_t result = observer.StopSliding();
  EXPECT_FALSE(observer.is_off_screen_);
  EXPECT_EQ(result, -1);
}

TEST(SlidingObserverTest, StopSlidingTest10) {
  SlidingObserver observer;
  observer.strategy_ = LTPOStrategy::APS_FLING;
  observer.is_off_screen_ = true;
  int32_t result = observer.StopSliding();
  EXPECT_FALSE(observer.is_off_screen_);
  EXPECT_EQ(result, -1);
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

TEST(SlidingObserverTest, StartFlingTest05) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_inited_ = true;
  observer.StartFling();
  EXPECT_TRUE(observer.is_off_screen_);
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_TRUE(observer.is_inited_);
}

TEST(SlidingObserverTest, StartFlingTest06) {
  SlidingObserver observer;
  observer.is_sliding_ = false;
  observer.is_inited_ = true;
  observer.StartFling();
  EXPECT_FALSE(observer.is_off_screen_);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_TRUE(observer.is_inited_);
}

TEST(SlidingObserverTest, StartFlingTest07) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_inited_ = false;
  observer.StartFling();
  EXPECT_FALSE(observer.is_off_screen_);
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_inited_);
}

TEST(SlidingObserverTest, StartFlingTest08) {
  SlidingObserver observer;
  observer.is_sliding_ = false;
  observer.is_inited_ = false;
  observer.StartFling();
  EXPECT_FALSE(observer.is_off_screen_);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_inited_);
}

TEST(SlidingObserverTest, StartFlingTest09) {
  SlidingObserver observer;
  observer.StartFling();
  EXPECT_TRUE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartFlingTest10) {
  SlidingObserver observer;
  observer.StartFling();
  EXPECT_TRUE(observer.is_off_screen_);
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

TEST(SlidingObserverTest, OnScrollUpdateTest001) {
  SlidingObserver observer;
  observer.is_sliding_ = false, observer.is_off_screen_ = true,
  observer.strategy_ = LTPOStrategy::APS_FLING;
  EXPECT_EQ(observer.OnScrollUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest002) {
  SlidingObserver observer;
  observer.is_sliding_ = false, observer.is_off_screen_ = false,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  EXPECT_EQ(observer.OnScrollUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest003) {
  SlidingObserver observer;
  observer.is_sliding_ = false, observer.is_off_screen_ = true,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  EXPECT_EQ(observer.OnScrollUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest004) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = true,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  EXPECT_EQ(observer.OnScrollUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest005) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = false,
  observer.strategy_ = LTPOStrategy::APS_FLING;
  EXPECT_EQ(observer.OnScrollUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest006) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = false,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 120;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 30},
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(20, 20), 30);
}

TEST(SlidingObserverTest, OnScrollUpdateTest007) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = false,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 120;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 30},
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(100, 50), 30);
}

TEST(SlidingObserverTest, OnScrollUpdateTest008) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = false,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 60;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 60},
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(100, 50), -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest001) {
  SlidingObserver observer;
  observer.is_sliding_ = false, observer.is_off_screen_ = false,
  observer.strategy_ = LTPOStrategy::APS_FLING;
  EXPECT_EQ(observer.OnFlingUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest002) {
  SlidingObserver observer;
  observer.is_sliding_ = false, observer.is_off_screen_ = true,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  EXPECT_EQ(observer.OnFlingUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest003) {
  SlidingObserver observer;
  observer.is_sliding_ = false, observer.is_off_screen_ = false,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  EXPECT_EQ(observer.OnFlingUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest004) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = false,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  EXPECT_EQ(observer.OnFlingUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest005) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = true,
  observer.strategy_ = LTPOStrategy::APS_FLING;
  EXPECT_EQ(observer.OnFlingUpdate(20, 20), -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest006) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = true,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 120;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 30},
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(100, 50), 30);
}

TEST(SlidingObserverTest, OnFlingUpdateTest007) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = true,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 120;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 30},
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(100, 50), 30);
}

TEST(SlidingObserverTest, OnFlingUpdateTest008) {
  SlidingObserver observer;
  observer.is_sliding_ = true, observer.is_off_screen_ = true,
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 60;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 60},
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(100, 50), -1);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest001) {
  SlidingObserver observer;
  float velocity_x = 10.0f;
  std::vector<OHOS::NWeb::FrameRateSetting> settings;
  EXPECT_EQ(observer.GetPreferedFrameRate(velocity_x, settings), 120);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest002) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 30}};
  testing::internal::CaptureStderr();
  int32_t frameRate = observer.GetPreferedFrameRate(15.0f, settings);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("can not find proper prefered frame rate"),
            std::string::npos);
  EXPECT_EQ(frameRate, 120);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest003) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings;
  testing::internal::CaptureStderr();
  int32_t frameRate = observer.GetPreferedFrameRate(15.0f, settings);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(frameRate, 120);
  EXPECT_EQ(log_output.find("can not find proper prefered frame rate"),
            std::string::npos);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest004) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{20.0f, 30.0f, 60},
                                                        {40.0f, 50.0f, 90}};
  EXPECT_EQ(observer.GetPreferedFrameRate(35.0f, settings), 120);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest005) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 30}};
  EXPECT_EQ(observer.GetPreferedFrameRate(5.0f, settings), 30);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest006) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{20.0f, 30.0f, 60}};
  testing::internal::CaptureStderr();
  int32_t frameRate = observer.GetPreferedFrameRate(15.0f, settings);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("can not find proper prefered frame rate"),
            std::string::npos);
  EXPECT_EQ(frameRate, 120);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest007) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{10.0f, 30.0f, 30}};
  EXPECT_EQ(observer.GetPreferedFrameRate(15.0f, settings), 30);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest008) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{10.0f, 30.0f, 60}};
  testing::internal::CaptureStderr();
  int32_t frameRate = observer.GetPreferedFrameRate(40.0f, settings);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("can not find proper prefered frame rate"),
            std::string::npos);
  EXPECT_EQ(frameRate, 120);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest009) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{10.0f, -30.0f, 30}};
  EXPECT_EQ(observer.GetPreferedFrameRate(15.0f, settings), 30);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest010) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{20.0f, 40.0f, 30}};
  EXPECT_EQ(observer.GetPreferedFrameRate(25.0f, settings), 30);
}

}  // namespace ohos
}  // namespace base
#endif
