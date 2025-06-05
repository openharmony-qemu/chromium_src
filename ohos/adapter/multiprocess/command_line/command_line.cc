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

#include "command_line.h"

namespace ohos::adapter::multiprocess {

static const std::string kSwitchPrefixes[] = {"--", "-"};

static const std::string kSwitchValueSeparator = "=";

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
}  // namespace ohos::adapter::multiprocess
