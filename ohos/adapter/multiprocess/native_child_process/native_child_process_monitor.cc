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

#include "native_child_process_monitor.h"

#include <AbilityKit/native_child_process.h>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"

namespace ohos::adapter::multiprocess {

namespace {
int EncodeStatus(int32_t signal) {
  if (signal != 0) {
    // Signal is the lowest 7 bits in status.
    return signal & 0x7f;
  }
  // Since system does not support exit code,
  // returning 0 means child process exited normally.
  return 0;
}
}  // namespace

void NativeChildProcessMonitor::HandleChildStarted(const int32_t pid) {
  std::lock_guard lock(monitor_mutex_);

  auto it = native_processes_.find(pid);
  if (it != native_processes_.end()) {
    LOGW("[ChildProcess] HandleChildStarted | pid %{public}d already exists.",
         pid);
    it->second->ready=true;
    it->second->cv.notify_all();
  }
  native_processes_[pid] = std::make_shared<ProcessNode>();
}

void NativeChildProcessMonitor::HandleChildSignal(const int32_t pid,
                                                  const int32_t signal) {
  std::lock_guard lock(monitor_mutex_);

  auto it = native_processes_.find(pid);
  if (it == native_processes_.end()) {
    LOGW(
        "[ChildProcess] HandleChildSignal | pid %{public}d not found. The "
        "child process may crashed before adding it's pid to monitor. Or this "
        "signal comes too late, WaitChildPid is already timeout.",
        pid);
    return;
  }

  std::shared_ptr<ProcessNode> node = it->second;
  node->ready = true;
  node->signal = signal;
  node->cv.notify_all();
}

int32_t NativeChildProcessMonitor::WaitChildPid(const int32_t pid,
                                                int* status,
                                                const bool can_block) {
  TRACE_EVENT_2("NativeChildProcessMonitor::WaitChildPid", "pid", pid,
                "can_block", can_block);
  std::unique_lock<std::mutex> lock(monitor_mutex_);

  int tmpStatus = 0;
  if (!status) {
    status = &tmpStatus;
  }

  auto it = native_processes_.find(pid);
  if (it == native_processes_.end()) {
    // Case 1: The child process is not a native child process;
    // Case 2: Exit status of this child process has already been consumed.
    LOGW("[ChildProcess] WaitChildPid pid %{public}d not found, return -1.",
         pid);
    return -1;
  }

  std::shared_ptr<ProcessNode> node = it->second;
  if (node->ready) {
    *status = EncodeStatus(node->signal);
    native_processes_.erase(pid);
    LOGW("[ChildProcess] WaitChildPid | ready, pid: %{public}d, status: "
         "%{public}d", pid, *status);
    return pid;
  }

  if (!can_block) {
    // The child process is still running.
    return 0;
  }

  LOGW("[ChildProcess] WaitChildPid | Waiting for pid: %{public}d", pid);
  node->cv.wait_for(lock, std::chrono::seconds(1), [&] { return node->ready; });
  if (!node->ready) {
    LOGW("[ChildProcess] WaitChildPid | Wait timeout for pid: %{public}d", pid);
    native_processes_.erase(pid);
    return -1;
  }

  if (node->consumed) {
    LOGW("[ChildProcess] WaitChildPid | Status already consumed for pid: "
         "%{public}d", pid);
    return -1;
  }

  node->consumed = true;
  *status = EncodeStatus(node->signal);
  native_processes_.erase(pid);
  LOGW("[ChildProcess] WaitChildPid | After wait, pid: %{public}d, status: "
       "%{public}d", pid, *status);
  return pid;
}
}  // namespace ohos::adapter::multiprocess
