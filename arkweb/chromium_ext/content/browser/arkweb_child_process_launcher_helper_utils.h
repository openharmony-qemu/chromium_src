/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARKWEB_CHILD_PROCESS_LAUNCHER_HELPER_UTILS_H_
#define ARKWEB_CHILD_PROCESS_LAUNCHER_HELPER_UTILS_H_

#include <execinfo.h>

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/process/launch.h"
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "content/public/common/content_descriptors.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
#include "arkweb/chromium_ext/content/renderer/host_proxy.h"
#endif
#endif

#include "content/browser/child_process_launcher_helper.h"

namespace content {
namespace internal {
class FuzzArkwebChildProcessLauncherHelperUtilsFriend;

class ArkwebChildProcessLauncherHelperUtils {
 public:
  friend class ChildProcessLauncherHelper;
  friend class FuzzArkwebChildProcessLauncherHelperUtilsFriend;
  explicit ArkwebChildProcessLauncherHelperUtils(
      ChildProcessLauncherHelper* child_process_launcher_helper);
  ~ArkwebChildProcessLauncherHelperUtils();

  ArkwebChildProcessLauncherHelperUtils(
      const ArkwebChildProcessLauncherHelperUtils&) = delete;
  ArkwebChildProcessLauncherHelperUtils& operator=(
      const ArkwebChildProcessLauncherHelperUtils&) = delete;

  void LaunchChildProcess(
      base::CommandLine* command_line,
      const base::LaunchOptions* options,
      ChildProcessLauncherHelper::Process& process);

  void GetTerminationInfoArkweb(
      const base::Process& process,
      bool known_dead,
      ChildProcessTerminationInfo& info);

 private:
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
  static base::TerminationStatus GetProcessStatusByExitCode(int status,
                                                            bool known_dead);
  static bool TerminateProcessByAppMgr(const base::Process& process);
  static std::string GetExitReasonByTerminationStatus(
      base::TerminationStatus status);
#endif
  const raw_ptr<ChildProcessLauncherHelper> child_process_launcher_helper_;
};

}  // namespace internal
}  // namespace content

#endif  // ARKWEB_CHILD_PROCESS_LAUNCHER_HELPER_UTILS_H_
