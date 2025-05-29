// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/gpu/gl_surface_egl_ohos.h"

#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/trace_event/trace_event.h"
#include "native_window/external_window.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_display.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_surface_egl.h"

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
  if (size == size_) {
    return true;
  }

  size_ = size;

  TRACE_EVENT0("gpu", "NativeViewGLSurfaceEGLOhos::Resize");

  int32_t ret = OH_NativeWindow_NativeWindowHandleOpt(
      reinterpret_cast<OHNativeWindow*>(window_), SET_BUFFER_GEOMETRY,
      size_.width(), size_.height());
  if (ret != 0) {
    LOG(ERROR) << "NativeViewGLSurfaceEGLOhos::Resize error: " << ret;
    return false;
  }
  return true;
}

}  // namespace gl
