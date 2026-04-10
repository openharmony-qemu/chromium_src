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

#ifndef CONTENT_BROWSER_DFX_COLLECT_MEMORY_UTILS_H_
#define CONTENT_BROWSER_DFX_COLLECT_MEMORY_UTILS_H_

#include <cstddef>
#include <cstdint>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/types.h>

#include "base/logging.h"

namespace content {

static constexpr size_t kMemoryBytesPerKb = 1024;

class CollectMemoryUtils {
public:
  CollectMemoryUtils();
  ~CollectMemoryUtils() = default;

  template<typename T>
  void ReadProcFile(const std::string& filePath, const std::string& token, T& value)
  {
    std::ifstream file(filePath);
    if (!file.is_open()) {
      LOG(ERROR) << "Failed to open file: " << filePath << ", error: " << std::strerror(errno);
      return;
    }

    std::string line;
    while (std::getline(file, line)) {
      if (!token.empty() && line.find(token) != 0) {
        continue;
      }

      size_t pos = token.empty() ? std::string::npos : line.find(":");
      if (token.empty() || pos != std::string::npos) {
        std::string valueStr = token.empty() ? line : line.substr(pos + 1);
        std::stringstream ss(valueStr);
        ss >> value;
        return;
      }
    }

    LOG(ERROR) << "Failed to find token: " << token << " in file: " << filePath;
  }

  void GetFdCount(const std::string& dirPath, uint32_t& value);
  void GetOomScoreAdj(const std::string& filePath, int32_t& value);
  void GetPartitionAllocatorMem(size_t& value);
  void GetProcessBasicMemoryInfo(pid_t& pid, size_t& rss, size_t& pss);
};

}  // namespace content

#endif  // CONTENT_BROWSER_DFX_COLLECT_MEMORY_UTILS_H_
