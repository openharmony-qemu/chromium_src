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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_DEVICE_VULKAN_UTILS_H_
#define COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_DEVICE_VULKAN_UTILS_H_

#include "components/viz/service/display_embedder/skia_output_device_vulkan.h"
#include "base/cancelable_callback.h"
#include "base/memory/raw_ptr.h"

namespace viz {
class SkiaOutputDeviceVulkan;

class SkiaOutputDeviceVulkanUtils {
public:
    raw_ptr<viz::SkiaOutputDeviceVulkan> deviceVulkan;
    SkiaOutputDeviceVulkanUtils(SkiaOutputDeviceVulkan* device);

    #if BUILDFLAG(ARKWEB_VULKAN)
        void DiscardBackbuffer();
    #endif

#if BUILDFLAG(ARKWEB_CLEAN_BUFFERS_WHEN_INVISIBLE)
    void SetIfNeedCleanBuffers(bool need_clean_buffers);
    void CleanBuffersIfNeed();
#endif
private:
#if BUILDFLAG(ARKWEB_CLEAN_BUFFERS_WHEN_INVISIBLE)
  bool clean_buffers_when_invisible_enabled_ = false;
  bool need_clean_buffers_ = false;
  bool do_clean_buffers_ = false;
  base::TimeTicks prev_clean_buffers_time_ = base::TimeTicks::Now();
  bool has_delay_clean_buffer_task_ = false;
  std::unique_ptr<base::CancelableOnceClosure> delay_clean_buffer_task_closure_;
  void DoCleanBuffers();
#endif
  base::WeakPtrFactory<SkiaOutputDeviceVulkanUtils> weak_ptr_factory_{this};
};
}  // namespace gpu

#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_DEVICE_VULKAN_UTILS_H_