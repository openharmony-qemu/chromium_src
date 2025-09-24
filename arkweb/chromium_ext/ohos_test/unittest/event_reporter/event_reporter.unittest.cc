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

TEST_F(EventReportTest, ReportChildProcessInitFail) {
    bool is_gpu = true;
    int err = 0;

    testing::internal::CaptureStderr();
    ReportChildProcessInitFail(is_gpu, err);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(is_gpu);
    log_output = "";

    is_gpu = false;
    err = 1;
    testing::internal::CaptureStderr();
    ReportChildProcessInitFail(is_gpu, err);
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(is_gpu);
}

TEST_F(EventReportTest, ReportPageLoadErrorInfo) {
    testing::internal::CaptureStderr();
    ReportPageLoadErrorInfo(1, "", 404, 1, "page not found");
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportPageLoadErrorInfo { error_type= , errorDesc=Page not found }"), std::string::npos);

    testing::internal::CaptureStderr();
    ReportPageLoadErrorInfo(1, "one_error", 404, 1, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    
    EXPECT_NE(log_output.find("ReportPageLoadErrorInfo { error_type=one_error , errorDesc=Page not found }"), std::string::npos);
    testing::internal::CaptureStderr();
    ReportPageLoadErrorInfo(1, "one_error", 404, 1, "");
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportPageLoadErrorInfo { error_type=one_error , errorDesc=Page not found }"), std::string::npos);

    testing::internal::CaptureStderr();
    ReportPageLoadErrorInfo(1, "", 404, 1, "");
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportPageLoadErrorInfo { error_type= , errorDesc= }"), std::string::npos);
}

TEST_F(EventReportTest, ReportAudioPlayErrorInfo) {
    testing::internal::CaptureStderr();
    ReportAudioPlayErrorInfo("", 404, "page not found");
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportAudioPlayErrorInfo { error_type= , errorDesc=Page not found }"), std::string::npos);

    testing::internal::CaptureStderr();
    ReportAudioPlayErrorInfo("", 404, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    
    EXPECT_NE(log_output.find("ReportAudioPlayErrorInfo { error_type=one_error , errorDesc=Page not found }"), std::string::npos);
    testing::internal::CaptureStderr();
    ReportAudioPlayErrorInfo("", 404, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportAudioPlayErrorInfo { error_type=one_error , errorDesc=Page not found }"), std::string::npos);

    testing::internal::CaptureStderr();
    ReportAudioPlayErrorInfo("", 404, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportAudioPlayErrorInfo { error_type= , errorDesc= }"), std::string::npos);
}

TEST_F(EventReportTest, ReportVideoPlayErrorInfo) {
    testing::internal::CaptureStderr();
    ReportVideoPlayErrorInfo("", 404, "page not found");
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportVideoPlayErrorInfo { error_type= , errorDesc=Page not found }"), std::string::npos);

    testing::internal::CaptureStderr();
    ReportVideoPlayErrorInfo("", 404, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    
    EXPECT_NE(log_output.find("ReportVideoPlayErrorInfo { error_type=one_error , errorDesc=Page not found }"), std::string::npos);
    testing::internal::CaptureStderr();
    ReportVideoPlayErrorInfo("", 404, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportVideoPlayErrorInfo { error_type=one_error , errorDesc=Page not found }"), std::string::npos);

    testing::internal::CaptureStderr();
    ReportVideoPlayErrorInfo("", 404, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportVideoPlayErrorInfo { error_type= , errorDesc= }"), std::string::npos);
}

TEST_F(EventReportTest, ReportWebMediaPlayyErrorInfo) {
    testing::internal::CaptureStderr();
    ReportWebMediaPlayyErrorInfo("", 404, "page not found");
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportWebMediaPlayyErrorInfo { error_type= , errorDesc=Page not found }"), std::string::npos);

    testing::internal::CaptureStderr();
    ReportWebMediaPlayyErrorInfo("", 404, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    
    EXPECT_NE(log_output.find("ReportWebMediaPlayyErrorInfo { error_type=one_error , errorDesc=Page not found }"), std::string::npos);
    testing::internal::CaptureStderr();
    ReportWebMediaPlayyErrorInfo("", 404, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportWebMediaPlayyErrorInfo { error_type=one_error , errorDesc=Page not found }"), std::string::npos);

    testing::internal::CaptureStderr();
    ReportWebMediaPlayyErrorInfo("", 404, "page not found");
    log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("ReportWebMediaPlayyErrorInfo { error_type= , errorDesc= }"), std::string::npos);
}

TEST_F(EventReportTest, ReportGpuProcessEvent) {
    CrashType type = CrashType::TIMEOUT;
    ReportGpuProcessEvent(type, "TIMEOUT");
    type = CrashType::MAILBOX_NONEXISTENT;
    ReportGpuProcessEvent(type, "MAILBOX_NONEXISTENT");
}

TEST_F(EventReportTest, ReportSkiaOOMError) {
    const std::string errorDesc = "one_error";
    ReportSkiaOOMError(errorDesc);

    const std::string errorDesc1 = "";
    ReportSkiaOOMError(errorDesc1);
}

}//namespace content