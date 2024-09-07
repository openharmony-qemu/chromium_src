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

namespace {
const std::string IPC_FD_NAME = "IPC_FD";
const std::string SHARED_FD_NAME = "SHARED_FD";
const std::string CRASH_FD_NAME = "CRASH_FD";
const int FD_COUNTS = 3;
} // namespace

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
      return;
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
  std::string fdStr = std::to_string(ipcFd) + "-" + std::to_string(sharedFd) + "-" + std::to_string(crashFd);

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
  (void)CefExecuteProcess(main_args, nullptr, static_cast<void*>(&fdStr));

  WVLOG_I("NWebRenderMain end, sandbox pid=%{public}d global pid=%{public}d", getpid());
}