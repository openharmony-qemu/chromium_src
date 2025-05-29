// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "child_process_starter.h"

#include <IPCKit/ipc_kit.h>
#include <sys/types.h>
#include <unistd.h>

#include <future>
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

const int INVALID_PID = -1;

namespace {
void LogStartChildProcessFixesCommands(
    const std::vector<std::string>& commands) {
  for (auto& command : commands) {
    LOGD("StartChildProcess command %{public}s", command.c_str());
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
  AppSpawnCommunication appspawn_server;
  if (appspawn_server.PrepareHandleRequest(commands, fds)) {
    start_func(set_child_pid);
    int cpid = child_pid.get_future().get();
    appspawn_server.HandleRequestAfterChildProcStart(cpid);
    return cpid;
  }
  return INVALID_PID;
}

int ChildProcessStarter::StartChildProcess(
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds) {
  TRACE_EVENT_0("ChildProcessStarter::StartChildProcess");

  auto start_child_process_func = ohos::adapter::GetJSFunction(
      "ChromiumChildProcessStarter.StartChildProcess");
  if (!start_child_process_func) {
    LOGE("ChromiumChildProcessStarter.StartChildProcess not defined");
    return INVALID_PID;
  }

  std::function<void(StartCallbackFunc)> start_func =
      [start_child_process_func](StartCallbackFunc callback) {
        start_child_process_func->Invoke<void>(callback);
      };

  return StartChildProcess(start_func, commands, fds);
}

int ChildProcessStarter::StartIsolateChildProcess(
    const std::vector<std::string>& command,
    const std::vector<std::pair<int, int>>& fds) {
#if defined(ENABLE_START_ISOLATE_PROCESS)
  TRACE_EVENT_0("ChildProcessStarter::StartIsolateChildProcess");
  if (IsStartNativeChildProcessAvailable()) {
    return IsolateProcessHelper::StartChildProcess(command, fds);
  } else {
    LOGW("fallback to StartChildProcess");
    return StartChildProcess(command, fds);
  }
#else
  LOGW("fallback to StartChildProcess");
  return StartChildProcess(command, fds);
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
          callback(INVALID_PID);
        }
        LOGI("ChildProcessStarter::StartGpuProcess Start GPU process succeed.");
      };

  return StartChildProcess(start_func, commands, fds);
}

}  // namespace ohos::adapter::multiprocess
