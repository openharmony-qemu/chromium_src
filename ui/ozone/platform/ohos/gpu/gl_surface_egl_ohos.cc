/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ui/ozone/platform/ohos/gpu/gl_surface_egl_ohos.h"

#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/trace_event/trace_event.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_display.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_surface_egl.h"
#include "native_window/external_window.h"

namespace gl {

gl::NativeViewGLSurfaceEGLOhos::NativeViewGLSurfaceEGLOhos(
    GLDisplayEGL* display,
    EGLNativeWindowType window,
    std::unique_ptr<gfx::VSyncProvider> vsync_provider)
    : NativeViewGLSurfaceEGL(display, window, std::move(vsync_provider)) {}

gfx::Size NativeViewGLSurfaceEGLOhos::GetSize() {
  return size_;
}

bool NativeViewGLSurfaceEGLOhos::Resize(const gfx::Size& size,
                                        float scale_factor,
                                        const gfx::ColorSpace& color_space,
                                        bool has_alpha) {
  if (size == size_)
    return true;

  size_ = size;

  TRACE_EVENT0("gpu", "NativeViewGLSurfaceEGLOhos::Resize");

  int32_t ret = OH_NativeWindow_NativeWindowHandleOpt(reinterpret_cast<OHNativeWindow*>(window_),
                                                      SET_BUFFER_GEOMETRY,
                                                      size_.width(),
                                                      size_.height());
  if (ret != 0) {
    LOG(ERROR) << "NativeViewGLSurfaceEGLOhos::Resize error: " << ret;
    return false;
  }
  return true;
}

}  // namespace gl
