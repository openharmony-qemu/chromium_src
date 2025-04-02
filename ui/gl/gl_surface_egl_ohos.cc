// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_surface_egl_ohos.h"
#include "base/ohos/sys_info_utils.h"
#include "base/threading/platform_thread.h"
#include "base/trace_event/trace_event.h"
#include "content/public/common/content_switches.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"

#include "ohos_adapter_helper.h"
#include "base/logging.h"
namespace gl {

namespace {
constexpr uint32_t kMaxSwapInterval = 16;
}

const std::string PRODUCT_MODEL_EMULATOR = "emulator";

scoped_refptr<gl::NativeViewGLSurfaceEGLOhos>
NativeViewGLSurfaceEGLOhos::CreateNativeViewGLSurfaceEGLOhos(
    GLDisplay* display,
    gfx::AcceleratedWidget widget) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(::switches::kOhosHanceSurface)) {
    LOG(INFO) << "CreateNativeViewGLSurfaceEGLOhos:: enhance surface";
    WindowsSurfaceInfo* surfaceInfo = static_cast<WindowsSurfaceInfo*>(
        NWebNativeWindowTracker::GetInstance()->GetNativeWindow(widget));
    if (surfaceInfo != nullptr) {
      LOG(INFO) << "clear surface from NWEB";
      eglDestroySurface(surfaceInfo->display, surfaceInfo->surface);
      eglDestroyContext(surfaceInfo->display, surfaceInfo->context);

      return scoped_refptr<NativeViewGLSurfaceEGLOhos>(
          new NativeViewGLSurfaceEGLOhos(display->GetAs<gl::GLDisplayEGL>(),
              reinterpret_cast<EGLNativeWindowType>(surfaceInfo->window)));
    }
  } else {
    LOG(INFO) << "CreateNativeViewGLSurfaceEGLOhos:: normal surface"
              << ", widgetid = " << widget;
    auto& system_properties_adapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance();
    std::string product_model = system_properties_adapter.GetDeviceInfoProductModel();
    if (product_model == PRODUCT_MODEL_EMULATOR) {
      LOG(INFO)  << "emulator CreateNativeViewGLSurfaceEGLOhos delay 100ms";
      base::PlatformThread::Sleep(base::Milliseconds(100));
    }
    void* window = NWebNativeWindowTracker::GetInstance()->GetNativeWindow(widget);
    return scoped_refptr<NativeViewGLSurfaceEGLOhos>(
        new NativeViewGLSurfaceEGLOhos(display->GetAs<gl::GLDisplayEGL>(),
            reinterpret_cast<EGLNativeWindowType>(window)));
  }
  return nullptr;
}

NativeViewGLSurfaceEGLOhos::NativeViewGLSurfaceEGLOhos(
    GLDisplayEGL* display,
    EGLNativeWindowType window)
    : NativeViewGLSurfaceEGL(display, window, nullptr), window_(window) {
  enable_debug_backgroound_color_ =
    OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetBoolParameter(
      "web.debug.eglSwapBuffersBackgroundColor", false);
  if (enable_debug_backgroound_color_) {
    LOG(INFO) << "NativeViewGLSurfaceEGLOhos:: enable debug background color," \
      " The rendering output will be replaced with green.";
  }
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetWindowAdapterInstance()
      .AddNativeWindowRef(reinterpret_cast<void*>(window_));
  LOG(INFO) << "NativeViewGLSurfaceEGLOhos add window ref.";
}

gfx::SwapResult NativeViewGLSurfaceEGLOhos::SwapBuffers(
    PresentationCallback callback,
    gfx::FrameData data) {
  if (enable_debug_backgroound_color_) {
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  if (is_first_swapbuffers_) {
    is_first_swapbuffers_ = false;
    LOG(INFO) << "web render log: first call SwapBuffers, width = " << NativeViewGLSurfaceEGL::GetSize().width()
      << ", higth = " << NativeViewGLSurfaceEGL::GetSize().height();
  }

  auto start = std::chrono::high_resolution_clock::now();

  auto result = NativeViewGLSurfaceEGL::SwapBuffers(std::move(callback), data);

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  if (duration > kMaxSwapInterval) {
    LOG(WARNING) << "web render log: eglSwapBuffers cost time = " << duration << "ms";
  }

  if (result == gfx::SwapResult::SWAP_FAILED) {
    LOG(ERROR) << "web render log: SwapBuffers failed, width = " << NativeViewGLSurfaceEGL::GetSize().width() <<
      ", height = " << NativeViewGLSurfaceEGL::GetSize().height();
  }
  return result;
}

bool NativeViewGLSurfaceEGLOhos::Resize(const gfx::Size& size,
                                        float scale_factor,
                                        const gfx::ColorSpace& color_space,
                                        bool has_alpha) {
  TRACE_EVENT0("gpu", "NativeViewGLSurfaceEGLOhos::Resize");

  auto& system_properties_adapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance();
  std::string product_model = system_properties_adapter.GetDeviceInfoProductModel();

  if (base::ohos::IsMobileDevice() && product_model != PRODUCT_MODEL_EMULATOR && !isInnerWeb_) {
    NativeViewGLSurfaceEGL::Resize(size, scale_factor, color_space, has_alpha);
  }

  int32_t ret =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWindowAdapterInstance()
          .NativeWindowSetBufferGeometry(reinterpret_cast<void*>(window_),
                                 size.width(), size.height());
  if (ret != OHOS::NWeb::GSErrorCode::GSERROR_OK) {
    LOG(ERROR) << "fail to set NativeWindowHandleOpt, ret=" << ret;
    return false;
  }

  if (product_model == PRODUCT_MODEL_EMULATOR) {
    NativeViewGLSurfaceEGL::Resize(size, scale_factor, color_space, has_alpha);
  }

  return true;
}
void NativeViewGLSurfaceEGLOhos::SetNativeInnerWeb(bool isInnerWeb) {
  LOG(INFO)<<"NativeViewGLSurfaceEGLOhos::SetNativeInnerWeb is "<<isInnerWeb;
  isInnerWeb_ = isInnerWeb;
}

bool NativeViewGLSurfaceEGLOhos::SetBackbufferAllocation(bool allocated) {
  TRACE_EVENT1("gpu", "NativeViewGLSurfaceEGLOhos::SetBackbufferAllocation",
               "allocated", allocated);
  // emulator不触发（接口改动未同步蓝区）
  // Notify the bufferqueue associated with the OHNativeWindow to clean cache
  if (!allocated) {
    NativeViewGLSurfaceEGL::Recreate();
  }
  return true;
}

NativeViewGLSurfaceEGLOhos::~NativeViewGLSurfaceEGLOhos() {
  Destroy();
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetWindowAdapterInstance()
      .NativeWindowUnRef(reinterpret_cast<void*>(window_));
  LOG(INFO) << "~NativeViewGLSurfaceEGLOhos unref the window.";
}

}  // namespace gl
