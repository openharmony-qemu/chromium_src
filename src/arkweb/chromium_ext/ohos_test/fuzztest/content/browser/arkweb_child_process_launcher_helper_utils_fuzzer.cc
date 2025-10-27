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

#include <fuzzer/FuzzedDataProvider.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "arkweb/chromium_ext/content/browser/arkweb_child_process_launcher_helper_utils.h"
#include "base/command_line.h"
#include "base/process/process.h"
#include "content/public/common/content_switches.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/aafwk_app_mgr_client_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/aafwk_browser_host_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"

namespace content {
namespace internal {

class FuzzArkwebChildProcessLauncherHelperUtilsFriend {
 public:
  static base::TerminationStatus GetProcessStatusByExitCode(int status,
                                                            bool known_dead) {
    return content::internal::ArkwebChildProcessLauncherHelperUtils::
        GetProcessStatusByExitCode(status, known_dead);
  }

  static std::string GetExitReasonByTerminationStatus(
      base::TerminationStatus status) {
    return content::internal::ArkwebChildProcessLauncherHelperUtils::
        GetExitReasonByTerminationStatus(status);
  }
};
}  // namespace internal
}  // namespace content

void FuzzArkwebChildProcessLauncherHelperUtils(FuzzedDataProvider& provider) {
  if (provider.remaining_bytes() >= sizeof(int) + sizeof(bool)) {
    int exit_status = provider.ConsumeIntegral<int>();
    bool known_dead = provider.ConsumeBool();

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
    auto status =
        content::internal::FuzzArkwebChildProcessLauncherHelperUtilsFriend::
            GetProcessStatusByExitCode(exit_status, known_dead);
    (void)status;
#endif
  }

  if (provider.remaining_bytes() > 0) {
    auto status_value = provider.ConsumeIntegralInRange<int>(0, 7);
    base::TerminationStatus termination_status =
        static_cast<base::TerminationStatus>(status_value);

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
    std::string reason =
        content::internal::FuzzArkwebChildProcessLauncherHelperUtilsFriend::
            GetExitReasonByTerminationStatus(termination_status);
    (void)reason;
#endif
  }
}

void BrutalCoverageTest() {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
  const std::vector<std::tuple<int, bool>> signal_cases = {
      {SIGABRT, false}, {SIGBUS, false},  {SIGFPE, false}, {SIGILL, false},
      {SIGSEGV, false}, {SIGTRAP, false}, {SIGSYS, false}, {SIGKILL, true},
      {SIGKILL, false}, {SIGINT, false},  {SIGTERM, false}};

  for (const auto& [sig, known_dead] : signal_cases) {
    int status = 0x80 | sig;
    auto result =
        content::internal::FuzzArkwebChildProcessLauncherHelperUtilsFriend::
            GetProcessStatusByExitCode(status, known_dead);
    (void)result;
  }

  const std::vector<int> exit_cases = {0, 1, 255};
  for (int exit_code : exit_cases) {
    int status = (exit_code << 8) | 0x7F;
    auto result =
        content::internal::FuzzArkwebChildProcessLauncherHelperUtilsFriend::
            GetProcessStatusByExitCode(status, false);
    (void)result;
  }

  const std::vector<base::TerminationStatus> termination_cases = {
    base::TERMINATION_STATUS_NORMAL_TERMINATION,
    base::TERMINATION_STATUS_ABNORMAL_TERMINATION,
    base::TERMINATION_STATUS_PROCESS_CRASHED,
    base::TERMINATION_STATUS_PROCESS_WAS_KILLED,
#if BUILDFLAG(IS_CHROMEOS_ASH) || BUILDFLAG(IS_CHROMEOS_LACROS)
    base::TERMINATION_STATUS_PROCESS_WAS_KILLED_BY_OOM,
#endif
    static_cast<base::TerminationStatus>(8),
    static_cast<base::TerminationStatus>(-1)
  };

  for (auto status : termination_cases) {
    auto reason =
        content::internal::FuzzArkwebChildProcessLauncherHelperUtilsFriend::
            GetExitReasonByTerminationStatus(status);
    (void)reason;
  }
#endif
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  FuzzedDataProvider provider(data, size);
  FuzzArkwebChildProcessLauncherHelperUtils(provider);
  BrutalCoverageTest();
  return 0;
}
