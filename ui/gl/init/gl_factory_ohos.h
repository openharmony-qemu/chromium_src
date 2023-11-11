// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_INIT_GL_FACTORY_OHOS_H_
#define UI_GL_INIT_GL_FACTORY_OHOS_H_

#include "base/memory/ref_counted.h"
#include "ui/gl/gl_surface_egl_ohos.h"
namespace gl {

class GLSurface;

namespace init {

// Creates a GL surface that renders directly to a view.
scoped_refptr<GLSurface> CreateViewGLSurfaceOhos(gfx::AcceleratedWidget window);

}  // namespace init

}  // namespace gl

#endif  // UI_GL_INIT_GL_FACTORY_OHOS_H_
