// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_surface_egl_ohos.h"

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/threading/platform_thread.h"
#include "base/trace_event/trace_event.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/window_adapter_impl.h"

namespace gl {

const std::string PRODUCT_MODEL_EMULATOR = "emulator";
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
constexpr int32_t DEFAULT_FENCE_FD = -1;
#endif

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
          new NativeViewGLSurfaceEGLOhos(
              display->GetAs<gl::GLDisplayEGL>(),
              reinterpret_cast<EGLNativeWindowType>(surfaceInfo->window.get())));
    }
  } else {
    LOG(INFO) << "CreateNativeViewGLSurfaceEGLOhos:: normal surface"
              << ", widgetid = " << widget;
    auto& system_properties_adapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance()
            .GetSystemPropertiesInstance();
    std::string product_model =
        system_properties_adapter.GetDeviceInfoProductModel();
    if (product_model == PRODUCT_MODEL_EMULATOR) {
      LOG(INFO) << "emulator CreateNativeViewGLSurfaceEGLOhos delay 100ms";
      base::PlatformThread::Sleep(base::Milliseconds(100));
    }
    void* window =
        NWebNativeWindowTracker::GetInstance()->GetNativeWindow(widget);
    return scoped_refptr<NativeViewGLSurfaceEGLOhos>(
        new NativeViewGLSurfaceEGLOhos(
            display->GetAs<gl::GLDisplayEGL>(),
            reinterpret_cast<EGLNativeWindowType>(window)));
  }
  return nullptr;
}

//LCOV_EXCL_START
NativeViewGLSurfaceEGLOhos::NativeViewGLSurfaceEGLOhos(
    GLDisplayEGL* display,
    EGLNativeWindowType window)
    : NativeViewGLSurfaceEGL(display, window, nullptr), window_(window)
{
  OHOS::NWeb::WindowAdapterNdkImpl::GetInstance()
      .AddNativeWindowRef(reinterpret_cast<void*>(window_));
  LOG(INFO) << "NativeViewGLSurfaceEGLOhos add window ref.";
}
//LCOV_EXCL_STOP

gfx::SwapResult NativeViewGLSurfaceEGLOhos::SwapBuffers(
    PresentationCallback callback,
    gfx::FrameData data) {
  if (enable_replace_swap_buffer_output_) {
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  if (is_first_swapbuffers_) {
    is_first_swapbuffers_ = false;
    LOG(INFO) << "web render log: first call SwapBuffers, size = "
              << NativeViewGLSurfaceEGL::GetSize().ToString();
  }

  auto start = std::chrono::high_resolution_clock::now();

  auto result = NativeViewGLSurfaceEGL::SwapBuffers(std::move(callback), data);

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();
  if (duration > kMaxSwapIntervalOhos) {
    LOG(WARNING) << "web render log: eglSwapBuffers cost time = " << duration
                 << "ms";
  }

  if (result == gfx::SwapResult::SWAP_FAILED) {
    LOG(ERROR) << "web render log: SwapBuffers failed, size = "
               << NativeViewGLSurfaceEGL::GetSize().ToString();
  }
  return result;
}

bool NativeViewGLSurfaceEGLOhos::Resize(const gfx::Size& size,
                                        float scale_factor,
                                        const gfx::ColorSpace& color_space,
                                        bool has_alpha) {
  TRACE_EVENT0("gpu", "NativeViewGLSurfaceEGLOhos::Resize");

  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  std::string product_model =
      system_properties_adapter.GetDeviceInfoProductModel();

  if (base::ohos::IsMobileDevice() && product_model != PRODUCT_MODEL_EMULATOR && !isInnerWeb_) {
    if (NativeViewGLSurfaceEGL::Resize(size, scale_factor, color_space,
                                       has_alpha)) {
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWindowAdapterInstance()
          .NativeWindowSurfaceCleanCache(reinterpret_cast<void*>(window_));
    }
  }

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  TRACE_EVENT0("gpu", "NativeViewGLSurfaceEGLOhos::Resize");
#endif
  int32_t ret =
      OHOS::NWeb::OhosAdapterHelperExt::GetWindowAdapterNdkInstance()
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

//LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
bool NativeViewGLSurfaceEGLOhos::SetBackbufferAllocation(bool allocated) {
  TRACE_EVENT1("gpu", "NativeViewGLSurfaceEGLOhos::SetBackbufferAllocation",
               "allocated", allocated);
  if (base::ohos::IsEmulator() || isInnerWeb_) {
    return true;
  }

  // EglDestroy has notified the bufferqueue associated with the OHNativeWindow to clean cache
  if (!allocated && NWebNativeWindowTracker::GetInstance()->CheckNativeWindow(reinterpret_cast<void*>(window_))) {
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
    if (!condition_) {
#endif
    GLContext* context = GLContext::GetCurrent();
    if (context) {
      if (NativeViewGLSurfaceEGL::Recreate()) {
      // Notify the bufferqueue associated with the OHNativeWindow to clean
      // cache
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWindowAdapterInstance()
          .NativeWindowSurfaceCleanCache(reinterpret_cast<void*>(window_));
      }
    } else {
      LOG(WARNING) << "GLContext::GetCurrent() returned null, skipping surface recreation";
    }
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
    }
#endif
  }
  return true;
}
#endif

NativeViewGLSurfaceEGLOhos::~NativeViewGLSurfaceEGLOhos()
{
  Destroy();
  OHOS::NWeb::WindowAdapterNdkImpl::GetInstance()
      .NativeWindowUnRef(reinterpret_cast<void*>(window_));
  LOG(INFO) << "~NativeViewGLSurfaceEGLOhos unref the window.";
}

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void NativeViewGLSurfaceEGLOhos::SetNativeInnerWeb(bool isInnerWeb) {
  LOG(INFO)<<"NativeViewGLSurfaceEGLOhos::SetNativeInnerWeb is "<<isInnerWeb;
  isInnerWeb_ = isInnerWeb;
}
#endif
//LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void NativeViewGLSurfaceEGLOhos::SetBypassVsyncCondition(int32_t condition) {
  LOG(INFO) << "NativeViewGLSurfaceEGLOhos::SetBypassVsyncCondition is " << condition;
  if (condition_ == 1 || condition == 0) {
    return;
  }
  condition_ = condition;
  int32_t buffer_num = OHOS::NWeb::WindowAdapterNdkImpl::GetInstance()
                          .GetNativeWindowQueueSize(reinterpret_cast<void*>(window_));
  if (buffer_num == 0) {
    return;
  }
  int32_t fence_fd = DEFAULT_FENCE_FD;
  std::vector<NativeWindowBuffer*> native_window_buffers(buffer_num, nullptr);
  int32_t buffer_cnt = 0;
  for (int32_t i = 0; i < buffer_num - 1; i++) {
    int32_t ret = OHOS::NWeb::WindowAdapterNdkImpl::GetInstance()
                          .GetNativeWindowRequestBuffer(reinterpret_cast<void*>(window_),
                          &native_window_buffers[i], &fence_fd);
    if (ret != 0) {
      break;
    }
    buffer_cnt++;
    if (fence_fd != DEFAULT_FENCE_FD) {
      close(fence_fd);
      fence_fd = DEFAULT_FENCE_FD;
    }
  }
  for (int32_t i = 0; i < buffer_cnt; i++) {
    OHOS::NWeb::WindowAdapterNdkImpl::GetInstance()
                          .GetNativeWindowAbortBuffer(reinterpret_cast<void*>(window_),
                          native_window_buffers[i]);
  }
}
#endif
}  // namespace gl
