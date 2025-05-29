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

#ifndef CONTENT_GPU_IN_PROCESS_GPU_THREAD_UTILS_H_
#define CONTENT_GPU_IN_PROCESS_GPU_THREAD_UTILS_H_

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "base/threading/thread.h"
#include "content/common/content_export.h"
#include "content/common/in_process_child_thread_params.h"
#include "gpu/config/gpu_preferences.h"

#include "base/feature_list.h"
#include "base/time/time.h"

namespace content {

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  void TryForReportThread();
  int32_t GetGpuThreadId(int32_t pid);
  int32_t GetTidListByName(int32_t pid, const std::string& thread_name);
  bool LoadStringFromFile(const std::string& file_path, std::string& content);
  void ResetTryForReportThread();
  void InProcessGpuThreadDestroy();
#endif

}
#endif
