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

#include <gtest/gtest.h>

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"

namespace content {

class LogConsoleMessageUtilsTest : public ::testing::Test {};

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_InfoLevel) {
  int32_t resolved_level = logging::LOGGING_INFO;
  std::u16string message = u"Test info message";
  int32_t line_number = 42;
  std::u16string source_id = u"https://example.com/test.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=0"), std::string::npos);
  EXPECT_NE(log_output.find("message=Test info message"), std::string::npos);
  EXPECT_NE(log_output.find("line_number=42"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/test.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_WarningLevel) {
  int32_t resolved_level = logging::LOGGING_WARNING;
  std::u16string message = u"Test warning message";
  int32_t line_number = 100;
  std::u16string source_id = u"https://example.com/warning.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=1"), std::string::npos);
  EXPECT_NE(log_output.find("message=Test warning message"), std::string::npos);
  EXPECT_NE(log_output.find("line_number=100"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/warning.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_ErrorLevel) {
  int32_t resolved_level = logging::LOGGING_ERROR;
  std::u16string message = u"Test error message";
  int32_t line_number = 25;
  std::u16string source_id = u"https://example.com/error.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=2"), std::string::npos);
  EXPECT_NE(log_output.find("message=Test error message"), std::string::npos);
  EXPECT_NE(log_output.find("line_number=25"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/error.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_FatalLevel) {
  int32_t resolved_level = logging::LOGGING_FATAL;
  std::u16string message = u"Test fatal message";
  int32_t line_number = 999;
  std::u16string source_id = u"https://example.com/fatal.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=3"), std::string::npos);
  EXPECT_NE(log_output.find("message=Test fatal message"), std::string::npos);
  EXPECT_NE(log_output.find("line_number=999"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/fatal.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_DebugLevel) {
  int32_t resolved_level = logging::LOGGING_DEBUG;
  std::u16string message = u"Test debug message";
  int32_t line_number = 10;
  std::u16string source_id = u"https://example.com/debug.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=4"), std::string::npos);
  EXPECT_NE(log_output.find("message=Test debug message"), std::string::npos);
  EXPECT_NE(log_output.find("line_number=10"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/debug.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_NegativeLevel) {
  int32_t resolved_level = -1;
  std::u16string message = u"Test negative level message";
  int32_t line_number = 5;
  std::u16string source_id = u"https://example.com/negative.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=-1"), std::string::npos);
  EXPECT_NE(log_output.find("message=Test negative level message"),std::string::npos);
  EXPECT_NE(log_output.find("line_number=5"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/negative.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_EmptyMessage) {
  int32_t resolved_level = logging::LOGGING_INFO;
  std::u16string message = u"";
  int32_t line_number = 1;
  std::u16string source_id = u"https://example.com/empty.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=0"), std::string::npos);
  EXPECT_NE(log_output.find("message="), std::string::npos);
  EXPECT_NE(log_output.find("line_number=1"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/empty.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_EmptySourceId) {
  int32_t resolved_level = logging::LOGGING_WARNING;
  std::u16string message = u"Message with empty source";
  int32_t line_number = 15;
  std::u16string source_id = u"";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=1"), std::string::npos);
  EXPECT_NE(log_output.find("message=Message with empty source"),std::string::npos);
  EXPECT_NE(log_output.find("line_number=15"), std::string::npos);
  EXPECT_NE(log_output.find("source_id="), std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_ZeroLineNumber) {
  int32_t resolved_level = logging::LOGGING_ERROR;
  std::u16string message = u"Message with zero line number";
  int32_t line_number = 0;
  std::u16string source_id = u"https://example.com/zero.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=2"), std::string::npos);
  EXPECT_NE(log_output.find("message=Message with zero line number"),std::string::npos);
  EXPECT_NE(log_output.find("line_number=0"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/zero.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_LongMessage) {
  int32_t resolved_level = logging::LOGGING_INFO;
  std::u16string message(1000, u'A');
  int32_t line_number = 999999;
  std::u16string source_id = u"https://example.com/long.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=0"), std::string::npos);
  EXPECT_NE(log_output.find("line_number=999999"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/long.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_UndefinedLevel) {
  int32_t resolved_level = 999;
  std::u16string message = u"Message with undefined level";
  int32_t line_number = 50;
  std::u16string source_id = u"https://example.com/undefined.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=999"), std::string::npos);
  EXPECT_NE(log_output.find("message=Message with undefined level"),std::string::npos);
  EXPECT_NE(log_output.find("line_number=50"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/undefined.js"),std::string::npos);
}

TEST_F(LogConsoleMessageUtilsTest, LogConsoleMessage_negativeLevel) {
  int32_t resolved_level = -11;
  std::u16string message = u"Message with negative level";
  int32_t line_number = 50;
  std::u16string source_id = u"https://example.com/negative.js";

  testing::internal::CaptureStderr();
  LogConsoleMessageUtils::LogConsoleMessage(resolved_level, message,line_number, source_id);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("resolved_level=-11"), std::string::npos);
  EXPECT_NE(log_output.find("message=Message with negative level"),std::string::npos);
  EXPECT_NE(log_output.find("line_number=50"), std::string::npos);
  EXPECT_NE(log_output.find("source_id=https://example.com/negative.js"),std::string::npos);
}
}  // namespace content