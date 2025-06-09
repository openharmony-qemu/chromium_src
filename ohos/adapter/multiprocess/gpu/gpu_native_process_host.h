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
