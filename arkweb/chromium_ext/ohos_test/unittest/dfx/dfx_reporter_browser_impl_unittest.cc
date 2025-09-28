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

#include "arkweb/chromium_ext/content/browser/dfx/dfx_reporter_browser_impl.h"

#include <vector>
#include <gtest/gtest.h>
#include <map>
#include <sstream>

#include "base/logging.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "content/public/browser/gpu_data_manager.h"
#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"
#include "base/values.h"
#include "base/json/json_reader.h"
#include "base/trace_event/trace_event.h"

class DfxReporterBrowserImplTest : public testing::Test {
public:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(DfxReporterBrowserImplTest, OnVideoMemoryUsageStatsUpdate) {
    pid_t pid = 1234;
    bool isSysEvent = false;
    std::map<std::string, std::string> memMap0;
    memMap0["type"] = "unknown";
    memMap0["pid"] = "1234";
    memMap0["rss"] = "unknown";
    memMap0["fd_num"] = "unknown";
    memMap0["js_heap_total"] = "unknown";
    memMap0["js_heap_used"] = "unknown";
    memMap0["gpu_mem"] = "unknown";
    const std::map<std::string, std::string> memMap = memMap0;
    gpu::VideoMemoryUsageStats gpu_memory_stats;
    if(!gpu_memory_stats.process_map.empty()) {
        gpu_memory_stats.process_map.clear();
    }
    testing::internal::CaptureStderr();
    OnVideoMemoryUsageStatsUpdate(pid, memMap, isSysEvent, gpu_memory_stats);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("gpu_mem = unknown"), std::string::npos);
    log_output = "";

    gpu::VideoMemoryUsageStats::ProcessStats stats;
    isSysEvent = true;
    stats.has_duplicates = true;
    stats.video_memory = 2048;
    gpu_memory_stats.process_map[1] = stats;
    testing::internal::CaptureStderr();
    OnVideoMemoryUsageStatsUpdate(pid, memMap, isSysEvent, gpu_memory_stats);
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("gpu_mem = 0"), std::string::npos);
    log_output = "";

    gpu_memory_stats.process_map[1234] = stats;
    testing::internal::CaptureStderr();
    OnVideoMemoryUsageStatsUpdate(pid, memMap, isSysEvent, gpu_memory_stats);
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("gpu_mem = 2"), std::string::npos);
}

TEST_F(DfxReporterBrowserImplTest, ReportRendererInfo001) {
    const std::string sysEventInfoJson = "";
    ReportRendererInfo(sysEventInfoJson, false);
    ReportRendererInfo(sysEventInfoJson, true);
}

TEST_F(DfxReporterBrowserImplTest, ReportRendererInfo002) {
    const std::string sysEventInfoJson = "invalid json string";
    ReportRendererInfo(sysEventInfoJson, false);
    ReportRendererInfo(sysEventInfoJson, true);
}

TEST_F(DfxReporterBrowserImplTest, ReportRendererInfo003) {
    const std::string sysEventInfoJson = 
    R"({
        "fd_num":"43690",
        "js_heap_total":"12297829382473034410",
        "js_heap_used":"12297829382473034410",
        "pid":"-1431655766",
        "pss":"12297829382473034410",
        "rss":"12297829382473034410",
        "type":"gpu",
        "url":""
    })";
    ReportRendererInfo(sysEventInfoJson, false);
    ReportRendererInfo(sysEventInfoJson, true);
}

TEST_F(DfxReporterBrowserImplTest, ReportRendererInfo004) {
    const std::string sysEventInfoJson = 
    R"({
        "fd_num":"43690",
        "js_heap_total":"12297829382473034410",
        "js_heap_used":"12297829382473034410",
        "pid":"0",
        "pss":"12297829382473034410",
        "rss":"12297829382473034410",
        "type":"gpu",
        "url":""
    })";
    ReportRendererInfo(sysEventInfoJson, false);
    ReportRendererInfo(sysEventInfoJson, true);
}

TEST_F(DfxReporterBrowserImplTest, ReportHiSysEvent) {
    DfxReporterImpl dfxreporterimpl;
    dfxreporterimpl.ReportHiSysEvent("test_eventname", "");
    dfxreporterimpl.ReportHiSysEvent("PAGE_MEM_LEAK", "");
}
