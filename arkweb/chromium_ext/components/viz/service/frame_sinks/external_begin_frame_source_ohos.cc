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

#include "arkweb/chromium_ext/components/viz/service/frame_sinks/external_begin_frame_source_ohos.h"

#include <cstdint>
#include <memory>

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
#include "arkweb/chromium_ext/base/ohos/dynamic_frame_loss_monitor.h"
#endif
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#endif

namespace viz {
using namespace OHOS::NWeb;

constexpr int64_t VSYNC_PERIOD_90HZ = 11111111;
constexpr int64_t VSYNC_PERIOD_60HZ = 16666666;
constexpr int64_t VSYNC_PERIOD_6090HZ_MID = 13000000;
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
constexpr int64_t VSYNC_TIME_FOR_CALCULATION = 1000000000;
constexpr int VSYNC_30HZ = 30;
constexpr int VSYNC_60HZ = 60;
#endif

class ExternalBeginFrameSourceOHOS::VSyncUserData {
 public:
  VSyncUserData(const scoped_refptr<base::SingleThreadTaskRunner>& current,
                const base::WeakPtr<viz::ExternalBeginFrameSourceOHOS>& weakPtr)
      : current_(current), weak_ptr_(weakPtr) {
    LOG(INFO) << "VSyncUserData constructor!!!";
  }
  VSyncUserData(const VSyncUserData&) = delete;
  VSyncUserData& operator=(const VSyncUserData&) = delete;
  ~VSyncUserData() { LOG(INFO) << "VSyncUserData destructor!!!"; }

  const scoped_refptr<base::SingleThreadTaskRunner>& current_;
  base::WeakPtr<viz::ExternalBeginFrameSourceOHOS> weak_ptr_;
};

ExternalBeginFrameSourceOHOS::ExternalBeginFrameSourceOHOS(uint32_t restart_id)
    : ExternalBeginFrameSource(this, restart_id),
      vsync_notification_enabled_(false),
      vsync_adapter_(OhosAdapterHelper::GetInstance().GetVSyncAdapter()) {
  TRACE_EVENT0("viz",
               "ExternalBeginFrameSourceOHOS::ExternalBeginFrameSourceOHOS");
  LOG(INFO) << "ExternalBeginFrameSourceOHOS constructor!!!";
  user_data_ = std::make_unique<VSyncUserData>(
      base::SingleThreadTaskRunner::GetCurrentDefault(),
      weak_factory_.GetWeakPtr());
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  vsync_adapter_.SetOnVsyncCallback(
      ExternalBeginFrameSourceOHOS::OnVSyncCallback);
#endif
}

void ExternalBeginFrameSourceOHOS::SendInternalBeginFrame() {
  TRACE_EVENT0("viz", "ExternalBeginFrameSourceOHOS::SendInternalBeginFrame");
  base::TimeDelta vsync_period(base::Nanoseconds(vsync_period_));
  base::TimeTicks frame_time = base::TimeTicks::Now();
  auto begin_frame_args = begin_frame_args_generator_.GenerateBeginFrameArgs(
      source_id(), frame_time, last_dead_line_, vsync_period);

  begin_frame_args.internal_frame = true;
  OnBeginFrame(begin_frame_args);
}

ExternalBeginFrameSourceOHOS::~ExternalBeginFrameSourceOHOS() {
  LOG(INFO) << "ExternalBeginFrameSourceOHOS destructor!!!";
  SetEnabled(false);
}

void ExternalBeginFrameSourceOHOS::OnVSync(int64_t timestamp, void* data) {
  if (data == nullptr) {
    LOG(ERROR) << "OnVSync data is nullptr";
    return;
  }
  VSyncUserData* userData = reinterpret_cast<VSyncUserData*>(data);
  if (!userData->current_) {
    LOG(ERROR) << "OnVSync data current is nullptr";
    return;
  }
  userData->current_->PostTask(
      FROM_HERE, base::BindOnce(&ExternalBeginFrameSourceOHOS::OnVSyncImpl,
                                userData->weak_ptr_, timestamp, userData));
}

void ExternalBeginFrameSourceOHOS::OnVSyncImpl(int64_t timestamp,
                                               VSyncUserData* user_data) {
  user_data_.reset(user_data);
  last_vsync_period_ = timestamp;
  int64_t period = last_vsync_period_ - pre_vsync_period_;
  pre_vsync_period_ = last_vsync_period_;
  if (period > 0 && period < VSYNC_PERIOD_6090HZ_MID) {
    vsync_period_ = VSYNC_PERIOD_90HZ;
  } else {
    vsync_period_ = VSYNC_PERIOD_60HZ;
  }

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  // TODO(arkweb): frequent log.
  // LOG(DEBUG) << "ExternalBeginFrameSourceOHOS::OnVSyncImpl vsync_period_: "
  // << vsync_period_;
  int64_t cur_vsync_frequency = 0;
  if (vsync_period_ != 0) {
    cur_vsync_frequency = (VSYNC_TIME_FOR_CALCULATION - 1) / vsync_period_ + 1;
  }
#endif

  base::TimeDelta vsync_period(base::Nanoseconds(vsync_period_));
  base::TimeTicks frame_time = base::TimeTicks() + base::Nanoseconds(timestamp);
  base::TimeTicks deadline = frame_time + vsync_period;
  last_dead_line_ = deadline;
  // OHOS_TRACE_EVENT2("viz", "ExternalBeginFrameSourceOHOS::OnVSyncImpl",
  // "frame_time",
  //              frame_time, "deadline", deadline);
  auto begin_frame_args = begin_frame_args_generator_.GenerateBeginFrameArgs(
      source_id(), frame_time, deadline, vsync_period);
  begin_frame_args.draw_rect = draw_rect_;
  OnBeginFrame(begin_frame_args);

  if (!vsync_notification_enabled_ || user_data_ == nullptr) {
    return;
  }

  vsync_adapter_.RequestVsync(user_data_.release(),
                              ExternalBeginFrameSourceOHOS::OnVSync);

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  if (update_vsync_frequency_ &&
      vsync_frequency_to_update_ != cur_vsync_frequency) {
    vsync_frequency_to_reset_ = cur_vsync_frequency;
    TRACE_EVENT1(
        "viz",
        "ExternalBeginFrameSourceOHOS::OnVSyncImpl::UpdateVSyncFrequency",
        "VSyncFrequency", vsync_frequency_to_update_);
    base::ohos::DynamicFrameRateDecision::GetInstance().ReportVideoFrameRate(
        vsync_frequency_to_update_);
  }
  if (reset_vsync_frequency_) {
    TRACE_EVENT1(
        "viz", "ExternalBeginFrameSourceOHOS::OnVSyncImpl::ResetVSyncFrequency",
        "VSync", vsync_frequency_to_reset_);
    base::ohos::DynamicFrameRateDecision::GetInstance().ReportVideoFrameRate(0);
    reset_vsync_frequency_ = false;
  }
#endif
}

void ExternalBeginFrameSourceOHOS::OnNeedsBeginFrames(bool needs_begin_frames) {
  SetEnabled(needs_begin_frames);
}

void ExternalBeginFrameSourceOHOS::SetEnabled(bool enabled) {
  if (vsync_notification_enabled_ == enabled) {
    return;
  }
  TRACE_EVENT1("viz", "ExternalBeginFrameSourceOHOS::SetEnabled", "enabled",
               enabled);
  base::ohos::DynamicFrameRateDecision::GetInstance().SetVsyncEnabled(enabled);
  vsync_notification_enabled_ = enabled;
  if (vsync_notification_enabled_ && user_data_ != nullptr) {
    vsync_adapter_.RequestVsync(user_data_.release(),
                                ExternalBeginFrameSourceOHOS::OnVSync);
  }
}

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void ExternalBeginFrameSourceOHOS::UpdateVSyncFrequency(int frame_rate) {
  update_vsync_frequency_ = true;
  if (frame_rate <= VSYNC_30HZ) {
    vsync_frequency_to_update_ = VSYNC_30HZ;
  } else if (frame_rate < VSYNC_60HZ) {
    vsync_frequency_to_update_ = (frame_rate / 10) * 10;
  } else {
    vsync_frequency_to_update_ = VSYNC_60HZ;
  }
}

void ExternalBeginFrameSourceOHOS::ResetVSyncFrequency() {
  reset_vsync_frequency_ = true;
  update_vsync_frequency_ = false;
}
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void ExternalBeginFrameSourceOHOS::OnVSyncCallback() {
  base::ohos::DynamicFrameLossMonitor::GetInstance().OnVsync();
}
#endif
}  // namespace viz
