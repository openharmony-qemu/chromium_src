// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_PROCESS_HOST_H
#define OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_PROCESS_HOST_H

#include <atomic>
#include <cstdint>
#include <future>
#include <mutex>

#include "gpu_native_ipc_proxy.h"

namespace ohos::adapter::multiprocess {

class GpuNativeProcessHost {
 public:
  int32_t GetPid();

  int InitializeWindowAdapter(
      const std::vector<std::pair<std::string, void*>>& windows);

  void AddWindow(const std::string& window_id, void* window);

  void RemoveWindow(const std::string& window_id);

  void SetWindowWidget(const std::string& window_id, int32_t widget_id);

  void SetChildPid(int32_t child_pid);

  void NotifyWindowChanged(const std::string& window_id, void* window);

  void Initialize(OHIPCRemoteProxy* remote_proxy);

  void Initialize(std::shared_ptr<GpuNativeIpcProxy> ipc_proxy);

  void Reset();

  bool IsInitialized();

  int StartGpuProcess(std::function<void(int32_t)> callback);

  static GpuNativeProcessHost& GetInstance();

  bool NeedSendRequest();

 private:
  GpuNativeProcessHost() = default;

  ~GpuNativeProcessHost() = default;

  GpuNativeProcessHost(const GpuNativeProcessHost&);

  GpuNativeProcessHost& operator=(const GpuNativeProcessHost&);

  std::shared_ptr<GpuNativeIpcProxy> ipc_proxy_{nullptr};

  std::function<void(int32_t)> callback_;

  int start_times_ = 0;
  std::mutex ipc_proxy_mutex_;
};

}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_PROCESS_HOST_H
