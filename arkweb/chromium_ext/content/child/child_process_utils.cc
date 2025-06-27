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

#include "arkweb/chromium_ext/base/process/process_handle_posix_ex.h"
#include "arkweb/chromium_ext/content/child/child_process_utils.h"
#include "content/child/child_process.h"
#include "content/child/child_thread_impl.h"
#include "content/public/common/content_switches.h"

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
#include "arkweb/chromium_ext/gpu/ipc/common/nweb_native_window_tracker.h"
#include "base/threading/platform_thread.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

namespace content {
ChildProcessUtils::ChildProcessUtils(ChildProcess* impl) {
  this->childProcess = impl;
}

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void ChildProcessUtils::ReportIoThreadStatus(bool is_created, bool is_in_browser_process) {
  if (!childProcess->main_thread_) {
    return;
  }

  using namespace OHOS::NWeb;
  ResSchedStatusAdapter status = is_created
                                     ? ResSchedStatusAdapter::THREAD_CREATED
                                     : ResSchedStatusAdapter::THREAD_DESTROYED;
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      switches::kProcessType);
#endif  // BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  // If this thread is in browser process, then report key thread info to RSS
  // directly. Otherwise, report key thread info to the browser process firstly.
  if (is_in_browser_process) {
    ResSchedClientAdapter::ReportKeyThread(status, base::GetCurrentRealPid(),
                                           childProcess->io_thread_->GetThreadRealId(),
                                           ResSchedRoleAdapter::USER_INTERACT);
  }
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  else if (type == switches::kGpuProcess) {
    if (NWebNativeWindowTracker::Get() &&
        NWebNativeWindowTracker::Get()->g_browser_client_) {
      LOG(DEBUG) << "get native window success pid:"
                 << base::GetCurrentRealPid()
                 << ", tid = " << childProcess->io_thread_->GetThreadRealId();
      NWebNativeWindowTracker::Get()->g_browser_client_->ReportThread(
          status, base::GetCurrentRealPid(), childProcess->io_thread_->GetThreadRealId(),
          ResSchedRoleAdapter::USER_INTERACT);
    }
  }
#endif  // BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  else {
    childProcess->main_thread_->ReportKeyThread(
        static_cast<int32_t>(status), base::GetCurrentRealPid(),
        childProcess->io_thread_->GetThreadRealId(),
        static_cast<int32_t>(ResSchedRoleAdapter::USER_INTERACT));
  }
}

void ChildProcessUtils::ReportCompositorKeyThread(bool is_created, bool is_in_browser_process) {
  if (!childProcess->main_thread_) {
    LOG(WARNING) << "main thread is nullptr, can not report key"
                 << base::PlatformThread::CurrentRealId()
                 << " id created: " << is_created;
    return;
  }
  if (is_in_browser_process) {
    return;
  }
  using namespace OHOS::NWeb;
  ResSchedStatusAdapter status = is_created
                                     ? ResSchedStatusAdapter::THREAD_CREATED
                                     : ResSchedStatusAdapter::THREAD_DESTROYED;
  childProcess->main_thread_->ReportKeyThread(
      static_cast<int32_t>(status), base::GetCurrentRealPid(),
      base::PlatformThread::CurrentRealId(),
      static_cast<int32_t>(ResSchedRoleAdapter::IMPORTANT_DISPLAY));
  LOG(DEBUG) << "child process pid: " << base::GetCurrentRealPid()
             << ", tid: " << base::PlatformThread::CurrentRealId()
             << " id created: " << is_created;
}
#endif // !BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)

#if BUILDFLAG(ARKWEB_DFX_TRACING)
void ChildProcessUtils::ReportHisyevent(int64_t block_time, const std::string& mode, bool is_in_browser_process) {
  if (!childProcess->main_thread_) {
    LOG(WARNING) << "main thread is nullptr";
    return;
  }
  if(is_in_browser_process) {
    return;
  }
  childProcess->main_thread_->ReportHisyevent(block_time, mode);
}

void ChildProcess::ReportHisyevent(int64_t block_time, const std::string& mode) {
  if (implUtils) {
    if (!main_thread_) {
      implUtils->ReportHisyevent(block_time, mode, main_thread_->IsInBrowserProcess());
    }
  }
}
#endif
}