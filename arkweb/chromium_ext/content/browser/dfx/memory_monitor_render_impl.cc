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

#include "memory_monitor_render_impl.h"

#include <string>
#include <sys/types.h>
#include <unistd.h>

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/json/json_writer.h"
#include "base/process/process_metrics.h"
#include "base/task/single_thread_task_runner.h"
#include "base/values.h"
#include "base/strings/string_split.h"
#include "collect_memory_utils.h"
#include "v8/include/v8-isolate.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

#define WARNING_MEMORY_LEAK_THRESHOLD 800
#define ERROR_MEMORY_LEAK_THRESHOLD 1500
#define COUNTER_INITIAL 0
#define COUNTER_THRESHOLD 10
#define INTERVAL 60000
#define BASIC_INTERVAL 120000

namespace content {

using DfxMemInfo = MemoryMonitorImpl::DfxMemInfo;
using RenderMemInfo = MemoryMonitorImpl::RenderMemInfo;
using DfxMemStatus = MemoryMonitorImpl::DfxMemStatus;

void MemoryMonitorImpl::UpdateProcessBasicMemoryInfo(DfxMemInfo &mem_info)
{
  CollectMemoryUtils collectMemoryUtils;
  collectMemoryUtils.GetProcessBasicMemoryInfo(mem_info.pid, mem_info.rss, mem_info.pss);
  collectMemoryUtils.GetFdCount("/proc/self/fd", mem_info.fd_num);
}

void MemoryMonitorImpl::UpdateProcessMemoryInfo(DfxMemInfo &mem_info)
{
  UpdateProcessBasicMemoryInfo(mem_info);

  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  if (isolate) {
    v8::HeapStatistics heap_statistics;
    isolate->GetHeapStatistics(&heap_statistics);
    mem_info.js_heap_total = heap_statistics.total_heap_size() / kMemoryBytesPerKb;
    mem_info.js_heap_used = heap_statistics.used_heap_size() / kMemoryBytesPerKb;
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
  if (mem_info_.pss > ERROR_MEMORY_LEAK_THRESHOLD * kMemoryBytesPerKb) {
    ++mem_status_.error_threshold_counter;
    ++mem_status_.warning_threshold_counter;
  } else if (mem_info_.pss > WARNING_MEMORY_LEAK_THRESHOLD * kMemoryBytesPerKb) {
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
  InitDfxMemStatus(mem_status_);
  mem_status_.lastTime = "";
  mem_info_.pid = 0;
  StartMonitoring(base::Milliseconds(INTERVAL));
  has_initialized_ = true;
}

void MemoryMonitorImpl::StartCollectBasicRenderMemory(bool is_hidden)
{
  if (is_hidden) {
    CollectBasicRenderMemory();
  } else {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
          FROM_HERE,
          base::BindOnce(&MemoryMonitorImpl::CollectBasicRenderMemory, base::Unretained(this)),
            base::Milliseconds(BASIC_INTERVAL));
  }
}

static std::string ConvertToJson(const RenderMemInfo& render_info)
{
  auto mem_info_json = base::Value::Dict()
    .Set("pid", std::to_string(render_info.pid)).Set("rss", std::to_string(render_info.rss))
    .Set("pss", std::to_string(render_info.pss)).Set("swap_pss", std::to_string(render_info.swap_pss))
    .Set("fd_num", std::to_string(render_info.fd_num)).Set("oom_score_adj", std::to_string(render_info.oom_score_adj))
    .Set("js_heap_total", std::to_string(render_info.js_heap_total))
    .Set("js_heap_used", std::to_string(render_info.js_heap_used)).Set("pa", std::to_string(render_info.pa));
  std::string mem_info_json_string;
  base::JSONWriter::Write(mem_info_json, &mem_info_json_string);
  return mem_info_json_string;
}

void MemoryMonitorImpl::CollectBasicRenderMemory()
{
  RenderMemInfo render_info;
  // get basic info
  CollectMemoryUtils collectMemoryUtils;
  collectMemoryUtils.GetProcessBasicMemoryInfo(render_info.pid, render_info.rss, render_info.pss);
  collectMemoryUtils.ReadProcFile("/proc/self/smaps_rollup", "SwapPss:", render_info.swap_pss);
  collectMemoryUtils.GetFdCount("/proc/self/fd", render_info.fd_num);
  collectMemoryUtils.GetOomScoreAdj("/proc/self/oom_score_adj", render_info.oom_score_adj);

  // get js_heap info
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  if (isolate) {
    v8::HeapStatistics heap_statistics;
    isolate->GetHeapStatistics(&heap_statistics);
    render_info.js_heap_total = heap_statistics.total_heap_size() / kMemoryBytesPerKb;
    render_info.js_heap_used = heap_statistics.used_heap_size() / kMemoryBytesPerKb;
  } else {
    LOG(ERROR) << "V8 isolate is null";
    render_info.js_heap_total = 0;
    render_info.js_heap_used = 0;
  }

  // get PartitionAllocator
  collectMemoryUtils.GetPartitionAllocatorMem(render_info.pa);

  // get gpuMem
  if (!remote_.is_bound()) {
    LOG(ERROR) << "ReportWebMemory remote is not bound";
    return;
  }
  std::string render_info_json_string = ConvertToJson(render_info);
  remote_->ReportHiSysEvent("BASIC_RENDER_MEM", render_info_json_string);
}

std::shared_ptr<MemoryMonitorImpl> MemoryMonitorImpl::GetInstance()
{
  static std::shared_ptr<MemoryMonitorImpl> instance = std::make_shared<MemoryMonitorImpl>();
  return instance;
}

} // namespace content
