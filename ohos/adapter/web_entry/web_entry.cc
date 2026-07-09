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

#include "web_entry.h"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/multiprocess/app_spawn_communication.h"
#include "ohos/adapter/multiprocess/command_line/command_line.h"
#include "ohos/adapter/multiprocess/fd_remap_types.h"
#include "ohos/adapter/multiprocess/global_file_descriptor_mappings.h"
#include "ohos/adapter/web_entry/web_loader.h"

using namespace std::chrono_literals;
namespace ohos::adapter::web_entry {
using ohos::adapter::multiprocess::AppSpawnCommunication;
using ohos::adapter::multiprocess::GlobalFileDescriptorMappings;
using ohos::adapter::multiprocess::CommandLine;

namespace {

bool g_initialized = false;
std::atomic<bool> g_destroyed(false);

static int GetEntryType() {
  int entry_type = ProcessType::kChromeMain;
  auto* cmd = CommandLine::ForCurrentProcess();
  std::string type = cmd ? cmd->GetSwitchValue("type") : "";
  if (type == kCrashPadHandlerType) {
    entry_type = ProcessType::kCrashPadHandler;
  }
  return entry_type;
}
} // namespace

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

void RunBrowser(std::vector<std::string>& args) {
  LOGI("RunBrowser in g_initialized: %{public}d", g_initialized);
  CommandLine::InitForCurrentProcess(args);

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

int RunChildProcess() {
  std::vector<std::string> commands;
  FdRemapVector fd_mappings;
  AppSpawnCommunication::RequestCommandline(commands, fd_mappings);
  GlobalFileDescriptorMappings::SetFdIdsRemap(fd_mappings);
  CommandLine::FixCrashpadClientFd(commands, fd_mappings);
  CommandLine::InitForCurrentProcess(commands);

  auto loader = BasicLoader::Create(USE_LOADER_TARGET);
  int entry_type = GetEntryType();
  auto argc_argv = CommandLine::BuildArgcArgv(commands);

  int result = loader->StartChildProcess(entry_type, argc_argv.argc,
                                         argc_argv.argv.data());
  LOGI("RunChildProcess ret = %{public}d", result);
  return result;
}

int RunNativeProcess(std::vector<std::string>& commands,
                     FdRemapVector fd_mappings) {
  GlobalFileDescriptorMappings::SetFdIdsRemap(fd_mappings);
  CommandLine::FixCrashpadClientFd(commands, fd_mappings);
  CommandLine::InitForCurrentProcess(commands);

  auto loader = BasicLoader::Create(USE_LOADER_TARGET);
  int entry_type = GetEntryType();
  auto argc_argv = CommandLine::BuildArgcArgv(commands);

  auto result = loader->StartChildProcess(entry_type, argc_argv.argc,
                                          argc_argv.argv.data());
  LOGI("RunNativeProcess ret = %{public}d", result);
  return result;
}

int RunArkProcess(const char* entry_params,
                  size_t entry_params_len,
                  const std::vector<int32_t>& fds_id,
                  const std::vector<int32_t>& fds) {
  std::string entry_params_str(entry_params, entry_params_len);
  std::vector<std::string> commands = CommandLine::Split(
      entry_params_str, CommandLine::kArgSeparator);

  FdRemapVector fd_mappings;
  for (size_t i = 0; i < fds_id.size() && i < fds.size(); ++i) {
    fd_mappings.push_back(std::make_pair(fds[i], fds_id[i]));
  }
  GlobalFileDescriptorMappings::SetFdIdsRemap(fd_mappings);
  CommandLine::FixCrashpadClientFd(commands, fd_mappings);
  CommandLine::InitForCurrentProcess(commands);

  auto loader = BasicLoader::Create(USE_LOADER_TARGET);
  int entry_type = GetEntryType();
  auto argc_argv = CommandLine::BuildArgcArgv(commands);
  auto result = loader->StartChildProcess(entry_type, argc_argv.argc,
                                          argc_argv.argv.data());
  LOGI("RunArkProcess ret = %{public}d", result);
  return result;
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(BrowserDestroyed);
}

}  // namespace ohos::adapter::web_entry
