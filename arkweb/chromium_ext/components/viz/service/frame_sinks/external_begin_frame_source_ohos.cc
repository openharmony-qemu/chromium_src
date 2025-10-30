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
#include "arkweb/chromium_ext/base/report_loss_frame_ext.h"
#endif
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
#include "arkweb/chromium_ext/base/ohos/d_vsync/include/d_vsync_controller.h"
#endif

namespace viz {
using namespace OHOS::NWeb;

constexpr int64_t VSYNC_PERIOD_90HZ = 11111111;
constexpr int64_t VSYNC_PERIOD_60HZ = 16666666;
constexpr int64_t VSYNC_PERIOD_6090HZ_MID = 13000000;
constexpr int64_t VSYNC_PERIOD_120HZ = 8333333;
constexpr int64_t VSYNC_PERIOD_90120HZ_MID = 9800000;
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
constexpr int64_t VSYNC_TIME_FOR_CALCULATION = 1000000000;
constexpr int VSYNC_30HZ = 30;
constexpr int VSYNC_60HZ = 60;
#endif

//LCOV_EXCL_START
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
//LCOV_EXCL_STOP

ExternalBeginFrameSourceOHOS::ExternalBeginFrameSourceOHOS(
    uint32_t restart_id,
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
    FrameSinkManagerImpl* frame_sink_manager
#endif
    )
    : ExternalBeginFrameSource(this, restart_id),
      vsync_notification_enabled_(false),
      first_vsync_since_notify_enabled_(false),
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
      vsync_adapter_(OhosAdapterHelper::GetInstance().GetVSyncAdapter()),
      frame_sink_manager_(frame_sink_manager) {
#else
    vsync_adapter_(OhosAdapterHelper::GetInstance().GetVSyncAdapter()) {
#endif
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

#if BUILDFLAG(ARKWEB_D_VSYNC)
  last_dvsync_state_ = base::ohos::DVsyncController::GetInstance().GetIsFling();
#endif
  managerImplUtils = std::make_unique<FrameSinkManagerImplUtils>(frame_sink_manager);
}

//LCOV_EXCL_START
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
//LCOV_EXCL_STOP

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

  if (!vsync_notification_enabled_ || user_data_ == nullptr) {
    return;
  }
  last_vsync_period_ = timestamp;
  vsync_period_ = vsync_adapter_.GetVSyncPeriod();
  if (vsync_period_ == 0) {
    if (first_vsync_since_notify_enabled_) {
      first_vsync_since_notify_enabled_ = false;
      pre_vsync_period_ = last_vsync_period_;
    } else {
      int64_t period = last_vsync_period_ - pre_vsync_period_;
      pre_vsync_period_ = last_vsync_period_;
      if (period > 0 && period < VSYNC_PERIOD_90120HZ_MID) {
        vsync_period_ = VSYNC_PERIOD_120HZ;
      } else if (period > 0 && period < VSYNC_PERIOD_6090HZ_MID) {
        vsync_period_ = VSYNC_PERIOD_90HZ;
      } else {
        vsync_period_ = VSYNC_PERIOD_60HZ;
      }
    }
  }

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
ReportLossFrame::GetInstance()->SetVsyncPeriod(vsync_period_);
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  LOG(DEBUG) << "ExternalBeginFrameSourceOHOS::OnVSyncImpl vsync_period_: "
             << vsync_period_;
  int64_t cur_vsync_frequency = 0;
  if (vsync_period_ != 0) {
    cur_vsync_frequency = (VSYNC_TIME_FOR_CALCULATION - 1) / vsync_period_ + 1;
  }
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  static bool isAlreadyThrottle = false;
  static bool isHalfAlreadyThrottle = false;
#if BUILDFLAG(ARKWEB_PIP)
  if (!pip_active_) {
#endif
    if (lower_frame_rate_enabled_) {
      if (!isAlreadyThrottle) {
        frame_sink_manager_->StartThrottlingAllFrameSinks(base::Hertz(0.01));
        isAlreadyThrottle = true;
        LOG(DEBUG) << "OnVSyncImpl StartThrottlingAllFrameSinks";
      }
    } else if (isAlreadyThrottle && !half_frame_rate_enabled_) {
      frame_sink_manager_->StopThrottlingAllFrameSinks();
      isAlreadyThrottle = false;
    } else if (isAlreadyThrottle && half_frame_rate_enabled_) {
      isAlreadyThrottle = false;
    }

    if (half_frame_rate_enabled_) {
      if (!isHalfAlreadyThrottle) {
        frame_sink_manager_->StartThrottlingAllFrameSinks(base::Hertz(30));
        isHalfAlreadyThrottle = true;
        LOG(DEBUG) << "OnVSyncImpl StartHalfThrottlingAllFrameSinks";
      }
    } else if (isHalfAlreadyThrottle && !lower_frame_rate_enabled_) {
      frame_sink_manager_->StopThrottlingAllFrameSinks();
      isHalfAlreadyThrottle = false;
    } else if (isAlreadyThrottle && lower_frame_rate_enabled_) {
      isHalfAlreadyThrottle = false;
    }
#if BUILDFLAG(ARKWEB_PIP)
  }
#endif
#if BUILDFLAG(ARKWEB_PIP)
  if (pip_active_ && isAlreadyThrottle) {
    if (frame_sink_manager_) {
      frame_sink_manager_->StopThrottlingAllFrameSinks();
      isAlreadyThrottle = false;
    }
  }
#endif  // ARKWEB_PIP
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
bool currentDysyncState = base::ohos::DVsyncController::GetInstance().GetIsFling();
if (last_dvsync_state_ != currentDysyncState) {
    LOG(INFO) << "ExternalBeginFrameSourceOHOS::OnVSyncImpl::SetDVSyncSwitch: " << currentDysyncState;
    TRACE_EVENT1("viz", "ExternalBeginFrameSourceOHOS::OnVSyncImpl::SetDVSyncSwitch", "SetDVSyncSwitch",
            currentDysyncState);
    vsync_adapter_.SetDVSyncSwitch(currentDysyncState);
    last_dvsync_state_ = currentDysyncState;
}
#endif

  base::TimeDelta vsync_period(base::Nanoseconds(vsync_period_));
  base::TimeTicks frame_time = base::TimeTicks() + base::Nanoseconds(timestamp);
  base::TimeTicks deadline = frame_time + vsync_period;
  last_dead_line_ = deadline;
#if BUILDFLAG(ARKWEB_DFX_TRACING)
  OHOS_TRACE_EVENT2("viz", "ExternalBeginFrameSourceOHOS::OnVSyncImpl", "frame_time",
                    frame_time, "deadline", deadline);
#endif
  if ((lower_frame_rate_enabled_ || half_frame_rate_enabled_) && g_skip_vsync_) {
    TRACE_EVENT0("viz", "vsync skip");
    g_skip_vsync_ = false;
  } else {
    TRACE_EVENT0("viz", "vsync not skip");
    auto begin_frame_args = begin_frame_args_generator_.GenerateBeginFrameArgs(
        source_id(), frame_time, deadline, vsync_period);
    begin_frame_args.draw_rect = draw_rect_;
    OnBeginFrame(begin_frame_args);

    g_skip_vsync_ = true;
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
        "VSyncFrequency",
        vsync_frequency_to_update_);
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

//LCOV_EXCL_START
void ExternalBeginFrameSourceOHOS::OnNeedsBeginFrames(bool needs_begin_frames) {
  SetEnabled(needs_begin_frames);
}

void ExternalBeginFrameSourceOHOS::SetEnabled(bool enabled) {
  if (vsync_notification_enabled_ == enabled) {
    return;
  }
  TRACE_EVENT1("viz", "ExternalBeginFrameSourceOHOS::SetEnabled", "enabled",
               enabled);
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  TRACE_EVENT1("viz", "ExternalBeginFrameSourceOHOS::SetEnabled", "condition_",
               condition_);
#endif
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  if (condition_) {
    SendInternalBeginFrame();
  } else {
#endif
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
    base::ohos::DynamicFrameRateDecision::GetInstance().SetVsyncEnabled(enabled);
#endif
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  }
#endif
  vsync_notification_enabled_ = enabled;
  if (vsync_notification_enabled_ && user_data_ != nullptr) {
    vsync_adapter_.RequestVsync(user_data_.release(),
                                ExternalBeginFrameSourceOHOS::OnVSync);
  }
}

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void ExternalBeginFrameSourceOHOS::OnSetBypassVsyncCondition(int32_t condition) {
  condition_ = condition;
}
#endif
//LCOV_EXCL_STOP

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

//LCOV_EXCL_START
void ExternalBeginFrameSourceOHOS::ResetVSyncFrequency() {
  reset_vsync_frequency_ = true;
  update_vsync_frequency_ = false;
  base::ohos::DynamicFrameRateDecision::GetInstance().ReportVideoFrameRate(0);
}
//LCOV_EXCL_STOP
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
//LCOV_EXCL_START
void ExternalBeginFrameSourceOHOS::OnVSyncCallback() {
  base::ohos::DynamicFrameLossMonitor::GetInstance().OnVsync();
}
//LCOV_EXCL_STOP
#endif
}  // namespace viz
