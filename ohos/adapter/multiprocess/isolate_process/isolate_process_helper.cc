// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "isolate_process_helper.h"

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/multiprocess/command_line/command_line_helper.h"
#include "ohos/adapter/multiprocess/isolate_process/native_childprocess_args_wrapper.h"
#include "ohos/adapter/multiprocess/isolate_process/native_childprocess_interface.h"

namespace ohos::adapter::multiprocess {

int32_t IsolateProcessHelper::StartChildProcess(
    const std::vector<std::string>& command,
    const std::vector<std::pair<int, int>>& fds) {
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

  int32_t errCode = OH_Ability_StartNativeChildProcessFn(
      "libadapter.so:IsolateMain", args, options, &pid);
  if (errCode != 0) {
    LOGE("OH_Ability_StartNativeChildProcess failed, errcode%{public}d",
         errCode);
  }
  NativeChildProcessArgsWrapper::Free(args);
  return pid;
}
}  // namespace ohos::adapter::multiprocess
