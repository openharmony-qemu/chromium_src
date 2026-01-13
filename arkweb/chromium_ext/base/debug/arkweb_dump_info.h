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
#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_UNITTESTS)
#undef private
#endif
#include "base/containers/circular_deque.h"
#if BUILDFLAG(ARKWEB_UNITTESTS)
#define private public
#endif

namespace base::debug {

enum class DumpInfoType {
  DUMP_NWEB_INFO = 0,
  DUMP_OTHER_INFO = 1 << 31 // preserved
};

class ArkWebDumpInfo {
public:
  static ArkWebDumpInfo& GetInstance() {
    static base::NoDestructor<ArkWebDumpInfo> instance_;
    return *instance_;
  }

  bool IsDumpEnabled() const;
  size_t GetBufferSize();
  void ParseCmdParamAndDump(const std::string& param, std::string& result);
  void DumpArkWebAllInfo(std::string& result);
  void DumpArkWebNWebInfo(std::string& result);
  std::string GetCurrentTimeInfo() const;
  std::string GetProcessAndThreadIdInfo() const;
  void WriteArkWebDumpInfo(const std::string& info, DumpInfoType type);
  void FormatAndWriteNWebDumpInfo(const std::string& nwebInfo);

private:
  ArkWebDumpInfo();
  ~ArkWebDumpInfo() = default;

  ArkWebDumpInfo(const ArkWebDumpInfo&) = delete;
  ArkWebDumpInfo& operator=(const ArkWebDumpInfo&) = delete;

  static constexpr size_t max_capacity_ = 5000;
  friend class base::NoDestructor<ArkWebDumpInfo>;
  bool dump_enable_;
  std::shared_mutex dumpMutex_;
  base::circular_deque<std::pair<std::string, std::string>> buffer_;
};
} // namespace base::debug
#endif //ARKWEB_DUMP_INFO_H_