/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include "base/command_line.h"
#include "base/test/scoped_command_line.h"
#include "content/public/common/content_switches.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/content_settings/core/browser/content_settings_default_provider_for_include.cc"

#include "testing/gtest/include/gtest/gtest.h"

namespace content_settings {

class ContentSettingsDefaultProviderForIncludeTest : public ::testing::Test {
 protected:
   void SetUp() override {
     base::CommandLine::Init(0, nullptr);
     base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
     if (cmd_line->HasSwitch(switches::kEnableNwebExExceptionList)) {
        cmd_line->RemoveSwitch(switches::kEnableNwebExExceptionList);
     }
   }
   void TearDown() override {}
};

TEST_F(ContentSettingsDefaultProviderForIncludeTest, ContentSettingsDefaultProviderForIncludeTest001) {
    base::test::ScopedCommandLine scoped_command_line;
    auto* command_line = base::CommandLine::ForCurrentProcess();

    ASSERT_NE(command_line, nullptr);

    bool is_skipped = ShouldSkipSettingForContentTypeExt(ContentSettingsType::IMAGES);
    EXPECT_TRUE(is_skipped);
}

TEST_F(ContentSettingsDefaultProviderForIncludeTest, ContentSettingsDefaultProviderForIncludeTest002) {
    base::test::ScopedCommandLine scoped_command_line;
    auto* command_line = base::CommandLine::ForCurrentProcess();

    ASSERT_NE(command_line, nullptr);

    bool is_skipped = ShouldSkipSettingForContentTypeExt(ContentSettingsType::COOKIES);
    EXPECT_TRUE(is_skipped);
}

TEST_F(ContentSettingsDefaultProviderForIncludeTest, ContentSettingsDefaultProviderForIncludeTest003) {
    base::test::ScopedCommandLine scoped_command_line;
    auto* command_line = base::CommandLine::ForCurrentProcess();

    ASSERT_NE(command_line, nullptr);

    bool is_skipped = ShouldSkipSettingForContentTypeExt(ContentSettingsType::JAVASCRIPT);
    EXPECT_TRUE(is_skipped);
}

TEST_F(ContentSettingsDefaultProviderForIncludeTest, ContentSettingsDefaultProviderForIncludeTest004) {
    base::test::ScopedCommandLine scoped_command_line;
    auto* command_line = base::CommandLine::ForCurrentProcess();

    ASSERT_NE(command_line, nullptr);
    command_line->AppendSwitch(switches::kEnableNwebExExceptionList);

    bool is_skipped = ShouldSkipSettingForContentTypeExt(ContentSettingsType::IMAGES);
    EXPECT_TRUE(is_skipped);
}

TEST_F(ContentSettingsDefaultProviderForIncludeTest, ContentSettingsDefaultProviderForIncludeTest005) {
    base::test::ScopedCommandLine scoped_command_line;
    auto* command_line = base::CommandLine::ForCurrentProcess();

    ASSERT_NE(command_line, nullptr);
    command_line->AppendSwitch(switches::kEnableNwebExExceptionList);

    bool is_skipped = ShouldSkipSettingForContentTypeExt(ContentSettingsType::COOKIES);
    EXPECT_TRUE(is_skipped);
}

TEST_F(ContentSettingsDefaultProviderForIncludeTest, ContentSettingsDefaultProviderForIncludeTest006) {
    base::test::ScopedCommandLine scoped_command_line;
    auto* command_line = base::CommandLine::ForCurrentProcess();

    ASSERT_NE(command_line, nullptr);
    command_line->AppendSwitch(switches::kEnableNwebExExceptionList);

    bool is_skipped = ShouldSkipSettingForContentTypeExt(ContentSettingsType::JAVASCRIPT);
    EXPECT_TRUE(is_skipped);

    command_line->RemoveSwitch(switches::kEnableNwebExExceptionList);
}

} // namespace content_settings