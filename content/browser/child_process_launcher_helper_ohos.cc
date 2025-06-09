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

#include <utility>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/path_service.h"
#include "base/posix/global_descriptors.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "content/browser/child_process_launcher.h"
#include "content/browser/child_process_launcher_helper.h"
#include "content/browser/child_process_launcher_helper_posix.h"
#include "content/browser/sandbox_host_linux.h"
#include "content/public/browser/child_process_launcher_utils.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_constants.h"
#include "content/public/common/content_descriptors.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/result_codes.h"
#include "content/public/common/sandboxed_process_launcher_delegate.h"

namespace content {
namespace internal {

std::optional<mojo::NamedPlatformChannel>
ChildProcessLauncherHelper::CreateNamedPlatformChannelOnLauncherThread() {
  DCHECK(CurrentlyOnProcessLauncherTaskRunner());
  return std::nullopt;
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
  return true;
}

int GetSandboxFD() {
  return kSandboxIPCChannel;
}

bool ChildProcessLauncherHelper::BeforeLaunchOnLauncherThread(
    PosixFileDescriptorInfo& files_to_register,
    base::LaunchOptions* options) {
  if (options) {
    // Convert FD mapping to FileHandleMappingVector
    options->fds_to_remap = files_to_register.GetMapping();

    if (GetProcessType() == switches::kRendererProcess) {
      const int sandbox_fd = SandboxHostLinux::GetInstance()->GetChildSocket();
      options->fds_to_remap.emplace_back(sandbox_fd, GetSandboxFD());
    }

    options->environment = delegate_->GetEnvironment();
    options->enable_native_spawn = true;
    if (GetProcessType() == switches::kGpuProcess) {
      options->is_gpu_process = true;
    }
  } else {
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
  TRACE_EVENT1("ui", "LaunchProcessOnLauncherThread", "args",
               command_line()->GetCommandLineString());
  *is_synchronous_launch = true;
  ChildProcessLauncherHelper::Process process;
  process.process = base::LaunchProcess(*command_line(), *options);
  *launch_result = process.process.IsValid() ? LAUNCH_RESULT_SUCCESS
                                             : LAUNCH_RESULT_FAILURE;

  return process;
}

void ChildProcessLauncherHelper::AfterLaunchOnLauncherThread(
    const ChildProcessLauncherHelper::Process& process,
    const base::LaunchOptions* options) {
  // Reset any FDs still held open.
  file_data_.reset();
}

ChildProcessTerminationInfo ChildProcessLauncherHelper::GetTerminationInfo(
    const ChildProcessLauncherHelper::Process& process,
    bool known_dead) {
  ChildProcessTerminationInfo info;
  if (known_dead) {
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
  return process.Terminate(exit_code, false);
}

// static
void ChildProcessLauncherHelper::ForceNormalProcessTerminationSync(
    ChildProcessLauncherHelper::Process process) {
  DCHECK(CurrentlyOnProcessLauncherTaskRunner());
  process.process.Terminate(RESULT_CODE_NORMAL_EXIT, false);
  base::EnsureProcessTerminated(std::move(process.process));
}

void ChildProcessLauncherHelper::SetProcessPriorityOnLauncherThread(
    base::Process process,
    base::Process::Priority priority) {
  DCHECK(CurrentlyOnProcessLauncherTaskRunner());
  if (process.CanSetPriority()) {
    process.SetPriority(priority);
  }
}

base::File OpenFileToShare(const base::FilePath& path,
                           base::MemoryMappedFile::Region* region) {
  // Not used in ohos yet
  return base::File();
}

}  // namespace internal
}  // namespace content
