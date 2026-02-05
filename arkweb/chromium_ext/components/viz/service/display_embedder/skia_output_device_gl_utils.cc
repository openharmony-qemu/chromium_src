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

#include "arkweb/chromium_ext/components/viz/service/display_embedder/skia_output_device_gl_utils.h"

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/task/single_thread_task_runner.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"

namespace viz {
//LCOV_EXCL_START
SkiaOutputDeviceGLUtils::SkiaOutputDeviceGLUtils(SkiaOutputDeviceGL* impl)
    : skiaOutPutDeviceGl_(impl)
{
  supports_damage_region_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                .GetSystemPropertiesInstance()
                                .GetBoolParameter("web.damageRegion.enable", 1);
}
//LCOV_EXCL_STOP

gfx::SwapResult SkiaOutputDeviceGLUtils::SwapBuffers(
    const std::optional<gfx::Rect>& update_rect,
    SkiaOutputDevice::BufferPresentedCallback& feedback,
    OutputSurfaceFrame& frame)
{
  gfx::SwapResult result;
  auto data = frame.data;
  if (!base::ohos::IsPageScale() && supports_damage_region_) {
    result = skiaOutPutDeviceGl_->gl_surface_->SwapBuffersWithDamage(
        {update_rect->x(),
         skiaOutPutDeviceGl_->gl_surface_->GetSize().height() -
             update_rect->y() - update_rect->height(),
         update_rect->width(), update_rect->height()},
        std::move(feedback), std::move(data));
    LOG(DEBUG) << "Present calling SwapBuffersWithDamage [" << update_rect->x()
               << ", "
               << skiaOutPutDeviceGl_->gl_surface_->GetSize().height() -
                      update_rect->y() - update_rect->height()
               << ", " << update_rect->width() << ", " << update_rect->height()
               << "]";
#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
    CleanBufferAfterSwapBuffer(result);
#endif
  } else {
    result = skiaOutPutDeviceGl_->gl_surface_->SwapBuffers(std::move(feedback),
                                                           std::move(data));
  }
  return result;
}

#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
void SkiaOutputDeviceGLUtils::SetDelayClean(bool delay_clean) {
  delay_clean_ = delay_clean;
}

void SkiaOutputDeviceGLUtils::CleanBufferAfterSwapBuffer(gfx::SwapResult result) {
  if (result == gfx::SwapResult::SWAP_ACK && delay_clean_) {
    LOG(DEBUG) << "Post delay task clean offline buffer";
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
      FROM_HERE,
      base::BindOnce(&SkiaOutputDeviceGLUtils::CleanOfflineBuffer, weak_ptr_factory_.GetWeakPtr()),
        base::Milliseconds(100)); // 100: delay 100ms.
  }
}

void SkiaOutputDeviceGLUtils::CleanOfflineBuffer() {
  if (delay_clean_) {
    delay_clean_ = false;
    if (!gl::GLContext::GetCurrent() || !gl::GLSurface::GetCurrent()) {
      LOG(ERROR) << "context or surface is nullptr";
      return;
    }
    if (skiaOutPutDeviceGl_ && skiaOutPutDeviceGl_->gl_surface_) {
      LOG(DEBUG) << "SkiaOutputDeviceGLUtils::CleanOfflineBuffer";
      skiaOutPutDeviceGl_->gl_surface_->Recreate();
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_CLEAN_BUFFERS_WHEN_INVISIBLE)
void SkiaOutputDeviceGLUtils::SetIfNeedCleanBuffers(bool need_clean_buffers)
{
  need_clean_buffers_ = need_clean_buffers;
}

// When the web component is invisible clean it's buffers after finish swap buffers.
void SkiaOutputDeviceGLUtils::CleanBuffersIfNeed()
{
  if (!need_clean_buffers_) {
    return;
  }

  base::TimeDelta delta = base::TimeTicks::Now() - prev_clean_buffers_time_;
  if (delta.InMilliseconds() < CLEAN_BUFFERS_TIME_INTERVAL_MIN && !has_delay_clean_buffer_task) {
    do_clean_buffers_ = true;
    delay_clean_buffer_task_closure_ = std::make_unique<base::CancelableOnceClosure>(
      base::BindOnce(&SkiaOutputDeviceGLUtils::DoCleanBuffers, weak_ptr_factory_.GetWeakPtr()));
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
      FROM_HERE,
      delay_clean_buffer_task_closure_->callback(),
      base::Milliseconds(CLEAN_BUFFERS_TIME_INTERVAL_MIN * 2));
    has_delay_clean_buffer_task = true;
    return;
  }
  do_clean_buffers_ = true;
  DoCleanBuffers();
}

void SkiaOutputDeviceGLUtils::DoCleanBuffers()
{
  if (!need_clean_buffers_ || !do_clean_buffers_) {
    return;
  }

  if (!gl::GLContext::GetCurrent() || !gl::GLSurface::GetCurrent()) {
    LOG(ERROR) << "Clean buffers failed for context or surface is nullptr";
    return;
  }
  if (!skiaOutPutDeviceGl_ || !skiaOutPutDeviceGl_->gl_surface_) {
    LOG(ERROR) << "Clean buffers failed for glSurface is nullptr";
    return;
  }
  skiaOutPutDeviceGl_->gl_surface_->Recreate();
  prev_clean_buffers_time_ = base::TimeTicks::Now();
  do_clean_buffers_ = false;
  if (delay_clean_buffer_task_closure_) {
    delay_clean_buffer_task_closure_->Cancel();
    has_delay_clean_buffer_task = false;
  }
}
#endif
}  // namespace viz
