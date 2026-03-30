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
#include "native_child_process_args_wrapper.h"

#include <cstdio>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/multiprocess/command_line/command_line_helper.h"

namespace ohos::adapter::multiprocess {

constexpr size_t kFdNameSize = 16;

void NodeDeleter::operator()(NativeChildProcess_Fd* node) const noexcept {
  while (node) {
    NativeChildProcess_Fd* next = node->next;
    if (node->fdName) {
      delete[] node->fdName;
      node->fdName = nullptr;
    }
    
    delete node;
    node = next;
  }
}

NativeChildProcessArgsWrapper::NativeChildProcessArgsWrapper(
    const std::vector<std::string>& commands,
    const std::vector<std::pair<int32_t, int32_t>>& fds_to_remap) {
  auto shared_fd_command = CommandLineHelper::BuildSharedFDCommand(fds_to_remap);
  params_ = CommandLineHelper::BuildCommands(commands, {shared_fd_command});
  native_args_.entryParams = params_.data();

  UniqueNode head(nullptr);
  NativeChildProcess_Fd* tail = nullptr;

  for (const auto& [fd_from, fd_to] : fds_to_remap) {
    auto node =
        UniqueNode(new NativeChildProcess_Fd({nullptr, fd_from, nullptr}));
    auto fd_name = std::make_unique<char[]>(kFdNameSize);
    std::snprintf(fd_name.get(), kFdNameSize, "%d", fd_to);
    node->fdName = fd_name.release();

    if (tail) {
      tail->next = node.get();
    } else {
      head.reset(node.get());
    }
    tail = node.release();
  }

  native_args_.fdList.head = head.get();
  fd_list_ = std::move(head);
}

NativeChildProcess_Args NativeChildProcessArgsWrapper::Get() {
  return native_args_;
}

// static
void NativeChildProcessArgsWrapper::Parse(
    NativeChildProcess_Args args,
    std::vector<std::string>& commands,
    std::vector<std::pair<int32_t, int32_t>>& fds_to_remap) {
  commands.clear();
  if (args.entryParams == nullptr) {
    return;
  }

  std::vector<std::string> commandline =
      CommandLineHelper::Split(args.entryParams);

  auto current = args.fdList.head;
  while (current != nullptr) {
    int fd = current->fd;
    LOGD("Parse %{public}d => %{public}s", fd, current->fdName);
    fds_to_remap.push_back({std::stoi(current->fdName), fd});
    current = current->next;
  }
}
}  // namespace ohos::adapter::multiprocess
