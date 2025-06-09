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

#ifndef OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_PROXY_H
#define OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_PROXY_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <IPCKit/ipc_kit.h>

#include "gpu_native_ipc_common.h"
#include "native_ipc_proxy.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::multiprocess {

class RequestGetPid : public NativeIpcRequest {
 public:
  OH_IPC_ErrorCode Run(const OHIPCRemoteProxy* remote_proxy) override;

  std::string ToString() const override { return "RequestGetPid()"; }

  explicit RequestGetPid(int32_t* child_pid) : child_pid_(child_pid) {}

 private:
  int32_t* child_pid_;
};

class RequestInitializeWindowAdapter : public NativeIpcRequest {
 public:
  OH_IPC_ErrorCode Run(const OHIPCRemoteProxy* remote_proxy) override;

  std::string ToString() const override {
    std::string s = "RequestInitializeWindowAdapter(";
    for (auto pair : windows_) {
      s += pair.first + ((&pair != &windows_.back()) ? ", " : "");
    }
    return s + ")";
  }

  explicit RequestInitializeWindowAdapter(
      const std::vector<std::pair<std::string, void*>>& windows)
      : windows_(windows) {}

 private:
  std::vector<std::pair<std::string, void*>> windows_;
};

class RequestAddWindow : public NativeIpcRequest {
 public:
  OH_IPC_ErrorCode Run(const OHIPCRemoteProxy* remote_proxy) override;

  std::string ToString() const override {
    return "RequestAddWindow(" + window_id_ + ")";
  }

  RequestAddWindow(const std::string& window_id, void* window)
      : window_id_(window_id), window_(window) {}

 private:
  std::string window_id_;
  void* window_;
};

class RequestRemoveWindow : public NativeIpcRequest {
 public:
  OH_IPC_ErrorCode Run(const OHIPCRemoteProxy* remote_proxy) override;

  std::string ToString() const override {
    return "RequestRemoveWindow(window_id=" + window_id_ + ")";
  }

  explicit RequestRemoveWindow(const std::string& window_id)
      : window_id_(window_id) {}

 private:
  std::string window_id_;
};

class RequestSetWindowWidget : public NativeIpcRequest {
 public:
  OH_IPC_ErrorCode Run(const OHIPCRemoteProxy* remote_proxy) override;

  std::string ToString() const override {
    return "RequestSetWindowWidget(window_id=" + window_id_ + ", " +
           "widget_id=" + std::to_string(widget_id_) + ")";
  }

  RequestSetWindowWidget(const std::string& window_id, int32_t widget_id)
      : window_id_(window_id), widget_id_(widget_id) {}

 private:
  std::string window_id_;
  int32_t widget_id_;
};

class RequestNotifyWindowChange : public NativeIpcRequest {
 public:
  OH_IPC_ErrorCode Run(const OHIPCRemoteProxy* remote_proxy) override;

  std::string ToString() const override {
    return "RequestNotifyWindowChange(widget_id=" + window_id_ + ")";
  }

  RequestNotifyWindowChange(const std::string& window_id, void* window)
      : window_id_(window_id), window_(window) {}

 private:
  std::string window_id_;
  void* window_;
};

class GpuNativeIpcProxy : public NativeIpcProxy {
 public:
  virtual OH_IPC_ErrorCode GetPid(int32_t* child_pid);

  virtual OH_IPC_ErrorCode InitializeWindowAdapter(
      const std::vector<std::pair<std::string, void*>>& windows);

  virtual OH_IPC_ErrorCode AddWindow(const std::string& window_id, void* window);

  virtual OH_IPC_ErrorCode RemoveWindow(const std::string& window_id);

  virtual OH_IPC_ErrorCode SetWindowWidget(const std::string& window_id,
                                   int32_t widget_id);

  virtual OH_IPC_ErrorCode NotifyWindowChange(const std::string& window_id,
                                      void* window);

  explicit GpuNativeIpcProxy(OHIPCRemoteProxy* remote_proxy)
      : NativeIpcProxy(remote_proxy) {}
};

}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_PROXY_H
