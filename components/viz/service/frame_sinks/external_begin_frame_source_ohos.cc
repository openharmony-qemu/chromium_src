#include "components/viz/service/frame_sinks/external_begin_frame_source_ohos.h"
#include <cstdint>
#include <memory>

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "ohos_adapter_helper.h"
#if BUILDFLAG(IS_OHOS)
#include "base/report_loss_frame.h"
#include "base/ohos/dynamic_frame_loss_monitor.h"
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#include "base/ohos/ltpo/include/sliding_observer.h"
#include "base/ohos/input_sync/input_vsync_sync_lock.h"
#include "base/task/thread_pool.h"
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#include "base/ohos/d_vsync/include/d_vsync_controller.h"

extern bool QueryBoolFromBrowserProcess(const std::string& key, bool defaultValue);
#endif

namespace viz {
using namespace OHOS::NWeb;
using base::ohos::InputSyncLock;

constexpr int64_t VSYNC_PERIOD_90HZ = 11111111;
constexpr int64_t VSYNC_PERIOD_60HZ = 16666666;
constexpr int64_t VSYNC_PERIOD_6090HZ_MID = 13000000;
bool g_skip_vsync = false;
constexpr int64_t VSYNC_PERIOD_120HZ = 8333333;
constexpr int64_t VSYNC_PERIOD_90120HZ_MID = 9800000;
constexpr int64_t VSYNC_TIME_FOR_CALCULATION = 1000000000;

constexpr int VSYNC_30HZ = 30;
constexpr int VSYNC_60HZ = 60;

class ExternalBeginFrameSourceOHOS::VSyncUserData {
 public:
  VSyncUserData(const scoped_refptr<base::SingleThreadTaskRunner>& current,
                const base::WeakPtr<viz::ExternalBeginFrameSourceOHOS>& weakPtr)
#if defined(OHOS_BUGFIX_CRASH)
      : current_(current), weak_ptr_(weakPtr){
          LOG(INFO) << "VSyncUserData constructor!!!";
      }
#else
      : current_(current), weak_ptr_(weakPtr) {}
#endif // defined(OHOS_BUGFIX_CRASH)
  VSyncUserData(const VSyncUserData&) = delete;
  VSyncUserData& operator=(const VSyncUserData&) = delete;
#if defined(OHOS_BUGFIX_CRASH)
  ~VSyncUserData() {
     LOG(INFO) << "VSyncUserData destructor!!!";
  }
#else
  ~VSyncUserData() = default;
#endif // defined(OHOS_BUGFIX_CRASH)

  const scoped_refptr<base::SingleThreadTaskRunner>& current_;
  base::WeakPtr<viz::ExternalBeginFrameSourceOHOS> weak_ptr_;
};

ExternalBeginFrameSourceOHOS::ExternalBeginFrameSourceOHOS(
    uint32_t restart_id,
#if defined(OHOS_PERFORMANCE_JITTER)
    FrameSinkManagerImpl* frame_sink_manager
#endif
    )
    : ExternalBeginFrameSource(this, restart_id),
      vsync_notification_enabled_(false),
      first_vsync_since_notify_enabled_(false),
#if defined(OHOS_PERFORMANCE_JITTER)
      vsync_adapter_(OhosAdapterHelper::GetInstance().GetVSyncAdapter()),
      frame_sink_manager_(frame_sink_manager) {
#else
     vsync_adapter_(OhosAdapterHelper::GetInstance().GetVSyncAdapter()) {
#endif
  TRACE_EVENT0("viz",
               "ExternalBeginFrameSourceOHOS::ExternalBeginFrameSourceOHOS");
  LOG(INFO) << "ExternalBeginFrameSourceOHOS constructor!!!";
  user_data_ = std::make_unique<VSyncUserData>(
      base::SingleThreadTaskRunner::GetCurrentDefault(), weak_factory_.GetWeakPtr());
#if BUILDFLAG(IS_OHOS)
  vsync_adapter_.SetOnVsyncCallback(ExternalBeginFrameSourceOHOS::OnVSyncCallback);
  vsync_adapter_.SetOnVsyncEndCallback(ExternalBeginFrameSourceOHOS::OnVSyncEndCallback);
  base::ohos::DynamicFrameRateDecision::GetInstance().Init();
  last_dvsync_state_ = base::ohos::DVsyncController::GetInstance().GetIsFling();
  dvsync_enable_ = QueryBoolFromBrowserProcess("web.ohos.dvsync", false);
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

void ExternalBeginFrameSourceOHOS::SetDynamicBeginFrameDeadlineOffsetSource(
    DynamicBeginFrameDeadlineOffsetSource*
        dynamic_begin_frame_deadline_offset_source) {
  TRACE_EVENT0(
      "viz",
      "ExternalBeginFrameSourceOHOS::SetDynamicBeginFrameDeadlineOffsetSource");
  begin_frame_args_generator_.set_dynamic_begin_frame_deadline_offset_source(
      dynamic_begin_frame_deadline_offset_source);
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
  int64_t cur_vsync_frequency = 0;
  if (vsync_period_ != 0) {
    cur_vsync_frequency = (VSYNC_TIME_FOR_CALCULATION - 1) / vsync_period_ + 1;
  }
#if defined(OHOS_PERFORMANCE_JITTER)
  static bool isAlreadyThrottle = false;
  if (lower_frame_rate_enabled_) {
    if (!isAlreadyThrottle) {
      frame_sink_manager_->StartThrottlingAllFrameSinks(base::Hertz(0.01));
      isAlreadyThrottle = true;
      LOG(DEBUG) << "OnVSyncImpl StartThrottlingAllFrameSinks";
    }
  } else if (isAlreadyThrottle) {
    frame_sink_manager_->StopThrottlingAllFrameSinks();
    isAlreadyThrottle = false;
  }
#endif

#if BUILDFLAG(IS_OHOS)
  ReportLossFrame::GetInstance()->SetVsyncPeriod(vsync_period_);
  if (dvsync_enable_) {
    bool currentDvsyncState = base::ohos::DVsyncController::GetInstance().GetIsFling();
    if (last_dvsync_state_ != currentDvsyncState) {
      LOG(INFO) << "ExternalBeginFrameSourceOHOS::OnVSyncImpl::SetDVSyncSwitch: " << currentDvsyncState;
      TRACE_EVENT1("viz", "ExternalBeginFrameSourceOHOS::OnVSyncImpl::SetDVSyncSwitch", "SetDVSyncSwitch",
                  currentDvsyncState);
      vsync_adapter_.SetDVSyncSwitch(currentDvsyncState);
      last_dvsync_state_ = currentDvsyncState;
    }
  }
#endif
  base::TimeDelta vsync_period(base::Nanoseconds(vsync_period_));
  base::TimeTicks frame_time = base::TimeTicks() + base::Nanoseconds(timestamp);
  base::TimeTicks deadline = frame_time + vsync_period;
  last_dead_line_ = deadline;
  OHOS_TRACE_EVENT2("viz", "ExternalBeginFrameSourceOHOS::OnVSyncImpl", "frame_time",
               frame_time, "deadline", deadline);
    if (lower_frame_rate_enabled_ && g_skip_vsync) {
    TRACE_EVENT0("viz", "vsync skip");
    g_skip_vsync = false;
  } else {
    TRACE_EVENT0("viz", "vsync not skip");
    auto begin_frame_args = begin_frame_args_generator_.GenerateBeginFrameArgs(
        source_id(), frame_time, deadline, vsync_period);
    begin_frame_args.draw_rect = draw_rect_;
    OnBeginFrame(begin_frame_args);

#if defined(OHOS_PERFORMANCE_JITTER)
    if (frame_sink_manager_) {
      TRACE_EVENT0("viz",
                   "ExternalBeginFrameSourceOHOS::OnVSyncImpl::BackToMainThread");
      frame_sink_manager_->OnVsync(frame_sink_id_);
    }
#endif
    g_skip_vsync = true;
  }

  if (!vsync_notification_enabled_ || user_data_ == nullptr) {
    return;
  }

  vsync_adapter_.RequestVsync(user_data_.release(),
                               ExternalBeginFrameSourceOHOS::OnVSync);
  if (update_vsync_frequency_ &&
      vsync_frequency_to_update_ != cur_vsync_frequency) {
    vsync_frequency_to_reset_ = cur_vsync_frequency;
    LOG(DEBUG) << "ExternalBeginFrameSourceOHOS::OnVSyncImpl::UpdateVSyncFrequency vsync_frequency_to_update_: " << vsync_frequency_to_update_ << ", cur_vsync_frequency: " << cur_vsync_frequency;
    TRACE_EVENT1("viz", "ExternalBeginFrameSourceOHOS::OnVSyncImpl::UpdateVSyncFrequency", "VSyncFrequency",
            vsync_frequency_to_update_);

    base::ohos::DynamicFrameRateDecision::GetInstance().ReportVideoFrameRate(vsync_frequency_to_update_);
  }
  if (reset_vsync_frequency_) {
    LOG(DEBUG) << "ExternalBeginFrameSourceOHOS::OnVSyncImpl::ResetVSyncFrequency vsync_frequency_to_reset_: " << vsync_frequency_to_reset_ << ", cur_vsync_frequency: " << cur_vsync_frequency;
    TRACE_EVENT1("viz", "ExternalBeginFrameSourceOHOS::OnVSyncImpl::ResetVSyncFrequency", "VSync",
        vsync_frequency_to_reset_);
    base::ohos::DynamicFrameRateDecision::GetInstance().ReportVideoFrameRate(0);
    reset_vsync_frequency_ = false;
  }
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
  first_vsync_since_notify_enabled_ = true;
  if (vsync_notification_enabled_ && user_data_ != nullptr) {
    vsync_adapter_.RequestVsync(user_data_.release(),
                                 ExternalBeginFrameSourceOHOS::OnVSync);
  }
}

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

void ExternalBeginFrameSourceOHOS::OnVSyncCallback()
{
  base::ohos::DynamicFrameLossMonitor::GetInstance().OnVsync();
}

void ExternalBeginFrameSourceOHOS::OnVSyncEndCallback()
{
  InputSyncLock::GetInstance().SetNeedWaitForInput(false);
  InputSyncLock::GetInstance().SetHandledTouchEvent(false);
}

void ExternalBeginFrameSourceOHOS::SetNeedWaitForInput(bool need_wait_for_input) {
  InputSyncLock::GetInstance().SetNeedWaitForInput(need_wait_for_input);
}

void ExternalBeginFrameSourceOHOS::TriggerVsync() {
}

void ExternalBeginFrameSourceOHOS::TriggerVsyncImpl() {
}

void ExternalBeginFrameSourceOHOS::EmplaceVSyncImpl(int64_t timestamp, VSyncUserData* user_data) {
}
}  // namespace viz
