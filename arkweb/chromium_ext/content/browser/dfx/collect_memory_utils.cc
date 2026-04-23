/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "collect_memory_utils.h"

#include <dirent.h>
#include <memory>
#include <unistd.h>

#include "base/allocator/partition_allocator/src/partition_alloc/partition_root.h"
#include "base/allocator/partition_allocator/src/partition_alloc/shim/allocator_shim_default_dispatch_to_partition_alloc.h"

namespace content {

CollectMemoryUtils::CollectMemoryUtils() = default;

void CollectMemoryUtils::GetFdCount(const std::string& dirPath, uint32_t& value)
{
  value = 0;
  std::unique_ptr<DIR, decltype(&closedir)> dir(opendir(dirPath.c_str()), closedir);
  if (!dir) {
    LOG(ERROR) << "Failed to open dir: " << dirPath;
    return;
  }

  struct dirent* entry = nullptr;
  while ((entry = readdir(dir.get()))) {
    if (entry->d_name[0] != '.') {
      value++;
    }
  }

  if (value > 0) {
    value--;
  }
}

void CollectMemoryUtils::GetOomScoreAdj(const std::string& filePath, int32_t& value)
{
  value = 0;
  std::ifstream file(filePath);
  if (!file.is_open()) {
    LOG(ERROR) << "Failed to open file: " << filePath;
    return;
  }

  file >> value;
  file.close();
}

void CollectMemoryUtils::GetPartitionAllocatorMem(size_t& value)
{
  value = 0;
  partition_alloc::SimplePartitionStatsDumper dumper;
  auto* root = allocator_shim::internal::PartitionAllocMalloc::Allocator();
  if (!root) {
    LOG(ERROR) << "Failed to get Partition allocator root";
    return;
  }

  root->DumpStats("arkweb", true, &dumper);
  const auto& stats = dumper.stats();
  value = stats.total_allocated_bytes / kMemoryBytesPerKb;
}

void CollectMemoryUtils::GetProcessBasicMemoryInfo(pid_t& pid, size_t& rss, size_t& pss)
{
  pid = 0;
  ReadProcFile("/proc/self/status", "NSpid:", pid);
  rss = 0;
  ReadProcFile("/proc/self/smaps_rollup", "Rss:", rss);
  pss = 0;
  ReadProcFile("/proc/self/smaps_rollup", "Pss:", pss);
}

}  // namespace content
