// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "render_remote_proxy_ohos.h"

#include <thread>
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/posix/global_descriptors.h"
#include "content/public/common/content_descriptors.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#if BUILDFLAG(IS_OHOS)
#include "ohos_adapter_helper.h"

namespace content {
std::unique_ptr<OHOS::NWeb::AafwkAppMgrClientAdapter> g_app_mgr_client_adapter{
    nullptr};
std::shared_ptr<RenderRemoteProxy> g_render_remote_proxy{nullptr};

std::mutex RenderRemoteProxy::browser_fd_mtx_;
std::condition_variable RenderRemoteProxy::browser_fd_cv_;
bool RenderRemoteProxy::is_browser_fd_received_{false};
bool RenderRemoteProxy::is_for_test_{false};

void RenderRemoteProxy::NotifyBrowserFd(int32_t ipcFd,
                                        int32_t sharedFd,
                                        int32_t crashFd
) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  if (g_fds != nullptr) {
    int new_ipc_fd;
    if ((new_ipc_fd = dup(ipcFd)) < 0) {
      LOG(ERROR) << "ipcFd duplicate error";
      g_fds->Set(kMojoIPCChannel, ipcFd);
      ipc_fd_ = ipcFd;
    } else {
      g_fds->Set(kMojoIPCChannel, new_ipc_fd);
      ipc_fd_ = new_ipc_fd;
      close(ipcFd);
    }

    int new_shared_fd;
    if ((new_shared_fd = dup(sharedFd)) < 0) {
      LOG(ERROR) << "sharedFd duplicate error";
      g_fds->Set(kFieldTrialDescriptor, sharedFd);
      shared_fd_ = sharedFd;
    } else {
      g_fds->Set(kFieldTrialDescriptor, new_shared_fd);
      shared_fd_ = new_shared_fd;
      close(sharedFd);
    }

    int new_crash_id;
    if ((new_crash_id = dup(crashFd)) < 0) {
      LOG(ERROR) << "crashFd duplicate error";
      g_fds->Set(kCrashDumpSignal, crashFd);
      crash_id_ = crashFd;
    } else {
      g_fds->Set(kCrashDumpSignal, new_crash_id);
      crash_id_ = new_crash_id;
      close(crashFd);
    }
  }
  LOG(INFO) << "Wait for AMS to return IPC fd success and wake up process";
  RenderRemoteProxy::is_browser_fd_received_ = true;
  RenderRemoteProxy::browser_fd_cv_.notify_one();
}

void RenderRemoteProxy::NotifyBrowser(int32_t ipcFd,
                                      int32_t sharedFd,
                                      int32_t crashFd,
                                      std::shared_ptr<OHOS::NWeb::AafwkBrowserClientAdapter> clientAdapter
) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  LOG(INFO) << "NotifyBrowser ipcFd:" << ipcFd << " sharedFd:" << sharedFd << " crashFd:" << crashFd;
  if (g_fds != nullptr) {
    int new_ipc_fd;
    if ((new_ipc_fd = dup(ipcFd)) < 0) {
      LOG(ERROR) << "ipcFd duplicate error";
      g_fds->Set(kMojoIPCChannel, ipcFd);
      ipc_fd_ = ipcFd;
    } else {
      g_fds->Set(kMojoIPCChannel, new_ipc_fd);
      ipc_fd_ = new_ipc_fd;
      close(ipcFd);
    }

    int new_shared_fd;
    if ((new_shared_fd = dup(sharedFd)) < 0) {
      LOG(ERROR) << "sharedFd duplicate error";
      g_fds->Set(kFieldTrialDescriptor, sharedFd);
      shared_fd_ = sharedFd;
    } else {
      g_fds->Set(kFieldTrialDescriptor, new_shared_fd);
      shared_fd_ = new_shared_fd;
      close(sharedFd);
    }

    int new_crash_id;
    if ((new_crash_id = dup(crashFd)) < 0) {
      LOG(ERROR) << "crashFd duplicate error";
      g_fds->Set(kCrashDumpSignal, crashFd);
      crash_id_ = crashFd;
    } else {
      g_fds->Set(kCrashDumpSignal, new_crash_id);
      crash_id_ = new_crash_id;
      close(crashFd);
    }
  }
  LOG(INFO) << "Wait for AMS to return IPC fd success and wake up process";
  RenderRemoteProxy::is_browser_fd_received_ = true;
  RenderRemoteProxy::browser_fd_cv_.notify_one();
  if (clientAdapter) {
    LOG(DEBUG) << "NWebNativeWindowTracker set g_browser_client_";
    NWebNativeWindowTracker::Get()->g_browser_client_ = clientAdapter;
  }
}

void RenderRemoteProxy::CreateAndRegist(const base::CommandLine& command_line) {
  is_for_test_ = command_line.HasSwitch(switches::kForTest);
  if (!is_for_test_) {
    g_app_mgr_client_adapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateAafwkAdapter();
    g_render_remote_proxy = std::make_shared<RenderRemoteProxy>();
    LOG(INFO) << "Request to AMS to obtain the main process IPC fd";
    g_app_mgr_client_adapter->AttachRenderProcess(g_render_remote_proxy);
  }
}

bool RenderRemoteProxy::WaitForBrowserFd() {
  if (is_for_test_) {
    return true;
  }
  LOG(INFO) << "Wait for AMS to return the main process IPC fd";
  std::unique_lock<std::mutex> lk(browser_fd_mtx_);
  constexpr uint32_t kTimeOutDur = 10;  // milliseconds
  constexpr uint32_t kMaxWaitCount = 10;
  uint32_t wait_count = 0;
  while (++wait_count <= kMaxWaitCount) {
    if (!browser_fd_cv_.wait_for(lk, std::chrono::milliseconds(kTimeOutDur),
                                 []() { return is_browser_fd_received_; })) {
      LOG(INFO) << "Retry to wait for AMS to return the main process IPC fd for " << wait_count * kTimeOutDur << " ms";
    } else {
      LOG(INFO) << "Request to AMS to obtain the main process IPC fd successfully";
      return true;
    }
  }
  LOG(ERROR) << "Request to AMS to obtain the main process IPC fd failed, timeout("
             << (kTimeOutDur * kMaxWaitCount) << "ms)";
  return false;
}

}  // namespace content
#endif
