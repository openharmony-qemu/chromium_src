/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "components/viz/service/frame_sinks/external_begin_frame_source_ohos.h"

#include <native_vsync/native_vsync.h>

#include <cstdint>
#include <limits>
#include <memory>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/strings/to_string.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"


namespace viz {

constexpr int64_t VSYNC_PERIOD_60HZ = 16666666;
const int API_VERSION_14 = 14;

class ExternalBeginFrameSourceOHOS::VSyncUserData {
 public:
  VSyncUserData(const scoped_refptr<base::SingleThreadTaskRunner>& current,
                const base::WeakPtr<viz::ExternalBeginFrameSourceOHOS>& weakPtr)
      : current_(current), weak_ptr_(weakPtr) {}
  VSyncUserData(const VSyncUserData&) = delete;
  VSyncUserData& operator=(const VSyncUserData&) = delete;
  ~VSyncUserData() = default;

  const scoped_refptr<base::SingleThreadTaskRunner>& current_;
  base::WeakPtr<viz::ExternalBeginFrameSourceOHOS> weak_ptr_;
};

ExternalBeginFrameSourceOHOS::ExternalBeginFrameSourceOHOS(uint32_t restart_id,
                                                           uint64_t surface_id)
    : ExternalBeginFrameSource(this, restart_id),
      vsync_notification_enabled_(false),
      surface_id_(surface_id) {
  std::string vsync_name = base::ToString("vsync-", restart_id);
  LOG(INFO) << "ExternalBeginFrameSourceOHOS constructor!!! id: " << vsync_name;

  const base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

  if (OH_GetSdkApiVersion() >= API_VERSION_14 &&
      surface_id_ != std::numeric_limits<std::uint64_t>::max() &&
      !command_line->HasSwitch(switches::KDisableOcclusionFeature)) {
    LOG(INFO) << "ExternalBeginFrameSourceOHOS surfaceId: " << surface_id_;
    native_vsync_ = OH_NativeVSync_Create_ForAssociatedWindow(
        surface_id_, vsync_name.data(), vsync_name.size());
  } else {
    native_vsync_ = OH_NativeVSync_Create(vsync_name.data(), vsync_name.size());
  }

  user_data_ = std::make_unique<VSyncUserData>(
      base::SingleThreadTaskRunner::GetCurrentDefault(),
      weak_factory_.GetWeakPtr());
}

ExternalBeginFrameSourceOHOS::~ExternalBeginFrameSourceOHOS() {
  LOG(INFO) << "ExternalBeginFrameSourceOHOS destructor!!!";
  if (native_vsync_) {
    OH_NativeVSync_Destroy(native_vsync_);
  }
  SetEnabled(false);
}

void ExternalBeginFrameSourceOHOS::OnVSync(long long timestamp, void* data) {
  TRACE_EVENT0("viz", "OnVSync");
  if (data == nullptr) {
    LOG(ERROR) << "OnVSync data is nullptr";
    return;
  }
  VSyncUserData* user_data = reinterpret_cast<VSyncUserData*>(data);
  if (!user_data->current_) {
    LOG(ERROR) << "OnVSync data current is nullptr";
    return;
  }

  user_data->current_->PostTask(
      FROM_HERE, base::BindOnce(&ExternalBeginFrameSourceOHOS::OnVSyncImpl,
                                user_data->weak_ptr_, timestamp, user_data));
}

void ExternalBeginFrameSourceOHOS::OnVSyncImpl(int64_t timestamp,
                                               VSyncUserData* user_data) {
  TRACE_EVENT1("viz", "OnVSyncImpl", "enabled", vsync_notification_enabled_);
  int32_t ret = OH_NativeVSync_GetPeriod(native_vsync_, &vsync_period_);
  if (ret != 0) {
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
  user_data_.reset(user_data);

  if (!vsync_notification_enabled_ || user_data == nullptr) {
    return;
  }
  OH_NativeVSync_RequestFrame(native_vsync_,
                              ExternalBeginFrameSourceOHOS::OnVSync,
                              (void*)(user_data_.release()));
}

void ExternalBeginFrameSourceOHOS::OnNeedsBeginFrames(bool needs_begin_frames) {
  TRACE_EVENT1("viz", "OnNeedsBeginFrames", "enable", needs_begin_frames);
  SetEnabled(needs_begin_frames);
}

void ExternalBeginFrameSourceOHOS::SetEnabled(bool enabled) {
  TRACE_EVENT1("viz", "ExternalBeginFrameSourceOHOS::SetEnabled", "enable",
               enabled);
  if (vsync_notification_enabled_ == enabled) {
    return;
  }

  vsync_notification_enabled_ = enabled;
  if (vsync_notification_enabled_ && user_data_ != nullptr) {
    OH_NativeVSync_RequestFrame(native_vsync_,
                                ExternalBeginFrameSourceOHOS::OnVSync,
                                (void*)(user_data_.release()));
  }
}
}  // namespace viz
