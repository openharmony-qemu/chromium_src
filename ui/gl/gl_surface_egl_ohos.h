// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_SURFACE_EGL_OHOS_H_
#define UI_GL_GL_SURFACE_EGL_OHOS_H_

#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_export.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/gl/gl_display.h"

namespace gl {
typedef struct WindowsSurfaceInfoTag {
  void* window;
  EGLDisplay display;
  EGLContext context;
  EGLSurface surface;
} WindowsSurfaceInfo;

class GL_EXPORT NativeViewGLSurfaceEGLOhos : public NativeViewGLSurfaceEGL {
 public:
  explicit NativeViewGLSurfaceEGLOhos(GLDisplayEGL* display,
                                NativeWindowType window);
  NativeViewGLSurfaceEGLOhos(const NativeViewGLSurfaceEGLOhos& other) = delete;
  NativeViewGLSurfaceEGLOhos& operator=(const NativeViewGLSurfaceEGLOhos& rhs) =
      delete;

  static scoped_refptr<gl::NativeViewGLSurfaceEGLOhos>
  CreateNativeViewGLSurfaceEGLOhos(GLDisplay* display, gfx::AcceleratedWidget window);
  gfx::SwapResult SwapBuffers(PresentationCallback callback, gfx::FrameData data) override;
  bool Resize(const gfx::Size& size,
              float scale_factor,
              const gfx::ColorSpace& color_space,
              bool has_alpha) override;
  bool SetBackbufferAllocation(bool allocated) override;
  
 private:
  EGLNativeWindowType window_;
};

}  // namespace gl

#endif  // UI_GL_GL_SURFACE_EGL_OHOS_H_

