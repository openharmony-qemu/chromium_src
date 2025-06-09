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
#include "command_line_helper.h"

#include <sstream>
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::multiprocess {
const std::string kCommandSeparator = "#";
const std::string kFdRemapCommand = "--fds_to_remap=";
const std::string kFdSeparator = ",";

std::string CommandLineHelper::BuildCommands(
    const std::vector<std::string>& commands,
    const std::vector<std::string>& extra_commands) {
  const size_t min_commands_size = 1;
  if (commands.size() < min_commands_size) {
    return "";
  }
  std::stringstream argv_ss;
  for (size_t i = 0; i < commands.size() - 1; ++i) {
    argv_ss << commands[i] << kCommandSeparator;
  }
  argv_ss << commands[commands.size() - 1];
  if (extra_commands.size() != 0) {
    argv_ss << kCommandSeparator;
  }
  for (size_t i = 0; i < extra_commands.size() - 1; ++i) {
    argv_ss << extra_commands[i] << kCommandSeparator;
  }
  argv_ss << extra_commands[extra_commands.size() - 1];
  return argv_ss.str();
}

std::vector<std::string> CommandLineHelper::Split(const std::string& src) {
  const size_t limit = 64;
  std::vector<std::string> res;
  if (src.empty()) {
    return res;
  }
  size_t start = 0;
  size_t end = 0;
  while (start < src.size() && (limit == 0 || res.size() < limit)) {
    end = src.find(kCommandSeparator, start);
    if (end == std::string::npos) {
      res.push_back(src.substr(start));
      return res;
    }
    res.push_back(src.substr(start, end - start));
    start = end + kCommandSeparator.length();
  }
  return res;
}

std::string CommandLineHelper::BuildSharedFDCommand(
    const std::vector<std::pair<int32_t, int32_t>>& fds) {
  std::string shared_fd_command = kFdRemapCommand;
  for (auto& [rawfd, remap_fd] : fds) {
    LOGD("HandleRequest fd %{public}d => %{public}d", rawfd, remap_fd);
    shared_fd_command += std::to_string(remap_fd) + kFdSeparator;
  }
  if (!shared_fd_command.empty()) {
    shared_fd_command.pop_back();
  }
  return shared_fd_command;
}
}  // namespace ohos::adapter::multiprocess
