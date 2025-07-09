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

#include "arkweb/chromium_ext/content/browser/gpu/ark_web_compositor_util.h"

#include <optional>
#include <gtest/gtest.h>
#include <sys/mman.h>

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "base/test/scoped_command_line.h"
#include "cc/base/switches.h"

#define private public

using namespace testing;

namespace content {

class ArkWebRenderUtilsTest : public testing::Test {
protected:
  void SetUp() {
    original_command_line_ = base::CommandLine::ForCurrentProcess()->argv();
  }

  void TearDown() {
    base::CommandLine::Reset();
    base::CommandLine::Init(0, nullptr);
    base::CommandLine::ForCurrentProcess()->InitFromArgv(original_command_line_);
  }
private:
  std::vector<std::string> original_command_line_;
};

TEST_F(ArkWebRenderUtilsTest, WithoutNwebExFlag)
{
    auto result = ArkWebNumberOfRendererRasterThreads();
    EXPECT_FALSE(result.has_value());
}

#if BUILDFLAG(ARKWEB_NWEB_EX)
TEST_F(ArkWebRenderUtilsTest, WithNwebExFlag_NoEnableSwitch)
{
  base::test::ScopedCommandLine scoped_command_line;
  auto* command_line = base::CommandLine::ForCurrentProcess();

  ASSERT_NE(command_line, nullptr);
  command_line->AppendSwitch(switches::kEnableNwebEx);

  auto result = ArkWebNumberOfRendererRasterThreads();
  EXPECT_FALSE(result.has_value());
}

TEST_F(ArkWebRenderUtilsTest, WithNwebExFlag_EnableSwitchOnly)
{
  base::test::ScopedCommandLine scoped_command_line;
  auto* command_line = base::CommandLine::ForCurrentProcess();
  
  ASSERT_NE(command_line, nullptr);
  command_line->AppendSwitch(switches::kEnableNwebEx);

  auto result = ArkWebNumberOfRendererRasterThreads();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 1);
}

TEST_F(ArkWebRenderUtilsTest, WithNwebExFlag_BothSwitches)
{
  base::test::ScopedCommandLine scoped_command_line;
  auto* command_line = base::CommandLine::ForCurrentProcess();
  
  ASSERT_NE(command_line, nullptr);
  command_line->AppendSwitch(switches::kEnableNwebEx);
  command_line->AppendSwitch(switches::kNumRasterThreads);

  auto result = ArkWebNumberOfRendererRasterThreads();
  EXPECT_FALSE(result.has_value());
}
#endif
}