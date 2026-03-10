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

#include "ohos/adapter/web_entry/web_entry.h"
#include "ohos/adapter/web_entry/web_loader.h"

#include <atomic>
#include <chrono>
#include <thread>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/multiprocess/app_spawn_communication.h"
#include "ohos/adapter/multiprocess/command_line/command_line.h"

using namespace std::chrono_literals;
namespace ohos::adapter::web_entry {

const std::string kFdRemapCommand = "--fds_to_remap";
const std::string kCrashpadHandler = "CrashpadHandler";

static bool g_initialized = false;
static std::atomic<bool> g_destroyed(false);

void DestroyBrowser() {
  LOGE("The browser process has exited");
  auto jsfunc = ohos::adapter::GetJSFunction(
      "AppLifecycleAdapter.onWebDestroy");
  if (!jsfunc) {
      LOGE("AppLifecycleAdapter.onWebDestroy not defined");
      return;
  }
  jsfunc->Invoke<void>();
}

bool BrowserDestroyed() {
  return g_destroyed.load();
}

void RunBrowser(const std::vector<std::string>& args) {
  LOGI("RunBrowser in g_initialized: %{public}d", g_initialized);
  ohos::adapter::multiprocess::CommandLine::InitForCurrentProcess(args);

  if (!g_initialized) {
    std::thread chromiumMainthread([args]() {
      auto loader = BasicLoader::Create(USE_LOADER_TARGET);
      if (loader->StartMainProcess(args) != 0) {
        return;
      }
      DestroyBrowser();
      g_destroyed.store(true);
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
        continue;
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
      result = loader->StartChildProcess(process_type, cstrs.size(), cstrs.data());
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
        continue;
      }
    }
    if (process_type == ProcessType::kCrashpadHandler &&
        commands[i].starts_with(kFdRemapCommand)) {
      continue;
    }

    cstrs.push_back(commands[i].data());
  }
  auto result = loader->StartChildProcess(process_type, cstrs.size(), cstrs.data());
  LOGI("RunIsolateProcessType ret = %{public}d", result);
  return result;
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(BrowserDestroyed);
}

}  // namespace ohos::adapter::web_entry
