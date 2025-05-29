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

#include "arkweb/chromium_ext/content/gpu/in_process_gpu_thread_utils.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include <fstream>
#include <dirent.h>
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
const int MAX_FILE_LENGTH = 32* 1024 * 1024;
static int retry_times = 0;
const int retry_delay_ms = 100;
const int retry_max_times = 4;
#endif

namespace content {
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void TryForReportThread() {
  using namespace OHOS::NWeb;
  auto tid = GetGpuThreadId(base::GetCurrentProcId());
  if (tid > 0) {
    ResSchedClientAdapter::ReportKeyThread(
      ResSchedStatusAdapter::THREAD_CREATED, base::GetCurrentProcId(),
      tid, ResSchedRoleAdapter::IMPORTANT_DISPLAY);
    return;
  }
  if (retry_times < retry_max_times) {
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
  DIR* dir = opendir(path_name.c_str());
  if (!dir) {
    LOG(ERROR) << "opendir " << path_name << " failed, errno: " << errno;
    return tid;
  }

  struct dirent* de = nullptr;
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

void ResetTryForReportThread() {
  retry_times = 0;
  TryForReportThread();
}

void InProcessGpuThreadDestroy() {
  using namespace OHOS::NWeb;
  auto tid = GetGpuThreadId(base::GetCurrentProcId());
  if (tid > 0) {
    ResSchedClientAdapter::ReportKeyThread(
      ResSchedStatusAdapter::THREAD_DESTROYED, base::GetCurrentProcId(),
      tid, ResSchedRoleAdapter::IMPORTANT_DISPLAY);
  }
}
#endif
}