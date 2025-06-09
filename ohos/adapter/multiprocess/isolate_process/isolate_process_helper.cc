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

#include "isolate_process_helper.h"

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/multiprocess/command_line/command_line_helper.h"
#include "ohos/adapter/multiprocess/isolate_process/native_childprocess_args_wrapper.h"
#include "ohos/adapter/multiprocess/isolate_process/native_childprocess_interface.h"

namespace ohos::adapter::multiprocess {

static constexpr const char* kDefaultEntryPoint = "libadapter.so:IsolateMain";

int32_t IsolateProcessHelper::StartChildProcess(
    const std::vector<std::string>& command,
    const std::vector<std::pair<int, int>>& fds,
    const std::string& entry_point) {
  TRACE_EVENT_0("IsolateProcessHelper::StartChildProcess");

  int32_t pid = -1;

  auto shared_fd_command = CommandLineHelper::BuildSharedFDCommand(fds);
  std::string command_line =
      CommandLineHelper::BuildCommands(command, {shared_fd_command});
  NativeChildProcess_Args args =
      NativeChildProcessArgsWrapper::Build(command_line, fds);

  NativeChildProcess_Options options = {
      .isolationMode = NCP_ISOLATION_MODE_NORMAL,
  };

  std::vector<std::string> fd_strs;
  auto entry_point_str = entry_point.empty() ? kDefaultEntryPoint : entry_point;
  int32_t errCode =
      OH_Ability_StartNativeChildProcessFn(entry_point_str.c_str(),
                                           args,
                                           options,
                                           &pid);
  if (errCode != 0) {
    LOGE("OH_Ability_StartNativeChildProcess failed, errcode: %{public}d",
         errCode);
  }
  NativeChildProcessArgsWrapper::Free(args);
  return pid;
}
}  // namespace ohos::adapter::multiprocess
