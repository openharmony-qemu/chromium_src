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

#include <map>
#include <sstream>

#include "dfx_reporter_browser_impl.h"
#include "base/logging.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "content/public/browser/gpu_data_manager.h"
#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"
#include "base/values.h"
#include "base/json/json_reader.h"
#include "base/trace_event/trace_event.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

void DfxReporterImpl::ProcessPendingReceiver(mojo::PendingReceiver<dfx::mojom::DfxReporter>& receiver)
{
  mojo::MakeSelfOwnedReceiver(std::make_unique<DfxReporterImpl>(), std::move(receiver));
}

void ReportMemTraceNLog(std::map<std::string, std::string>& memMap)
{
  std::ostringstream logStream;
  logStream << "type = " << memMap["type"] << ", "
            << "pid = " << memMap["pid"] << ", "
            << "rss = " << memMap["rss"] << "KB, "
            << "pss = " << memMap["pss"] << "KB, "
            << "fd_num = " << memMap["fd_num"] << ", "
            << "js_heap_total = " << memMap["js_heap_total"] << "KB, "
            << "js_heap_used = " << memMap["js_heap_used"] << "KB, "
            << "gpu_mem = " << memMap["gpu_mem"] << "KB";
  std::string logMsg = logStream.str();

  TRACE_EVENT1("base", "WEBVIEW::PAGE_MEM_LEAK", "MEMORY_INFO", logMsg);
  LOG(INFO) << logMsg;
}

void OnVideoMemoryUsageStatsUpdate(pid_t pid, const std::map<std::string, std::string>& memMap, bool isSysEvent,
                                   const gpu::VideoMemoryUsageStats& gpu_memory_stats)
{
  std::map<std::string, std::string> reportMap = memMap;
  if (!gpu_memory_stats.process_map.empty()) {
    auto it = gpu_memory_stats.process_map.find(pid);
    if (it != gpu_memory_stats.process_map.end()) {
      const auto& stats = it->second;
      reportMap["gpu_mem"] = std::to_string(stats.video_memory / DfxReporterImpl::BYTES_TO_KB);
    } else {
      reportMap["gpu_mem"] = "0";
    }
  }

  ReportMemTraceNLog(reportMap);
#if !defined(COMPONENT_BUILD)
  if (isSysEvent) {
    ReportRendererMem(reportMap["type"], reportMap["pid"], reportMap["rss"], reportMap["pss"],
                      reportMap["js_heap_total"], reportMap["js_heap_used"], reportMap["gpu_mem"], reportMap["url"]);
  }
#endif
}

void GetGpuUsage(pid_t pid, const std::map<std::string, std::string>& memMap, bool isSysEvent)
{
  const auto gpu_data_manager = content::GpuDataManager::GetInstance();
  if (!gpu_data_manager) {
    LOG(ERROR) << "GetGpuUsage gpu_data_manager is nullptr";
    return;
  }

  gpu_data_manager->RequestVideoMemoryUsageStatsUpdate(
      base::BindOnce(&OnVideoMemoryUsageStatsUpdate, pid, memMap, isSysEvent));
}

void ReportRendererInfo(const std::string& sysEventInfoJson, bool isSysEvent)
{
  const auto sysEventInfo = base::JSONReader::ReadAndReturnValueWithError(sysEventInfoJson);
  if (!sysEventInfo.has_value() || !sysEventInfo->is_dict()) {
    LOG(ERROR) << "get sysEventInfo error:" << sysEventInfo.error().message;
    return;
  }

  const base::Value::Dict& sysEventInfoDict = sysEventInfo->GetDict();
  const auto extractString = [&](const std::string& key, const std::string& defaultValue) {
    return sysEventInfoDict.FindString(key) ? *sysEventInfoDict.FindString(key) : defaultValue;
  };

  std::string pid = extractString("pid", "0");
  if (pid == "0" || !std::all_of(pid.begin(), pid.end(), ::isdigit)) {
    LOG(ERROR) << "render process pid is invalid";
    return;
  }
  std::map<std::string, std::string> memMap = {
    {"type", extractString("type", "")},
    {"pid", pid},
    {"rss", extractString("rss", "0")},
    {"pss", extractString("pss", "0")},
    {"fd_num", extractString("fd_num", "0")},
    {"js_heap_total", extractString("js_heap_total", "0")},
    {"js_heap_used", extractString("js_heap_used", "0")},
    {"url", extractString("url", "")}
  };
  GetGpuUsage(static_cast<pid_t>(std::stoi(pid)), memMap, isSysEvent);
}

void DfxReporterImpl::ReportMemInfo(const std::string& sysEventInfoJson)
{
  ReportRendererInfo(sysEventInfoJson, false);
}

void DfxReporterImpl::ReportHiSysEvent(const std::string& eventName, const std::string& sysEventInfoJson)
{
  if (eventName == DfxReporterImpl::PAGE_MEM_LEAK) {
    ReportRendererInfo(sysEventInfoJson, true);
  }
}

// the param `eventInfo` may be used in the future
void FreezeReporterImpl::ReportRenderFreeze(dfx::mojom::FreezeInfoPtr freezeInfo)
{
#if !defined(COMPONENT_BUILD)
  auto packageName = OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetBundleName();
  ReportAppfreeze(freezeInfo->pid, packageName, freezeInfo->processName, freezeInfo->freezeMsg, freezeInfo->uid);
#endif
}

// static
void FreezeReporterImpl::ProcessPendingReceiver(mojo::PendingReceiver<dfx::mojom::FreezeReporter> receiver)
{
  mojo::MakeSelfOwnedReceiver(std::make_unique<FreezeReporterImpl>(), std::move(receiver));
}