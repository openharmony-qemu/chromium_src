// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/path_service.h"
#include "base/posix/global_descriptors.h"
#include "build/build_config.h"
#include "content/browser/child_process_launcher.h"
#include "content/browser/child_process_launcher_helper.h"
#include "content/browser/child_process_launcher_helper_posix.h"
#include "content/browser/sandbox_host_linux.h"
#include "content/browser/zygote_host/zygote_host_impl_linux.h"
#include "content/common/zygote/zygote_communication_linux.h"
#include "content/public/browser/child_process_launcher_utils.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_constants.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/result_codes.h"
#include "content/public/common/sandboxed_process_launcher_delegate.h"
#include "content/public/common/zygote/sandbox_support_linux.h"
#include "content/public/common/zygote/zygote_handle.h"
#include "content/public/common/content_descriptors.h"
#include "sandbox/policy/linux/sandbox_linux.h"

#if BUILDFLAG(IS_OHOS)
#include "content/renderer/host_proxy.h"
#include "res_sched_client_adapter.h"
#endif

namespace content {
namespace internal {
#if BUILDFLAG(IS_OHOS)
static bool save_browser_connect_{false};
#endif

absl::optional<mojo::NamedPlatformChannel>
ChildProcessLauncherHelper::CreateNamedPlatformChannelOnLauncherThread() {
  DCHECK(CurrentlyOnProcessLauncherTaskRunner());
  return absl::nullopt;
}

void ChildProcessLauncherHelper::BeforeLaunchOnClientThread() {
  DCHECK(client_task_runner_->RunsTasksInCurrentSequence());
}

std::unique_ptr<FileMappedForLaunch>
ChildProcessLauncherHelper::GetFilesToMap() {
  DCHECK(CurrentlyOnProcessLauncherTaskRunner());
  return CreateDefaultPosixFilesToMap(
      child_process_id(), mojo_channel_->remote_endpoint(),
      file_data_->files_to_preload, GetProcessType(), command_line());
}

bool ChildProcessLauncherHelper::IsUsingLaunchOptions() {
  return !GetZygoteForLaunch();
}

bool ChildProcessLauncherHelper::BeforeLaunchOnLauncherThread(
    PosixFileDescriptorInfo& files_to_register,
    base::LaunchOptions* options) {
  if (options) {
    DCHECK(!GetZygoteForLaunch());
    // Convert FD mapping to FileHandleMappingVector
    options->fds_to_remap = files_to_register.GetMappingWithIDAdjustment(
        base::GlobalDescriptors::kBaseDescriptor);

    if (GetProcessType() == switches::kRendererProcess) {
      const int sandbox_fd = SandboxHostLinux::GetInstance()->GetChildSocket();
      options->fds_to_remap.emplace_back(sandbox_fd, GetSandboxFD());
    }

    options->environment = delegate_->GetEnvironment();
  } else {
    DCHECK(GetZygoteForLaunch());
    // Environment variables could be supported in the future, but are not
    // currently supported when launching with the zygote.
    DCHECK(delegate_->GetEnvironment().empty());
  }

  return true;
}

ChildProcessLauncherHelper::Process
ChildProcessLauncherHelper::LaunchProcessOnLauncherThread(
    const base::LaunchOptions* options,
    std::unique_ptr<FileMappedForLaunch> files_to_register,
    bool* is_synchronous_launch,
    int* launch_result) {
  *is_synchronous_launch = true;
  Process process;
  ZygoteCommunication* zygote_handle = GetZygoteForLaunch();
  if (zygote_handle) {
    // TODO(crbug.com/569191): If chrome supported multiple zygotes they could
    // be created lazily here, or in the delegate GetZygote() implementations.
    // Additionally, the delegate could provide a UseGenericZygote() method.
    base::ProcessHandle handle = zygote_handle->ForkRequest(
        command_line()->argv(), files_to_register->GetMapping(),
        GetProcessType());
    *launch_result = LAUNCH_RESULT_SUCCESS;

#if !BUILDFLAG(IS_OPENBSD)
    if (handle) {
      // It could be a renderer process or an utility process.
      int oom_score = content::kMiscOomScore;
      if (command_line()->GetSwitchValueASCII(switches::kProcessType) ==
          switches::kRendererProcess)
        oom_score = content::kLowestRendererOomScore;
      ZygoteHostImpl::GetInstance()->AdjustRendererOOMScore(handle, oom_score);
    }
#endif

    process.process = base::Process(handle);
    process.zygote = zygote_handle;
  } else {
#if BUILDFLAG(IS_OHOS)
    bool for_test =
        base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kForTest);
    if (for_test) {
      process.process = base::LaunchProcess(*command_line(), *options);
    } else {
      const std::vector<std::string> argv_str = command_line()->argv();
      std::stringstream argv_ss;
      const char separator = '#';
      for (auto& item : argv_str) {
        argv_ss << item << separator;
      }
      argv_ss << argv_str[argv_str.size() - 1];

      int32_t shared_fd = -1;
      int32_t ipc_fd = -1;
      int32_t crash_signal_fd = -1;
      int32_t sandbox_fd = -1;
      for (const auto& fd_pair : options->fds_to_remap) {
        switch (fd_pair.second - base::GlobalDescriptors::kBaseDescriptor) {
          case kMojoIPCChannel:
            ipc_fd = fd_pair.first;
            break;
          case kFieldTrialDescriptor:
            shared_fd = fd_pair.first;
            break;
          case kCrashDumpSignal:
            crash_signal_fd = fd_pair.first;
            break;
          case kSandboxIPCChannel:
            sandbox_fd = fd_pair.first;
            break;
          default:
            break;
        }
      }

      pid_t render_pid = 0;
      if (app_mgr_client_adapter_ == nullptr) {
        app_mgr_client_adapter_ =
            OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateAafwkAdapter();
      }
      if (!save_browser_connect_) {
        auto browser_host = std::make_shared<content::HostProxy>();
        app_mgr_client_adapter_->SaveBrowserConnect(browser_host);
        save_browser_connect_ = true;
      }

      LOG(INFO) << "Initiate a request to AMS to create a child process, child type: " << GetProcessType();
      int ret = app_mgr_client_adapter_->StartChildProcess(
          argv_ss.str(), ipc_fd, shared_fd, crash_signal_fd, render_pid, GetProcessType());
      if (ret != 0) {
        LOG(ERROR) << "start render process error, ret=" << ret
                   << ", render pid=" << render_pid << ", process type=" << GetProcessType();
        process.process = base::Process();
      } else {
        process.process = base::Process(render_pid);
        OHOS::NWeb::ResSchedClientAdapter::ReportKeyThread(OHOS::NWeb::ResSchedStatusAdapter::THREAD_CREATED,
          render_pid, render_pid, OHOS::NWeb::ResSchedRoleAdapter::IMPORTANT_DISPLAY);
        LOG(DEBUG) << "report render process create event success, render pid: " << render_pid
                   << ", process type = " << GetProcessType();
      }
    }
#else
    process.process = base::LaunchProcess(*command_line(), *options);
#endif
    *launch_result = process.process.IsValid() ? LAUNCH_RESULT_SUCCESS
                                               : LAUNCH_RESULT_FAILURE;
  }

#if BUILDFLAG(IS_CHROMEOS)
  if (GetProcessType() == switches::kRendererProcess) {
    process.process.InitializePriority();
  }
#endif

  return process;
}

void ChildProcessLauncherHelper::AfterLaunchOnLauncherThread(
    const ChildProcessLauncherHelper::Process& process,
    const base::LaunchOptions* options) {
  // Reset any FDs still held open.
  file_data_.reset();
}

#if BUILDFLAG(IS_OHOS)
base::TerminationStatus ChildProcessLauncherHelper::GetProcessStatusByExitCode(
    int status) {
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
#if BUILDFLAG(IS_CHROMEOS_ASH) || BUILDFLAG(IS_CHROMEOS_LACROS)
        // On ChromeOS, only way a process gets kill by SIGKILL
        // is by oom-killer.
        return TERMINATION_STATUS_PROCESS_WAS_KILLED_BY_OOM;
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
#endif

ChildProcessTerminationInfo ChildProcessLauncherHelper::GetTerminationInfo(
    const ChildProcessLauncherHelper::Process& process,
    bool known_dead) {
  ChildProcessTerminationInfo info;
  if (process.zygote) {
    info.status = process.zygote->GetTerminationStatus(
        process.process.Handle(), known_dead, &info.exit_code);
#if BUILDFLAG(IS_OHOS)
  } else if (app_mgr_client_adapter_) {
    int exitStatus;
    int ret = app_mgr_client_adapter_->GetRenderProcessTerminationStatus(
        process.process.Handle(), exitStatus);
    if (ret != 0) {
      LOG(ERROR) << "get render process termination status failed, ret = "
                 << ret;
    } else if (exitStatus < 0) {
      LOG(ERROR)
          << "get render process termination status success, invalid status = "
          << exitStatus;
    } else {
      info.status = GetProcessStatusByExitCode(exitStatus);
    }
#endif
  } else if (known_dead) {
    info.status = base::GetKnownDeadTerminationStatus(process.process.Handle(),
                                                      &info.exit_code);
  } else {
    info.status =
        base::GetTerminationStatus(process.process.Handle(), &info.exit_code);
  }
  return info;
}

// static
bool ChildProcessLauncherHelper::TerminateProcess(const base::Process& process,
                                                  int exit_code) {
  // TODO(https://crbug.com/818244): Determine whether we should also call
  // EnsureProcessTerminated() to make sure of process-exit, and reap it.
  return process.Terminate(exit_code, false);
}

// static
void ChildProcessLauncherHelper::ForceNormalProcessTerminationSync(
    ChildProcessLauncherHelper::Process process) {
  DCHECK(CurrentlyOnProcessLauncherTaskRunner());
  process.process.Terminate(RESULT_CODE_NORMAL_EXIT, false);
  // On POSIX, we must additionally reap the child.
  if (process.zygote) {
    // If the renderer was created via a zygote, we have to proxy the reaping
    // through the zygote process.
    process.zygote->EnsureProcessTerminated(process.process.Handle());
  } else {
    base::EnsureProcessTerminated(std::move(process.process));
  }
}

void ChildProcessLauncherHelper::SetProcessBackgroundedOnLauncherThread(
    base::Process process,
    bool is_background) {
  DCHECK(CurrentlyOnProcessLauncherTaskRunner());
  if (process.CanBackgroundProcesses())
    process.SetProcessBackgrounded(is_background);
}

ZygoteCommunication* ChildProcessLauncherHelper::GetZygoteForLaunch() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kNoZygote)
             ? nullptr
             : delegate_->GetZygote();
}

base::File OpenFileToShare(const base::FilePath& path,
                           base::MemoryMappedFile::Region* region) {
  base::FilePath exe_dir;
  bool result = base::PathService::Get(base::BasePathKey::DIR_ASSETS, &exe_dir);
  DCHECK(result);
  base::File file(exe_dir.Append(path),
                  base::File::FLAG_OPEN | base::File::FLAG_READ);
  *region = base::MemoryMappedFile::Region::kWholeFile;
  return file;
}

}  // namespace internal
}  // namespace content
