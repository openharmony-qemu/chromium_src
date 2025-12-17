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

#include "third_party/blink/renderer/core/frame/navigator.h"
#include "base/command_line.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace blink {

class NavigatorForIncludeTest : public ::testing::Test {};

TEST_F(NavigatorForIncludeTest, AppPackageNameTest)
{
  std::string package_name = "";
  ExecutionContext *content = nullptr;
  Navigator navigator(content);
  String name = navigator.appPackageName();

  EXPECT_EQ(name, String::FromUTF8(package_name));
}

TEST_F(NavigatorForIncludeTest, AppPackageNameTest2)
{
  const char* argv[] = {"test_program", "--bundle-name=com.example.app"};
  base::CommandLine::ForCurrentProcess()->InitFromArgv(2, argv);
  ExecutionContext *content = nullptr;
  Navigator navigator(content);
  String package_name = navigator.appPackageName();
  EXPECT_EQ(package_name, "com.example.app");
}

TEST_F(NavigatorForIncludeTest, AppPackageName_NullCommandLine) {
  base::CommandLine* original_command_line =
      base::CommandLine::ForCurrentProcess();
  ASSERT_NE(nullptr, original_command_line);

  base::CommandLine::Reset();
  EXPECT_EQ(base::CommandLine::ForCurrentProcess(), nullptr);

  ExecutionContext *content = nullptr;
  Navigator navigator(content);
  String result = navigator.appPackageName();
  EXPECT_EQ(result, "");

  base::CommandLine::Init(0, nullptr);
  EXPECT_NE(base::CommandLine::ForCurrentProcess(), nullptr);
}
}  // namespace blink
