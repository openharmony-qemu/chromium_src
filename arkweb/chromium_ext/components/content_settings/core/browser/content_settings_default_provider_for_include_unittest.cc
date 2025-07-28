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

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "base/command_line.h"
#include "base/test/scoped_command_line.h"
#include "content/public/common/content_switches.h"
#include "components/content_settings/core/common/content_settings_types.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace content_settings {

TEST(ContentSettingsDefaultProviderForIncludeTest, ShouldSkipSettingForContentTypeExt_EnableNwebExExceptionList) {
    base::test::ScopedCommandLine scoped_command_line;
    auto* command_line = base::CommandLine::ForCurrentProcess();

    ASSERT_NE(command_line, nullptr);
    command_line->AppendSwitch(switches::kEnableNwebExExceptionList);

    EXPECT_FALSE(ShouldSkipSettingForContentTypeExt(ContentSettingsType::JAVASCRIPT));
    EXPECT_FALSE(ShouldSkipSettingForContentTypeExt(ContentSettingsType::COOKIES));

    EXPECT_TRUE(ShouldSkipSettingForContentTypeExt(ContentSettingsType::IMAGES));
}

TEST(ContentSettingsDefaultProviderForIncludeTest, ShouldSkipSettingForContentTypeExt_DisableNwebExExceptionList) {
    base::test::ScopedCommandLine scoped_command_line;
    auto* command_line = base::CommandLine::ForCurrentProcess();

    ASSERT_NE(command_line, nullptr);
    command_line->RemoveSwitch(switches::kEnableNwebExExceptionList);

    EXPECT_TRUE(ShouldSkipSettingForContentTypeExt(ContentSettingsType::JAVASCRIPT));
    EXPECT_TRUE(ShouldSkipSettingForContentTypeExt(ContentSettingsType::COOKIES));

    EXPECT_TRUE(ShouldSkipSettingForContentTypeExt(ContentSettingsType::IMAGES));
}

} // namespace content_settings

#endif // BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)