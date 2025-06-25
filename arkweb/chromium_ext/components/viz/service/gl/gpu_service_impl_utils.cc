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

#include "components/viz/service/gl/gpu_service_impl.h"

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
#include "gpu/ipc/common/gpu_surface_id_tracker.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
#include "arkweb/chromium_ext/base/ohos/dynamic_frame_loss_monitor.h"
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
#include "base/ohos/dynamic_frame_loss_monitor.h"
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#include "base/ohos/ltpo/include/sliding_observer.h"
#endif

#if BUILDFLAG(IS_OHOS)
#include "base/ohos/dynamic_frame_loss_monitor.h"
#include "gpu/ipc/common/gpu_surface_id_tracker.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#include "base/ohos/ltpo/include/sliding_observer.h"
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
#include "skia/ext/skia_memory_tracer.h"
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
#include "base/ohos/d_vsync/include/d_vsync_controller.h"
#endif

namespace viz {

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
void GpuServiceImpl::GetSurfaceId(int32_t native_embed_id,
                                  GetSurfaceIdCallback callback) {
  std::string res =
      gpu::GpuSurfaceIdTracker::Get()->AcquireNativeImageSurfaceId(
          native_embed_id);
  LOG(DEBUG) << "GetSurfaceId native_embed_id: " << native_embed_id
             << ", getSurfaceId: " << res;
  std::move(callback).Run(res);
}

void GpuServiceImpl::SetTransformHint(uint32_t rotation, uint32_t window_id) {
  void* window =
      NWebNativeWindowTracker::GetInstance()->GetNativeWindow(window_id);
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetWindowAdapterInstance()
      .SetTransformHint(rotation, window);
}

void GpuServiceImpl::DestroyNativeWindow(uint32_t native_window_id)
{
  if (io_runner_->BelongsToCurrentThread()) {
    compositor_gpu_task_runner()->PostTask(
        FROM_HERE, base::BindOnce(&GpuServiceImpl::DestroyNativeWindow,
                                  weak_ptr_, native_window_id));
    return;
  }
  LOG(DEBUG) << "GpuServiceImpl::DestroyNativeWindow native_window_id: "
             << native_window_id;
  NWebNativeWindowTracker::GetInstance()->DestroyNativeWindow(native_window_id);
}

void GpuServiceImpl::Discard(uint32_t native_window_id)
{
  void* window = NWebNativeWindowTracker::GetInstance()->GetNativeWindow(native_window_id);
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetWindowAdapterInstance()
      .NativeWindowSurfaceCleanCacheWithPara(reinterpret_cast<void*>(window), true);
}

#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void GpuServiceImpl::StartMonitor() {
  base::ohos::DynamicFrameLossMonitor::GetInstance().StartMonitor();
}

void GpuServiceImpl::StopMonitor() {
  base::ohos::DynamicFrameLossMonitor::GetInstance().StopMonitor();
}
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
void GpuServiceImpl::SetVisible(int32_t nweb_id, bool visible) {
  base::ohos::DynamicFrameRateDecision::GetInstance().SetVisible(nweb_id,
                                                                 visible);
}

void GpuServiceImpl::SetHasTouchPoint(bool has_touch_point) {
  base::ohos::DynamicFrameRateDecision::GetInstance().SetHasTouchPoint(
      has_touch_point);
}

void GpuServiceImpl::ReportSlidingFrameRate(int32_t frame_rate) {
  base::ohos::DynamicFrameRateDecision::GetInstance().ReportSlidingFrameRate(
      frame_rate);
}

void GpuServiceImpl::SetLTPOStrategy(int32_t strategy) {
  base::ohos::DynamicFrameRateDecision::GetInstance().SetLTPOStrategy(strategy);
}
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
void GpuServiceImpl::DumpGpuInfo(DumpGpuInfoCallback callback) {
  if (io_runner_->BelongsToCurrentThread()) {
    auto wrap_callback = base::BindPostTask(io_runner_, std::move(callback));
    compositor_gpu_task_runner()->PostTask(
        FROM_HERE, base::BindOnce(&GpuServiceImpl::DumpGpuInfo,
                                  weak_ptr_, std::move(wrap_callback)));
    return;
  }
  float totalSize = 0;
  if (compositor_gpu_thread_) {
    GrDirectContext* grContext =
        compositor_gpu_thread_->GetSharedContextState()->gr_context();
    auto skiaMemoryTracer =
        std::make_shared<SkiaMemoryTracer>("category", true);
    if (grContext != nullptr && skiaMemoryTracer) {
      grContext->dumpMemoryStatistics(skiaMemoryTracer.get());
      totalSize = skiaMemoryTracer->GetGpuMemorySizeInMB();
    }
  }
  std::move(callback).Run(totalSize);
}
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
void GpuServiceImpl::SetIsFling(bool is_fling_enabled) {
  base::ohos::DVsyncController::GetInstance().SetIsFling(is_fling_enabled);
}
#endif
 } // namespace viz