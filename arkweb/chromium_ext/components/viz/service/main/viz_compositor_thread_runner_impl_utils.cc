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

#include "arkweb/chromium_ext/components/viz/service/main/viz_compositor_thread_runner_impl_utils.h"
#include "components/viz/service/main/viz_compositor_thread_runner_impl.h"

namespace viz {
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
void ReportThreadForInit(const std::unique_ptr<base::Thread>& thread)
{
  using namespace OHOS::NWeb;
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      switches::kProcessType);
  if (type == switches::kGpuProcess) {
    NWebNativeWindowTracker::Get()->g_browser_client_->ReportThread(
        ResSchedStatusAdapter::THREAD_CREATED, base::GetCurrentRealPid(),
        thread->GetThreadRealId(), ResSchedRoleAdapter::IMPORTANT_DISPLAY);
  } else {
    thread->task_runner()->PostTask(
        FROM_HERE,
        base::BindOnce(
            base::IgnoreResult(&ResSchedClientAdapter::ReportKeyThread),
            ResSchedStatusAdapter::THREAD_CREATED, base::GetCurrentRealPid(),
            thread->GetThreadRealId(), ResSchedRoleAdapter::IMPORTANT_DISPLAY));
  }
  thread->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(
          &base::ohos::DynamicFrameRateDecision::Init,
          base::Unretained(
              &base::ohos::DynamicFrameRateDecision::GetInstance())));
}
#endif

VizCompositorThreadRunnerImplUtils::VizCompositorThreadRunnerImplUtils(VizCompositorThreadRunnerImpl* impl)
{
    this->VizCompositorThreadRunner = impl;
}

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
void VizCompositorThreadRunnerImplUtils::ReportThreadForDestroy(const std::unique_ptr<base::Thread>& thread)
{
  using namespace OHOS::NWeb;
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      switches::kProcessType);
  if (type == switches::kGpuProcess) {
    NWebNativeWindowTracker::Get()->g_browser_client_->ReportThread(
        ResSchedStatusAdapter::THREAD_DESTROYED, base::GetCurrentRealPid(),
        thread->GetThreadRealId(), ResSchedRoleAdapter::IMPORTANT_DISPLAY);
  } else {
    thread->task_runner()->PostTask(
        FROM_HERE,
        base::BindOnce(
            base::IgnoreResult(&ResSchedClientAdapter::ReportKeyThread),
            ResSchedStatusAdapter::THREAD_DESTROYED, base::GetCurrentRealPid(),
            thread->GetThreadRealId(),
            ResSchedRoleAdapter::IMPORTANT_DISPLAY));
  }
}
#endif
}