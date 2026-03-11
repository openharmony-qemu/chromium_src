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

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include "arkweb/chromium_ext/content/browser/dfx/dfx_reporter_browser_impl.h"
#include "content/public/browser/gpu_data_manager.h"

namespace OHOS::NWeb {
namespace {
    constexpr size_t MAX_STRING_LENGTH = 255;
    constexpr int32_t MAX_INT32_VALUE = 5000;
}

void DfxReporterBrowserImplFuzz001Test(FuzzedDataProvider* fdp) {
    DfxReporterImpl reporter;
    // 安全地构造 JSON 数据
    size_t remaining = fdp->remaining_bytes();
    size_t json_length = std::min(static_cast<size_t>(MAX_STRING_LENGTH), remaining);

    std::string jsonData;
    if (json_length > 0) {
        jsonData = fdp->ConsumeRandomLengthString(json_length);
    }
    
    // Mem 测试 RepotInfo
    reporter.ReportMemInfo(jsonData);
    
    // Test with empty JSON
    reporter.ReportMemInfo("");
    // Test with invalid JSON
    reporter.ReportMemInfo("invalid_json");
}

void DfxReporterBrowserImplFuzz002Test(FuzzedDataProvider* fdp) {
    DfxReporterImpl reporter;
    // 安全地构造 JSON 数据
    size_t remaining = fdp->remaining_bytes();
    size_t event_length = std::min(static_cast<size_t>(MAX_STRING_LENGTH), remaining / 2);
    size_t json_length = std::min(static_cast<size_t>(MAX_STRING_LENGTH), remaining / 2);

    std::string eventName;
    std::string jsonData;

    if (json_length > 0) {
        jsonData = fdp->ConsumeRandomLengthString(json_length);
    }
    if (event_length > 0) {
        eventName = fdp->ConsumeRandomLengthString(event_length);
    }
    
    reporter.ReportHiSysEvent(eventName, jsonData);
    // Test with empty eventName
    reporter.ReportHiSysEvent("", jsonData);

    // Test with empty JSON
    reporter.ReportHiSysEvent(eventName, "");
    // Test with both empty
    reporter.ReportHiSysEvent("", "");
    // 测试 PAGE_MEM_LEAK
    reporter.ReportHiSysEvent("PAGE_MEM_LEAK", "");
}

void DfxReporterBrowserImplFuzz003Test(FuzzedDataProvider* fdp) {
    DfxReporterImpl reporter;
    // 安全地构造 JSON 数据
    size_t remaining = fdp->remaining_bytes();
    size_t json_length = std::min(static_cast<size_t>(MAX_STRING_LENGTH), remaining);
    
    std::string jsonData;
    if (json_length > 0) {
        jsonData = fdp->ConsumeRandomLengthString(json_length);
    }
    
    bool isSysEvent = fdp->ConsumeBool();
    // Test both isSysEvent true and false
    ReportRendererInfo(jsonData, isSysEvent);
    // 测试 空 JSON
    ReportRendererInfo("", isSysEvent);
    // 测试无效 JSON
    ReportRendererInfo("invalid_json", isSysEvent);
}

void DfxReporterBrowserImplFuzz004Test(FuzzedDataProvider* fdp) {
    int32_t pid = fdp->ConsumeIntegral<int32_t>();
    int32_t uid = fdp->ConsumeIntegral<int32_t>();
    
    // 安全地构造字符串
    size_t remaining = fdp->remaining_bytes();
    size_t process_length = std::min(static_cast<size_t>(MAX_STRING_LENGTH), remaining / 2);
    size_t freeze_length = std::min(static_cast<size_t>(MAX_STRING_LENGTH), remaining / 2);
    
    std::string processName;
    std::string freezeMsg;
    
    if (process_length > 0) {
        processName = fdp->ConsumeRandomLengthString(process_length);
    }
    if (freeze_length > 0) {
        freezeMsg = fdp->ConsumeRandomLengthString(freeze_length);
    }
    
    FreezeReporterImpl reporter;
    reporter.ReportRenderFreeze(pid, processName, freezeMsg, uid);

    // 测试边界情况
    reporter.ReportRenderFreeze(0, "", "", 0);
    reporter.ReportRenderFreeze(-1, "test", "freeze", -1);
    reporter.ReportRenderFreeze(MAX_INT32_VALUE, "long_process_name", "long_freeze_name", MAX_INT32_VALUE);
}

void DfxReporterBrowserImplFuzz005Test(FuzzedDataProvider* fdp) {
    pid_t pid = fdp->ConsumeIntegral<pid_t>();
    bool isSysEvent = fdp->ConsumeBool();

    std::map<std::string, std::string> memMap;
    size_t remaining = fdp->remaining_bytes();
    size_t max_remaining = std::min(static_cast<size_t>(MAX_STRING_LENGTH), remaining);
    
    if (max_remaining > 0) {
        size_t per_string_length = max_remaining / 7; // 7个字段
        memMap["type"] = fdp->ConsumeRandomLengthString(per_string_length);
        memMap["pid"] = std::to_string(fdp->ConsumeIntegral<int32_t>());
        memMap["rss"] = fdp->ConsumeRandomLengthString(per_string_length);
        memMap["fd_num"] = fdp->ConsumeRandomLengthString(per_string_length);
        memMap["js_heap_total"] = fdp->ConsumeRandomLengthString(per_string_length);
        memMap["js_heap_used"] = fdp->ConsumeRandomLengthString(per_string_length);
        memMap["gpu_mem"] = fdp->ConsumeRandomLengthString(per_string_length);
    }

    // 测试空映射
    std::map<std::string, std::string> emptyMap;

    // 测试无效数据
    std::map<std::string, std::string> invalidMap;
    invalidMap["type"] = "unknown";
    invalidMap["pid"] = "invalid_pid";
    invalidMap["rss"] = "invalid_rss";
    invalidMap["fd_num"] = "invalid_fd";
    invalidMap["js_heap_total"] = "invalid_js_heap_total";
    invalidMap["js_heap_used"] = "invalid_js_heap_used";
    invalidMap["gpu_mem"] = "invalid_gpu_mem";

    // 测试有效映射
    std::map<std::string, std::string> validMap;
    validMap["type"] = "gpu";
    validMap["pid"] = "1234";
    validMap["rss"] = "1024";
    validMap["fd_num"] = "10";
    validMap["js_heap_total"] = "2048";
    validMap["js_heap_used"] = "1024";
    validMap["gpu_mem"] = "512";

    gpu::VideoMemoryUsageStats gpu_memory_stats;

    // 测试各种场景
    OnVideoMemoryUsageStatsUpdate(pid, emptyMap, isSysEvent, gpu_memory_stats);
    OnVideoMemoryUsageStatsUpdate(pid, memMap, isSysEvent, gpu_memory_stats);
    OnVideoMemoryUsageStatsUpdate(pid, invalidMap, isSysEvent, gpu_memory_stats);
    OnVideoMemoryUsageStatsUpdate(pid, validMap, isSysEvent, gpu_memory_stats);

    // 测试带进场统计的场景
    gpu::VideoMemoryUsageStats::ProcessStats stats;
    stats.has_duplicates = fdp->ConsumeBool();
    stats.video_memory = fdp->ConsumeIntegral<int64_t>();
    
    gpu_memory_stats.process_map[1] = stats;
    OnVideoMemoryUsageStatsUpdate(pid, validMap, isSysEvent, gpu_memory_stats);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    DfxReporterBrowserImplFuzz001Test(&fdp);
    DfxReporterBrowserImplFuzz002Test(&fdp);
    DfxReporterBrowserImplFuzz003Test(&fdp);
    DfxReporterBrowserImplFuzz004Test(&fdp);
    DfxReporterBrowserImplFuzz005Test(&fdp);
    return 0;
}
}   // namespace OHOS::NWeb
