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

namespace viz {

class SkiaOutputDeviceGLUtils {
 public:
  SkiaOutputDeviceGLUtils(SkiaOutputDeviceGL* impl);
  gfx::SwapResult SwapBuffers(const std::optional<gfx::Rect>& update_rect,
                              SkiaOutputDevice::BufferPresentedCallback& feedback,
                              OutputSurfaceFrame& frame);

 private:
  SkiaOutputDeviceGL* skiaOutPutDeviceGl_;
  bool supports_damage_region_;
};
}  // namespace viz
#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_DEVICE_GL_UTILS_H
