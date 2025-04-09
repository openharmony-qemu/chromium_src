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

namespace gl {

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
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .GetWindowAdapterInstance()
    .AddNativeWindowRef(reinterpret_cast<void*>(window_));
  LOG(INFO) << "NativeViewGLSurfaceEGLOhos add window ref.";
}

NativeViewGLSurfaceEGLOhos::~NativeViewGLSurfaceEGLOhos() {
  Destroy();
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetWindowAdapterInstance()
      .NativeWindowUnRef(reinterpret_cast<void*>(window_));
  LOG(INFO) << "~NativeViewGLSurfaceEGLOhos unref the window.";
}

gfx::SwapResult NativeViewGLSurfaceEGLOhos::SwapBuffers(
    PresentationCallback callback,
    gfx::FrameData data) {
  auto result = NativeViewGLSurfaceEGL::SwapBuffers(std::move(callback),data);
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

  if (base::ohos::IsMobileDevice() && product_model != PRODUCT_MODEL_EMULATOR) {
    if (NativeViewGLSurfaceEGL::Resize(size, scale_factor, color_space, has_alpha)) {
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetWindowAdapterInstance()
        .NativeWindowSurfaceCleanCache(reinterpret_cast<void*>(window_));
    }
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

}  // namespace gl
