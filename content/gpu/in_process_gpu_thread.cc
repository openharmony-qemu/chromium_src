// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/gpu/in_process_gpu_thread.h"

#include "base/command_line.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "content/child/child_process.h"
#include "content/gpu/gpu_child_thread.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_switches.h"
#include "content/public/gpu/content_gpu_client.h"
#include "gpu/config/gpu_preferences.h"
#include "gpu/ipc/service/gpu_init.h"
#include "media/gpu/buildflags.h"

#if BUILDFLAG(IS_OHOS)
#include <fstream>
#include <dirent.h>
#include "res_sched_client_adapter.h"
#endif

#if BUILDFLAG(USE_VAAPI)
#include "media/gpu/vaapi/vaapi_wrapper.h"
#endif

#if BUILDFLAG(IS_ANDROID)
#include "base/android/jni_android.h"
#endif

#if BUILDFLAG(IS_OHOS)
const int MAX_FILE_LENGTH = 32* 1024 * 1024;
static int retry_times = 0;
const int retry_delay_ms = 100;
#endif

namespace content {

InProcessGpuThread::InProcessGpuThread(
    const InProcessChildThreadParams& params,
    const gpu::GpuPreferences& gpu_preferences)
    : base::Thread("Chrome_InProcGpuThread"),
      params_(params),
      gpu_process_(nullptr),
      gpu_preferences_(gpu_preferences) {}

InProcessGpuThread::~InProcessGpuThread() {
  Stop();
#if BUILDFLAG(IS_OHOS)
  using namespace OHOS::NWeb;

  auto tid = GetGpuThreadId(base::GetCurrentProcId());
  if (tid > 0) {
    ResSchedClientAdapter::ReportKeyThread(
      ResSchedStatusAdapter::THREAD_DESTROYED, base::GetCurrentProcId(),
      tid, ResSchedRoleAdapter::IMPORTANT_DISPLAY);
  }
#endif
}

void InProcessGpuThread::Init() {
  base::ThreadType io_thread_type = base::ThreadType::kDefault;

  // In single-process mode, we never enter the sandbox, so run the post-sandbox
  // code now.
  content::ContentGpuClient* client = GetContentClient()->gpu();
  if (client) {
    client->PostSandboxInitialized();
  }
#if BUILDFLAG(IS_ANDROID)
  // Call AttachCurrentThreadWithName, before any other AttachCurrentThread()
  // calls. The latter causes Java VM to assign Thread-??? to the thread name.
  // Please note calls to AttachCurrentThreadWithName after AttachCurrentThread
  // will not change the thread name kept in Java VM.
  base::android::AttachCurrentThreadWithName(thread_name());
  // Up the priority of the |io_thread_| on Android.
  io_thread_type = base::ThreadType::kDisplayCritical;
#endif

  gpu_process_ = std::make_unique<ChildProcess>(io_thread_type);

  auto gpu_init = std::make_unique<gpu::GpuInit>();
  gpu_init->InitializeInProcess(base::CommandLine::ForCurrentProcess(),
                                gpu_preferences_);

#if BUILDFLAG(USE_VAAPI)
  media::VaapiWrapper::PreSandboxInitialization();
#endif

  GetContentClient()->SetGpuInfo(gpu_init->gpu_info());

  // The process object takes ownership of the thread object, so do not
  // save and delete the pointer.
  GpuChildThread* child_thread =
      new GpuChildThread(params_, std::move(gpu_init));

  // Since we are in the browser process, use the thread start time as the
  // process start time.
  child_thread->Init(base::TimeTicks::Now());

  gpu_process_->set_main_thread(child_thread);
#if BUILDFLAG(IS_OHOS)
  retry_times = 0;
  TryForReportThread();
#endif
}

void InProcessGpuThread::CleanUp() {
  SetThreadWasQuitProperly(true);
  gpu_process_.reset();
}

base::Thread* CreateInProcessGpuThread(
    const InProcessChildThreadParams& params,
    const gpu::GpuPreferences& gpu_preferences) {
  return new InProcessGpuThread(params, gpu_preferences);
}

#if BUILDFLAG(IS_OHOS)
void TryForReportThread() {
  using namespace OHOS::NWeb;
  auto tid = GetGpuThreadId(base::GetCurrentProcId());
  if (tid > 0) {
    ResSchedClientAdapter::ReportKeyThread(
      ResSchedStatusAdapter::THREAD_CREATED, base::GetCurrentProcId(),
      tid, ResSchedRoleAdapter::IMPORTANT_DISPLAY);
    return;
  }
  if (retry_times < 4) {
    retry_times = retry_times + 1;
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(FROM_HERE, base::BindOnce(&TryForReportThread),
      base::Milliseconds(retry_delay_ms));
  }
}

int32_t GetGpuThreadId(int32_t pid)
{
  int32_t tid = GetTidListByName(pid, "gpu-work-server");
  if (tid < 0) {
    tid = GetTidListByName(pid, "mali-cmar-backe");
  }
  return tid;
}

int32_t GetTidListByName(int32_t pid, const std::string& thread_name)
{
  int32_t tid = -1;
  if (pid <= 0) {
    return tid;
  }

  std::string path_name = std::string("/proc/").append(std::to_string(pid)).append("/task");
  DIR *dir = opendir(path_name.c_str());
  if (!dir) {
    LOG(ERROR) << "opendir " << path_name <<" failed, errno: " << errno;
    return tid;
  }

  struct dirent *de = nullptr;
  while ((de = readdir(dir))) {
    if (!(de->d_type & DT_DIR) || !isdigit(de->d_name[0])) {
        continue;
    }
    std::string comm_path = path_name + std::string("/").append(de->d_name).append("/comm");
    std::string comm;
    if (!LoadStringFromFile(comm_path, comm)) {
        continue;
    }
    if (tid < 0 && comm.find(thread_name) != std::string::npos) {
        tid = atoi(de->d_name);
        if (tid >= 0) {
            break;
        }
    }
  }
  closedir(dir);
  return tid;
}

bool LoadStringFromFile(const std::string& file_path, std::string& content)
{
  std::ifstream file(file_path.c_str());
  if (!file.is_open()) {
    LOG(ERROR) << "open file failed! file path: " << file_path;
    return false;
  }

  file.seekg(0, std::ios::end);
  int file_length = file.tellg();
  if (file_length > MAX_FILE_LENGTH) {
    LOG(ERROR) << "invalid file length: " << file_length;
    return false;
  }
  content.clear();
  file.seekg(0, std::ios::beg);
  std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(content));
  return true;
}
#endif
}  // namespace content
