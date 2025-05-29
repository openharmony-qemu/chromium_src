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

#ifndef VIZ_SERVICE_MAIN_COMPOSITOR_THREAD_RUNNER_IMPL_UTILS_H_
#define VIZ_SERVICE_MAIN_COMPOSITOR_THREAD_RUNNER_IMPL_UTILS_H_

#include "arkweb/build/features/features.h"
#include "base/command_line.h"
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
#include "arkweb/chromium_ext/base/process/process_handle_posix_ex.h"
#include "arkweb/chromium_ext/gpu/ipc/common/nweb_native_window_tracker.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/content_switches.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif
#include "base/task/single_thread_task_runner.h"
#include "base/threading/thread.h"

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#endif

namespace viz {
class VizCompositorThreadRunnerImpl;

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
void ReportThreadForInit(const std::unique_ptr<base::Thread>& thread);
#endif

class VizCompositorThreadRunnerImplUtils {
public:
  VizCompositorThreadRunnerImpl* VizCompositorThreadRunner;
  VizCompositorThreadRunnerImplUtils(VizCompositorThreadRunnerImpl* impl);
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  void ReportThreadForDestroy(const std::unique_ptr<base::Thread>& thread);
#endif
};

}
#endif
