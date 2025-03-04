/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "base/trace_event/trace_event_ohos.h"

#include "ohos_adapter_helper.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

class TraceObserver;

extern std::unique_ptr<TraceObserver> traceObserver;
TEST(TraceEventOhosTest, StartObserveTraceEnable) {
  StartObserveTraceEnable();
  EXPECT_NE(traceObserver, nullptr);
}

TEST(TraceEventOhosTest, IsOHOSBytraceEnable_True) {
  bool result = IsOHOSBytraceEnable();
  EXPECT_EQ(result, true);
}

TEST(TraceEventOhosTest, IsCategoryEnable) {
  bool result = IsCategoryEnable("disabled-");
  EXPECT_EQ(result, false);

  result = IsCategoryEnable(nullptr);
  EXPECT_EQ(result, false);

  result = IsCategoryEnable("abled-");
  EXPECT_EQ(result, true);
}

TEST(TraceEventOhosTest, GetArg_double) {
  BytraceArg result = GetArg(1.0);
  EXPECT_EQ(result.value.as_double, 1.0);
  EXPECT_EQ(result.type, TYPE_NUMBER);
}

TEST(TraceEventOhosTest, GetArg_char) {
  BytraceArg result = GetArg("test");
  EXPECT_EQ(result.value.as_string, "test");
  EXPECT_EQ(result.type, TYPE_STRING);
}

TEST(TraceEventOhosTest, GetStringFromArg_TYPE_NUMBER) {
  BytraceArg number;
  number.value.as_double = 1.0;
  number.type = TYPE_NUMBER;
  std::string result = GetStringFromArg(number);
  EXPECT_EQ(result, "1.000000");
}

TEST(TraceEventOhosTest, GetStringFromArg_TYPE_STRING) {
  BytraceArg str;
  str.value.as_string = "test";
  str.type = TYPE_STRING;
  std::string result = GetStringFromArg(str);
  EXPECT_EQ(result, "test");
}

TEST(TraceEventOhosTest, GetStringFromArg_UnsupportArgType) {
  BytraceArg invalid_argument;
  invalid_argument.type = TYPE_INVALID;
  std::string result = GetStringFromArg(invalid_argument);
  EXPECT_EQ(result, "UnsupportArgType");
}

TEST(TraceEventOhosTest, GetStringWithArgs) {
  std::string test = "test";
  std::string result = GetStringWithArgs(test);
  EXPECT_EQ(result, test);
}

TEST(TraceEventOhosTest, Trace_NormalScene_01) {
  StartBytrace("test");
  StartAsyncBytrace("test", 0);
  CountBytrace("test", 1);
  FinishAsyncBytrace("test", 0);
  FinishBytrace();
}

TEST(TraceEventOhosTest, Trace_NormalScene_02) {
  StartOHOSBytrace("test");
  CountOHOSBytrace("test", 1);
  FinishOHOSBytrace();
}

TEST(TraceEventOhosTest, ScopedBytrace_01) {
  ScopedBytrace* scopedBytrace = new ScopedBytrace();
  EXPECT_NE(scopedBytrace, nullptr);
  if (scopedBytrace) {
    delete scopedBytrace;
    scopedBytrace = nullptr;
  }
  EXPECT_EQ(scopedBytrace, nullptr);
}

TEST(TraceEventOhosTest, ScopedBytrace_02) {
  ScopedBytrace* scopedBytrace = new ScopedBytrace("test");
  EXPECT_NE(scopedBytrace, nullptr);
  if (scopedBytrace) {
    delete scopedBytrace;
    scopedBytrace = nullptr;
  }
  EXPECT_EQ(scopedBytrace, nullptr);
}

TEST(TraceEventOhosTest, ScopedOHOSBytrace_01) {
  ScopedOHOSBytrace* scopedOHOSBytrace = new ScopedOHOSBytrace();
  EXPECT_NE(scopedOHOSBytrace, nullptr);
  if (scopedOHOSBytrace) {
    delete scopedOHOSBytrace;
    scopedOHOSBytrace = nullptr;
  }
  EXPECT_EQ(scopedOHOSBytrace, nullptr);
}

TEST(TraceEventOhosTest, ScopedOHOSBytrace_02) {
  ScopedOHOSBytrace* scopedOHOSBytrace = new ScopedOHOSBytrace("test");
  EXPECT_NE(scopedOHOSBytrace, nullptr);
  if (scopedOHOSBytrace) {
    delete scopedOHOSBytrace;
    scopedOHOSBytrace = nullptr;
  }
  EXPECT_EQ(scopedOHOSBytrace, nullptr);
}
 