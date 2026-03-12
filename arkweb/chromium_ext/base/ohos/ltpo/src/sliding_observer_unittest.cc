// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdint>
#include <vector>
#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#include "base/features.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/display_manager_adapter.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#define private public
#include "base/ohos/ltpo/include/sliding_observer.h"
#undef private
#include "build/build_config.h"
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)

#include "adapter_base.h"
#include "base/trace_event/trace_event.h"
#include "ohos_nweb/src/sysevent/event_reporter.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/gesture_detection/gesture_configuration.h"
#include "gmock/gmock.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_system_properties_adapter.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_sys_info_util_ext.h"

using namespace OHOS::NWeb;

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

TEST(SlidingObserverTest, SlidingObserver001) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  MockSystemPropertiesAdapter adapter;

  EXPECT_CALL(*instance, GetSystemPropertiesInstance()).WillOnce(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, GetLTPOStrategy()).WillOnce(testing::Return(1));
  SlidingObserver observer;
  EXPECT_EQ(observer.strategy_, LTPOStrategy::APS_FLING);
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST(SlidingObserverTest, SlidingObserver002) {
  OhosAdapterHelper& original_instance = OhosAdapterHelper::GetInstance();
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  MockSystemPropertiesAdapter adapter;

  EXPECT_CALL(*instance, GetSystemPropertiesInstance()).WillOnce(testing::ReturnRef(adapter))
      .WillRepeatedly(testing::Invoke(&original_instance,
      &OhosAdapterHelper::GetSystemPropertiesInstance));
  EXPECT_CALL(adapter, GetLTPOStrategy()).WillOnce(testing::Return(2));
  SlidingObserver observer;
  EXPECT_EQ(observer.strategy_, LTPOStrategy::HGM_FLING);
  EXPECT_TRUE(observer.is_inited_);
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST(SlidingObserverTest, GetVelocity001) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = 1.0;
  float test_velocity_y = 2.0;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit =
      kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = std::sqrt(text_velocity_x * text_velocity_x +
                             test_velocity_y * test_velocity_y);
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit * velocity);
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
  observer.strategy_ = LTPOStrategy::HGM_FLING;
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
  observer.is_off_screen_ = false;
  int32_t result = observer.StopSliding();
  EXPECT_FALSE(observer.is_off_screen_);
  EXPECT_EQ(result, -1);
}

TEST(SlidingObserverTest, StopSlidingTest10) {
  SlidingObserver observer;
  observer.is_off_screen_ = true;
  int32_t result = observer.StopSliding();
  EXPECT_TRUE(observer.is_off_screen_);
  EXPECT_EQ(result, -1);
}

TEST(SlidingObserverTest, StopSlidingTest11) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_inited_ = true;
  observer.is_off_screen_ = true;
  int32_t result = observer.StopSliding();
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
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartFlingTest10) {
  SlidingObserver observer;
  observer.StartFling();
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartFlingTest11) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  observer.strategy_ = LTPOStrategy::APS_FLING;
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
  EXPECT_NE(log_output.find("can not find proper prfered frame rate"),
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
  EXPECT_NE(log_output.find("can not find proper prfered frame rate"),
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
  EXPECT_NE(log_output.find("can not find proper prfered frame rate"),
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

TEST(SlidingObserverTest, GetPreferedFrameRateTest011) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings;
  observer.use_pdf_rate_ = true;
  EXPECT_EQ(observer.GetPreferedFrameRate(25.0f, settings), 90);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest012) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{20.0f, 40.0f, 100}};
  observer.use_pdf_rate_ = true;
  EXPECT_EQ(observer.GetPreferedFrameRate(25.0f, settings), 90);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest013) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{-40.0f, -20.0f, -50}};
  EXPECT_EQ(observer.GetPreferedFrameRate(5.0f, settings), -50);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest014) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{20.0f, 40.0f, 50}};
  EXPECT_EQ(observer.GetPreferedFrameRate(5.0f, settings), 120);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest016) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{20.0f, 22.0f, 50}};
  observer.use_pdf_rate_ = true;
  EXPECT_EQ(observer.GetPreferedFrameRate(25.0f, settings), 90);
}

TEST(SlidingObserverTest, OnDisplayInfoChange001) {
  SlidingObserver observer;
  observer.is_inited_ = false;
  observer.OnDisplayInfoChange();
  EXPECT_EQ(observer.virtual_pixel_ratio_, -1);
}

TEST(SlidingObserverTest, OnDisplayInfoChange002) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.OnDisplayInfoChange();
  EXPECT_NE(observer.virtual_pixel_ratio_, -1);
}

TEST(SlidingObserverTest, OnDisplayInfoChange003) {
  SlidingObserver observer;
  observer.is_inited_ = false;
  observer.dpi_ = -1;
  observer.virtual_pixel_ratio_ = -1;

  observer.OnDisplayInfoChange();
  EXPECT_EQ(observer.dpi_, -1);
  EXPECT_EQ(observer.virtual_pixel_ratio_, -1);
}

TEST(SlidingObserverTest, OnDisplayInfoChange004) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.dpi_ = -1;
  observer.virtual_pixel_ratio_ = -1;

  observer.OnDisplayInfoChange();
  EXPECT_NE(observer.dpi_, -1);
  EXPECT_NE(observer.virtual_pixel_ratio_, -1);
}

TEST(SlidingObserverTest, OnDisplayInfoChange005) {
  SlidingObserver observer;
  observer.is_inited_ = false;
  observer.dpi_ = 100;
  observer.virtual_pixel_ratio_ = 2.0;

  // When not initialized, values should not change
  observer.OnDisplayInfoChange();
  EXPECT_EQ(observer.dpi_, 100);
  EXPECT_EQ(observer.virtual_pixel_ratio_, 2.0);
}

TEST(SlidingObserverTest, OnDisplayInfoChange006) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.dpi_ = 100;
  observer.virtual_pixel_ratio_ = 2.0;

  // When initialized, values should be updated to display info
  observer.OnDisplayInfoChange();
  // Values should be updated from display info
  EXPECT_GT(observer.dpi_, 0);
  EXPECT_GT(observer.virtual_pixel_ratio_, 0);
}

TEST(SlidingObserverTest, SetIsPdf001) {
  SlidingObserver observer;
  observer.SetIsPdf(false);
  EXPECT_FALSE(observer.use_pdf_rate_);
}

TEST(SlidingObserverTest, SetIsPdf002) {
  SlidingObserver observer;
  observer.SetIsPdf(true);
  EXPECT_TRUE(observer.use_pdf_rate_);
}

TEST(SlidingObserverTest, SetIsPdf003) {
  SysInfoUtilsMock::mockIsPcDevice = true;
  auto& mock = SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(mock, IsPcDevice()).WillOnce(testing::Return(true));
  SlidingObserver observer;
  observer.use_pdf_rate_ = true;
  observer.SetIsPdf(true);
  EXPECT_FALSE(observer.use_pdf_rate_);
  SysInfoUtilsMock::mockIsPcDevice = false;
}

TEST(SlidingObserverTest, StopFling001) {
  SlidingObserver observer;
  EXPECT_EQ(observer.StopFling(), -1);
}

TEST(SlidingObserverTest, StopFling002) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  EXPECT_EQ(observer.StopFling(), -1);
}

TEST(SlidingObserverTest, StopFling003) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  EXPECT_EQ(observer.StopFling(), -1);
}

TEST(SlidingObserverTest, StopFling004) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  EXPECT_EQ(observer.StopFling(), 0);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StopFling005) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::APS_FLING;
  EXPECT_EQ(observer.StopFling(), 0);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StopFling006) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  EXPECT_EQ(observer.StopFling(), 0);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StopFling007) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::ALL;
  EXPECT_EQ(observer.StopFling(), 0);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StopFling008) {
  SlidingObserver observer;
  observer.is_inited_ = false;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::APS_FLING;
  EXPECT_EQ(observer.StopFling(), -1);
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_TRUE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StopFling009) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = false;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::APS_FLING;
  EXPECT_EQ(observer.StopFling(), -1);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_TRUE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StopFling010) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::APS_FLING;
  EXPECT_EQ(observer.StopFling(), -1);
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, GetVelocity002) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = 0.0f;
  float test_velocity_y = 0.0f;
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, 0.0f);
}

TEST(SlidingObserverTest, GetVelocity003) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = -10.0f;
  float test_velocity_y = -20.0f;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit =
      kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = std::sqrt(text_velocity_x * text_velocity_x +
                            test_velocity_y * test_velocity_y);
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit * velocity);
}

TEST(SlidingObserverTest, GetVelocity004) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = 100.0f;
  float test_velocity_y = 200.0f;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit =
      kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = std::sqrt(text_velocity_x * text_velocity_x +
                            test_velocity_y * test_velocity_y);
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit * velocity);
}

TEST(SlidingObserverTest, GetVelocity005) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = 0.5f;
  float test_velocity_y = 0.5f;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit =
      kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = std::sqrt(text_velocity_x * text_velocity_x +
                            test_velocity_y * test_velocity_y);
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit * velocity);
}

TEST(SlidingObserverTest, GetVelocity006) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = 1.0f;
  float test_velocity_y = 0.0f;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit =
      kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = std::sqrt(text_velocity_x * text_velocity_x +
                            test_velocity_y * test_velocity_y);
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit * velocity);
}

TEST(SlidingObserverTest, GetVelocity007) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = 0.0f;
  float test_velocity_y = 1.0f;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit =
      kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = std::sqrt(text_velocity_x * text_velocity_x +
                            test_velocity_y * test_velocity_y);
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit * velocity);
}

TEST(SlidingObserverTest, OnScrollUpdateTest009) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 30;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 30},
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(10, 10), -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest010) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 90;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 90},
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(50, 50), -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest011) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 120;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 120},
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(100, 100), -1);
}

TEST(SlidingObserverTest, OnScrollUpdateTest012) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 30;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 1000.0f, 30},
      {1000.0f, 3000.0f, 60},
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(10, 10), 30);
}

TEST(SlidingObserverTest, OnScrollUpdateTest013) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 60;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 1000.0f, 30},
      {1000.0f, 3000.0f, 60},
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(100, 100), 60);
}

TEST(SlidingObserverTest, OnFlingUpdateTest009) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 30;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 30},
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(10, 10), -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest010) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 90;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 90},
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(50, 50), -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest011) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 120;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 3000.0f, 120},
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(100, 100), -1);
}

TEST(SlidingObserverTest, OnFlingUpdateTest012) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 30;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 1000.0f, 30},
      {1000.0f, 3000.0f, 60},
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(10, 10), 30);
}

TEST(SlidingObserverTest, OnFlingUpdateTest013) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 60;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
      {0.0f, 1000.0f, 30},
      {1000.0f, 3000.0f, 60},
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(100, 100), 60);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest017) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 30},
                                                        {10.0f, 20.0f, 60},
                                                        {20.0f, -1.0f, 90}};
  EXPECT_EQ(observer.GetPreferedFrameRate(5.0f, settings), 30);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest018) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 30},
                                                        {10.0f, 20.0f, 60},
                                                        {20.0f, -1.0f, 90}};
  EXPECT_EQ(observer.GetPreferedFrameRate(15.0f, settings), 60);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest019) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 30},
                                                        {10.0f, 20.0f, 60},
                                                        {20.0f, -1.0f, 90}};
  EXPECT_EQ(observer.GetPreferedFrameRate(25.0f, settings), 90);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest020) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 30},
                                                        {10.0f, 20.0f, 60},
                                                        {20.0f, 30.0f, 90}};
  testing::internal::CaptureStderr();
  int32_t frameRate = observer.GetPreferedFrameRate(35.0f, settings);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("can not find proper prfered frame rate"),
            std::string::npos);
  EXPECT_EQ(frameRate, 120);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest021) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{5.0f, 15.0f, 30}};
  EXPECT_EQ(observer.GetPreferedFrameRate(10.0f, settings), 30);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest022) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{5.0f, 15.0f, 30}};
  testing::internal::CaptureStderr();
  int32_t frameRate = observer.GetPreferedFrameRate(4.0f, settings);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("can not find proper prfered frame rate"),
            std::string::npos);
  EXPECT_EQ(frameRate, 120);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest023) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{5.0f, 15.0f, 30}};
  testing::internal::CaptureStderr();
  int32_t frameRate = observer.GetPreferedFrameRate(16.0f, settings);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("can not find proper prfered frame rate"),
            std::string::npos);
  EXPECT_EQ(frameRate, 120);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest024) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 100.0f, 120}};
  observer.use_pdf_rate_ = true;
  EXPECT_EQ(observer.GetPreferedFrameRate(50.0f, settings), 90);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest025) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 100.0f, 80}};
  observer.use_pdf_rate_ = true;
  EXPECT_EQ(observer.GetPreferedFrameRate(50.0f, settings), 80);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest026) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 100.0f, 100}};
  observer.use_pdf_rate_ = true;
  EXPECT_EQ(observer.GetPreferedFrameRate(50.0f, settings), 90);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest027) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, -1.0f, 30}};
  EXPECT_EQ(observer.GetPreferedFrameRate(1000.0f, settings), 30);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest028) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, -1.0f, 60}};
  EXPECT_EQ(observer.GetPreferedFrameRate(5000.0f, settings), 60);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest029) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 24},
                                                        {10.0f, 30.0f, 30},
                                                        {30.0f, 60.0f, 60},
                                                        {60.0f, -1.0f, 120}};
  EXPECT_EQ(observer.GetPreferedFrameRate(5.0f, settings), 24);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest030) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 24},
                                                        {10.0f, 30.0f, 30},
                                                        {30.0f, 60.0f, 60},
                                                        {60.0f, -1.0f, 120}};
  EXPECT_EQ(observer.GetPreferedFrameRate(20.0f, settings), 30);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest031) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 24},
                                                        {10.0f, 30.0f, 30},
                                                        {30.0f, 60.0f, 60},
                                                        {60.0f, -1.0f, 120}};
  EXPECT_EQ(observer.GetPreferedFrameRate(45.0f, settings), 60);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest032) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 10.0f, 24},
                                                        {10.0f, 30.0f, 30},
                                                        {30.0f, 60.0f, 60},
                                                        {60.0f, -1.0f, 120}};
  EXPECT_EQ(observer.GetPreferedFrameRate(100.0f, settings), 120);
}

TEST(SlidingObserverTest, OnScrollUpdateTest014) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 24;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
    {0.0f, 10.0f, 24},
    {10.0f, 30.0f, 30},
    {30.0f, 60.0f, 60},
    {60.0f, -1.0f, 120}
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(5, 5), 24);
}

TEST(SlidingObserverTest, OnScrollUpdateTest015) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 30;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
    {0.0f, 10.0f, 24},
    {10.0f, 30.0f, 30},
    {30.0f, 60.0f, 60},
    {60.0f, -1.0f, 120}
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(20, 20), 30);
}

TEST(SlidingObserverTest, OnScrollUpdateTest016) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 60;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
    {0.0f, 10.0f, 24},
    {10.0f, 30.0f, 30},
    {30.0f, 60.0f, 60},
    {60.0f, -1.0f, 120}
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(50, 50), 60);
}

TEST(SlidingObserverTest, OnScrollUpdateTest017) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = false;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 120;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
    {0.0f, 10.0f, 24},
    {10.0f, 30.0f, 30},
    {30.0f, 60.0f, 60},
    {60.0f, -1.0f, 120}
  };
  observer.on_screen_setting_ = settings;
  EXPECT_EQ(observer.OnScrollUpdate(100, 100), 120);
}

TEST(SlidingObserverTest, OnFlingUpdateTest014) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 24;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
    {0.0f, 10.0f, 24},
    {10.0f, 30.0f, 30},
    {30.0f, 60.0f, 60},
    {60.0f, -1.0f, 120}
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(5, 5), 24);
}

TEST(SlidingObserverTest, OnFlingUpdateTest015) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 30;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
    {0.0f, 10.0f, 24},
    {10.0f, 30.0f, 30},
    {30.0f, 60.0f, 60},
    {60.0f, -1.0f, 120}
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(20, 20), 30);
}

TEST(SlidingObserverTest, OnFlingUpdateTest016) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 60;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
    {0.0f, 10.0f, 24},
    {10.0f, 30.0f, 30},
    {30.0f, 60.0f, 60},
    {60.0f, -1.0f, 120}
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(50, 50), 60);
}

TEST(SlidingObserverTest, OnFlingUpdateTest017) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_off_screen_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.sliding_frame_rate_ = 120;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {
    {0.0f, 10.0f, 24},
    {10.0f, 30.0f, 30},
    {30.0f, 60.0f, 60},
    {60.0f, -1.0f, 120}
  };
  observer.off_screen_setting_ = settings;
  EXPECT_EQ(observer.OnFlingUpdate(100, 100), 120);
}

TEST(SlidingObserverTest, StartFlingTest12) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.StartFling();
  EXPECT_TRUE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartFlingTest13) {
  SlidingObserver observer;
  observer.is_inited_ = true;
  observer.is_sliding_ = true;
  observer.strategy_ = LTPOStrategy::ALL;
  observer.StartFling();
  EXPECT_TRUE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartSliding10) {
  SlidingObserver observer;
  observer.strategy_ = LTPOStrategy::HGM_FLING;
  observer.is_inited_ = true;
  observer.is_sliding_ = false;
  observer.StartSliding();
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, StartSliding11) {
  SlidingObserver observer;
  observer.strategy_ = LTPOStrategy::ALL;
  observer.is_inited_ = true;
  observer.is_sliding_ = false;
  observer.StartSliding();
  EXPECT_TRUE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
}

TEST(SlidingObserverTest, GetVelocity008) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = -1.0f;
  float test_velocity_y = 1.0f;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit =
      kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = {text_velocity_x * text_velocity_x +
                            test_velocity_y * test_velocity_y};
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit * velocity);
}

TEST(SlidingObserverTest, GetVelocity009) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = 50.0f;
  float test_velocity_y = -50.0f;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit =
      kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = {text_velocity_x * text_velocity_x +
                            test_velocity_y * test_velocity_y};
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit * velocity);
}

TEST(SlidingObserverTest, GetVelocity010) {
  SlidingObserver observer;
  static base::NoDestructor<SlidingObserver> instance;
  float text_velocity_x = 0.1f;
  float test_velocity_y = 0.1f;
  const float kMilliMeterPerInch = 25.4;
  float convert_unit =
      kMilliMeterPerInch / instance->dpi_ * instance->virtual_pixel_ratio_;
  float velocity = {text_velocity_x * text_velocity_x +
                            test_velocity_y * test_velocity_y};
  float get_result = observer.GetVelocity(text_velocity_x, test_velocity_y);
  EXPECT_EQ(get_result, convert_unit * velocity);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest033) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 50.0f, 24},
                                                        {50.0f, 100.0f, 30},
                                                        {100.0f, 200.0f, 60},
                                                        {200.0f, -1.0f, 120}};
  observer.use_pdf_rate_ = true;
  EXPECT_EQ(observer.GetPreferedFrameRate(25.0f, settings), 24);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest034) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 50.0f, 100},
                                                        {50.0f, 100.0f, 100},
                                                        {100.0f, 200.0f, 100},
                                                        {200.0f, -1.0f, 100}};
  observer.use_pdf_rate_ = true;
  EXPECT_EQ(observer.GetPreferedFrameRate(75.0f, settings), 90);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest035) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 50.0f, 80},
                                                        {50.0f, 100.0f, 80},
                                                        {100.0f, 200.0f, 80},
                                                        {200.0f, -1.0f, 80}};
  observer.use_pdf_rate_ = true;
  EXPECT_EQ(observer.GetPreferedFrameRate(150.0f, settings), 80);
}

TEST(SlidingObserverTest, GetPreferedFrameRateTest036) {
  SlidingObserver observer;
  std::vector<OHOS::NWeb::FrameRateSetting> settings = {{0.0f, 50.0f, 24},
                                                        {50.0f, 100.0f, 30},
                                                        {100.0f, 200.0f, 60},
                                                        {200.0f, -1.0f, 120}};
  observer.use_pdf_rate_ = false;
  EXPECT_EQ(observer.GetPreferedFrameRate(300.0f, settings), 120);
}

TEST(SlidingObserverTest, StopSlidingTest12) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_inited_ = true;
  observer.is_off_screen_ = false;
  observer.sliding_frame_rate_ = 60;
  int32_t result = observer.StopSliding();
  EXPECT_EQ(result, 0);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
  EXPECT_EQ(observer.sliding_frame_rate_, 0);
}

TEST(SlidingObserverTest, StopSlidingTest13) {
  SlidingObserver observer;
  observer.is_sliding_ = true;
  observer.is_inited_ = true;
  observer.is_off_screen_ = false;
  observer.sliding_frame_rate_ = 120;
  observer.current_timestamp_ = 123456;
  int32_t result = observer.StopSliding();
  EXPECT_EQ(result, 0);
  EXPECT_FALSE(observer.is_sliding_);
  EXPECT_FALSE(observer.is_off_screen_);
  EXPECT_EQ(observer.sliding_frame_rate_, 0);
  EXPECT_EQ(observer.current_timestamp_, -1);
}

}  // namespace ohos
}  // namespace base
#endif
