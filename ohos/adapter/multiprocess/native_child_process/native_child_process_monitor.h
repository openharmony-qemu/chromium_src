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

#ifndef OHOS_ADAPTER_CHILD_PROCESS_MONITOR_H
#define OHOS_ADAPTER_CHILD_PROCESS_MONITOR_H

#include <condition_variable>
#include <mutex>
#include <unordered_map>

#include "native_child_process_library.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::multiprocess {
struct ProcessNode {
  std::condition_variable cv;
  bool consumed = false;
  bool ready = false;
  int32_t signal = 0;
};

class NativeChildProcessMonitor {
 public:
  NativeChildProcessMonitor() = default;
  ~NativeChildProcessMonitor() = default;

  void HandleChildStarted(const int32_t pid);
  void HandleChildSignal(const int32_t pid, const int32_t signal);
  int32_t WaitChildPid(const int32_t pid, int* status, const bool can_block);

 private:
  std::unordered_map<int32_t, std::shared_ptr<ProcessNode>> native_processes_;
  // Protect native_processes_
  std::mutex monitor_mutex_;
};
} // namespace ohos::adapter::multiprocess
#endif // OHOS_ADAPTER_CHILD_PROCESS_MONITOR_H