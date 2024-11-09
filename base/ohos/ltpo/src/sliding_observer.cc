/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "base/ohos/ltpo/include/sliding_observer.h"

#include <chrono>
#include <algorithm>

#include "base/command_line.h"
#include "base/base_switches.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/task/thread_pool.h"
#include "ohos_nweb/src/sysevent/event_reporter.h"
#include "base/trace_event/trace_event.h"
#include "ohos_adapter_helper.h"
#include "ui/events/gesture_detection/gesture_configuration.h"

namespace {
  const float kMilliMeterPerInch = 25.4;
  const float kMicroSecondPerSecond = 1000000.0;
  static const int32_t kDefaultPreferedFrameRate = 120;
  static const int32_t STOP_FLING_LTPO = 0;
  static const int32_t START_FLING_LTPO = 1;
  static const int32_t STOP_ALL_FLING_LTPO = 2;
}

namespace base {
namespace ohos {
using OHOS::NWeb::FrameRateSetting;
SlidingObserver::~SlidingObserver() {
  if (strategy_ == LTPOStrategy::APS_FLING) {
    OHOS::NWeb::OhosAdapterHelper::GetInstance().GetVSyncAdapter().SetScene(
      "WEB_LIST_FLING",
      STOP_ALL_FLING_LTPO);
  }
}

SlidingObserver& SlidingObserver::GetInstance(){
  static base::NoDestructor<SlidingObserver> instance;
  return *instance.get();
}

SlidingObserver::SlidingObserver() {
  strategy_ = static_cast<LTPOStrategy>(OHOS::NWeb::OhosAdapterHelper::GetInstance().
    GetSystemPropertiesInstance().GetLTPOStrategy());
  LOG(DEBUG) << "SlidingObserver strategy: " << static_cast<int>(strategy_);
  if (strategy_ == LTPOStrategy::DISABLED) {
    return;
  }

  if (strategy_ == LTPOStrategy::APS_FLING) {
    is_inited_ = true;
    return;
  }

  on_screen_setting_ = OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance()
                  .GetLTPOConfig("scroll");
  off_screen_setting_ = OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance()
                  .GetLTPOConfig("fling");

  auto display_manager_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
  if (!display_manager_adapter) {
    LOG(ERROR) << "display_manager_adapter is nullptr";
    return;
  }
  std::shared_ptr<OHOS::NWeb::DisplayAdapter> display =
      display_manager_adapter->GetDefaultDisplay();
  if (!display) {
    LOG(ERROR) << "display is nullptr";
    return;
  }
  dpi_ = display->GetDpi();
  virtual_pixel_ratio_ = display->GetVirtualPixelRatio();
  if (dpi_ <= 0  || virtual_pixel_ratio_ <= 0) {
    LOG(ERROR) << "dpi_: " << dpi_ << ", virtual_pixel_ratio_: " << virtual_pixel_ratio_;
    return;
  }
  is_inited_ = true;

  LOG(INFO) << "virtual_pixel_ratio: " << virtual_pixel_ratio_ << ", dpi " << dpi_
    << ", on_screen_setting: " << on_screen_setting_.size() << ", off_screen_setting: " << off_screen_setting_.size();
}

void SlidingObserver::StartSliding() {
  if (!is_inited_) {
    return;
  }
  if (is_sliding_) {
    return;
  }
  current_timestamp_ = GetCurrentTimestamp();
  is_sliding_ = true;
  is_off_screen_ = false;
}

int32_t SlidingObserver::StopSliding() {
  if (!is_inited_ || !is_sliding_) {
    return -1;
  }
  if (strategy_ == LTPOStrategy::APS_FLING) {
    if (is_off_screen_) {
      OHOS::NWeb::OhosAdapterHelper::GetInstance().GetVSyncAdapter().SetScene(
        "WEB_LIST_FLING",
        STOP_FLING_LTPO);
    }
  }
  current_timestamp_ = -1;
  is_sliding_ = false;
  is_off_screen_ = false;
  sliding_frame_rate_ = 0;
  return 0;
}

int64_t SlidingObserver::GetCurrentTimestamp() {
  auto currentTime = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(currentTime)
      .count();
}

void SlidingObserver::StartFling() {
  if (!is_inited_ || !is_sliding_) {
    return;
  }
  if (strategy_ == LTPOStrategy::APS_FLING) {
    OHOS::NWeb::OhosAdapterHelper::GetInstance().GetVSyncAdapter().SetScene(
      "WEB_LIST_FLING",
      START_FLING_LTPO);
  }
  is_off_screen_ = true;
}

int32_t SlidingObserver::OnScrollUpdate(float delta_x, float delta_y) {
  if (!is_sliding_ || is_off_screen_ || strategy_ == LTPOStrategy::APS_FLING) {
    return -1;
  }
  auto current_timestamp = GetCurrentTimestamp();

  // unit of velocity_x is device independent pixels per seconds
  float velocity_x = delta_x * kMicroSecondPerSecond / (current_timestamp - current_timestamp_);
  float velocity_y = delta_y * kMicroSecondPerSecond / (current_timestamp - current_timestamp_);
  current_timestamp_ = current_timestamp;
  float velocity = GetVelocity(velocity_x, velocity_y);
  int32_t preferred_frame_rate =  GetPreferedFrameRate(velocity, on_screen_setting_);
  LOG(DEBUG) << "OnScrollUpdate " << sliding_frame_rate_ << ", " << preferred_frame_rate;
  if (sliding_frame_rate_ == preferred_frame_rate) {
    return -1;
  }
  sliding_frame_rate_ = preferred_frame_rate;
  return sliding_frame_rate_;
}

int32_t SlidingObserver::OnFlingUpdate(float velocity_x, float velocity_y) {
  if (!is_sliding_ || !is_off_screen_ || strategy_ == LTPOStrategy::APS_FLING) {
    return -1;
  }

  float velocity = GetVelocity(velocity_x, velocity_y);
  // off screen fling
  int32_t preferred_frame_rate = GetPreferedFrameRate(velocity, off_screen_setting_);
  LOG(DEBUG) << "OnFlingUpdate " << sliding_frame_rate_ << ", " << preferred_frame_rate;
  if (sliding_frame_rate_ == preferred_frame_rate) {
    return -1;
  }
  sliding_frame_rate_ = preferred_frame_rate;
  return sliding_frame_rate_;
}

int32_t SlidingObserver::GetPreferedFrameRate(float velocity,
  const std::vector<OHOS::NWeb::FrameRateSetting>& setting) {
  if (setting.empty()) {
      return kDefaultPreferedFrameRate;
  }
  for (auto& item : setting) {
    if (velocity >= item.min_ && (velocity < item.max_ || item.max_ < 0)) {
      return item.preferredFrameRate_;
    }
  }
  LOG(WARNING) << "can not find proper prefered frame rate";
  return kDefaultPreferedFrameRate;
}

float SlidingObserver::GetVelocity(float velocity_x, float velocity_y) {
  // mm per virtual pixel in phone, mm_per_inch/ppi_of_device * default_virtual_pixel_ratio_
  float convert_unit = kMilliMeterPerInch / dpi_ * virtual_pixel_ratio_;
  float velocity = std::sqrt(velocity_x * velocity_x + velocity_y * velocity_y);
  LOG(DEBUG) << "velocity_x " << velocity_x << " velocity_y " << velocity_y  << " velocity " << convert_unit * velocity;
  return convert_unit * velocity;
}

void SlidingObserver::OnDisplayInfoChange() {
  LOG(INFO) << "SlidingObserver::OnDisplayInfoChange";
  if (!is_inited_) {
    return;
  }
  auto display_manager_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
  if (!display_manager_adapter) {
      return;
  }
  std::shared_ptr<OHOS::NWeb::DisplayAdapter> display =
      display_manager_adapter->GetDefaultDisplay();
  if (!display) {
    return;
  }
  auto dpi = display->GetDpi();
  auto ratio = display->GetVirtualPixelRatio();
  if (dpi_ > 0 || ratio > 0) {
    dpi_ = dpi;
    virtual_pixel_ratio_ = ratio;
  }
}
}  // namespace ohos
}  // namespace base
