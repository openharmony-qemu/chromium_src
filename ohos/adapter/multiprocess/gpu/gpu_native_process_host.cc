/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

const int kInvalidPid = -1;

static const int kProcessStartInterval = 200000;

// Starts the child process by calling OH_Ability_CreateNativeChildProcess(),
// after the child process is launched and the OHIPCRemoteStub object is
// created, this function is called in the main process:
// 1. synchronize the WindowAdapter with the child process,
// 2. and then query PID of the child process.
void onProcessStarted(int err_code, OHIPCRemoteProxy* remote_proxy) {
  GpuNativeProcessHost& host = GpuNativeProcessHost::GetInstance();

  if (err_code != NCP_NO_ERROR) {
    LOGE("Child process start failed, err_code=%{public}d", err_code);
    host.SetChildPid(kInvalidPid);
    return;
  }

  if (remote_proxy == nullptr) {
    LOGE("Initialize IPC remote proxy failed, which is nullptr.");
    host.SetChildPid(kInvalidPid);
    return;
  }

  host.Initialize(remote_proxy);

  if (WindowAdapter::GetInstance().SyncWindowToGpuProcess() !=
        CrossProcessSyncResult::SUCCESS) {
    LOGE("Sync window adapter to GPU process failed.");
    host.SetChildPid(kInvalidPid);
    return;
  }

  int32_t child_pid = host.GetPid();
  if (child_pid == kInvalidPid) {
    LOGE("Get child process pid failed, child pid got: %{public}d", child_pid);
    host.SetChildPid(kInvalidPid);
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
  return !command_line->HasSwitch(switches::kInProcessGpu) && IsInitialized();
}

int GpuNativeProcessHost::StartGpuProcess(
    std::function<void(int32_t)> callback) {
  callback_ = callback;

  LOGW("GPU process start times: %{public}d", start_times_);
  if (start_times_ > 0) {
    // Since the previous GPU process has crashed, wait for a while
    // before launching a new GPU process, to let the previous one exit.
    // We can only have one native process at a time.
    usleep(kProcessStartInterval);
  }
  ++start_times_;
  return OH_Ability_CreateNativeChildProcess("libadapter.so",
                                             &onProcessStarted);
}

int32_t GpuNativeProcessHost::GetPid() {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  if (!NeedSendRequest()) {
    return kInvalidPid;
  }

  int32_t child_pid;
  OH_IPC_ErrorCode ret = ipc_proxy_->GetPid(&child_pid);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Get GPU process pid error: %{public}d", ret);
    return kInvalidPid;
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
