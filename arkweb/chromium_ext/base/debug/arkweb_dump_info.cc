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
  dump_enable_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                  .GetSystemPropertiesInstance()
                  .GetBoolParameter("web.debug.dump.on", true);
}

bool ArkWebDumpInfo::IsDumpEnabled() const {
  return dump_enable_;
}

void ArkWebDumpInfo::ParseCmdParamAndDump(const std::string& param, std::string& result) {
  if (!dump_enable_) {
    result.append("web.debug.dump.on = false");
    return;
  }

  if (param.empty()) {
    DumpArkWebAllInfo(result);
    return;
  }

  if (param == "--all") {
    DumpArkWebAllInfo(result);
  } else if (param == "--NWeb") {
    DumpArkWebNWebInfo(result);
  } else {
    result.append("Paramter not supported. More features are under development.");
  }
}

void ArkWebDumpInfo::DumpArkWebAllInfo(std::string& result) {
  std::shared_lock<std::shared_mutex> lockGuard(dumpMutex_);
  for (const auto& [key, value] : buffer_) {
    result.append(key).append(":").append(value);
  }
}

void ArkWebDumpInfo::DumpArkWebNWebInfo(std::string& result) {
  std::shared_lock<std::shared_mutex> lockGuard(dumpMutex_);
  for (const auto& [key, value] : buffer_) {
    if (key == "NWeb") {
      result.append(value);
    }
  }
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

void ArkWebDumpInfo::WriteArkWebDumpInfo(const std::string& info, DumpInfoType type) {
  std::shared_lock<std::shared_mutex> lockGuard(dumpMutex_);
  std::string key;
  switch(type) {
    case DUMP_NWEB_INFO:
      key = "key";
      break;
    default:
      return;
  }

  if (buffer_.size() >= max_capacity_) {
    buffer_.pop_front();
  }
  buffer_.push_back(std::make_pair(key, info));
}

void ArkWebDumpInfo::FormatAndWriteNWebDumpInfo(const std::string& nwebInfo) {
  if (!dump_enable_) {
    return;
  }

  std::string timeInfo = GetCurrentTimeInfo();
  std::string processThreadInfo = GetProcessAndThreadIdInfo();

  std::string formatStr = base::StringPrintf("[%s] %s %s\n",
                                            timeInfo.c_str(), processThreadInfo.c_str(), nwebInfo.c_str());
  WriteNWebDumpInfo(formatStr, DUMP_NWEB_INFO);
}
} //namespace base::debug