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

#include "native_child_process_main.h"

#include <thread>

#include "native_child_process_args_wrapper.h"
#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/multiprocess/app_spawn_communication.h"
#include "ohos/adapter/multiprocess/command_line/command_line_helper.h"
#include "ohos/adapter/web_entry/web_entry.h"

using namespace std::chrono_literals;
using namespace ohos::adapter;
using namespace ohos::adapter::multiprocess;

void ChildMain(NativeChildProcess_Args args) {
  if (args.entryParams == nullptr) {
    return;
  }
  {
    TRACE_EVENT_0("ChildMain");
  }
  LOGD("[ChildProcess] ChildMain %{public}s", args.entryParams);
  std::vector<std::string> commandlines =
      CommandLineHelper::Split(args.entryParams);
  if (commandlines.size() == 0) {
    return;
  }
  for (const auto& commandline : commandlines) {
    LOGD("[ChildProcess] ChildMain %{public}s", commandline.c_str());
  }
  std::vector<std::string> commands;
  std::vector<std::pair<int32_t, int32_t>> fds;
  NativeChildProcessArgsWrapper::Parse(args, commands, fds);
  AppSpawnCommunication::SetFdIdsRemap(fds);
  int ret = web_entry::RunIsolateProcessType(ProcessType::kRenderProcess,
                                             commandlines);
  LOGI("[ChildProcess] ChildMain exited with return: %{public}d", ret);
}
