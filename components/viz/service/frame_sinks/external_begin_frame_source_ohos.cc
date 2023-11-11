#include "components/viz/service/frame_sinks/external_begin_frame_source_ohos.h"
#include <cstdint>
#include <memory>

#include "base/bind.h"
#include "base/logging.h"
#include "base/threading/thread_task_runner_handle.h"
#include "base/trace_event/trace_event.h"
#include "ohos_adapter_helper.h"

namespace viz {
using namespace OHOS::NWeb;

constexpr int64_t VSYNC_PERIOD_90HZ = 11111111;
constexpr int64_t VSYNC_PERIOD_60HZ = 16666666;
constexpr int64_t VSYNC_PERIOD_6090HZ_MID = 13000000;

class ExternalBeginFrameSourceOHOS::VSyncUserData {
 public:
  VSyncUserData(const scoped_refptr<base::SingleThreadTaskRunner>& current,
                const base::WeakPtr<viz::ExternalBeginFrameSourceOHOS>& weakPtr)
      : current_(current), weak_ptr_(weakPtr) {
    LOG(INFO) << "VSyncUserData constructor!!!";
  };
  VSyncUserData(const VSyncUserData&) = delete;
  VSyncUserData& operator=(const VSyncUserData&) = delete;
  ~VSyncUserData() { LOG(INFO) << "VSyncUserData destructor!!!"; };

  const scoped_refptr<base::SingleThreadTaskRunner>& current_;
  base::WeakPtr<viz::ExternalBeginFrameSourceOHOS> weak_ptr_;
};

ExternalBeginFrameSourceOHOS::ExternalBeginFrameSourceOHOS(
    uint32_t restart_id,
    FrameSinkManagerImpl* frame_sink_manager)
    : ExternalBeginFrameSource(this, restart_id),
      vsync_notification_enabled_(false),
      vsync_adapter_(OhosAdapterHelper::GetInstance().GetVSyncAdapter()),
      frame_sink_manager_(frame_sink_manager) {
  TRACE_EVENT0("viz",
               "ExternalBeginFrameSourceOHOS::ExternalBeginFrameSourceOHOS");
  LOG(INFO) << "ExternalBeginFrameSourceOHOS constructor!!!";
  user_data_ = std::make_unique<VSyncUserData>(
      base::ThreadTaskRunnerHandle::Get(), weak_factory_.GetWeakPtr());
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
  int64_t period = last_vsync_period_ - pre_vsync_period_;
  pre_vsync_period_ = last_vsync_period_;
  if (period > 0 && period < VSYNC_PERIOD_6090HZ_MID) {
    vsync_period_ = VSYNC_PERIOD_90HZ;
  } else {
    vsync_period_ = VSYNC_PERIOD_60HZ;
  }
  base::TimeDelta vsync_period(base::Nanoseconds(vsync_period_));
  base::TimeTicks frame_time = base::TimeTicks() + base::Nanoseconds(timestamp);
  base::TimeTicks deadline = frame_time + vsync_period;
  TRACE_EVENT2("viz", "ExternalBeginFrameSourceOHOS::OnVSyncImpl", "frame_time",
               frame_time, "deadline", deadline);
  auto begin_frame_args = begin_frame_args_generator_.GenerateBeginFrameArgs(
      source_id(), frame_time, deadline, vsync_period);
  OnBeginFrame(begin_frame_args);

  if (frame_sink_manager_) {
    TRACE_EVENT0("viz",
                 "ExternalBeginFrameSourceOHOS::OnVSyncImpl::BackToMainThread");
    frame_sink_manager_->OnVsync(frame_sink_id_);
  }

  if (!vsync_notification_enabled_ || user_data_ == nullptr) {
    return;
  }

  vsync_adapter_->RequestVsync(user_data_.release(),
                               ExternalBeginFrameSourceOHOS::OnVSync);
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
  vsync_notification_enabled_ = enabled;
  if (vsync_notification_enabled_ && user_data_ != nullptr) {
    vsync_adapter_->RequestVsync(user_data_.release(),
                                 ExternalBeginFrameSourceOHOS::OnVSync);
  }
}
}  // namespace viz
