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

namespace ohos::adapter::multiprocess {

const int kInvalidPid = -1;

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
    LOGE("Add window to GPU process error: %{public}d, window_id: %{public}s",
         ret, window_id.c_str());
  }
}

void GpuNativeProcessHost::RemoveWindow(const std::string& window_id) {
  std::lock_guard<std::mutex> lock_protect(ipc_proxy_mutex_);
  if (!NeedSendRequest()) {
    return;
  }

  OH_IPC_ErrorCode ret = ipc_proxy_->RemoveWindow(window_id);
  if (ret != OH_IPC_SUCCESS) {
    LOGE(
        "Remove window from GPU process error: %{public}d, window_id: "
        "%{public}s",
        ret, window_id.c_str());
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
    LOGE(
        "Set window from GPU process error: %{public}d, window_id: %{public}s, "
        "widget_id: %{public}d",
        ret, window_id.c_str(), widget_id);
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
    LOGE(
        "Notify window change to GPU process error: %{public}d, window_id: "
        "%{public}s",
        ret, window_id.c_str());
  }
}
}  // namespace ohos::adapter::multiprocess
