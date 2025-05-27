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

#include "arkweb/chromium_ext/components/viz/service/display_embedder/skia_output_device_vulkan_utils.h"
#include "components/viz/service/display_embedder/skia_output_device_vulkan.h"
#include "gpu/vulkan/vulkan_surface.h"
#include "base/trace_event/trace_event.h"

namespace viz {
#if BUILDFLAG(ARKWEB_VULKAN)
const int bufferSize = 2;
#endif
 
SkiaOutputDeviceVulkanUtils::SkiaOutputDeviceVulkanUtils(SkiaOutputDeviceVulkan* device)
{
    this->deviceVulkan = device;
}

#if BUILDFLAG(ARKWEB_VULKAN)
void SkiaOutputDeviceVulkanUtils::DiscardBackbuffer()
{
  TRACE_EVENT0("base", "SkiaOutputDeviceVulkan::DiscardBackbuffer");
  deviceVulkan->vulkan_surface_->Reshape(gfx::Size(bufferSize, bufferSize),
                                         gfx::OverlayTransform::OVERLAY_TRANSFORM_INVALID);
}
#endif
}
