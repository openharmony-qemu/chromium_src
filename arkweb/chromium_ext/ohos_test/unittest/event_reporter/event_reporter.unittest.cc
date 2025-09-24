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

#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <string>
#include <cstring>

#include "arkweb/ohos_nweb/src/sysevent/oh_web_performance_timing.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/hisysevent_adapter.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"

namespace content {

class EventReportTest : public testing ::Test {
public: 
    void SetUp() {}
    void TearDown() {}
};

TEST_F(EventReportTest, ReportPageLoadErrorInfo) {
    ReportPageLoadErrorInfo(1, "", 404, 1, "page not found");
    ReportPageLoadErrorInfo(1, "one_error", 404, 1, "page not found");
    ReportPageLoadErrorInfo(1, "one_error", 404, 1, "");
    ReportPageLoadErrorInfo(1, "", 404, 1, "");
}

TEST_F(EventReportTest, ReportAudioPlayErrorInfo) {
    ReportAudioPlayErrorInfo("one_error", 404, "page not found");
    ReportAudioPlayErrorInfo("", 404, "");
    ReportAudioPlayErrorInfo("one_error", 404, "");
    ReportAudioPlayErrorInfo("", 404, "page not found");
}

TEST_F(EventReportTest, ReportVideoPlayErrorInfo) {
    ReportVideoPlayErrorInfo("one_error", 404, "page not found");
    ReportVideoPlayErrorInfo("", 404, "");
    ReportVideoPlayErrorInfo("one_error", 404, "");
    ReportVideoPlayErrorInfo("", 404, "page not found");
}

TEST_F(EventReportTest, ReportWebMediaPlayErrorInfo) {
    ReportWebMediaPlayErrorInfo("one_error", 404, "page not found");
    ReportWebMediaPlayErrorInfo("", 404, "");
    ReportWebMediaPlayErrorInfo("one_error", 404, "");
    ReportWebMediaPlayErrorInfo("", 404, "page not found");
}

TEST_F(EventReportTest, ReportGpuProcessEvent) {
    CrashType type = CrashType::TIMEOUT;
    ReportGpuProcessEvent(type, "TIMEOUT");
    type = CrashType::MAILBOX_NONEXISTENT;
    ReportGpuProcessEvent(type, "MAILBOX_NONEXISTENT");
}

}//namespace content