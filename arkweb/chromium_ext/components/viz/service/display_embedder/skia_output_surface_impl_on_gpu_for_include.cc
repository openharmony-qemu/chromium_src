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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_SURFACE_IMPL_ON_GPU_H_
#error "must be in include form COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_SURFACE_IMPL_ON_GPU_H_"
#endif

#include "components/viz/service/display_embedder/skia_output_surface_impl_on_gpu.h"
#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_D_VSYNC)
#include "arkweb/chromium_ext/base/ohos/d_vsync/include/d_vsync_controller.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ohos_glue/base/include/ark_web_errno.h"
#endif
namespace viz {

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void SkiaOutputSurfaceImplOnGpu::SetBypassVsyncCondition(int32_t condition) {
  if (output_device_) {
    output_device_->SetBypassVsyncCondition(condition);
  }
}
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
void SkiaOutputSurfaceImplOnGpu::SetDVsyncIfNecessary() {
  static int delay_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                      .GetSystemPropertiesInstance().GetIntParameter("web.dvsync.delay", -1);

  if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
    LOG(DEBUG) << "SkiaOutputSurfaceImplOnGpu::SetDVsyncIfNecessary FAILED, cannot get delay_";
    return;
  }

  if (delay_ == -1) {
    return;
  }

  auto* host = content::GpuProcessHost::Get();
  if (!host) {
    return;
  } 

  auto* host_impl = host->gpu_host();
  if (!host_impl) {
    return;
  }

  if (!dependency_) {
    LOG(DEBUG) << "SkiaOutputSurfaceImplOnGpu::SetDVsyncIfNecessary FAILED, dependency_ is null";
    return;
  }

  bool is_scroll = dependency_->GetIsScroll();
  if (is_scroll && !did_dvsync_on_) {
    if (delay_num_ == delay_) {
      TRACE_EVENT0("viz", "SkiaOutputSurfaceImplOnGpu::SetDVsyncIfNecessary::SetIsFling TRUE");
      base::ohos::DVsyncController::GetInstance().SetIsFling(true);
      did_dvsync_on_ = true;
      delay_num_ = 0;
    } else {
      delay_num_++;
    }
  } else if (!is_scroll) {
    delay_num_ = 0;
    if (did_dvsync_on_) {
      TRACE_EVENT0("viz", "SkiaOutputSurfaceImplOnGpu::SetDVsyncIfNecessary::SetIsFling FALSE");
      base::ohos::DVsyncController::GetInstance().SetIsFling(false);
      did_dvsync_on_ = false;
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_PARTIAL_DRAW)
gfx::Rect SkiaOutputSurfaceImplOnGpu::GetLastBufferDamageRect() {
  if (output_device_) {
    return output_device_->GetLastBufferDamageRect();
  }
  return gfx::Rect();
}

int SkiaOutputSurfaceImplOnGpu::GetLastBufferAge() {
  if (output_device_) {
    return output_device_->GetLastBufferAge();
  }
  return 0;
}

int SkiaOutputSurfaceImplOnGpu::GetLastBufferSameCnt() {
  if (output_device_) {
    return output_device_->GetLastBufferSameCnt();
  }
  return 0;
}

void SkiaOutputSurfaceImplOnGpu::SetPresentBufferDamageRect(gfx::Rect damage_rect, gfx::Rect curr_rect) {
  if (output_device_) {
    (void)output_device_->SetPresentBufferDamageRect(damage_rect, curr_rect);
  }
}
#endif

} // namespace viz
