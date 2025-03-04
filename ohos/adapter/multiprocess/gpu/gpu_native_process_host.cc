// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu_native_process_host.h"

#include <AbilityKit/native_child_process.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <unistd.h>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/multiprocess/command_line/command_line.h"
#include "ohos/adapter/multiprocess/command_line/command_line_switches.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

using namespace ohos::adapter::xcomponent;

namespace ohos::adapter::multiprocess {

const int INVALID_PID = -1;

static const int PROCESS_START_INTERVAL = 200000;

// Starts the child process by calling OH_Ability_CreateNativeChildProcess(),
// after the child process is launched and the OHIPCRemoteStub object is
// created, this function is called in the main process:
// 1. synchronize the WindowAdapter with the child process,
// 2. and then query PID of the child process.
void onProcessStarted(int err_code, OHIPCRemoteProxy* remote_proxy) {
  GpuNativeProcessHost& host = GpuNativeProcessHost::GetInstance();

  if (err_code != NCP_NO_ERROR) {
    LOGE("Child process start failed, err_code=%{public}d", err_code);
    host.SetChildPid(INVALID_PID);
    return;
  }

  if (remote_proxy == nullptr) {
    LOGE("Initialize IPC remote proxy failed, which is nullptr.");
    host.SetChildPid(INVALID_PID);
    return;
  }

  host.Initialize(remote_proxy);

  if (WindowAdapter::GetInstance().SyncWindowToGpuProcess() !=
      CrossProcessSyncResult::SUCCESS) {
    LOGE("Sync window adapter to GPU process failed.");
    host.SetChildPid(INVALID_PID);
    return;
  }

  int32_t child_pid = host.GetPid();
  if (child_pid == INVALID_PID) {
    LOGE("Get child process pid failed, child pid got: %{public}d", child_pid);
    host.SetChildPid(INVALID_PID);
    return;
  }

  host.SetChildPid(child_pid);
}

GpuNativeProcessHost& GpuNativeProcessHost::GetInstance() {
  static GpuNativeProcessHost instance;
  return instance;
}

bool GpuNativeProcessHost::IsInitialized() {
  return ipc_proxy_ != nullptr;
}

void GpuNativeProcessHost::Initialize(OHIPCRemoteProxy* remote_proxy) {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  ipc_proxy_ = std::make_shared<GpuNativeIpcProxy>(remote_proxy);
}

void GpuNativeProcessHost::Initialize(
    std::shared_ptr<GpuNativeIpcProxy> ipc_proxy) {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  ipc_proxy_ = ipc_proxy;
}

void GpuNativeProcessHost::Reset() {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  if (ipc_proxy_) {
    ipc_proxy_ = nullptr;
  }
}

bool GpuNativeProcessHost::NeedSendRequest() {
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  if (command_line == nullptr) {
    return false;
  }
  return !command_line->HasSwitch(switches::IN_PROCESS_GPU) && IsInitialized();
}

int GpuNativeProcessHost::StartGpuProcess(
    std::function<void(int32_t)> callback) {
  callback_ = callback;

  LOGW("GPU process start times: %{public}d", start_times_);
  if (start_times_ > 0) {
    // Since the previous GPU process has crashed, wait for a while
    // before launching a new GPU process, to let the previous one exit.
    // We can only have one native process at a time.
    usleep(PROCESS_START_INTERVAL);
  }
  ++start_times_;
  return OH_Ability_CreateNativeChildProcess("libadapter.so",
                                             &onProcessStarted);
}

int32_t GpuNativeProcessHost::GetPid() {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  if (!NeedSendRequest()) {
    return INVALID_PID;
  }

  int32_t child_pid;
  OH_IPC_ErrorCode ret = ipc_proxy_->GetPid(&child_pid);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Get GPU process pid error: %{public}d", ret);
    return INVALID_PID;
  }

  return child_pid;
}

int GpuNativeProcessHost::InitializeWindowAdapter(
    const std::vector<std::pair<std::string, void*>>& windows) {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  if (!NeedSendRequest()) {
    return 0;
  }

  OH_IPC_ErrorCode ret = ipc_proxy_->InitializeWindowAdapter(windows);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Initialize window adapter in GPU process error: %{public}d", ret);
    return ret;
  }

  return 0;
}

void GpuNativeProcessHost::AddWindow(const std::string& window_id,
                                     void* window) {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  if (!NeedSendRequest()) {
    return;
  }

  OH_IPC_ErrorCode ret = ipc_proxy_->AddWindow(window_id, window);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Add window to GPU process error: %{public}d", ret);
  }
}

void GpuNativeProcessHost::RemoveWindow(const std::string& window_id) {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  if (!NeedSendRequest()) {
    return;
  }

  OH_IPC_ErrorCode ret = ipc_proxy_->RemoveWindow(window_id);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Remove window from GPU process error: %{public}d", ret);
  }
}

void GpuNativeProcessHost::SetWindowWidget(const std::string& window_id,
                                           int32_t widget_id) {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  if (!NeedSendRequest()) {
    return;
  }

  OH_IPC_ErrorCode ret = ipc_proxy_->SetWindowWidget(window_id, widget_id);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Remove window from GPU process error: %{public}d", ret);
  }
}

void GpuNativeProcessHost::NotifyWindowChanged(const std::string& window_id,
                                               void* window) {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  if (!NeedSendRequest()) {
    return;
  }

  OH_IPC_ErrorCode ret = ipc_proxy_->NotifyWindowChange(window_id, window);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Add window to GPU process error: %{public}d", ret);
  }
}

void GpuNativeProcessHost::SetChildPid(int32_t child_pid) {
  if (callback_ != nullptr) {
    callback_(child_pid);
  }
}
}  // namespace ohos::adapter::multiprocess
