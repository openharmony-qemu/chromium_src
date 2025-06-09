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

#include "native_childprocess_args_wrapper.h"
#include <cstdio>
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/multiprocess/command_line/command_line_helper.h"

namespace ohos::adapter::multiprocess {

NativeChildProcess_Args NativeChildProcessArgsWrapper::Build(
    const std::string& params,
    const std::vector<std::pair<int32_t, int32_t>>& fds) {
  NativeChildProcess_Args args;
  args.entryParams = new (std::nothrow) char[params.size() + 1]{0};
  if (args.entryParams != nullptr) {
    std::copy(params.c_str(), params.c_str() + params.size(), args.entryParams);
  }
  auto tail = args.fdList.head;
  for (size_t i = 0; i < fds.size(); i++) {
    auto fd = fds[i].first;
    LOGD("NativeChildProcessArgsWrapper::Build, FD %{public}d => %{public}d",
         fd, fds[i].second);
    NativeChildProcess_Fd* current = new (std::nothrow) NativeChildProcess_Fd;
    if (current == nullptr) {
      break;
    }
    const size_t fd_name_size = 16;
    char* fd_name = new (std::nothrow) char[fd_name_size]{0};
    if (fd_name == nullptr) {
      break;
    }
    sprintf(fd_name, "%d", fds[i].second);
    current->fdName = fd_name;
    current->fd = fd;
    current->next = nullptr;
    if (tail != nullptr) {
      tail->next = current;
    }
    if (args.fdList.head == nullptr) {
      args.fdList.head = current;
    }
    tail = current;
  }

  return args;
}

void NativeChildProcessArgsWrapper::Parse(
    NativeChildProcess_Args args,
    std::vector<std::string>& commands,
    std::vector<std::pair<int32_t, int32_t>>& fds) {
  commands.clear();
  if (args.entryParams == nullptr) {
    return;
  }

  std::vector<std::string> commandline =
      CommandLineHelper::Split(args.entryParams);

  auto tail = args.fdList.head;
  auto current = tail;

  while (current != nullptr) {
    int fd = current->fd;
    LOGD(" Parse %{public}d => %{public}s", fd, current->fdName);
    fds.push_back({std::stoi(current->fdName), fd});
    current = current->next;
  }
}

void NativeChildProcessArgsWrapper::Free(NativeChildProcess_Args& args) {
  if (args.entryParams != nullptr) {
    delete (args.entryParams);
    args.entryParams = nullptr;
  }

  auto tail = args.fdList.head;
  auto current = tail;
  while (current != nullptr) {
    auto next = current->next;
    delete (current);
    current = next;
  }
}
}  // namespace ohos::adapter::multiprocess
