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

#include "arkweb/chromium_ext/components/viz/service/frame_sinks/compositor_frame_sink_impl_utils.h"
#include "arkweb/chromium_ext/components/viz/service/frame_sinks/compositor_frame_sink_support_utils.h"

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
#include "arkweb/chromium_ext/gpu/ipc/common/nweb_native_window_tracker.h"
#include "base/command_line.h"
#include "base/system/sys_info.h"
#include "components/viz/service/frame_sinks/compositor_frame_sink_impl.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
#include "ohos_nweb/src/sysevent/event_reporter.h"
#endif

namespace viz {

namespace {
constexpr char kProcessType[] = "type";
constexpr char kGpuProcess[] = "gpu-process";
}  // namespace

//LCOV_EXCL_START
CompositorFrameSinkImplUtil::CompositorFrameSinkImplUtil(CompositorFrameSinkImpl* impl) {
  this->compositorFrameSinkImpl_ = impl;
}
//LCOV_EXCL_STOP
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
void CompositorFrameSinkImplUtil::SubmitCompositorFrameUtils(const CompositorFrameMetadata& metadata) {
  uint32_t count = static_cast<uint32_t>(metadata.dropped_frame_count);
  uint64_t duration = static_cast<uint64_t>(metadata.dropped_frame_duration);
  if (!!count && !!duration) {
    ReportVideoFrameDropStats(count, duration);
  }
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void CompositorFrameSinkImplUtil::ReportKeyThreadIdsUtils(const std::vector<int32_t>& thread_ids,
    int32_t process_id,
    bool is_created) {
  using namespace OHOS::NWeb;
  ResSchedStatusAdapter status = is_created
                                     ? ResSchedStatusAdapter::THREAD_CREATED
                                     : ResSchedStatusAdapter::THREAD_DESTROYED;
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      kProcessType);
  if (type == kGpuProcess) {
    for (auto thread_id : thread_ids) {
      NWebNativeWindowTracker::Get()->g_browser_client_->ReportThread(
          status, process_id, thread_id,
          ResSchedRoleAdapter::IMPORTANT_DISPLAY);
    }
  }
#else
  for (auto thread_id : thread_ids) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(
            base::IgnoreResult(&ResSchedClientAdapter::ReportKeyThread),
            status, process_id, thread_id,
            ResSchedRoleAdapter::IMPORTANT_DISPLAY));
  }
#endif  // BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
}
#endif  // BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)

//LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
int CompositorFrameSinkImplUtil::GetFrameRate() {
  if (compositorFrameSinkImpl_ && compositorFrameSinkImpl_->support_) {
    return (compositorFrameSinkImpl_->support_)->supportUtils->GetFrameRate();
  }
  return 0;
}
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void CompositorFrameSinkImplUtil::OnSetBypassVsyncCondition(int32_t condition) {
  if (compositorFrameSinkImpl_ && compositorFrameSinkImpl_->support_) {
    (compositorFrameSinkImpl_->support_)->supportUtils->OnSetBypassVsyncCondition(condition);
  }
}
#endif

#if BUILDFLAG(ARKWEB_THROTTLE_FRAME)
void CompositorFrameSinkImplUtil::UpdateThrottleMode(bool is_enable) {
  if (compositorFrameSinkImpl_ && compositorFrameSinkImpl_->support_) {
    (compositorFrameSinkImpl_->support_)->supportUtils->UpdateThrottleMode(is_enable);
  }
}
#endif
//LCOV_EXCL_STOP

}  // namespace cc
