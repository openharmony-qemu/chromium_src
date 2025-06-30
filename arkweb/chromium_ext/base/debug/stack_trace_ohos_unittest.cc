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

#include "testing/gtest/include/gtest/gtest.h"

#include "arkweb/chromium_ext/base/debug/stack_trace_ohos.cc"

class StackTraceOhosTest : public testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StackTraceOhosTest, EndsWith_01) {
  const std::string& s = "helloworld";
  const std::string& suffix = "world";
  bool result = EndsWith(s, suffix);
  EXPECT_TRUE(result);
}

TEST_F(StackTraceOhosTest, EndsWith_02) {
  const std::string& s = "helloworld";
  const std::string& suffix = "hello";
  bool result = EndsWith(s, suffix);
  EXPECT_FALSE(result);
}

TEST_F(StackTraceOhosTest, EndsWith_03) {
  const std::string& s = "hello";
  const std::string& suffix = "helloworld";
  bool result = EndsWith(s, suffix);
  EXPECT_FALSE(result);
}

TEST_F(StackTraceOhosTest, itoa_r_01) {
  long int i = 1;
  char buf[] = "abc";
  long unsigned int sz = 0;
  int base = 1;
  long unsigned int padding = 1;
  auto result = base::debug::internal::itoa_r(i, buf, sz, base, padding);
  EXPECT_EQ(result, nullptr);
}

TEST_F(StackTraceOhosTest, itoa_r_02) {
  long int i = 1;
  char buf[] = "abc";
  long unsigned int sz = 2;
  int base = 1;
  long unsigned int padding = 1;
  auto result = base::debug::internal::itoa_r(i, buf, sz, base, padding);
  EXPECT_EQ(result, nullptr);
}

TEST_F(StackTraceOhosTest, itoa_r_03) {
  long int i = 1;
  char buf[] = "abc";
  long unsigned int sz = 2;
  int base = 17;
  long unsigned int padding = 1;
  auto result = base::debug::internal::itoa_r(i, buf, sz, base, padding);
  EXPECT_EQ(result, nullptr);
}

TEST_F(StackTraceOhosTest, itoa_r_04) {
  long int i = -1;
  char buf[] = "abc";
  long unsigned int sz = 1;
  int base = 10;
  long unsigned int padding = 1;
  auto result = base::debug::internal::itoa_r(i, buf, sz, base, padding);
  EXPECT_EQ(result, nullptr);
}

TEST_F(StackTraceOhosTest, itoa_r_05) {
  long int i = -1;
  char buf[] = "abc";
  long unsigned int sz = 2;
  int base = 10;
  long unsigned int padding = 1;
  auto result = base::debug::internal::itoa_r(i, buf, sz, base, padding);
  EXPECT_EQ(result, nullptr);
}

TEST_F(StackTraceOhosTest, itoa_r_06) {
  long int i = -1;
  char buf[] = "abc";
  long unsigned int sz = 3;
  int base = 10;
  long unsigned int padding = 1;
  auto result = base::debug::internal::itoa_r(i, buf, sz, base, padding);
  EXPECT_EQ(*result, *buf);
}