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

#ifndef UI_OZONE_PLATFORM_OHOS_GL_SURFACE_EGL_READBACK_OHOS_H_
#define UI_OZONE_PLATFORM_OHOS_GL_SURFACE_EGL_READBACK_OHOS_H_

#include "base/memory/raw_ptr.h"
#include "ui/ozone/common/gl_surface_egl_readback.h"
#include "native_window/external_window.h"

namespace ui {

class GLSurfaceEglReadbackOhos : public GLSurfaceEglReadback {
 public:
  GLSurfaceEglReadbackOhos(gl::GLDisplayEGL* display,
                           gfx::AcceleratedWidget window);

  GLSurfaceEglReadbackOhos(const GLSurfaceEglReadbackOhos&) = delete;
  GLSurfaceEglReadbackOhos& operator=(const GLSurfaceEglReadbackOhos&) = delete;

  bool Initialize(gl::GLSurfaceFormat format) override;
  void Destroy() override;

 private:
  ~GLSurfaceEglReadbackOhos() override;
  bool CopyPixelsAndFlush(OHNativeWindow* nativeWindow,
                          uint32_t width_src,
                          uint32_t height_src,
                          uint8_t* pixels);
  bool HandlePixels(uint8_t* pixels) override;
  gfx::AcceleratedWidget window_;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_GL_SURFACE_EGL_READBACK_OHOS_H_
