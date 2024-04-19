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

#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/task/thread_pool.h"
#include "ohos_nweb/src/sysevent/event_reporter.h"
#include "base/trace_event/trace_event.h"
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#include "ohos_adapter_helper.h"
#include "ui/events/gesture_detection/gesture_configuration.h"

namespace {
  const float kMilliMeterPerInch = 25.4;
  const float kNanoSecondsPerSecond = 1000000000.0;
  const int32_t kDefaultPreferedFrameRate = 120;
}

namespace base {
namespace ohos {
using OHOS::NWeb::FrameRateSetting;
SlidingObserver& SlidingObserver::GetInstance()
{
    static base::NoDestructor<SlidingObserver> instance;
    return *instance.get();
}

void SlidingObserver::Init() {
    if (OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetProductDeviceType() !=
        OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE) {
      return;
    }

    onScreenSetting = OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance()
                    .GetLTPOConfig("scroll");
    offScreenSetting = OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance()
                    .GetLTPOConfig("fling");
    std::sort(onScreenSetting.begin(), onScreenSetting.end(), [](const FrameRateSetting& setting1,
        const FrameRateSetting& setting2) { return setting1.min_ < setting2.min_; });
    std::sort(offScreenSetting.begin(), offScreenSetting.end(), [](const FrameRateSetting& setting1,
        const FrameRateSetting& setting2) { return setting1.min_ < setting2.min_; });

    virtual_pixel_ratio_ = ui::GestureConfiguration::GetInstance()->virtual_pixel_ratio();
    auto display_manager_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
    if (!display_manager_adapter) {
        return;
    }
    std::shared_ptr<OHOS::NWeb::DisplayAdapter> display =
        display_manager_adapter->GetDefaultDisplay();
    if (!display) {
        return;
    }
    dpi_ = display->GetDpi();
    if (dpi_ == 0) {
        return;
    }
    isInited_ = true;

    LOG(INFO) << "virtual_pixel_ratio: " << virtual_pixel_ratio_ << ", dpi " << dpi_
      << ", onScreenSetting: " << onScreenSetting.size()  << ", offScreenSetting: " << offScreenSetting.size();
}

void SlidingObserver::StartSliding()
{
    if (!isInited_) {
        Init();
    }

    if (isSliding_) {
        return;
    }
    isSliding_ = true;
    isOffScreen_ = false;
}

void SlidingObserver::StopSliding()
{
    if (!isSliding_) {
        return;
    }
    isSliding_ = false;
    isOffScreen_ = false;
    DynamicFrameRateDecision::GetInstance().ReportSlidingFrameRate(0);
}

void SlidingObserver::StartFling()
{
    if (!isSliding_ || !isInited_) {
        return;
    }
    isOffScreen_ = true;
}

void SlidingObserver::OnScrollUpdate(float delta_x, float delta_y)
{
    if (!isSliding_) {
        return;
    }
    float velocity = ConvertToVelocity(delta_x, delta_y);
    int32_t preferredFrameRate = 0;
    if (isOffScreen_) {
        // off screen fling
        preferredFrameRate = GetPreferedFrameRate(velocity, offScreenSetting);
    } else {
        preferredFrameRate = GetPreferedFrameRate(velocity, onScreenSetting);
    }
    DynamicFrameRateDecision::GetInstance().ReportSlidingFrameRate(preferredFrameRate);
}

int32_t SlidingObserver::GetPreferedFrameRate(float velocity, const std::vector<OHOS::NWeb::FrameRateSetting>& setting)
{
    if (setting.empty()) {
        return kDefaultPreferedFrameRate;
    }
    for (auto& item : setting) {
        if (velocity >= item.min_ && (velocity <= item.max_ || item.max_ < 0)) {
            return item.preferredFrameRate_;
        }
    }
    return kDefaultPreferedFrameRate;
}

float SlidingObserver::ConvertToVelocity(float  delta_x, float delta_y)
{
    // unit of velocity_x is device independent pixels per seconds
    float velocity_x = delta_x / vsync_period_;
    float velocity_y = delta_y / vsync_period_;
    // mm per virtual pixel in mate 60, mm_per_inch/ppi_of_device * default_virtual_pixel_ratio_
    float convertUnit = kMilliMeterPerInch / dpi_ * virtual_pixel_ratio_;
    float velocity = std::sqrt(velocity_x * velocity_x + velocity_y * velocity_y);

    LOG(DEBUG) << "velocity_x " << velocity_x << " velocity_y " << velocity_y << " velocity " << convertUnit * velocity;
    return convertUnit * velocity;
}

void SlidingObserver::SetVsyncPeriod(int64_t vsync_period) {
  vsync_period_ = vsync_period / kNanoSecondsPerSecond;
}
}  // namespace ohos
}  // namespace base
