// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "arkweb/chromium_ext/content/common/arkweb_user_agent_ext.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_base_ohos_api.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "testing/gtest/include/gtest/gtest.h"
#include "base/command_line.h"
#include "base/ohos/sys_info_utils_ext.h"
using namespace base::ohos;

namespace content {

#if BUILDFLAG(ARKWEB_USERAGENT)

class ArkwebUserAgentExtTest : public testing::Test {
public:
  static void SetUpTestCase(){
      content::is_compatible_type_setted = false;
  }
  static void TearDownTestCase() {}
  void SetUp() override {}
  void TearDown() override {
      ResetArkwebUserAgentExtStateForTest();
  }
};

TEST_F(ArkwebUserAgentExtTest, GetDistVersionNormalFormat) {
  base::ohos::SysInfoUtilsMock::mockOsVersion = true;
  auto &sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, OsVersion())
      .WillOnce(testing::Return("3.1.2"))
      .WillRepeatedly(testing::Return("3.1.2"));
  std::string version = GetDistVersion();
  EXPECT_EQ(version, "3.1");
  base::ohos::SysInfoUtilsMock::mockOsVersion = false;
}

TEST_F(ArkwebUserAgentExtTest, GetDistVersionOnePart) {
  base::ohos::SysInfoUtilsMock::mockOsVersion = true;
  EXPECT_CALL(base::ohos::SysInfoUtilsMock::GetInstance(), OsVersion())
      .WillOnce(testing::Return("xxx"))
      .WillRepeatedly(testing::Return("3.1.2"));
  std::string version = GetDistVersion();
  EXPECT_EQ(version, "");
  base::ohos::SysInfoUtilsMock::mockOsVersion = false;
}

TEST_F(ArkwebUserAgentExtTest, CompatibleTypeSetIsTrue) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  content::SetArkwebUserAgentExtStateForTest(true);
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).Times(0);
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
}

TEST_F(ArkwebUserAgentExtTest, MobileDevice) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(true));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::HasSubstr("Mobile"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = false;
}

// Test case for tablet device
TEST_F(ArkwebUserAgentExtTest, TabletDevice) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Tablet"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillOnce(testing::Return(true));
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillOnce(testing::Return(false));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::Not(testing::HasSubstr("Mobile")));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
}

// Test case for PC device
TEST_F(ArkwebUserAgentExtTest, PcDevice) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("PC"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillOnce(testing::Return(true));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::Not(testing::HasSubstr("Mobile")));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
}

// Test case for compatible mode: phone on tablet
TEST_F(ArkwebUserAgentExtTest, CompatiblePhoneOnTablet) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillOnce(testing::Return(true));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::HasSubstr("Mobile"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

// Test case for compatible mode: phone on PC
TEST_F(ArkwebUserAgentExtTest, CompatiblePhoneOnPc) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillOnce(testing::Return(true));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::HasSubstr("Mobile"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockIsMobileDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
}

TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest0) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  content::SetArkwebUserAgentExtStateForTest(true);
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).Times(0);
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::Not(testing::HasSubstr("PC")));
  EXPECT_THAT(result, testing::Not(testing::HasSubstr("Tablet")));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
}

// Test case for GetOhosFullname function
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest1) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
}

// Test case for GetOhosFullname function
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest2) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("PC"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("PC"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
}

// Test case for GetOhosFullname function
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest3) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Tablet"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Tablet"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
}

// Test case for GetOhosFullname function
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest4) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("unknown"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
}

// Test case for GetOhosFullname function with empty base OS name
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest5) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(-1));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("OpenHarmony"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
}

// Test case for GetOhosFullname function with empty base OS name
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest6) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(-1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("OpenHarmony"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
}

// Test case for GetOhosFullname function with empty base OS name
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest7) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return(""));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("OpenHarmony"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
}

TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest8) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  base::ohos::SysInfoUtilsMock::mockOsVersion = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  EXPECT_CALL(base::ohos::SysInfoUtilsMock::GetInstance(), OsVersion())
      .WillOnce(testing::Return("xxx"))
      .WillRepeatedly(testing::Return("3.1.2"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
  base::ohos::SysInfoUtilsMock::mockOsVersion = false;
}

TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest9) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  base::ohos::SysInfoUtilsMock::mockOsVersion = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  EXPECT_CALL(base::ohos::SysInfoUtilsMock::GetInstance(), OsVersion())
      .WillRepeatedly(testing::Return("3.1.2"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
  base::ohos::SysInfoUtilsMock::mockOsVersion = false;
}

TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest10) {
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = true;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = true;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = true;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = true;
  base::ohos::SysInfoUtilsMock::mockOsVersion = true;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  EXPECT_CALL(base::ohos::SysInfoUtilsMock::GetInstance(), OsVersion())
      .WillRepeatedly(testing::Return(""));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::Not(testing::HasSubstr("3.1.2")));
  base::ohos::SysInfoUtilsMock::mockCompatibleDeviceType = false;
  base::ohos::SysInfoUtilsMock::mockMajorVersion = false;
  base::ohos::SysInfoUtilsMock::mockSeniorVersion = false;
  base::ohos::SysInfoUtilsMock::mockBaseOsName = false;
  base::ohos::SysInfoUtilsMock::mockOsVersion = false;
}
#endif // BUILDFLAG(ARKWEB_USERAGENT)

} // namespace content