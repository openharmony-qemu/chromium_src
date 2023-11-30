// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/init/gl_factory_ohos.h"

namespace gl {

namespace init {

scoped_refptr<GLSurface> CreateViewGLSurfaceOhos(
    GLDisplay* display,
    gfx::AcceleratedWidget window) {
  return InitializeGLSurface(
      NativeViewGLSurfaceEGLOhos::CreateNativeViewGLSurfaceEGLOhos(display,
                                                                   window));
}

}  // namespace init
}  // namespace gl
