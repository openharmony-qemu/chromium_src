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
#include "base/task/single_thread_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/browser_thread.h"
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

void DynamicFrameRateDecision::Init()
{
  if (!curent_task_runner_) {
    curent_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  }
}

DynamicFrameRateDecision& DynamicFrameRateDecision::GetInstance()
{
    static base::NoDestructor<DynamicFrameRateDecision> instance;
    return *instance.get();
}

void DynamicFrameRateDecision::ReportSlidingFrameRate(int32_t frame_rate)
{
  if (!curent_task_runner_) {
    return;
  }
  curent_task_runner_->PostTask(FROM_HERE, base::BindOnce(
    &DynamicFrameRateDecision::ReportSlidingFrameRateImpl,
    base::Unretained(this), frame_rate));
}

void DynamicFrameRateDecision::ReportSlidingFrameRateImpl(int32_t frame_rate)
{
  if (sliding_frame_rate_ == frame_rate) {
    return;
  }
  LOG(DEBUG) << "ReportSlidingFrameRate " << frame_rate << ", " << sliding_frame_rate_;
  sliding_frame_rate_ = frame_rate;
  UpdateFramePreferredRate();
}

void DynamicFrameRateDecision::ReportVideoFrameRate(int32_t frame_rate)
{
  if (!curent_task_runner_) {
    return;
  }
  curent_task_runner_->PostTask(FROM_HERE, base::BindOnce(
    &DynamicFrameRateDecision::ReportVideoFrameRateImpl,
    base::Unretained(this), frame_rate));
}

void DynamicFrameRateDecision::ReportVideoFrameRateImpl(int32_t frame_rate)
{
  if (video_frame_rate_ == frame_rate) {
    return;
  }
  LOG(DEBUG) << "ReportVideoFrameRate " << video_frame_rate_ << ", " << frame_rate;
  video_frame_rate_ = frame_rate;
  UpdateFramePreferredRate();
}

void DynamicFrameRateDecision::SetMaxFrameRateThreeSec()
{
  if (!curent_task_runner_) {
    return;
  }
  LOG(DEBUG) << "SetMaxFrameRateThreeSec " << sliding_frame_rate_;
  if (sliding_frame_rate_ != 0) {
    return;
  }
  if (vsync_cnt_ == 0) {
    return;
  }
  touch_up_timestamp_ = GetCurrentTimestampMS();
  LOG(DEBUG) << "SetMaxFrameRateThreeSec touch_up_timeStamp: " << touch_up_timestamp_;
  curent_task_runner_->PostDelayedTask(
    FROM_HERE,
    base::BindOnce(UpdateTimeOutFramePreferredRate),
    base::Milliseconds(kThreeSeconds)
  );
}

void DynamicFrameRateDecision::UpdateFramePreferredRate()
{
  if (!frame_rate_linker_enable_) {
    return;
  }
  cur_frame_rate_ = sliding_frame_rate_;
  if (sliding_frame_rate_ <= 0) {
    cur_frame_rate_ = has_touch_point_ ? kDefaultPreferedFrameRate120FPS : kDefaultPreferedFrameRate60FPS;
    if (GetCurrentTimestampMS() - touch_up_timestamp_< kThreeSeconds) {
      cur_frame_rate_ = kDefaultPreferedFrameRate120FPS;
    }
  }
  cur_frame_rate_ = std::max(cur_frame_rate_, video_frame_rate_);
  OhosAdapterHelper::GetInstance().GetVSyncAdapter().SetFramePreferredRate(cur_frame_rate_);
}

void DynamicFrameRateDecision::SetVsyncEnabled(bool enabled)
{
  if (!curent_task_runner_) {
    return;
  }
  curent_task_runner_->PostTask(FROM_HERE, base::BindOnce(
    &DynamicFrameRateDecision::SetVsyncEnabledImpl,
    base::Unretained(this), enabled));
}

void DynamicFrameRateDecision::SetVsyncEnabledImpl(bool enabled)
{
  if (enabled) {
    vsync_cnt_++;
  } else {
    vsync_cnt_--;
  }
  vsync_cnt_ = std::max(vsync_cnt_, 0);
  LOG(DEBUG) << "SetVsyncEnabled " << enabled << ", vsync_cnt_: " << vsync_cnt_;
  SetFrameRateLinkerEnable(visible_ && (vsync_cnt_ != 0));
  UpdateFramePreferredRate();
}

void DynamicFrameRateDecision::SetHasTouchPoint(bool has_touch_point)
{
  if (!curent_task_runner_) {
    return;
  }
  curent_task_runner_->PostTask(FROM_HERE, base::BindOnce(
    &DynamicFrameRateDecision::SetHasTouchPointImpl,
    base::Unretained(this), has_touch_point));
}

void DynamicFrameRateDecision::SetHasTouchPointImpl(bool has_touch_point)
{
  if (has_touch_point_ == has_touch_point) {
    return;
  }
  LOG(DEBUG) << "SetHasTouchPoint " << has_touch_point;
  has_touch_point_ = has_touch_point;
  if (has_touch_point_) {
    UpdateFramePreferredRate();
  } else {
    SetMaxFrameRateThreeSec();
  }
}

void DynamicFrameRateDecision::SetVisible(bool visible)
{
  if (!curent_task_runner_) {
    return;
  }
  curent_task_runner_->PostTask(FROM_HERE, base::BindOnce(
    &DynamicFrameRateDecision::SetVisibleImpl,
    base::Unretained(this), visible));
}

void DynamicFrameRateDecision::SetVisibleImpl(bool visible)
{
  if (visible_ == visible) {
    return;
  }
  LOG(DEBUG) << "SetVisible " << visible;
  visible_ = visible;
  SetFrameRateLinkerEnable(visible_ && (vsync_cnt_ != 0));
  UpdateFramePreferredRate();
}

void DynamicFrameRateDecision::SetFrameRateLinkerEnable(bool enabled)
{
  LOG(DEBUG) << "SetFrameRateLinkerEnable " << enabled;
  if (frame_rate_linker_enable_ == enabled) {
    return;
  }
  frame_rate_linker_enable_ = enabled;
  OhosAdapterHelper::GetInstance().GetVSyncAdapter().SetFrameRateLinkerEnable(enabled);
}

int64_t DynamicFrameRateDecision::GetCurrentTimestampMS() {
  auto currentTime = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(currentTime)
      .count() / kMicrosecondsPerMillisecond;
}
}  // namespace ohos
}  // namespace base
