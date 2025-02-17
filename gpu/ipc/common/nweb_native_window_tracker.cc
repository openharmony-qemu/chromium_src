// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "nweb_native_window_tracker.h"

#include "base/check.h"
#include "base/logging.h"
#if BUILDFLAG(IS_OHOS)
#include "content/renderer/render_remote_proxy_ohos.h"
#include "ohos_adapter_helper.h"
#endif

NWebNativeWindowTracker* g_instance = nullptr;

extern void* QueryRenderWindowFromBrowserProcess(int32_t window_id);
extern void DestoryRenderWindowFromBrowserProcess(int32_t window_id);
extern void PassWindow(int64_t window_id);

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
};

NWebNativeWindowTracker::NWebNativeWindowTracker()
    : g_browser_client_(std::make_shared<BrowserClientAdapterImpl>()),
      next_native_window_id_(1) {}

NWebNativeWindowTracker::~NWebNativeWindowTracker() {}

NWebNativeWindowTracker* NWebNativeWindowTracker::GetInstance()
{
    return base::Singleton<NWebNativeWindowTracker>::get();
}

int32_t NWebNativeWindowTracker::AddNativeWindow(void* native_window)
{
    base::AutoLock lock(window_map_lock_);
    int32_t native_window_id = next_native_window_id_++;
    native_window_map_.emplace(native_window_id,
                               std::move((void *)native_window));
    LOG(DEBUG) << "Add native window id = " << native_window_id;
    return native_window_id;
}

void *NWebNativeWindowTracker::GetNativeWindow(int32_t native_window_id)
{
    base::AutoLock lock(window_map_lock_);
    auto it = native_window_map_.find(native_window_id);
    if (it == native_window_map_.end()) {
        LOG(DEBUG) << __FUNCTION__
                   << "Cann't get for native_window id = " << native_window_id;
        std::shared_ptr<content::RenderRemoteProxy> g_render_remote_proxy =
            std::make_shared<content::RenderRemoteProxy>();
        if (g_browser_client_) {
            // call binder to get window from browser proc (sync call)
            void *window = g_browser_client_->QueryRenderSurface(native_window_id);
            if (window) {
                native_window_map_.emplace(native_window_id, std::move((void *)window));
                LOG(DEBUG) << "Add native window id = " << native_window_id;
                return window;
            }
        }
        return nullptr;
    }
    LOG(DEBUG) << __FUNCTION__
               << "Get for native_window id = " << native_window_id;
    return it->second;
}

void NWebNativeWindowTracker::DestroyNativeWindow(int32_t native_window_id)
{
    base::AutoLock lock(window_map_lock_);
    auto it = native_window_map_.find(native_window_id);
    if (it != native_window_map_.end()) {
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
