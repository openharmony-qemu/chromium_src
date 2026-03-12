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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_DEVICE_GL_H_
#error "must be in include form COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_DEVICE_GL_H_"
#endif

#include "components/viz/service/display_embedder/skia_output_device_gl.h"

namespace viz {

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void SkiaOutputDeviceGL::SetBypassVsyncCondition(int32_t condition) {
  if (gl_surface_) {
    gl_surface_->SetBypassVsyncCondition(condition);
  }
}
#endif

#if BUILDFLAG(ARKWEB_PARTIAL_DRAW)
gfx::Rect SkiaOutputDeviceGL::GetLastBufferDamageRect() {
  if (gl_surface_) {
    return gl_surface_->GetLastBufferDamageRect();
  }
  return gfx::Rect();
}

int SkiaOutputDeviceGL::GetLastBufferAge() {
  if (gl_surface_) {
    return gl_surface_->GetPresentBufferAge();
  }
  return 0;
}

int SkiaOutputDeviceGL::GetLastBufferSameCnt() {
  if (gl_surface_) {
    return gl_surface_->GetSameBufferDamageCnt();
  }
  return 0;
}

bool SkiaOutputDeviceGL::SetPresentBufferDamageRect(gfx::Rect damage_rect, gfx::Rect curr_rect) {
  if (gl_surface_) {
    return gl_surface_->SetPresentBufferDamage(damage_rect, curr_rect);
  }
  return false;
}
#endif
} // namespace viz
