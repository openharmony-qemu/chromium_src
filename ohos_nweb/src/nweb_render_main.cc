/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <sstream>

#include "include/cef_app.h"
#include "nweb_export.h"
#include "nweb_hilog.h"
#include "base/process/process_handle.h"
#include <AbilityKit/native_child_process.h>
#include <IPCKit/ipc_kit.h>
#include <native_window/external_window.h>

#include "base/posix/global_descriptors.h"
#include "content/public/common/content_descriptors.h"

namespace {
const std::string IPC_FD_NAME = "IPC_FD";
const std::string SHARED_FD_NAME = "SHARED_FD";
const std::string CRASH_FD_NAME = "CRASH_FD";
const int FD_COUNTS = 3;
} // namespace

bool SetGlobalDescriptors(int ipcFd, int sharedFd, int crashFd) {
  base::GlobalDescriptors* g_fds = base::GlobalDescriptors::GetInstance();
  if (g_fds == nullptr) {
    WVLOG_E("GlobalDescriptors is null");
    return false;
  }

  int new_ipc_fd;
  if ((new_ipc_fd = dup(ipcFd)) < 0) {
    WVLOG_E("ipcFd duplicate error");
    g_fds->Set(kMojoIPCChannel, ipcFd);
  } else {
    g_fds->Set(kMojoIPCChannel, new_ipc_fd);
    close(ipcFd);
  }

  int new_shared_fd;
  if ((new_shared_fd = dup(sharedFd)) < 0) {
    WVLOG_E("sharedFd duplicate error");
    g_fds->Set(kFieldTrialDescriptor, sharedFd);
  } else {
    g_fds->Set(kFieldTrialDescriptor, new_shared_fd);
    close(sharedFd);
  }

  // crashfd is invalid if crashpad closed. Skip set crashfd in this case.
  if (crashFd <= 0) {
    return true;
  }
  int new_crash_fd;
  if ((new_crash_fd = dup(crashFd)) < 0) {
    WVLOG_E("crashFd duplicate error");
    g_fds->Set(kCrashDumpSignal, crashFd);
  } else {
    g_fds->Set(kCrashDumpSignal, new_crash_fd);
    close(crashFd);
  }
  return true;
}

extern "C" OHOS_NWEB_EXPORT void NWebRenderMain(NativeChildProcess_Args args) {
  WVLOG_I("NWebRenderMain start, sandbox pid=%{public}d", getpid());

  std::string args_str = std::string(args.entryParams);
  NativeChildProcess_Fd *fdNode = args.fdList.head;
  int ipcFd = 0;
  int sharedFd = 0;
  int crashFd = 0;
  for (int i = 0; i < FD_COUNTS; ++i) {
    if (fdNode == nullptr) {
      WVLOG_E("get render fd failed");
      break;
    }
    
    if (std::string(fdNode->fdName) == IPC_FD_NAME) {
      ipcFd = fdNode->fd;
    } else if (std::string(fdNode->fdName) == SHARED_FD_NAME) {
      sharedFd = fdNode->fd;
    } else if (std::string(fdNode->fdName) == CRASH_FD_NAME) {
      crashFd = fdNode->fd;
    } else {
      WVLOG_E("unknow render fd name %{public}s", fdNode->fdName);
      return;
    }
    fdNode = fdNode->next;
  }
  if (!SetGlobalDescriptors(ipcFd, sharedFd, crashFd)) {
    WVLOG_E("failed to set global fd");
    return;
  }

  std::stringstream args_ss(args_str);
  const char separator = '#';
  std::vector<std::string> argv_str;
  std::string arg_str;
  while (std::getline(args_ss, arg_str, separator)) {
    argv_str.push_back(arg_str);
  }
  std::vector<char*> argv_cstr;
  int argc = argv_str.size();
  argv_cstr.reserve(argc + 1);
  for (const auto& arg : argv_str) {
    argv_cstr.push_back(const_cast<char*>(arg.c_str()));
  }
  argv_cstr.push_back(nullptr);

  CefMainArgs main_args(argc, const_cast<char**>(argv_cstr.data()));
  (void)CefExecuteProcess(main_args, nullptr, nullptr);

  WVLOG_I("NWebRenderMain end, sandbox pid=%{public}d global pid=%{public}d", getpid());
}