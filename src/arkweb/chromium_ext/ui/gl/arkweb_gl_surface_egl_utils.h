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

#ifndef UI_GL_ARKWEB_GL_SURFACE_EGL_UTILS_H_
#define UI_GL_ARKWEB_GL_SURFACE_EGL_UTILS_H_

#include "arkweb/build/features/features.h"
#include "ui/gfx/gpu_fence.h"
#include "ui/gl/egl_util.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_display_egl_util.h"
#include "ui/gl/gl_display_manager.h"
#include "ui/gl/gl_surface_presentation_helper.h"
#include "ui/gl/gl_surface_stub.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/scoped_make_current.h"
#include "ui/gl/sync_control_vsync_provider.h"
#include "base/logging.h"
#include <vector>
#if BUILDFLAG(ARKWEB_DFX_DUMP)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

namespace gl {

class ArkwebGlSurfaceEglUtils {
public:
  ArkwebGlSurfaceEglUtils();

#if BUILDFLAG(ARKWEB_DFX_DUMP)
  bool CheckSwapBufferOutputFlag();
  bool SwapBuffersSolution(bool enableReplaceOutput,
    bool isFirstSwapBuffer, gfx::Size size);
  void SwapBuffersWithDamageSolution(
    gfx::SwapResult buffer_result, std::chrono::steady_clock::time_point start, gfx::Size size);
#endif

};

}  // namespace gl

#endif  // UI_GL_ARKWEB_GL_SURFACE_EGL_UTILS_H_
