// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "nweb_native_window_tracker.h"

#include "base/check.h"
#include "base/logging.h"
#include "base/command_line.h"
#if BUILDFLAG(IS_OHOS)
#include "content/renderer/render_remote_proxy_ohos.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"

NWebNativeWindowTracker* g_instance = nullptr;
extern void* QueryRenderWindowFromBrowserProcess(int32_t window_id);
extern void DestoryRenderWindowFromBrowserProcess(int32_t window_id);
extern void PassWindow(int64_t window_id);
extern void DestroyPassedSurfaceFromGpuProcess(int64_t surface_id);

namespace {
constexpr char kProcessType[] = "type";
constexpr char kGpuProcess[] = "gpu-process";
}  // namespace

class BrowserClientAdapterImpl : public OHOS::NWeb::AafwkBrowserClientAdapter {
public:
    BrowserClientAdapterImpl() = default;
    ~BrowserClientAdapterImpl() = default;
    void* QueryRenderSurface(int32_t surface_id) override {
        LOG(INFO) << "BrowserClientAdapterImpl.QueryRenderSurface " << surface_id;
        return QueryRenderWindowFromBrowserProcess(surface_id);
    }

    void ReportThread(OHOS::NWeb::ResSchedStatusAdapter status, int32_t process_id, int32_t thread_id, OHOS::NWeb::ResSchedRoleAdapter role) override {
        LOG(INFO) << "[not used] ReportThread, process_id: " << process_id << ", thread_id: " << thread_id;
    }

    void PassSurface(int64_t surface_id) override {
        LOG(INFO) << "PassSurface, surfaceid: " << surface_id;
        PassWindow(surface_id);
    }

    void DestroyRenderSurface(int32_t surface_id) override {
        LOG(INFO) << "BrowserClientAdapterImpl.DestroyRenderSurface " << surface_id;
        DestoryRenderWindowFromBrowserProcess(surface_id);
    }

    void DestroyPassedSurface(int64_t surface_id) override {
        DestroyPassedSurfaceFromGpuProcess(surface_id);
    }
};

//LCOV_EXCL_START
NWebNativeWindowTracker::NWebNativeWindowTracker()
    : next_native_window_id_(1) {
  auto type = base::CommandLine::ForCurrentProcess()->
      GetSwitchValueASCII(kProcessType);
  if (type == kGpuProcess) {
    g_browser_client_ = std::make_shared<BrowserClientAdapterImpl>();
  }
}

NWebNativeWindowTracker::~NWebNativeWindowTracker() {}

NWebNativeWindowTracker* NWebNativeWindowTracker::GetInstance() {
  return base::Singleton<NWebNativeWindowTracker>::get();
}
//LCOV_EXCL_STOP

int32_t NWebNativeWindowTracker::AddNativeWindow(void* native_window) {
  base::AutoLock lock(window_map_lock_);
  int32_t native_window_id = next_native_window_id_++;
  native_window_map_.emplace(native_window_id, std::move((void*)native_window));
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .GetWindowAdapterInstance().NativeWindowSetUsage(native_window);
  LOG(DEBUG) << "Add native window id = " << native_window_id;
  return native_window_id;
}

void* NWebNativeWindowTracker::GetNativeWindow(int32_t native_window_id) {
  base::AutoLock lock(window_map_lock_);
  auto it = native_window_map_.find(native_window_id);
  if (it == native_window_map_.end()) {
#ifndef OHOS_FUZZ_COMPILE_ERROR_FIX
    LOG(DEBUG) << __FUNCTION__
               << "Cann't get for native_window id = " << native_window_id;
    std::shared_ptr<content::RenderRemoteProxy> g_render_remote_proxy =
        std::make_shared<content::RenderRemoteProxy>();
    if (g_browser_client_) {
      // call binder to get window from browser proc (sync call)
      void *window = g_browser_client_->QueryRenderSurface(native_window_id);
      if (window) {
        native_window_map_.emplace(native_window_id, std::move((void *)window));
        OHOS::NWeb::OhosAdapterHelper::GetInstance()
            .GetWindowAdapterInstance().NativeWindowSetUsage(window);
        LOG(DEBUG) << "Add native window id = " << native_window_id;
        return window;
      }
    }
#endif
    return nullptr;
  }

  LOG(DEBUG) << __FUNCTION__
             << "Get for native_window id = " << native_window_id;
  return it->second;
}

void NWebNativeWindowTracker::DestroyNativeWindow(int32_t native_window_id) {
  base::AutoLock lock(window_map_lock_);
  auto it = native_window_map_.find(native_window_id);
  if (it != native_window_map_.end()) {
        // g_browser_client_ is NOT nullptr means now NWebNativeWindowTracker is
        // working on GPU process. In this case we need to do a NativeWindow
        // UNREF, as OH_NativeWindow_ReadFromParcel did a NativeWindow REF
        // implicitly during passing NativeWindow from browser process, or else
        // a memory leak would occur.
        if (g_browser_client_) {
          OHOS::NWeb::OhosAdapterHelper::GetInstance()
              .GetWindowAdapterInstance()
              .NativeWindowUnRef(it->second);
        }
    native_window_map_.erase(native_window_id);
    LOG(DEBUG) << __FUNCTION__
               << "Destroy native_window id = " << native_window_id;
  }

  if (g_browser_client_) {
    g_browser_client_->DestroyRenderSurface(native_window_id);
    LOG(DEBUG) << __FUNCTION__
               << "Destroy browser native_window id = " << native_window_id;
  }
}

bool NWebNativeWindowTracker::CheckNativeWindow(void* native_window)
{
    base::AutoLock lock(window_map_lock_);
    for (auto it : native_window_map_) {
        if (it.second == native_window) {
            return true;
        }
    }
    return false;
}
