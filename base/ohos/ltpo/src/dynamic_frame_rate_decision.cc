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

#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"

#include <chrono>
#include <cmath>

#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/task/thread_pool.h"
#include "base/trace_event/trace_event.h"
#include "ohos_adapter_helper.h"

namespace base {
namespace ohos {
static const int32_t kDefaultPreferedFrameRate60FPS = 60;
static const int32_t kDefaultPreferedFrameRate120FPS = 120;
static const int kMicrosecondsPerMillisecond = 1000;
static const int kThreeSeconds = 3000;

using OHOS::NWeb::OhosAdapterHelper;

void UpdateTimeOutFramePreferredRate()
{
  DynamicFrameRateDecision::GetInstance().UpdateFramePreferredRate();
}

DynamicFrameRateDecision::DynamicFrameRateDecision()
{}

DynamicFrameRateDecision::~DynamicFrameRateDecision()
{}

DynamicFrameRateDecision& DynamicFrameRateDecision::GetInstance()
{
    static base::NoDestructor<DynamicFrameRateDecision> instance;
    return *instance.get();
}

void DynamicFrameRateDecision::ReportSlidingFrameRate(int32_t frame_rate)
{
  // check, enable ltpo only for phone
  if (frame_rate == slidingFrameRate_) {
    return;
  }
  slidingFrameRate_ = frame_rate;
  UpdateFramePreferredRate();
}

void DynamicFrameRateDecision::ReportDirtyRectFrameRate(int32_t frame_rate)
{
}

void DynamicFrameRateDecision::ReportVideoFrameRate(int32_t frame_rate)
{
  if (videoFrameRate_ == frame_rate) {
    return;
  }
  videoFrameRate_ = frame_rate;
  UpdateFramePreferredRate();
}

void DynamicFrameRateDecision::SetMaxFrameRateThreeSec()
{
  if (slidingFrameRate_ != 0) {
    return;
  }
  if (!vsyncEnabled_) {
    return;
  }
  LOG(DEBUG) << "SetMaxFrameRateThreeSec touch_up_timeStamp: " << touch_up_timeStamp_;
  touch_up_timeStamp_ = GetCurrentTimestampMS();
  base::ThreadPool::PostDelayedTask(
    FROM_HERE,
    base::BindOnce(UpdateTimeOutFramePreferredRate),
    base::Milliseconds(kThreeSeconds)
  );
}

void DynamicFrameRateDecision::UpdateFramePreferredRate()
{
  if (!frameRateLinkerEnable_) {
    return;
  }
  // invoke frame rate linker
  curFrameRate_ = std::max(slidingFrameRate_, videoFrameRate_);
  if (curFrameRate_ <= 0) {
    if (GetCurrentTimestampMS() - touch_up_timeStamp_ <= kThreeSeconds) {
      curFrameRate_ = kDefaultPreferedFrameRate120FPS;
    }
    curFrameRate_ = has_touch_point_ ? kDefaultPreferedFrameRate120FPS : kDefaultPreferedFrameRate60FPS;
  }
  LOG(INFO) << "final prefered frame rate " << curFrameRate_;
  OhosAdapterHelper::GetInstance().GetVSyncAdapter().SetFramePreferredRate(curFrameRate_);
}

void DynamicFrameRateDecision::SetVsyncEnabled(bool enabled)
{
  LOG(DEBUG) << "SetVsyncEnabled" << enabled;
  if (vsyncEnabled_ == enabled) {
    return;
  }
  vsyncEnabled_ = enabled;
  if (visible_ && vsyncEnabled_) {
    SetFrameRateLinkerEnable(true);
  } else {
    SetFrameRateLinkerEnable(false);
  }
  UpdateFramePreferredRate();
}

void DynamicFrameRateDecision::SetHasTouchPoint(bool has_touch_point)
{
  has_touch_point_ = has_touch_point;
}

void DynamicFrameRateDecision::SetVisible(bool visible)
{
  LOG(DEBUG) << "SetVisible" << visible;
  if (visible_ == visible) {
    return;
  }
  visible_ = visible;
  if (visible_ && vsyncEnabled_) {
    SetFrameRateLinkerEnable(true);
  } else {
    SetFrameRateLinkerEnable(false);
  }
  UpdateFramePreferredRate();
}

void DynamicFrameRateDecision::SetFrameRateLinkerEnable(bool enabled)
{
  LOG(DEBUG) << "SetFrameRateLinkerEnable" << enabled;
  if (frameRateLinkerEnable_ == enabled) {
    return;
  }
  frameRateLinkerEnable_ = enabled;
  OhosAdapterHelper::GetInstance().GetVSyncAdapter().SetFrameRateLinkerEnable(enabled);
}

int64_t DynamicFrameRateDecision::GetCurrentTimestampMS() {
  auto currentTime = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(currentTime)
      .count() / kMicrosecondsPerMillisecond;
}
}  // namespace ohos
}  // namespace base
