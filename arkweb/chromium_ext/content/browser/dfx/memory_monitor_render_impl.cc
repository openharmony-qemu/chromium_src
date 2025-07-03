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

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <deviceinfo.h>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <sched.h>
#include <sstream>
#include <string>

#include "base/check.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/process/process_metrics.h"
#include "base/values.h"
#include "base/json/json_writer.h"
#include "base/strings/string_split.h"
#include "memory_monitor_render_impl.h"
#include "v8/include/v8-isolate.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

#define MEM_CONVERT 1024
#define WARNING_MEMORY_LEAK_THRESHOLD 800
#define ERROR_MEMORY_LEAK_THRESHOLD 1500
#define COUNTER_INITIAL 0
#define COUNTER_THRESHOLD 60
#define INTERVAL 10000

namespace content {

using DfxMemInfo = MemoryMonitorImpl::DfxMemInfo;
using DfxMemStatus = MemoryMonitorImpl::DfxMemStatus;

template<typename T>
void ReadProcFile(const std::string& filePath, const std::string& token, T& value)
{
  std::ifstream file(filePath);
  if (!file.is_open()) {
    LOG(ERROR) << "Failed to open file: " << filePath;
    return;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (!token.empty() && line.find(token) != 0) {
      continue;
    }

    size_t pos = token.empty() ? 0 : line.find(":");
    if (pos != std::string::npos) {
      std::string valueStr = line.substr(pos + 1);
      std::stringstream ss(valueStr);
      ss >> value;
      return;
    }
  }
  LOG(ERROR) << "Failed to find token: " << token << " in file: " << filePath;
}

static void UpdateProcessBasicMemoryInfo(DfxMemInfo &mem_info)
{
  if (mem_info.pid == 0) {
    ReadProcFile("/proc/self/status", "NSpid:", mem_info.pid);
  }

  mem_info.rss = 0;
  ReadProcFile("/proc/self/smaps_rollup", "Rss:", mem_info.rss);

  mem_info.pss = 0;
  ReadProcFile("/proc/self/smaps_rollup", "Pss:", mem_info.pss);
}

static void UpdateProcessMemoryInfo(DfxMemInfo &mem_info)
{
  UpdateProcessBasicMemoryInfo(mem_info);

  mem_info.fd_num = 0;
  ReadProcFile("/proc/self/fd_num", "", mem_info.fd_num);

  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  if (isolate) {
    v8::HeapStatistics heap_statistics;
    isolate->GetHeapStatistics(&heap_statistics);
    mem_info.js_heap_total = heap_statistics.total_heap_size() / MEM_CONVERT;
    mem_info.js_heap_used = heap_statistics.used_heap_size() / MEM_CONVERT;
  } else {
    LOG(ERROR) << "V8 isolate is null";
    mem_info.js_heap_total = 0;
    mem_info.js_heap_used = 0;
  }
}

static std::string ConvertToBaseValue(const std::string& type, DfxMemInfo &mem_info)
{
  auto mem_info_json = base::Value::Dict()
    .Set("type", type).Set("pid", std::to_string(mem_info.pid)).Set("rss", std::to_string(mem_info.rss))
    .Set("pss", std::to_string(mem_info.pss)).Set("fd_num", std::to_string(mem_info.fd_num))
    .Set("js_heap_total", std::to_string(mem_info.js_heap_total))
    .Set("js_heap_used", std::to_string(mem_info.js_heap_used)).Set("url", mem_info.url);
  std::string mem_info_json_string;
  base::JSONWriter::Write(mem_info_json, &mem_info_json_string);
  return mem_info_json_string;
}

static void InitDfxMemStatus(DfxMemStatus &mem_status_)
{
  mem_status_.error_threshold_counter = COUNTER_INITIAL;
  mem_status_.warning_threshold_counter = COUNTER_INITIAL;
  mem_status_.error_reported = false;
  mem_status_.warning_reported = false;
  mem_status_.upto_error_level = false;
  mem_status_.read_global_param = false;
}

bool MemoryMonitorImpl::DfxMemSysParamObserve()
{
  std::string leakInfo = OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .GetSystemPropertiesInstance().GetStringParameter("web.debug.memleak.time", "");
  if (leakInfo.empty()) {
    return false;
  }

  auto parts = base::SplitStringOnce(leakInfo, ',');
  if (!parts || parts->first.empty() || parts->second.empty()) {
    return false;
  }
  auto [pidStr, timeStr] = *parts;

  if (std::to_string(mem_info_.pid) == pidStr && mem_status_.lastTime != timeStr) {
    mem_status_.lastTime = timeStr;
    return true;
  }
  return false;
}

void MemoryMonitorImpl::StartMonitoring(base::TimeDelta interval)
{
  timer_.Start(FROM_HERE, interval,
               base::BindRepeating(&MemoryMonitorImpl::CollectAndReport,
                                   base::Unretained(this)));
}

void MemoryMonitorImpl::CollectAndReport()
{
  if (DfxMemSysParamObserve()) {
    mem_status_.read_global_param = true;
    MemoryAllocReport();
  };

  UpdateProcessBasicMemoryInfo(mem_info_);
  if (mem_info_.pss > ERROR_MEMORY_LEAK_THRESHOLD * MEM_CONVERT) {
    ++mem_status_.error_threshold_counter;
    ++mem_status_.warning_threshold_counter;
  } else if (mem_info_.pss > WARNING_MEMORY_LEAK_THRESHOLD * MEM_CONVERT) {
    ++mem_status_.warning_threshold_counter;
  } else {
    InitDfxMemStatus(mem_status_);
    return;
  }
  if (mem_status_.error_threshold_counter >= COUNTER_THRESHOLD) {
    mem_status_.read_global_param = false;
    mem_status_.upto_error_level = true;
    MemoryAllocReport();
    mem_status_.error_threshold_counter = COUNTER_INITIAL;
    mem_status_.warning_threshold_counter = COUNTER_INITIAL;
    mem_status_.error_reported = true;
  } else if (mem_status_.warning_threshold_counter >= COUNTER_THRESHOLD) {
    mem_status_.read_global_param = false;
    mem_status_.upto_error_level = false;
    MemoryAllocReport();
    mem_status_.warning_threshold_counter = COUNTER_INITIAL;
    mem_status_.warning_reported = true;
  }
}

void MemoryMonitorImpl::ReportToBrowser(const bool reportSysEvent, const std::string& type)
{
  if (!remote_.is_bound()) {
    LOG(ERROR) << "MemoryAllocReport remote is not bound";
    return;
  }

  std::string mem_info_json_string = ConvertToBaseValue(type, mem_info_);
  reportSysEvent ? remote_->ReportHiSysEvent("PAGE_MEM_LEAK", mem_info_json_string)
                 : remote_->ReportMemInfo(mem_info_json_string);

}

void MemoryMonitorImpl::MemoryAllocReport()
{
  bool reportSysEvent = true;
  std::string type = MemoryMonitorImpl::MEM_LEAK_DETECTED;
  if (mem_status_.read_global_param) {
    reportSysEvent = true;
    type = MemoryMonitorImpl::MEM_LEAK_DETECTED;
  } else if (mem_status_.upto_error_level) {
    reportSysEvent = !mem_status_.error_reported;
    type = MemoryMonitorImpl::PAGE_MEM_LEAK_ERROR;
  } else {
    reportSysEvent = !mem_status_.warning_reported;
    type = MemoryMonitorImpl::PAGE_MEM_LEAK_WARNING;
  }
  UpdateProcessMemoryInfo(mem_info_);
  ReportToBrowser(reportSysEvent, type);
}

void MemoryMonitorImpl::Init()
{
  const char* osReleaseType = OH_GetOsReleaseType();
  const char* distributionReleaseType = OH_GetDistributionOSReleaseType();
  auto IsNullOrEmpty = [](const char* str) {
    return str == nullptr || str[0] == '\0';
  };
  auto IsRelease = [](const char* str) {
    return str != nullptr && strcmp(str, "Release") == 0;
  };
  bool isReleaseVersion =
    IsRelease(osReleaseType) || IsRelease(distributionReleaseType) ||
    (IsNullOrEmpty(osReleaseType) && IsNullOrEmpty(distributionReleaseType));
  if (isReleaseVersion) {
    LOG(WARNING) << "release version";
    return;
  }
  LOG(WARNING) << "beta version";

  InitDfxMemStatus(mem_status_);
  mem_status_.lastTime = "";
  mem_info_.pid = 0;
  StartMonitoring(base::Milliseconds(INTERVAL));
  has_initialized_ = true;
}

std::shared_ptr<MemoryMonitorImpl> MemoryMonitorImpl::GetInstance()
{
  static std::shared_ptr<MemoryMonitorImpl> instance = std::make_shared<MemoryMonitorImpl>();
  return instance;
}

} // namespace content