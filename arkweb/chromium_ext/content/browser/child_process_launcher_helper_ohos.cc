// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
#include "ohos/adapter/multiprocess/child_process_starter.h"

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
  return kSandboxIPCChannel + base::GlobalDescriptors::kBaseDescriptor;
}

bool ChildProcessLauncherHelper::BeforeLaunchOnLauncherThread(
    PosixFileDescriptorInfo& files_to_register,
    base::LaunchOptions* options) {
  if (options) {
    // Convert FD mapping to FileHandleMappingVector
    options->fds_to_remap = files_to_register.GetMappingWithIDAdjustment(
        base::GlobalDescriptors::kBaseDescriptor);

    if (GetProcessType() == switches::kRendererProcess) {
      const int sandbox_fd = SandboxHostLinux::GetInstance()->GetChildSocket();
      options->fds_to_remap.emplace_back(sandbox_fd, GetSandboxFD());
    }

    options->environment = delegate_->GetEnvironment();
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
  Process process;

  std::vector<std::string> argv_str = command_line()->argv();

  ohos::adapter::multiprocess::ChildProcessStarter child_process_starter =
      ohos::adapter::multiprocess::ChildProcessStarter::GetInstance();

  std::vector<std::pair<int, int>> fd_id_remap;
  size_t file_count = files_to_register->GetMappingSize();
  for (size_t i = 0; i < file_count; ++i) {
    int fd = files_to_register->GetFDAt(i);
    PCHECK(0 <= fd);
    int id = files_to_register->GetIDAt(i);
    fd_id_remap.push_back(std::make_pair(fd, id));
    //todo log
  }

  std::string process_type =
      command_line()->GetSwitchValueASCII(switches::kProcessType);

  int pid = -1;
  if (process_type == switches::kGpuProcess) {
    pid = child_process_starter.StartGpuProcess(argv_str, fd_id_remap);
    LOG(INFO) << "GPU process pid = " << pid;
  } else {
    pid = child_process_starter.StartIsolateChildProcess(argv_str, fd_id_remap);
    LOG(INFO) << "child process pid = " << pid;
  }
  LOG(INFO) << "start process pid = " << pid;

  if (pid == -1) {
    LOG(ERROR) << "start child process error";
    process.process = base::Process();
  } else {
    process.process = base::Process(pid);
  }
  *launch_result =
      process.process.IsValid() ? LAUNCH_RESULT_SUCCESS : LAUNCH_RESULT_FAILURE;

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
