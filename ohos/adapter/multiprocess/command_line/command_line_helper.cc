// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "command_line_helper.h"

#include <sstream>

#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::multiprocess {
const std::string CommandSeparator = "#";
const std::string FdRemapCommand = "--fds_to_remap=";
const std::string FdSeparator = ",";

std::string CommandLineHelper::BuildCommands(
    const std::vector<std::string>& commands,
    const std::vector<std::string>& extra_commands) {
  const size_t min_commands_size = 1;
  if (commands.size() < min_commands_size) {
    return "";
  }
  std::stringstream argv_ss;
  for (size_t i = 0; i < commands.size() - 1; ++i) {
    argv_ss << commands[i] << CommandSeparator;
  }
  argv_ss << commands[commands.size() - 1];
  if (extra_commands.size() != 0) {
    argv_ss << CommandSeparator;
  }
  for (size_t i = 0; i < extra_commands.size() - 1; ++i) {
    argv_ss << extra_commands[i] << CommandSeparator;
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
    end = src.find(CommandSeparator, start);
    if (end == std::string::npos) {
      res.push_back(src.substr(start));
      return res;
    }
    res.push_back(src.substr(start, end - start));
    start = end + CommandSeparator.length();
  }
  return res;
}

std::string CommandLineHelper::BuildSharedFDCommand(
    const std::vector<std::pair<int32_t, int32_t>>& fds) {
  std::string shared_fd_command = FdRemapCommand;
  for (auto& [rawfd, remap_fd] : fds) {
    LOGD("HandleRequest fd %{public}d => %{public}d", rawfd, remap_fd);
    shared_fd_command += std::to_string(remap_fd) + FdSeparator;
  }
  if (!shared_fd_command.empty()) {
    shared_fd_command.pop_back();
  }
  return shared_fd_command;
}
}  // namespace ohos::adapter::multiprocess
