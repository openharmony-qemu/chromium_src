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

#include "child_process_manager.h"

#include <cctype>
#include <cerrno>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include <AbilityKit/native_child_process.h>
#include <IPCKit/ipc_kit.h>
#include <sys/types.h>

#include "aki/jsbind.h"
#include "app_spawn_communication.h"
#include "gpu/gpu_native_process_host.h"
#include "native_child_process/native_child_process_args_wrapper.h"
#include "native_child_process/native_child_process_configs_wrapper.h"
#include "native_child_process/native_child_process_library.h"
#include "native_child_process/native_child_process_main.h"
#include "native_child_process/native_child_process_monitor.h"
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

namespace ohos::adapter::multiprocess {

static constexpr const char* kGpuProcessLib = "libadapter.so";
static constexpr const char* kGpuProcessName = "GPU";
static constexpr const char* kDefaultEntryPoint = "libadapter.so:ChildMain";
static const std::string kProcessTypePrefix = "--type=";
static const std::string kProcessSubTypePrefix = "--utility-sub-type=";
constexpr int32_t kInvalidPid = -1;
static constexpr int kProcessStartIntervalUs = 200000;
// Maximum wait time is 5 seconds, cannot wait indefinitely
static constexpr std::chrono::milliseconds kWait = std::chrono::milliseconds(5000);
// Maximum sleep time per iteration is 256 ms
static const uint32_t kMaxSleepInMicroseconds = 1 << 18;  // ~256 ms
// Initial sleep time is 1 ms
static const uint32_t kInitialSleepTime = 1 << 10;        // ~1 ms
// Adjust sleep time after every 4 sleep iterations
static const int kSleepTimeMultiplier = 4;
// Each adjustment doubles the sleep time
static const int kSleepTimeDouble = 2;
static constexpr const char* kPidPathDir = "/proc/";

using ohos::adapter::xcomponent::CrossProcessSyncResult;
using ohos::adapter::xcomponent::WindowAdapter;

namespace {
void LogStartChildProcessFixesCommands(
    const std::vector<std::string>& commands) {
  for (auto& command : commands) {
    LOGI("[ChildProcess] StartChildProcess command %{public}s",
         command.c_str());
  }
}

std::string NormalizedType(std::string value) {
  if (value.empty()) {
    return value;
  }

  // Remove parts before the last dot.
  if (auto pos = value.find_last_of('.'); pos != std::string::npos) {
    value.erase(0, pos + 1);
  }

  // Turn the first letter into uppercase.
  for (char& ch : value) {
    unsigned char uch = static_cast<unsigned char>(ch);
    if (std::isalpha(uch)) {
      if (std::islower(uch)) {
          ch = static_cast<char>(std::toupper(uch));
      }
      break;
    }
  }
  return value;
}

std::string GetProcessType(const std::string& arg, const std::string key) {
  if (arg.substr(0, key.length()) == key) {
    return NormalizedType(arg.substr(key.length()));
  }
  return "";
}

std::string GetProcessName(const std::vector<std::string>& commands) {
  std::string process_type;
  std::string process_sub_type;
  for (const std::string& command : commands) {
    if (process_type.empty()) {
      process_type = GetProcessType(command, kProcessTypePrefix);
    }
    if (process_sub_type.empty()) {
      process_sub_type = GetProcessType(command, kProcessSubTypePrefix);
    }
  }

  if (!process_sub_type.empty()) {
    return process_sub_type;
  }
  if (!process_type.empty()) {
    return process_type;
  }
  return "ChildProcess";
}

// Starts the child process by calling OH_Ability_CreateNativeChildProcess(),
// after the child process is launched and the OHIPCRemoteStub object is
// created, this function is called in the main process:
// 1. synchronize the WindowAdapter with the child process,
// 2. and then query PID of the child process.
void OnGpuProcessStarted(int err_code, OHIPCRemoteProxy* remote_proxy) {
  ChildProcessManager& manager = ChildProcessManager::GetInstance();

  if (err_code != NCP_NO_ERROR || !remote_proxy) {
    LOGE(
        "[ChildProcess] OnGpuProcessStarted | Child process start failed, "
        "err_code=%{public}d, remote_proxy: %{public}s",
        err_code, (remote_proxy ? "not null" : "null"));
    manager.GpuProcessStarted(kInvalidPid);
    return;
  }

  GpuNativeProcessHost& host = GpuNativeProcessHost::GetInstance();
  host.Initialize(remote_proxy);

  if (WindowAdapter::GetInstance().SyncWindowToGpuProcess() !=
      CrossProcessSyncResult::SUCCESS) {
    LOGE(
        "[ChildProcess] OnGpuProcessStarted | Sync window adapter to GPU "
        "process failed.");
    manager.GpuProcessStarted(kInvalidPid);
    return;
  }

  int32_t child_pid = host.GetPid();
  if (child_pid == kInvalidPid) {
    LOGE(
        "[ChildProcess] OnGpuProcessStarted | Get child process pid failed, "
        "child pid got: %{public}d",
        child_pid);
    manager.GpuProcessStarted(kInvalidPid);
    return;
  }

  LOGI(
      "[ChildProcess] OnGpuProcessStarted | GPU process started, pid: "
      "%{public}d",
      child_pid);
  manager.GpuProcessStarted(child_pid);
}
}  // namespace

ChildProcessManager& ChildProcessManager::GetInstance() {
  static ChildProcessManager instance;
  return instance;
}

ChildProcessManager::ChildProcessManager() {
  if (native_api_.SupportsNativeChildProcessExitCallback()) {
    Ability_NativeChildProcess_ErrCode err =
        native_api_.RegisterNativeChildProcessExitCallback(
            ChildProcessManager::OnChildSignal);
    if (err != NCP_NO_ERROR) {
      LOGW(
          "[ChildProcess] Register child process exit callback failed, err: "
          "%{public}d",
          err);
    } else {
      monitor_ = std::make_unique<NativeChildProcessMonitor>();
    }
  }
}

ChildProcessManager::~ChildProcessManager() {
  if (native_api_.SupportsNativeChildProcessExitCallback() && monitor_) {
    Ability_NativeChildProcess_ErrCode err =
        native_api_.UnregisterNativeChildProcessExitCallback(
            ChildProcessManager::OnChildSignal);
    if (err != NCP_NO_ERROR) {
      LOGW(
          "[ChildProcess] Unregister child process exit callback failed, err: "
          "%{public}d",
          err);
    }
  }
}

// static
void ChildProcessManager::OnChildSignal(int32_t pid, int32_t signal) {
  LOGI(
      "[ChildProcess] OnChildSignal | Receive child signal, pid: %{public}d, "
      "signal: %{public}d",
      pid, signal);
  ChildProcessManager::GetInstance().HandleChildSignal(pid, signal);
}

void ChildProcessManager::HandleChildSignal(int32_t pid, int32_t signal) {
  if (monitor_) {
    monitor_->HandleChildSignal(pid, signal);
  }
}

void ChildProcessManager::OnChildProcessStarted(int32_t pid) {
  if (monitor_) {
    monitor_->HandleChildStarted(pid);
  }
}

int32_t ChildProcessManager::WaitChildPid(const int32_t pid,
                                          int* status,
                                          const bool can_block) {
  if (!monitor_) {
    return -1;
  }

  return monitor_->WaitChildPid(pid, status, can_block);
}

int32_t ChildProcessManager::StartChildProcess(
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds_to_remap,
    const std::string& entry_point,
    bool is_isolated) {
  TRACE_EVENT_0("ChildProcessManager::StartChildProcess");

  LogStartChildProcessFixesCommands(commands);

#if !defined(ENABLE_ISOLATED_CHILD_PROCESS)
  if (is_isolated) {
    LOGW(
        "[ChildProcess] StartChildProcess | Isolated child process not enabled,"
        " fallback to normal process.");
    is_isolated = false;
  }
#endif
  std::string child_entry =
      entry_point.empty() ? kDefaultEntryPoint : entry_point;
  int32_t child_pid = kInvalidPid;

#if defined(ENABLE_NATIVE_CHILD_PROCESS)
  child_pid = TryStartNativeChildProcess(commands, fds_to_remap, child_entry,
                                         is_isolated);
#else
  child_pid = StartLegacyChildProcess(commands, fds_to_remap);
#endif
  return child_pid;
}

int32_t ChildProcessManager::StartGpuProcess(
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds_to_remap) {
  TRACE_EVENT_0("ChildProcessManager::StartGpuProcess");

  LogStartChildProcessFixesCommands(commands);

  std::function<void(StartCallbackFunc)> start_func =
      [this](StartCallbackFunc callback) {
        CreateGpuProcessInternal(callback);
      };

  return StartChildProcessInternal(start_func, commands, fds_to_remap);
}

int32_t ChildProcessManager::StartChildProcessInternal(
    std::function<void(StartCallbackFunc)> start_func,
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds_to_remap) {
  std::promise<int32_t> child_pid;
  StartCallbackFunc set_child_pid = [&child_pid](int32_t pid) -> void {
    child_pid.set_value(pid);
  };

  std::lock_guard<std::mutex> auto_lock(mutex_);
  // Must establish socket listen before start child process
  // if not, child process request will be "Connection failed"
  AppSpawnCommunication appspawn_server;
  if (appspawn_server.PrepareHandleRequest(commands, fds_to_remap)) {
    start_func(set_child_pid);
    int cpid = child_pid.get_future().get();
    appspawn_server.HandleRequestAfterChildProcStart(cpid);
    LOGI("[ChildProcess] StartChildProcessInternal | Child process started, "
         "pid: %{public}d", cpid);
    return cpid;
  }

  LOGE("[ChildProcess] StartChildProcessInternal | "
       "appspawn_server.PrepareHandleRequest failed");
  return kInvalidPid;
}

int32_t ChildProcessManager::StartLegacyChildProcess(
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds_to_remap) {
  TRACE_EVENT_0("ChildProcessManager::StartLegacyChildProcess");
  auto start_child_process_func = ohos::adapter::GetJSFunction(
      "ChromiumChildProcessStarter.StartChildProcess");
  if (!start_child_process_func) {
    LOGE(
        "[ChildProcess] StartLegacyChildProcess | "
        "ChromiumChildProcessStarter.StartChildProcess not defined");
    return kInvalidPid;
  }

  std::function<void(StartCallbackFunc)> start_func =
      [start_child_process_func](StartCallbackFunc callback) {
        start_child_process_func->Invoke<void>(callback);
      };

  return StartChildProcessInternal(start_func, commands, fds_to_remap);
}

int32_t ChildProcessManager::StartNativeChildProcess(
    const std::string& process_name,
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds_to_remap,
    const std::string& entry_point,
    bool is_isolated) {
  TRACE_EVENT_0("ChildProcessManager::StartNativeChildProcess");

  NativeChildProcessArgsWrapper args_wrapper(commands, fds_to_remap);
  NativeChildProcess_Options options{
      .isolationMode = is_isolated ? NCP_ISOLATION_MODE_ISOLATED
                                   : NCP_ISOLATION_MODE_NORMAL};

  int32_t pid = kInvalidPid;
  Ability_NativeChildProcess_ErrCode err = native_api_.StartNativeChildProcess(
      entry_point.c_str(), args_wrapper.Get(), options, &pid);
  if (err != NCP_NO_ERROR || pid == kInvalidPid) {
    LOGE(
        "[ChildProcess] StartNativeChildProcess failed, entry: %{public}s, "
        "process_name: %{public}s, is_isolated: %{public}d, errcode: "
        "%{public}d, pid=%{public}d",
        entry_point.c_str(), process_name.c_str(), is_isolated, err, pid);
    return kInvalidPid;
  }

  LOGI(
      "[ChildProcess] StartNativeChildProcess | Child process started, "
      "name: %{public}s, pid: %{public}d, isolated: %{public}d",
      process_name.c_str(), pid, is_isolated);
  OnChildProcessStarted(pid);
  return pid;
}

int32_t ChildProcessManager::StartNativeChildProcessWithName(
    const std::string& process_name,
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds_to_remap,
    const std::string& entry_point,
    bool is_isolated) {
  TRACE_EVENT_0("ChildProcessManager::StartNativeChildProcessWithName");

  NativeChildProcessArgsWrapper args_wrapper(commands, fds_to_remap);
  NativeChildProcessConfigsWrapper configs_wrapper(native_api_);

  Ability_ChildProcessConfigs* configs = configs_wrapper.Get();
  if (!configs) {
    LOGE("[ChildProcess] Create child process configs failed.");
    return kInvalidPid;
  }

  Ability_NativeChildProcess_ErrCode err =
      native_api_.SetProcessName(configs, process_name.c_str());
  if (err != NCP_NO_ERROR) {
    LOGE(
        "[ChildProcess] StartNativeChildProcessWithName | "
        "Set process name failed, process_name: %{public}s, "
        "errcode: %{public}d", process_name.c_str(), err);
  }

  err = native_api_.SetIsolationMode(configs, is_isolated
                                                  ? NCP_ISOLATION_MODE_ISOLATED
                                                  : NCP_ISOLATION_MODE_NORMAL);
  if (err != NCP_NO_ERROR) {
    LOGE(
        "[ChildProcess] StartNativeChildProcessWithName | Set isolation mode "
        "failed, entry: %{public}s, process_name: %{public}s, "
        "is_isolated: %{public}d, errcode: %{public}d",
        entry_point.c_str(), process_name.c_str(), is_isolated, err);
  }

  int32_t pid = kInvalidPid;
  err = native_api_.StartNativeChildProcessWithConfigs(
      entry_point.c_str(), args_wrapper.Get(), configs, &pid);
  if (err != NCP_NO_ERROR || pid == kInvalidPid) {
    LOGE(
        "[ChildProcess] StartNativeChildProcessWithName failed, entry: "
        "%{public}s, process_name: %{public}s, is_isolated: %{public}d, "
        "errcode: %{public}d, pid=%{public}d",
        entry_point.c_str(), process_name.c_str(), is_isolated, err, pid);
    return kInvalidPid;
  }

  LOGI(
      "[ChildProcess] StartNativeChildProcessWithName | Child process started, "
      "name: %{public}s, pid: %{public}d, isolated: %{public}d",
      process_name.c_str(), pid, is_isolated);
  OnChildProcessStarted(pid);
  return pid;
}

int32_t ChildProcessManager::TryStartNativeChildProcess(
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int, int>>& fds_to_remap,
    const std::string& entry_point,
    bool is_isolated) {
  std::string process_name = GetProcessName(commands);
  int32_t child_pid = kInvalidPid;
  if (native_api_.SupportsStartChildProcessWithConfigs()) {
    child_pid = StartNativeChildProcessWithName(
        process_name, commands, fds_to_remap, entry_point, is_isolated);
  } else if (native_api_.SupportsStartChildProcess()) {
    child_pid = StartNativeChildProcess(process_name, commands, fds_to_remap,
                                        entry_point, is_isolated);
  }
  return child_pid;
}

void ChildProcessManager::GpuProcessStarted(int32_t child_pid) {
  if (gpu_process_callback_) {
    std::move(gpu_process_callback_)(child_pid);
  }
}

int ChildProcessManager::CreateGpuProcess() {
  TRACE_EVENT_0("ChildProcessManager::CreateGpuProcess");
  return OH_Ability_CreateNativeChildProcess(kGpuProcessLib,
                                             &OnGpuProcessStarted);
}

int ChildProcessManager::CreateGpuProcessWithName() {
  TRACE_EVENT_0("ChildProcessManager::CreateGpuProcessWithName");

  NativeChildProcessConfigsWrapper configs_wrapper(native_api_);
  Ability_ChildProcessConfigs* configs = configs_wrapper.Get();
  if (!configs) {
    LOGE(
        "[ChildProcess] CreateGpuProcessWithName | Create child process "
        "configs failed.");
    return NCP_ERR_INTERNAL;
  }

  Ability_NativeChildProcess_ErrCode err =
      native_api_.SetProcessName(configs, kGpuProcessName);
  if (err != NCP_NO_ERROR) {
    LOGE(
        "[ChildProcess] CreateGpuProcessWithName | Set process name failed, "
        "process_name: %{public}s, errcode: %{public}d",
        kGpuProcessName, err);
  }

  return native_api_.CreateChildProcessWithConfigs(kGpuProcessLib, configs,
                                                   &OnGpuProcessStarted);
}

int ChildProcessManager::CreateGpuProcessInternal(
    std::function<void(int32_t)> callback) {
  gpu_process_callback_ = callback;

  LOGW(
      "[ChildProcess] CreateGpuProcessInternal | GPU process start times: "
      "%{public}d",
      gpu_process_start_times_);
  if (gpu_process_start_times_ > 0) {
    // Since the previous GPU process has crashed, wait for a while
    // before launching a new GPU process, to let the previous one exit.
    // We can only have one native process at a time.
    usleep(kProcessStartIntervalUs);
  }
  ++gpu_process_start_times_;

  int ret = NCP_NO_ERROR;
  if (native_api_.SupportsCreateChildProcessWithConfigs()) {
    ret = CreateGpuProcessWithName();
  } else {
    ret = CreateGpuProcess();
  }

  if (ret != NCP_NO_ERROR) {
    LOGE("[ChildProcess] StartGpuProcess error: %{public}d", ret);
    std::move(gpu_process_callback_)(kInvalidPid);
  }

  return ret;
}

bool CheckProcessDirectoryExists(pid_t pid) {
  std::string fileName = kPidPathDir + std::to_string(pid);
  struct stat buffer = {};
  return (stat(fileName.c_str(), &buffer) == 0);
}

pid_t WaitpidBlock(pid_t handle) {
  auto wakeupTime = std::chrono::steady_clock::now() + kWait;
  uint32_t maxSleepTimeUsecs = kInitialSleepTime;
  int doubleSleepTime = 0;
  while (CheckProcessDirectoryExists(handle)) {
    auto now = std::chrono::steady_clock::now();
    if (now > wakeupTime) {
      break;
    }

    auto sleep_time = std::chrono::microseconds(std::min<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(wakeupTime - now)
            .count(),
        maxSleepTimeUsecs));
    std::this_thread::sleep_for(sleep_time);
    if ((maxSleepTimeUsecs < kMaxSleepInMicroseconds) &&
        (doubleSleepTime++ % kSleepTimeMultiplier == 0)) {
      maxSleepTimeUsecs *= kSleepTimeDouble;
    }
  }

  if (CheckProcessDirectoryExists(handle)) {
    LOGE("[ChildProcess] Check Process Exists pid: %{public}d", handle);
    return -1;
  }
  return handle;
}

pid_t Waitpid(pid_t handle, int* status, int nonBlocking) {
  bool childProcessExist = CheckProcessDirectoryExists(handle);
  LOGI("[ChildProcess] Check Process Exists: %{public}d pid: %{public}d",
       childProcessExist, handle);
  int tmpStatus = 0;
  if (!status) {
    status = &tmpStatus;
  }

  pid_t retPid = ChildProcessManager::GetInstance().WaitChildPid(
      handle, status, !nonBlocking);
  LOGI(
      "[ChildProcess] get termination status impl can_block: %{public}d, "
      "pid: %{public}d, result: %{public}d,"
      " status: %{public}d, WIFSIGNALED(status): %{public}d, "
      "WTERMSIG(status): %{public}d",
      !nonBlocking, handle, retPid, *status, WIFSIGNALED(*status),
      WTERMSIG(*status));
  if (retPid >= 0) {
    return retPid;
  }

  childProcessExist = CheckProcessDirectoryExists(handle);
  LOGI("[ChildProcess] Check Process Exists: %{public}d pid: %{public}d",
       childProcessExist, handle);
  if (nonBlocking && childProcessExist) {
    return 0;
  }
  return WaitpidBlock(handle);
}
}  // namespace ohos::adapter::multiprocess
