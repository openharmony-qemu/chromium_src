// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "arkweb/chromium_ext/content/common/arkweb_user_agent_ext.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "testing/gtest/include/gtest/gtest.h"
#include "base/command_line.h"
#include "base/ohos/sys_info_utils_ext.h"

namespace base::ohos {
class SysInfoUtilsMock{
public:
  static SysInfoUtilsMock& GetInstance() {
      static SysInfoUtilsMock instance;
      return instance;
  }
  MOCK_METHOD(std::string, OsVersion, (), (const));
  MOCK_METHOD(std::string, CompatibleDeviceType, (), (const));
  MOCK_METHOD(bool, IsMobileDevice, (), (const));
  MOCK_METHOD(bool, IsTabletDevice, (), (const));
  MOCK_METHOD(bool, IsPcDevice, (), (const));
  MOCK_METHOD(std::string, BaseOsName, (), (const));
  MOCK_METHOD(int32_t, MajorVersion, (), (const));
  MOCK_METHOD(int32_t, SeniorVersion, (), (const));
};

#ifdef __cplusplus
extern "C" {
#endif
std::string __wrap_OsVersion() {
    return SysInfoUtilsMock::GetInstance().OsVersion();
}
std::string __wrap_CompatibleDeviceType() {
    return SysInfoUtilsMock::GetInstance().CompatibleDeviceType();
}
bool __wrap_IsMobileDevice() {
    return SysInfoUtilsMock::GetInstance().IsMobileDevice();
}
bool __wrap_IsTabletDevice() {
    return SysInfoUtilsMock::GetInstance().IsTabletDevice();
}
bool __wrap_IsPcDevice() {
    return SysInfoUtilsMock::GetInstance().IsPcDevice();
}
std::string __wrap_BaseOsName() {
    return SysInfoUtilsMock::GetInstance().BaseOsName();
}
int32_t __wrap_MajorVersion() {
    return SysInfoUtilsMock::GetInstance().MajorVersion();
}
int32_t __wrap_SeniorVersion() {
    return SysInfoUtilsMock::GetInstance().SeniorVersion();
}
#ifdef __cplusplus
}
#endif
} // namespace base::ohos
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
      //content::is_compatible_type_setted.store(false);
      ResetArkwebUserAgentExtStateForTest();
  }
};

TEST_F(ArkwebUserAgentExtTest, GetDistVersionNormalFormat) {
  auto &sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, OsVersion())
      .WillOnce(testing::Return("3.1.2"))
      .WillRepeatedly(testing::Return("3.1.2"));
  std::string version = GetDistVersion();
  EXPECT_EQ(version, "3.1");
}

TEST_F(ArkwebUserAgentExtTest, GetDistVersionOnePart) {
  EXPECT_CALL(base::ohos::SysInfoUtilsMock::GetInstance(), OsVersion())
      .WillOnce(testing::Return("xxx"))
      .WillRepeatedly(testing::Return("3.1.2"));
  std::string version = GetDistVersion();
  EXPECT_EQ(version, "");
}

TEST_F(ArkwebUserAgentExtTest, CompatibleTypeSetIsTrue) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  content::SetArkwebUserAgentExtStateForTest(true);
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).Times(0);
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
}

TEST_F(ArkwebUserAgentExtTest, MobileDevice) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(true));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::HasSubstr("Mobile"));
}

// Test case for tablet device
TEST_F(ArkwebUserAgentExtTest, TabletDevice) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Tablet"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillOnce(testing::Return(true));
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillOnce(testing::Return(false));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::Not(testing::HasSubstr("Mobile")));
}

// Test case for PC device
TEST_F(ArkwebUserAgentExtTest, PcDevice) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("PC"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillOnce(testing::Return(true));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::Not(testing::HasSubstr("Mobile")));
}

// Test case for compatible mode: phone on tablet
TEST_F(ArkwebUserAgentExtTest, CompatiblePhoneOnTablet) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillOnce(testing::Return(true));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::HasSubstr("Mobile"));
}

// Test case for compatible mode: phone on PC
TEST_F(ArkwebUserAgentExtTest, CompatiblePhoneOnPc) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, IsMobileDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillOnce(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillOnce(testing::Return(true));
  std::string user_agent = "Mozilla/5.0 ";
  SetProductString(user_agent);
  EXPECT_THAT(user_agent, testing::HasSubstr("ArkWeb/"));
  EXPECT_THAT(user_agent, testing::HasSubstr("Mobile"));
}

TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest0) {
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
}

// Test case for GetOhosFullname function
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest1) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
}

// Test case for GetOhosFullname function
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest2) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("PC"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("PC"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
}

// Test case for GetOhosFullname function
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest3) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Tablet"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Tablet"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
}

// Test case for GetOhosFullname function
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest4) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("unknown"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("HarmonyOS"));
  EXPECT_THAT(result, testing::HasSubstr("3.1"));
}

// Test case for GetOhosFullname function with empty base OS name
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest5) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(-1));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("OpenHarmony"));
}

// Test case for GetOhosFullname function with empty base OS name
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest6) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(-1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return("HarmonyOS"));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("OpenHarmony"));
}

// Test case for GetOhosFullname function with empty base OS name
TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest7) {
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, CompatibleDeviceType()).WillOnce(testing::Return("Phone"));
  EXPECT_CALL(sys_info_utils_mock, MajorVersion()).WillOnce(testing::Return(3));
  EXPECT_CALL(sys_info_utils_mock, SeniorVersion()).WillOnce(testing::Return(1));
  EXPECT_CALL(sys_info_utils_mock, BaseOsName()).WillRepeatedly(testing::Return(""));
  std::string result = GetOhosFullname();
  EXPECT_THAT(result, testing::HasSubstr("Phone"));
  EXPECT_THAT(result, testing::HasSubstr("OpenHarmony"));
}

TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest8) {
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
}

TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest9) {
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
}

TEST_F(ArkwebUserAgentExtTest, GetOhosFullnameTest10) {
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
}

#endif // BUILDFLAG(ARKWEB_USERAGENT)

} // namespace content