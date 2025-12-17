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

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include <gmock/gmock.h>
#include "gtest/gtest.h"

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "arkweb/chromium_ext/chrome/browser/profiles/profile_for_include.cc"
#endif

class ProfileForIncludeUnitTest : public ::testing::Test {
 protected:
   void SetUp() override {}
   void TearDown() override {}
}

TEST(ProfileForIncludeUnitTest, HasOTRProfile001) {
  if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
      switches::kEnableNwebEx)) {
    if (base::ohos::IsPcDevice()) {
      EXPECT_TRUE(HasOTRProfile());
    } else {
      EXPECT_FALSE(HasOTRProfile());
    }
  } else {
    EXPECT_FALSE(HasOTRProfile());
  }
}

TEST(ProfileForIncludeUnitTest, GetOTRProfile001) {
  if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
      switches::kEnableNwebEx)) {
    if (base::ohos::IsPcDevice()) {
      ASSERT_NE(GetOTRProfile(), nullptr);
    } else {
      ASSERT_NE(GetOTRProfile(), nullptr);
    }
  } else {
    ASSERT_NE(GetOTRProfile(), nullptr);
  }
}
#endif