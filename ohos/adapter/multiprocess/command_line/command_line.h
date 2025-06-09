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

#ifndef OHOS_ADAPTER_MULTIPROCESS_COMMAND_LINE_H
#define OHOS_ADAPTER_MULTIPROCESS_COMMAND_LINE_H

#include <map>
#include <string>
#include <vector>

#include "command_line_switches.h"

namespace ohos::adapter::multiprocess {
class CommandLine {
 public:
  static bool InitForCurrentProcess(const std::vector<std::string>& args);
  static void ResetForCurrentProcess();

  static CommandLine* ForCurrentProcess();

  bool HasSwitch(const char* switch_key);

 private:
  void ParseCommandLine(const std::vector<std::string>& args);

  bool ParseSwitch(const std::string& arg,
                   std::string* switch_key,
                   std::string* switch_val);

  void AppendSwitch(const std::string& switch_key,
                    const std::string& switch_val);

  std::size_t GetSwitchPrefixLength(const std::string& arg);

  explicit CommandLine(const std::vector<std::string>& args);
  ~CommandLine() = default;

  std::map<std::string, std::string> switches_;
  static CommandLine* current_process_command_line_;
};
}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_MULTIPROCESS_COMMAND_LINE_H
