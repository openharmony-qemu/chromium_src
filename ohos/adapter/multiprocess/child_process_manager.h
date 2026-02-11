/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#ifndef OHOS_ADAPTER_CHILD_PROCESS_MANAGER_H
#define OHOS_ADAPTER_CHILD_PROCESS_MANAGER_H

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "native_child_process/native_child_process_library.h"
#include "native_child_process/native_child_process_monitor.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::multiprocess {

class ADAPTER_EXPORT_API ChildProcessManager {
 public:
  using StartCallbackFunc = std::function<void(int32_t)>;

  static ChildProcessManager& GetInstance();

  static void OnChildSignal(int32_t pid, int32_t signal);
  void HandleChildSignal(int32_t pid, int32_t signal);
  int32_t WaitChildPid(const int32_t pid, int *status, const bool can_block);

  int32_t StartChildProcess(
      const std::vector<std::string>& commands,
      const std::vector<std::pair<int, int>>& fds_to_remap,
      const std::string& entry_point,
      bool is_isolated);

  int32_t StartLegacyChildProcess(
      const std::vector<std::string>& commands,
      const std::vector<std::pair<int, int>>& fds_to_remap);

  int32_t StartGpuProcess(const std::vector<std::string>& commands,
                          const std::vector<std::pair<int, int>>& fds_to_remap);
  void GpuProcessStarted(int32_t child_pid);

  ChildProcessManager(const ChildProcessManager&) = delete;
  ChildProcessManager& operator=(const ChildProcessManager&) = delete;
 private:
  ChildProcessManager();
  ~ChildProcessManager();

  int32_t StartChildProcessInternal(
      std::function<void(StartCallbackFunc)> start_func,
      const std::vector<std::string>& commands,
      const std::vector<std::pair<int, int>>& fds_to_remap);

  int32_t StartNativeChildProcess(
      const std::string& process_name,
      const std::vector<std::string>& commands,
      const std::vector<std::pair<int, int>>& fds_to_remap,
      const std::string& entry_point,
      bool is_isolated);

  int32_t StartNativeChildProcessWithName(
      const std::string& process_name,
      const std::vector<std::string>& commands,
      const std::vector<std::pair<int, int>>& fds_to_remap,
      const std::string& entry_point,
      bool is_isolated);

  int32_t TryStartNativeChildProcess(
      const std::vector<std::string>& commands,
      const std::vector<std::pair<int, int>>& fds_to_remap,
      const std::string& entry_point,
      bool is_isolated);

  int CreateGpuProcess();
  int CreateGpuProcessWithName();
  int CreateGpuProcessInternal(std::function<void(int32_t)> callback);

  void OnChildProcessStarted(int32_t pid);

  std::function<void(int32_t)> gpu_process_callback_;
  int gpu_process_start_times_ = 0;

  NativeChildProcessLibrary native_api_;
  std::unique_ptr<NativeChildProcessMonitor> monitor_;
};

pid_t ADAPTER_EXPORT_API Waitpid(pid_t handle, int *status, int nonBlocking);

}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_CHILD_PROCESS_MANAGER_H
