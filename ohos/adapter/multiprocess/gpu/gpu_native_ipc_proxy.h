// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_PROXY_H
#define OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_PROXY_H

#include <IPCKit/ipc_kit.h>

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

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

  virtual OH_IPC_ErrorCode AddWindow(const std::string& window_id,
                                     void* window);

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
