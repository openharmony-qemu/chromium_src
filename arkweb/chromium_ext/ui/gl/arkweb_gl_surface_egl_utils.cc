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
#include "arkweb/chromium_ext/ui/gl/arkweb_gl_surface_egl_utils.h"
#include "ui/gl/gl_surface_egl.h"
#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"

#define GPU_WAIT_IO_TIMEOUT 3000

namespace gl {

ArkwebGlSurfaceEglUtils::ArkwebGlSurfaceEglUtils() {}

#if BUILDFLAG(ARKWEB_DFX_DUMP)
bool ArkwebGlSurfaceEglUtils::CheckSwapBufferOutputFlag() {
  bool enable_replace_swap_buffer_output =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetSystemPropertiesInstance()
          .GetBoolParameter("web.debug.eglSwapBuffersBackgroundColor", false);
  if (enable_replace_swap_buffer_output) {
    LOG(INFO) << "NativeViewGLSurfaceEGLOhos:: enable debug background color,"
                 " The rendering output will be replaced with green.";
  }
  return enable_replace_swap_buffer_output;
}

bool ArkwebGlSurfaceEglUtils::SwapBuffersSolution(bool enableReplaceOutput,
    bool isFirstSwapBuffer, gfx::Size size) {

  bool result = false;
  if (enableReplaceOutput) {
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  if (isFirstSwapBuffer) {
    result = false;
    LOG(INFO) << "web render log: first call SwapBuffersWithDamage, size = "
              << size.ToString();
  }

  return result;
}

void ArkwebGlSurfaceEglUtils::SwapBuffersWithDamageSolution(
    gfx::SwapResult buffer_result, std::chrono::steady_clock::time_point start, gfx::Size size) {
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();

  if (duration > kMaxSwapIntervalOhos) {
    LOG(WARNING) << "web render log: SwapBuffersWithDamage cost time = "
                 << duration << "ms" << ", swap result = "
                 << static_cast<int32_t>(buffer_result);
  }

  if (duration >= GPU_WAIT_IO_TIMEOUT) {
    ReportGpuProcessEvent(CrashType::TIMEOUT, "GPU Process SwapBuffer timeout(>3000ms) exceeded");
  }

  if (buffer_result == gfx::SwapResult::SWAP_FAILED) {
    LOG(ERROR) << "web render log: SwapBuffersWithDamage failed, size = "
               << size.ToString();
  }
}

#endif
} // namespace gl