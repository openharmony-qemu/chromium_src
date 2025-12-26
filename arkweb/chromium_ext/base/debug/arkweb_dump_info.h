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

#ifndef ARKWEB_DUMP_INFO_H_
#define ARKWEB_DUMP_INFO_H_

#include <string>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include "base/no_destructor.h"

namespace base::debug {
class ArkWebDumpInfo {
public:
  static ArkWebDumpInfo& GetInstance() {
    static base::NoDestructor<ArkWebDumpInfo> instance_;
    return *instance_;
  }

  void InitializeDumpMap();
  bool IsDumpEnabled() const;
  void DumpArkWebInfo(std::string& result);
  std::string GetCurrentTimeInfo() const;
  std::string GetProcessAndThreadIdInfo() const;
  void WriteNWebDumpInfo(std::string& info);
  void FormatAndWriteNWebDumpInfo(std::string& nwebInfo);
  uint64_t GetDumpInfoSize();

private:
  ArkWebDumpInfo();
  ~ArkWebDumpInfo() = default;

  ArkWebDumpInfo(const ArkWebDumpInfo&) = delete;
  ArkWebDumpInfo& operator=(const ArkWebDumpInfo&) = delete;

  friend class base::NoDestructor<ArkWebDumpInfo>;
  bool dump_enable_;
  uint64_t totalSize_;
  std::shared_mutex dumpMutex_;
  std::unordered_map<std::string, std::string> dump_map_;
};
}
#endif //ARKWEB_DUMP_INFO_H_