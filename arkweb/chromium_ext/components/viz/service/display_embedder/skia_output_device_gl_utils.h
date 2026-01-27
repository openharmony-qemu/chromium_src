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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_DEVICE_GL_UTILS_H
#define COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_DEVICE_GL_UTILS_H

#include "components/viz/service/display_embedder/skia_output_device_gl.h"
#include "base/memory/raw_ptr.h"

namespace viz {

class SkiaOutputDeviceGLUtils {
 public:
  SkiaOutputDeviceGLUtils(SkiaOutputDeviceGL* impl);
  gfx::SwapResult SwapBuffers(const std::optional<gfx::Rect>& update_rect,
                              SkiaOutputDevice::BufferPresentedCallback& feedback,
                              OutputSurfaceFrame& frame);
#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
  void SetDelayClean(bool delay_clean);
  void CleanBufferAfterSwapBuffer(gfx::SwapResult result);
  void CleanOfflineBuffer();
#endif

 private:
  raw_ptr<SkiaOutputDeviceGL> skiaOutPutDeviceGl_;
  bool supports_damage_region_;
#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
  bool delay_clean_ = false;
  base::WeakPtrFactory<SkiaOutputDeviceGLUtils> weak_ptr_factory_{this};
#endif
};
}  // namespace viz
#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_DEVICE_GL_UTILS_H
