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
#include "build/build_config.h"
#include "build/buildflag.h"
#include "base/command_line.h"
#include "base/test/scoped_command_line.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_sys_info_util_ext.h"
#include "content/public/common/content_switches.h"

#include "arkweb/chromium_ext/chrome/browser/profiles/profile_for_include.cc"


class ProfileForIncludeUnitTest : public ::testing::Test {
 protected:
   void SetUp() override {
     static bool initialized = false;
     if (!initialized) {
       int argc = 1;
       const char* argv[] = {"test_program"};
       base::CommandLine::Init(argc, argv);
       initialized = true;
     }
    
     base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
   }
   void TearDown() override {
     base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
     base::CommandLine::ForCurrentProcess()->RemoveSwitch(switches::kEnableNwebEx);
   }
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

TEST(ProfileForIncludeUnitTest, HasOTRProfile002) {
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

TEST_F(ProfileForIncludeUnitTest, HasOTRProfile003) {
  // set mock return value
  ON_CALL(base::ohos::SysInfoUtilsMock::GetInstance(), IsPcDevice())
      .WillByDefault(testing::Return(true));
  
  base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kEnableNwebEx);
  
  Profile profile;
  profile.SetOffTheRecord(true);
  
  EXPECT_TRUE(profile.HasOTRProfile());
}

// 场景1: 条件为true，off_the_record_ 为 true
TEST_F(ProfileTest, HasOTRProfile004) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kEnableNwebEx);
  
  Profile profile;
  profile.SetOffTheRecord(true);
  
  EXPECT_TRUE(profile.HasOTRProfile());
}

// 场景2: 条件为true，off_the_record_ 为 false
TEST_F(ProfileTest, HasOTRProfile005) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kEnableNwebEx);
  
  Profile profile;
  profile.SetOffTheRecord(false);
  
  EXPECT_FALSE(profile.HasOTRProfile());
}

// 场景3: 条件为false（无开关）
TEST_F(ProfileTest, HasOTRProfile006) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  // 不添加开关
  
  Profile profile;
  profile.SetOffTheRecord(true);
  
  EXPECT_FALSE(profile.HasOTRProfile());
}

// 场景4: 条件为false（非PC设备）
TEST_F(ProfileTest, HasOTRProfile007) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kEnableNwebEx);
  
  Profile profile;
  profile.SetOffTheRecord(true);
  
  EXPECT_FALSE(profile.HasOTRProfile());
}

// GetOTRProfile 测试
TEST_F(ProfileTest, HasOTRProfile008) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kEnableNwebEx);
  
  Profile profile;
  Profile* otr_profile = new Profile();
  profile.SetOffTheRecordProfile(otr_profile);
  
  EXPECT_EQ(otr_profile, profile.GetOTRProfile());
}

TEST_F(ProfileTest, HasOTRProfile009) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  // 不添加开关
  
  Profile profile;
  
  EXPECT_EQ(nullptr, profile.GetOTRProfile());
}
#endif