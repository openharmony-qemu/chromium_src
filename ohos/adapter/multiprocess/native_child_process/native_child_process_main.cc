// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

namespace {
void FixInitClientFdForCrashDumpHandler(
    std::vector<std::string>& cmds,
    const AppSpawnCommunication::FdRemapVector& id_fds) {
  std::string init_fd;
  for (auto& [id, fd] : id_fds) {
    // refer to content/public/common/content_descriptors.h
    // id 0, is for crashdump
    if (id == 0) {
      init_fd = std::to_string((int)fd);
      break;
    }
  }
  for (int i = cmds.size() - 1; i >= 0; i--) {
    auto cmd = cmds[i];
    if (cmd.npos != cmd.find("--initial-client-fd=")) {
      std::string rep("--initial-client-fd=");
      rep.append(init_fd);
      cmds[i] = rep;
      return;
    }
  }
}
}

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
  FixInitClientFdForCrashDumpHandler(commandlines, fds);
  int ret = web_entry::RunIsolateProcessType(ProcessType::kRenderProcess,
                                             commandlines);
  LOGI("[ChildProcess] ChildMain exited with return: %{public}d", ret);
}
