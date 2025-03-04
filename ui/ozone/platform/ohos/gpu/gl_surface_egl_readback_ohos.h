// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_GL_SURFACE_EGL_READBACK_OHOS_H_
#define UI_OZONE_PLATFORM_OHOS_GL_SURFACE_EGL_READBACK_OHOS_H_

#include "base/memory/raw_ptr.h"
#include "native_window/external_window.h"
#include "ui/ozone/common/gl_surface_egl_readback.h"

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
