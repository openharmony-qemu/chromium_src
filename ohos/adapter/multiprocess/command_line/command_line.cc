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
/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
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

#include "command_line.h"

#include <cctype>
#include <cstring>
#include <sstream>

#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::multiprocess {
namespace {
const std::string kSwitchPrefixes[] = {"--", "-"};
const std::string kSwitchValueSeparator = "=";
constexpr int kCrashPadFileDescriptor = 0;
} // namespace

std::string FormatArgPrefix(const char* prefix,
                            const char* arg_name,
                            bool has_kv_sep) {
  std::string arg_str;
  arg_str.append(prefix).append(arg_name);
  if (has_kv_sep) {
    arg_str.append(kSwitchValueSeparator);
  }
  return arg_str;
}

std::string NormalizedType(std::string value) {
  if (value.empty()) {
    return value;
  }

  if (auto pos = value.find_last_of('.'); pos != std::string::npos) {
    value.erase(0, pos + 1);
  }

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
      process_type =
          GetProcessType(command, FormatArgPrefix("--", switches::kType, true));
    }
    if (process_sub_type.empty()) {
      process_sub_type = GetProcessType(
          command, FormatArgPrefix("--", switches::kUtilitySubType, true));
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

CommandLine* CommandLine::current_process_command_line_ = nullptr;

bool CommandLine::InitForCurrentProcess(const std::vector<std::string>& args) {
  if (current_process_command_line_) {
    return false;
  }
  current_process_command_line_ = new CommandLine(args);
  return true;
}

void CommandLine::ResetForCurrentProcess() {
  if (current_process_command_line_ == nullptr) {
    return;
  }

  delete current_process_command_line_;
  current_process_command_line_ = nullptr;
}

CommandLine* CommandLine::ForCurrentProcess() {
  return current_process_command_line_;
}

CommandLine::CommandLine(const std::vector<std::string>& args) {
  ParseCommandLine(args);
}

void CommandLine::ParseCommandLine(const std::vector<std::string>& args) {
  for (const std::string& arg : args) {
    std::string switch_key;
    std::string switch_val;
    if (ParseSwitch(arg, &switch_key, &switch_val)) {
      AppendSwitch(switch_key, switch_val);
    }
  }
}

bool CommandLine::ParseSwitch(const std::string& arg,
                              std::string* switch_key,
                              std::string* switch_val) {
  switch_key->clear();
  switch_val->clear();
  std::size_t prefix_len = GetSwitchPrefixLength(arg);
  if (prefix_len == 0 || prefix_len == arg.length()) {
    return false;
  }

  std::size_t equals_position = arg.find(kSwitchValueSeparator);
  if (equals_position == std::string::npos) {
    *switch_key = arg.substr(prefix_len);
  } else {
    *switch_key = arg.substr(prefix_len, equals_position - prefix_len);
    *switch_val = arg.substr(equals_position + 1);
  }
  return true;
}

std::size_t CommandLine::GetSwitchPrefixLength(const std::string& arg) {
  for (const std::string& prefix : kSwitchPrefixes) {
    if (arg.substr(0, prefix.length()) == prefix) {
      return prefix.length();
    }
  }
  return 0;
}

void CommandLine::AppendSwitch(const std::string& switch_key,
                               const std::string& switch_val) {
  switches_[switch_key] = switch_val;
}

bool CommandLine::HasSwitch(const char* switch_key) {
  if (switch_key == nullptr) {
    return false;
  }
  return switches_.contains(std::string(switch_key));
}

std::string CommandLine::GetSwitchValue(const char* switch_key) {
  if (switch_key == nullptr) {
    return "";
  }
  auto it = switches_.find(switch_key);
  if (it != switches_.end()) {
    return it->second;
  }
  return "";
}

std::string CommandLine::Join(const std::vector<std::string>& args,
                              char separator) {
  const size_t min_commands_size = 1;
  if (args.size() < min_commands_size) {
    return "";
  }
  std::stringstream argv_ss;
  for (size_t i = 0; i < args.size() - 1; ++i) {
    argv_ss << args[i] << separator;
  }
  argv_ss << args[args.size() - 1];
  return argv_ss.str();
}

std::vector<std::string> CommandLine::Split(const std::string& args,
                                            char separator) {
  const size_t limit = 64;
  std::vector<std::string> res;
  if (args.empty()) {
    return res;
  }
  size_t start = 0;
  size_t end = 0;
  while (start < args.size() && (limit == 0 || res.size() < limit)) {
    end = args.find(separator, start);
    if (end == std::string::npos) {
      res.push_back(args.substr(start));
      return res;
    }
    res.push_back(args.substr(start, end - start));
    start = end + 1;
  }
  return res;
}

std::string CommandLine::BuildFdRemapCommand(
    const std::vector<std::pair<int, int>>& fds) {
  std::string command = FormatArgPrefix("--", switches::kFdRemapSwitch, true);
  for (auto& [fd, id] : fds) {
    LOGD("BuildFdRemapCommand fd %{public}d => %{public}d", id, fd);
    command += std::to_string(id) + kFdPairSeparator;
  }
  if (fds.size() > 0) {
    command.pop_back();
  }
  return command;
}

std::vector<int> CommandLine::ParseFdRemapIds(
    const std::vector<std::string>& commands) {
  std::string arg_prefix =
      FormatArgPrefix("--", switches::kFdRemapSwitch, true);
  std::string fd_string;
  for (const auto& command : commands) {
    if (command.find(arg_prefix) == 0) {
      fd_string = command.substr(arg_prefix.size());
      break;
    }
  }
  LOGD("ParseFdRemapIds fd_string: %{public}s", fd_string.c_str());
  auto id_strs = Split(fd_string, kFdPairSeparator);
  std::vector<int> res;
  for (auto& id_str : id_strs) {
    if (id_str.empty()) {
      continue;
    }
    res.push_back(std::stoi(id_str));
  }
  return res;
}

void CommandLine::FixCrashpadClientFd(
    std::vector<std::string>& cmds,
    const std::vector<std::pair<int, int>>& fds) {
  bool is_crash_pad = false;
  size_t arg_index = -1;
  std::string type_prefix = FormatArgPrefix("--", switches::kType, true);
  std::string client_fd_prefix =
      FormatArgPrefix("--", switches::kInitialClientFd, true);
  for (size_t i = 0; i < cmds.size(); ++i) {
    std::string type = GetProcessType(cmds[i], type_prefix);
    if (type == kCrashPadHandlerType) {
      is_crash_pad = true;
    }
    if (cmds[i].find(client_fd_prefix) != std::string::npos) {
      arg_index = i;
    }
  }

  if (!is_crash_pad || arg_index < 0) {
    return;
  }

  std::string init_fd;
  for (auto& [fd, id] : fds) {
    if (id == kCrashPadFileDescriptor) {
      init_fd = std::to_string(static_cast<int>(fd));
      break;
    }
  }
  if (!init_fd.empty()) {
    cmds[arg_index] = client_fd_prefix + init_fd;
  }
}

::ohos::adapter::multiprocess::ArgcArgv CommandLine::BuildArgcArgv(
    const std::vector<std::string>& commands) {
    ::ohos::adapter::multiprocess::ArgcArgv result;
    std::string ohos_args_begin =
        FormatArgPrefix("--", switches::kOhosArgsBegin, false);
    size_t buffer_size = 0;
    for (const auto& cmd : commands) {
        if (cmd == ohos_args_begin) {
            break;
        }
        buffer_size += cmd.size() + 1;
    }

    if (buffer_size == 0) {
        return result;
    }

    result.buffer.resize(buffer_size);
    size_t offset = 0;

    for (const auto& cmd : commands) {
        if (cmd == ohos_args_begin) {
            break;
        }
        std::memcpy(result.buffer.data() + offset, cmd.data(), cmd.size());
        result.buffer[offset + cmd.size()] = '\0';
        result.argv.push_back(result.buffer.data() + offset);
        offset += cmd.size() + 1;
    }

    result.argc = static_cast<int>(result.argv.size());
    return result;
}
}  // namespace ohos::adapter::multiprocess
