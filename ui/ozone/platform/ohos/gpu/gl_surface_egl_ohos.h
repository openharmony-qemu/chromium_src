// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_GPU_GL_SURFACE_EGL_OHOS_H_
#define UI_OZONE_PLATFORM_OHOS_GPU_GL_SURFACE_EGL_OHOS_H_

#include "ui/gl/gl_surface_egl.h"

namespace gl {

class NativeViewGLSurfaceEGLOhos : public NativeViewGLSurfaceEGL {
 public:
  NativeViewGLSurfaceEGLOhos(
      GLDisplayEGL* display,
      EGLNativeWindowType window,
      std::unique_ptr<gfx::VSyncProvider> vsync_provider);

  NativeViewGLSurfaceEGLOhos(const NativeViewGLSurfaceEGLOhos&) = delete;
  NativeViewGLSurfaceEGLOhos& operator=(const NativeViewGLSurfaceEGLOhos&) =
      delete;

  gfx::Size GetSize() override;
  bool Resize(const gfx::Size& size,
              float scale_factor,
              const gfx::ColorSpace& color_space,
              bool has_alpha) override;
  bool SetBackbufferAllocation(bool allocated) override;

 private:
  ~NativeViewGLSurfaceEGLOhos() = default;
};

}  // namespace gl

#endif  // UI_OZONE_PLATFORM_OHOS_GPU_GL_SURFACE_EGL_OHOS_H_
