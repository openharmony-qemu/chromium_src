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

#include "child_process_starter.h"
#include <future>

#include <IPCKit/ipc_kit.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "aki/jsbind.h"
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/multiprocess/app_spawn_communication.h"
#include "ohos/adapter/multiprocess/gpu/gpu_native_process_host.h"
#include "ohos/adapter/multiprocess/isolate_process/isolate_process_helper.h"
#include "ohos/adapter/multiprocess/isolate_process/native_childprocess_interface.h"

namespace ohos::adapter::multiprocess {

const int kInvalidPid = -1;

namespace {
void LogStartChildProcessFixesCommands(
    const std::vector<std::string>& commands) {
  for (auto& command : commands) {
    LOGI("StartChildProcess command %{public}s", command.c_str());
  }
}
}  // namespace

ChildProcessStarter& ChildProcessStarter::GetInstance() {
  static ChildProcessStarter instance;
  return instance;
}

int ChildProcessStarter::StartChildProcess(
    std::function<void(StartCallbackFunc)> start_func,
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds) {
  LogStartChildProcessFixesCommands(commands);

  std::promise<int32_t> child_pid;
  StartCallbackFunc set_child_pid = [&child_pid](int32_t pid) -> void {
    child_pid.set_value(pid);
  };

  // Must establish socket listen before start child process
  // if not, child process request will be "Connection failed"
  std::lock_guard<std::mutex> lock(mutex_);
  AppSpawnCommunication appspawn_server;
  if (appspawn_server.PrepareHandleRequest(commands, fds)) {
    start_func(set_child_pid);
    int cpid = child_pid.get_future().get();
    appspawn_server.HandleRequestAfterChildProcStart(cpid);
    return cpid;
  }else {
    LOGE("appspawn_server.PrepareHandleRequest failed");
  }
  return kInvalidPid;
}

int ChildProcessStarter::StartLegacyChildProcess(
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds) {
  TRACE_EVENT_0("ChildProcessStarter::StartLegacyChildProcess");

  auto start_child_process_func = ohos::adapter::GetJSFunction(
      "ChromiumChildProcessStarter.StartChildProcess");
  if (!start_child_process_func) {
    LOGE("ChromiumChildProcessStarter.StartChildProcess not defined");
    return kInvalidPid;
  }

  std::function<void(StartCallbackFunc)> start_func =
      [start_child_process_func](StartCallbackFunc callback) {
        start_child_process_func->Invoke<void>(callback);
      };

  return StartChildProcess(start_func, commands, fds);
}

int ChildProcessStarter::StartIsolateChildProcess(
    const std::vector<std::string>& command,
    const std::vector<std::pair<int, int>>& fds,
    const std::string& entry_point) {
#if defined(ENABLE_START_ISOLATE_PROCESS)
  TRACE_EVENT_0("ChildProcessStarter::StartIsolateChildProcess");
  if (IsStartNativeChildProcessAvailable()) {
    return IsolateProcessHelper::StartChildProcess(command, fds, entry_point, true);
  } else {
    LOGW("fallback to StartLegacyChildProcess");
    return StartLegacyChildProcess(command, fds);
  }
#else
  LOGW("fallback to StartLegacyChildProcess");
  return StartLegacyChildProcess(command, fds);
#endif
}

int ChildProcessStarter::StartGpuProcess(
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds) {
  TRACE_EVENT_0("ChildProcessStarter::StartGpuProcess");

  std::function<void(StartCallbackFunc)> start_func =
      [](StartCallbackFunc callback) {
        int ret = GpuNativeProcessHost::GetInstance().StartGpuProcess(callback);
        if (ret != 0) {
          LOGE("ChildProcessStarter::StartGpuProcess error: %{public}d", ret);
          callback(kInvalidPid);
        }
        LOGI("ChildProcessStarter::StartGpuProcess Start GPU process succeed.");
      };

  return StartChildProcess(start_func, commands, fds);
}

int ChildProcessStarter::StartNormalChildProcess(
    const std::vector<std::string>& command,
    const std::vector<std::pair<int, int>>& fds,
    const std::string& entry_point) {
#if defined(ENABLE_START_ISOLATE_PROCESS)
  TRACE_EVENT_0("ChildProcessStarter::StartNormalpocess");
  if (IsStartNativeChildProcessAvailable()) {
    return IsolateProcessHelper::StartChildProcess(command, fds, entry_point, false);
  } else {
    LOGW("fallback to StartLegacyChildProcess");
    return StartLegacyChildProcess(command, fds);
  }
#else
  LOGW("fallback to StartLegacyChildProcess");
  return StartLegacyChildProcess(command, fds);
#endif
    }
}  // namespace ohos::adapter::multiprocess
