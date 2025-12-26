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

#include "arkweb_dump_info.h"
#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/time/time.h"
#include "base/threading/platform_thread.h"
#include "base/process/process_handle.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace base::debug {
ArkWebDumpInfo::ArkWebDumpInfo() {
  totalSize_ = 0;
  dump_enable_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                  .GetSystemPropertiesInstance()
                  .GetBoolParameter("web.debug.dump.on", true);
  InitializeDumpMap();
}

void ArkWebDumpInfo::InitializeDumpMap() {
  dump_map_["NWeb"] = "";  // Initialize "NWeb" key
}

bool ArkWebDumpInfo::IsDumpEnabled() const {
  return dump_enable_;
}

void ArkWebDumpInfo::WriteNWebDumpInfo(std::string& info) {
  std::shared_lock<std::shared_mutex> lockGuard(dumpMutex_);
  std::string& nwebValue = dump_map_["NWeb"];
  uint64_t infoSize = 0;
  if (nwebValue.empty()) {
    nwebValue += "NWEB:\n";
    infoSize += nwebValue.size();
  }

  infoSize += info.size();
  nwebValue += info;
  totalSize_ += infoSize;
}

std::string ArkWebDumpInfo::GetCurrentTimeInfo() const {
  // ICU is not initialized, so a time zone offset of 8 hours needs to be manually added.
  base::Time now = base::Time::Now() + base::Hours(8);
  base::Time::Exploded exploded;
  now.LocalExplode(&exploded);
  return base::StringPrintf(
    "%04d-%02d-%02d %02d:%02d:%02d",
    exploded.year, exploded.month, exploded.day_of_month,
    exploded.hour, exploded.minute, exploded.second);
}

std::string ArkWebDumpInfo::GetProcessAndThreadIdInfo() const {
  base::ProcessId pid = base::GetCurrentProcId();
  int tid = base::PlatformThread::CurrentId();
  return base::StringPrintf("[P%d-T%d]", pid, tid);
}

void ArkWebDumpInfo::FormatAndWriteNWebDumpInfo(std::string& nwebInfo) {
  if (!dump_enable_) {
    return;
  }

  std::string timeInfo = GetCurrentTimeInfo();
  std::string processThreadInfo = GetProcessAndThreadIdInfo();

  std::string formatStr = base::StringPrintf("[%s] %s %s\n",
                                            timeInfo.c_str(), processThreadInfo.c_str(), nwebInfo.c_str());
  WriteNWebDumpInfo(formatStr);
}

void ArkWebDumpInfo::DumpArkWebInfo(std::string& result) {
  std::shared_lock<std::shared_mutex> lockGuard(dumpMutex_);
  for (auto it : dump_map_) {
    result += it.second;
  }

  result += "get chromium result success!\n";
  return;
}

uint64_t ArkWebDumpInfo::GetDumpInfoSize() {
  std::shared_lock<std::shared_mutex> lockGuard(dumpMutex_);
  return totalSize_;
}
} //namespace base::debug