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

#include "arkweb/chromium_ext/content/browser/arkweb_child_process_launcher_helper_utils.h"

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
#include "content/public/common/content_switches.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"
#endif

namespace content {
namespace internal {
#if BUILDFLAG(IS_ARKWEB)
static bool save_browser_connect_{false};
#endif

ArkwebChildProcessLauncherHelperUtils::ArkwebChildProcessLauncherHelperUtils(
    ChildProcessLauncherHelper* child_process_launcher_helper)
    : child_process_launcher_helper_(child_process_launcher_helper) {}

ArkwebChildProcessLauncherHelperUtils::
    ~ArkwebChildProcessLauncherHelperUtils() = default;

void ArkwebChildProcessLauncherHelperUtils::LaunchChildProcess(
    base::CommandLine* command_line,
    const base::LaunchOptions* options,
    ChildProcessLauncherHelper::Process& process) {
  bool for_test =
      base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kForTest);
  if (for_test) {
    process.process = base::LaunchProcess(*command_line, *options);
  } else {
    const std::vector<std::string> argv_str = command_line->argv();
    std::stringstream argv_ss;
    const char separator = '#';
    for (auto& item : argv_str) {
      argv_ss << item << separator;
    }
    argv_ss << argv_str[argv_str.size() - 1];
    constexpr int SHARED_FD_INDEX = 0;
    constexpr int IPC_FD_INDEX = 1;
    constexpr int CRASH_SIGNAL_FD_INDEX = 2;
    int32_t shared_fd = options->fds_to_remap[SHARED_FD_INDEX].first;
    int32_t ipc_fd = options->fds_to_remap[IPC_FD_INDEX].first;
    int32_t crash_signal_fd =
        options->fds_to_remap[CRASH_SIGNAL_FD_INDEX].first;
    pid_t render_pid = 0;
    auto& adapterHelper = OHOS::NWeb::OhosAdapterHelper::GetInstance();
    if (child_process_launcher_helper_->app_mgr_client_adapter_ == nullptr) {
      child_process_launcher_helper_->app_mgr_client_adapter_ =
          adapterHelper.CreateAafwkAdapter();
    }
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
    if (!save_browser_connect_) {
      auto browser_host = std::make_shared<content::HostProxy>();
      child_process_launcher_helper_->app_mgr_client_adapter_
          ->SaveBrowserConnect(browser_host);
      save_browser_connect_ = true;
    }
#endif
    auto process_type = child_process_launcher_helper_->GetProcessType();
    LOG(INFO)
        << "Initiate a request to AMS to create a child process, child type: "
        << process_type;
    int ret =
        child_process_launcher_helper_->app_mgr_client_adapter_
            ->StartChildProcess(
                argv_ss.str(), ipc_fd, shared_fd, crash_signal_fd, render_pid,
                process_type);
    if (ret != 0) {
      LOG(ERROR) << "start render process error, ret=" << ret
                 << ", render pid=" << render_pid << ", process type=" << process_type;
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP) && !defined(COMPONENT_BUILD)
      if (process_type == switches::kRendererProcess) {
        ReportRenderProcessTerminate(false, -1, std::string("TERMINATION_STATUS_LAUNCH_FAILED"), ret);
      }
#endif
      process.process = base::Process();
    } else {
      process.process = base::Process(render_pid);
      OHOS::NWeb::ResSchedClientAdapter::ReportKeyThread(
          OHOS::NWeb::ResSchedStatusAdapter::THREAD_CREATED, render_pid,
          render_pid, OHOS::NWeb::ResSchedRoleAdapter::IMPORTANT_DISPLAY);
      LOG(DEBUG) << "report render process create event success, render pid: "
                 << render_pid << ", process type = "
                 << child_process_launcher_helper_->GetProcessType();
    }
  }
}

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
#define RENDER_NORMAL_KILLED 0x10000000
base::TerminationStatus
ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(
    int status,
    bool known_dead) {
  if (WIFSIGNALED(status)) {
    switch (WTERMSIG(status)) {
      case SIGABRT:
      case SIGBUS:
      case SIGFPE:
      case SIGILL:
      case SIGSEGV:
      case SIGTRAP:
      case SIGSYS:
        return base::TERMINATION_STATUS_PROCESS_CRASHED;
      case SIGKILL:
        if (!known_dead) {
          return base::TERMINATION_STATUS_NORMAL_TERMINATION;
        }
#if BUILDFLAG(IS_CHROMEOS_ASH) || BUILDFLAG(IS_CHROMEOS_LACROS)
        // On ChromeOS, only way a process gets kill by SIGKILL
        // is by oom-killer.
        return TERMINATION_STATUS_PROCESS_WAS_KILLED_BY_OOM;
#else
        [[fallthrough]];
#endif
      case SIGINT:
      case SIGTERM:
        return base::TERMINATION_STATUS_PROCESS_WAS_KILLED;
      default:
        break;
    }
  }

  if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
    return base::TERMINATION_STATUS_ABNORMAL_TERMINATION;
  }

  return base::TERMINATION_STATUS_NORMAL_TERMINATION;
}

void ArkwebChildProcessLauncherHelperUtils::RenderProcessExitedInfo(pid_t pid, int exitCode, bool known_dead) {
  base::TerminationStatus status = GetProcessStatusByExitCode(exitCode, known_dead);
  switch(status) {
    case base::TERMINATION_STATUS_NORMAL_TERMINATION:
      if (known_dead && WIFSIGNALED(exitCode)) {
        LOG(WARNING) << "RenderExited pid: " << pid << " known_dead: " << known_dead << " exitCode: " << exitCode
                     << " exitSig: " << WTERMSIG(exitCode) << " exitReason: process exit unknown";
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP) && !defined(COMPONENT_BUILD)
        ReportRenderProcessTerminate(false, pid, std::string("TERMINATION_STATUS_PROCESS_UNKNOWN"), exitCode);
#endif
        return;
      }
      LOG(INFO) << "RenderExited pid: " << pid << " known_dead: " << known_dead
                << " exitStatus: " << WEXITSTATUS(exitCode) << " exitReason: process normal termination";
      break;
    case base::TERMINATION_STATUS_ABNORMAL_TERMINATION:
      LOG(ERROR) << "RenderExited pid: " << pid << " known_dead: " << known_dead
                 << " exitStatus: " << WEXITSTATUS(exitCode) << " exitReason: process abnormal termination";
      break;
    case base::TERMINATION_STATUS_PROCESS_CRASHED:
      LOG(ERROR) << "RenderExited pid: " << pid << " known_dead: " << known_dead
                 << " exitSig: " << WTERMSIG(exitCode) << " exitReason: process crashed";
      break;
    case base::TERMINATION_STATUS_PROCESS_WAS_KILLED:
      LOG(ERROR) << "RenderExited pid: " << pid << " known_dead: "
                 << " exitSig: " << WTERMSIG(exitCode) << " exitReason: process was killed";
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP) && !defined(COMPONENT_BUILD)
      ReportRenderProcessTerminate(false, pid, std::string("TERMINATION_STATUS_PROCESS_WAS_KILLED"),
        WTERMSIG(exitCode));
#endif
      break;
    default:
      LOG(ERROR) << "RenderExited pid: " << pid << " known_dead: " << known_dead << " exitCode: " << exitCode
                 << " exitStatus: " << WEXITSTATUS(exitCode) << " exitSig: " << WTERMSIG(exitCode)
		         << " exitReason: process exit unknown";
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP) && !defined(COMPONENT_BUILD)
      ReportRenderProcessTerminate(false, pid, std::string("TERMINATION_STATUS_PROCESS_UNKNOWN"), exitCode);
#endif
      break;
  }
}

std::string
ArkwebChildProcessLauncherHelperUtils::GetExitReasonByTerminationStatus(
    base::TerminationStatus status) {
  std::string reason;
  switch (status) {
    case base::TERMINATION_STATUS_NORMAL_TERMINATION:
      reason = "process normal termination";
      break;
    case base::TERMINATION_STATUS_ABNORMAL_TERMINATION:
      reason = "process abnormal termination";
      break;
    case base::TERMINATION_STATUS_PROCESS_CRASHED:
      reason = "process crashed";
      break;
    case base::TERMINATION_STATUS_PROCESS_WAS_KILLED:
      reason = "process was killed";
      break;
#if BUILDFLAG(IS_CHROMEOS_ASH) || BUILDFLAG(IS_CHROMEOS_LACROS)
    case base::TERMINATION_STATUS_PROCESS_WAS_KILLED_BY_OOM:
      reason = "process out of memory";
      break;
#endif
    default:
      reason = "process exit unknown";
      break;
  }
  return reason;
}
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
bool ArkwebChildProcessLauncherHelperUtils::TerminateProcessByAppMgr(
    const base::Process& process) {
  auto app_mgr_client_adapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateAafwkAdapter();
  if (app_mgr_client_adapter == nullptr) {
    LOG(ERROR) << "GetTermination get app manager client adapter failed";
    return false;
  }

  int exitStatus;
  int ret = app_mgr_client_adapter->GetRenderProcessTerminationStatus(
      process.Handle(), exitStatus);
  if (ret != 0) {
    LOG(ERROR) << "GetTermination process termination status failed, pid "
               << process.Handle() << " error = " << ret;
    return false;
  }

  RenderProcessExitedInfo(process.Handle(), exitStatus, false);
  return true;
}
#endif

void ArkwebChildProcessLauncherHelperUtils::GetTerminationInfoArkweb(
    const base::Process& process,
    bool known_dead,
    ChildProcessTerminationInfo& info) {
  if (known_dead) {
    int exitStatus;
    int ret =
        child_process_launcher_helper_->app_mgr_client_adapter_
            ->GetRenderProcessTerminationStatus(process.Handle(), exitStatus);
    if (ret != 0) {
      LOG(ERROR) << "GetTermination get render process termination status "
                    "failed, errno = "
                 << ret;
    } else if (exitStatus < 0) {
      LOG(ERROR) << "GetTermination get render process termination status "
                    "success, invalid status = "
                 << exitStatus;
    } else {
      info.status =
          ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(
              exitStatus, known_dead);
      RenderProcessExitedInfo(process.Handle(), exitStatus, known_dead);
    }
  } else {
    info.status = base::TERMINATION_STATUS_STILL_RUNNING;
  }
}

}  // namespace internal
}  // namespace content
