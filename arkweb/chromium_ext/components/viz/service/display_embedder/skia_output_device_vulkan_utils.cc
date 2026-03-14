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
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace viz {
#if BUILDFLAG(ARKWEB_VULKAN)
const int bufferSize = 2;
#endif
#if BUILDFLAG(ARKWEB_CLEAN_BUFFERS_WHEN_INVISIBLE)
// 100 ms : minimum time interval between two clean buffers 
static const int32_t CLEAN_BUFFERS_TIME_INTERVAL_MIN = 100;
#endif

SkiaOutputDeviceVulkanUtils::SkiaOutputDeviceVulkanUtils(SkiaOutputDeviceVulkan* device)
{
    this->deviceVulkan = device;
#if BUILDFLAG(ARKWEB_CLEAN_BUFFERS_WHEN_INVISIBLE)
  clean_buffers_when_invisible_enabled_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .GetSystemPropertiesInstance().GetBoolParameter("const.web.clean_buffers_when_invisible.enabled", false);
#endif
}

#if BUILDFLAG(ARKWEB_VULKAN)
void SkiaOutputDeviceVulkanUtils::DiscardBackbuffer()
{
  TRACE_EVENT0("base", "SkiaOutputDeviceVulkan::DiscardBackbuffer");
  deviceVulkan->vulkan_surface_->Reshape(gfx::Size(bufferSize, bufferSize),
                                         gfx::OverlayTransform::OVERLAY_TRANSFORM_INVALID);
}
#endif

#if BUILDFLAG(ARKWEB_CLEAN_BUFFERS_WHEN_INVISIBLE)
void SkiaOutputDeviceVulkanUtils::SetIfNeedCleanBuffers(bool need_clean_buffers)
{
  TRACE_EVENT1("base", "SkiaOutputDeviceVulkanUtils::SetIfNeedCleanBuffers ",
               "need_clean_buffers:", need_clean_buffers);
  need_clean_buffers_ = need_clean_buffers;
}

// When the web component is invisible clean its buffers after finish swap buffers.
void SkiaOutputDeviceVulkanUtils::CleanBuffersIfNeed()
{
  if (!clean_buffers_when_invisible_enabled_ || !need_clean_buffers_) {
    return;
  }
  base::TimeDelta delta = base::TimeTicks::Now() - prev_clean_buffers_time_;
  if (delta.InMilliseconds() < CLEAN_BUFFERS_TIME_INTERVAL_MIN) {
    if (!has_delay_clean_buffer_task_) {
      do_clean_buffers_ = true;
      delay_clean_buffer_task_closure_ = std::make_unique<base::CancelableOnceClosure>(
        base::BindOnce(&SkiaOutputDeviceVulkanUtils::DoCleanBuffers, weak_ptr_factory_.GetWeakPtr()));
      base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
        FROM_HERE,
        delay_clean_buffer_task_closure_->callback(),
        base::Milliseconds(CLEAN_BUFFERS_TIME_INTERVAL_MIN * 2));
      has_delay_clean_buffer_task_ = true;
    }
    return;
  }
  do_clean_buffers_ = true;
  DoCleanBuffers();
}

void SkiaOutputDeviceVulkanUtils::DoCleanBuffers()
{
  if (!clean_buffers_when_invisible_enabled_ || !need_clean_buffers_ || !do_clean_buffers_) {
    return;
  }

  if (!deviceVulkan || !deviceVulkan->vulkan_surface_) {
    LOG(ERROR) << "Clean buffers failed for vulkan or Surface is nullptr";
    return;
  }
  TRACE_EVENT0("base", "SkiaOutputDeviceVulkanUtils::DoCleanBuffers");
#if BUILDFLAG(ARKWEB_VULKAN)
  DiscardBackbuffer();
#endif
  prev_clean_buffers_time_ = base::TimeTicks::Now();
  do_clean_buffers_ = false;
  if (delay_clean_buffer_task_closure_) {
    delay_clean_buffer_task_closure_->Cancel();
    has_delay_clean_buffer_task_ = false;
  }
}
#endif
}
