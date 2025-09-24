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

#include "arkweb/chromium_ext/content/browser/log_console_message_utils.h"

#include <string>
#include "gtest/gtest.h"
#include "base/logging.h"

namespace content {

class LogConsoleMessageUtilsTest : public testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(LogConsoleMessageUtilsTest, LogInfoMessage) {
    std::u16string message = u"This is an info log";
    std::u16string source_id = u"test.js";
    int32_t line_number = 42;
    
    EXPECT_NO_FATAL_FAILURE(
        LogConsoleMessageUtils::LogConsoleMessage(
            logging::LOGGING_INFO, message, line_number, source_id)
    );
}

TEST_F(LogConsoleMessageUtilsTest, LogWarningMessage) {
    std::u16string message = u"This is a warning log";
    std::u16string source_id = u"test.js";
    int32_t line_number = 100;
    
    EXPECT_NO_FATAL_FAILURE(
        LogConsoleMessageUtils::LogConsoleMessage(
            logging::LOGGING_WARNING, message, line_number, source_id)
    );
}

TEST_F(LogConsoleMessageUtilsTest, LogErrorMessage) {
    std::u16string message = u"This is an error log";
    std::u16string source_id = u"error.js";
    int32_t line_number = 15;
    
    EXPECT_NO_FATAL_FAILURE(
        LogConsoleMessageUtils::LogConsoleMessage(
            logging::LOGGING_ERROR, message, line_number, source_id)
    );
}

TEST_F(LogConsoleMessageUtilsTest, LogFatalMessage) {
    std::u16string message = u"This is a fatal error log";
    std::u16string source_id = u"fatal.js";
    int32_t line_number = 1;
    
    EXPECT_NO_FATAL_FAILURE(
        LogConsoleMessageUtils::LogConsoleMessage(
            logging::LOGGING_FATAL, message, line_number, source_id)
    );
}

TEST_F(LogConsoleMessageUtilsTest, LogDebugMessage) {
    std::u16string message = u"This is a debug log";
    std::u16string source_id = u"debug.js";
    int32_t line_number = 99;
    
    EXPECT_NO_FATAL_FAILURE(
        LogConsoleMessageUtils::LogConsoleMessage(
            logging::LOGGING_DEBUG, message, line_number, source_id)
    );
}

TEST_F(LogConsoleMessageUtilsTest, LogUnknownLevelMessage) {
    std::u16string message = u"This is an unknown level log";
    std::u16string source_id = u"unknown.js";
    int32_t line_number = 50;
    
    EXPECT_NO_FATAL_FAILURE(
        LogConsoleMessageUtils::LogConsoleMessage(999, message, line_number, source_id)
    );
}

TEST_F(LogConsoleMessageUtilsTest, LogNegativeLevelMessage) {
    std::u16string message = u"This is a negative level log";
    std::u16string source_id = u"negative.js";
    int32_t line_number = -1;
    
    EXPECT_NO_FATAL_FAILURE(
        LogConsoleMessageUtils::LogConsoleMessage(-1, message, line_number, source_id)
    );
}

TEST_F(LogConsoleMessageUtilsTest, LogEmptyMessage) {
    std::u16string message = u"";
    std::u16string source_id = u"empty.js";
    int32_t line_number = 0;
    
    EXPECT_NO_FATAL_FAILURE(
        LogConsoleMessageUtils::LogConsoleMessage(
            logging::LOGGING_INFO, message, line_number, source_id)
    );
}

TEST_F(LogConsoleMessageUtilsTest, LogEmptySourceId) {
    std::u16string message = u"Test empty source ID";
    std::u16string source_id = u"";
    int32_t line_number = 10;
    
    EXPECT_NO_FATAL_FAILURE(
        LogConsoleMessageUtils::LogConsoleMessage(
            logging::LOGGING_WARNING, message, line_number, source_id)
    );
}

}  // namespace content