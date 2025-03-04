// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/web_entry/web_entry.h"

#include <chrono>
#include <thread>

#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/life_cycle/lifecycle_init.h"
#include "ohos/adapter/multiprocess/app_spawn_communication.h"
#include "ohos/adapter/multiprocess/command_line/command_line.h"
#include "ohos/adapter/web_entry/web_loader.h"

using namespace std::chrono_literals;
namespace ohos::adapter::web_entry {

const std::string kFdRemapCommand = "--fds_to_remap=";
const std::string kCrashpadHandler = "crashpad-handler";

static bool g_initialized = false;
void RunBrowser(const std::vector<std::string>& args) {
  LOGI("RunBrowser in g_initialized: %{public}d", g_initialized);
  ohos::adapter::multiprocess::CommandLine::InitForCurrentProcess(args);

  if (!g_initialized) {
    std::thread chromiumMainthread([args]() {
      auto loader = BasicLoader::Create(USE_LOADER_TARGET);
      if (loader->StartMainProcess(args) != 0) {
        ohos::adapter::life_cycle::Launch(false);
        return;
      }
      ohos::adapter::life_cycle::Destroy();
    });
    chromiumMainthread.detach();
    g_initialized = true;
  }

  LOGI("RunBrowser out g_initialized: %{public}d", g_initialized);
}

int RunOtherProcessType(int process_type) {
  std::vector<std::string> commands;
  multiprocess::AppSpawnCommunication::RequestCommandline(commands);
  std::vector<const char*> cstrs;
  auto loader = BasicLoader::Create(USE_LOADER_TARGET);
  for (size_t i = 0; i < commands.size(); ++i) {
    if (commands[i].find("--type") != std::string::npos) {
      auto pos = commands[i].find_last_of("=");
      auto proc_type = commands[i].substr(pos + 1);
      if (proc_type == kCrashpadHandler) {
        process_type = ProcessType::kCrashpadHandler;
      }
    }
    if (process_type == ProcessType::kCrashpadHandler &&
        commands[i].starts_with(kFdRemapCommand)) {
      continue;
    }

    cstrs.push_back(commands[i].data());
  }
  ohos::adapter::multiprocess::CommandLine::InitForCurrentProcess(commands);

  int result = -1;
  switch (process_type) {
    case ProcessType::kRenderProcess:
    case ProcessType::kGpuProcess:
    case ProcessType::kCrashpadHandler:
      result =
          loader->StartChildProcess(process_type, cstrs.size(), cstrs.data());
      break;
    default:
      LOGI("RunOtherProcessType nothing to do");
      break;
  }
  return result;
}

int RunIsolateProcessType(int process_type,
                          const std::vector<std::string>& commands) {
  LOGI("RunIsolateProcessType begin process_type: %{public}d", process_type);
  auto loader = BasicLoader::Create(USE_LOADER_TARGET);
  std::vector<const char*> cstrs;
  for (size_t i = 0; i < commands.size(); ++i) {
    if (commands[i].find("--type") != std::string::npos) {
      auto pos = commands[i].find_last_of("=");
      auto proc_type = commands[i].substr(pos + 1);
      if (proc_type == kCrashpadHandler) {
        process_type = ProcessType::kCrashpadHandler;
      }
    }
    if (process_type == ProcessType::kCrashpadHandler &&
        commands[i].starts_with(kFdRemapCommand)) {
      continue;
    }

    cstrs.push_back(commands[i].data());
  }
  auto result =
      loader->StartChildProcess(process_type, cstrs.size(), cstrs.data());
  LOGI("RunIsolateProcessType ret = %{public}d", result);
  return result;
}

}  // namespace ohos::adapter::web_entry
