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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_COMPOSITOR_GPU_THREAD_UTILS_H_
#define COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_COMPOSITOR_GPU_THREAD_UTILS_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
#include "arkweb/chromium_ext/gpu/ipc/common/nweb_native_window_tracker.h"
#include "base/process/process_handle.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

namespace viz {

class CompositorGpuThreadUtils
{
 public:
  static void CompositorGpuThreadDestruct(base::Thread* thread_ptr);
  static void CompositorGptThreadInitializeUtils(base::Thread* thread_ptr);
};

} // namespace viz

#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_COMPOSITOR_GPU_THREAD_UTILS_H_
