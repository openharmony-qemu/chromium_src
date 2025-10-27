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
#include "arkweb/chromium_ext/components/viz/service/display_embedder/compositor_gpu_thread_utils.h"

#include "base/command_line.h"
#include "base/threading/thread.h"

namespace viz {

namespace {
constexpr char kProcessType[] = "type";
constexpr char kGpuProcess[] = "gpu-process";
}  // namespace

void CompositorGpuThreadUtils::CompositorGpuThreadDestruct(base::Thread* thread_ptr)
{
  using namespace OHOS::NWeb;
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      kProcessType);
  if (type == kGpuProcess) {
    NWebNativeWindowTracker::Get()->g_browser_client_->ReportThread(
        ResSchedStatusAdapter::THREAD_DESTROYED, base::GetCurrentRealPid(),
        thread_ptr->GetThreadRealId(), ResSchedRoleAdapter::IMPORTANT_DISPLAY);
    return;
  }
#endif // BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  thread_ptr->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(
          base::IgnoreResult(&ResSchedClientAdapter::ReportKeyThread),
          ResSchedStatusAdapter::THREAD_DESTROYED, base::GetCurrentRealPid(),
          thread_ptr->GetThreadRealId(), ResSchedRoleAdapter::IMPORTANT_DISPLAY));
#endif // // BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
}

void CompositorGpuThreadUtils::CompositorGptThreadInitializeUtils(base::Thread* thread_ptr)
{
  using namespace OHOS::NWeb;
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      kProcessType);
  if (type == kGpuProcess) {
      NWebNativeWindowTracker::Get()->g_browser_client_->ReportThread(
          ResSchedStatusAdapter::THREAD_CREATED, base::GetCurrentRealPid(),
          thread_ptr->GetThreadRealId(), ResSchedRoleAdapter::IMPORTANT_DISPLAY);
      return;
  }
#endif  // BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  thread_ptr->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(
          base::IgnoreResult(&ResSchedClientAdapter::ReportKeyThread),
          ResSchedStatusAdapter::THREAD_CREATED, base::GetCurrentRealPid(),
          thread_ptr->GetThreadRealId(), ResSchedRoleAdapter::IMPORTANT_DISPLAY));
#endif  // BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
}

} // namespace viz
