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

#include "ndk_version.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_system_properties_adapter.h"
#include "build/buildflag.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;
using ::testing::Return;
using ::testing::ReturnRef;

const int DEFAULT_API_VERSION = 60000;
const int TEST_API_VERSION_60000 = 60000;
const int TEST_API_VERSION_70000 = 70000;
const char* API_VERSION_STRING_60000 = "60000";
const char* API_VERSION_STRING_70000 = "70000";
const char* EMPTY_STRING = "";
const char* INVALID_STRING = "abc";
const char* OVERFLOW_STRING = "999999999999999999999999999999";
const char* UNDERFLOW_STRING = "-999999999999999999999999999999";

class NdkVersionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_helper_ = new MockOhosAdapterHelper();
    mock_system_properties_ = new MockSystemPropertiesAdapter();

    EXPECT_CALL(*mock_helper_, GetSystemPropertiesInstance())
        .WillRepeatedly(ReturnRef(*mock_system_properties_));

#if BUILDFLAG(ARKWEB_TEST)
    OhosAdapterHelper::SetInstance(mock_helper_);
#endif
  }

  void TearDown() override {
#if BUILDFLAG(ARKWEB_TEST)
    OhosAdapterHelper::SetInstance(nullptr);
#endif
    delete mock_system_properties_;
    delete mock_helper_;
    mock_system_properties_ = nullptr;
    mock_helper_ = nullptr;
  }

  MockOhosAdapterHelper* mock_helper_ = nullptr;
  MockSystemPropertiesAdapter* mock_system_properties_ = nullptr;
};

TEST_F(NdkVersionTest, GetParamApiVersion_Normal) {
  EXPECT_CALL(*mock_system_properties_, GetDeviceInfoApiVersion())
      .WillOnce(Return(API_VERSION_STRING_60000));

  int version = GetParamApiVersion();
  EXPECT_EQ(TEST_API_VERSION_60000, version);
}

TEST_F(NdkVersionTest, GetParamApiVersion_EmptyString) {
  EXPECT_CALL(*mock_system_properties_, GetDeviceInfoApiVersion())
      .WillOnce(Return(EMPTY_STRING));

  int version = GetParamApiVersion();
  EXPECT_EQ(DEFAULT_API_VERSION, version);
}

TEST_F(NdkVersionTest, GetParamApiVersion_InvalidString) {
  EXPECT_CALL(*mock_system_properties_, GetDeviceInfoApiVersion())
      .WillOnce(Return(INVALID_STRING));

  int version = GetParamApiVersion();
  EXPECT_EQ(DEFAULT_API_VERSION, version);
}

TEST_F(NdkVersionTest, GetParamApiVersion_Overflow) {
  EXPECT_CALL(*mock_system_properties_, GetDeviceInfoApiVersion())
      .WillOnce(Return(OVERFLOW_STRING));

  int version = GetParamApiVersion();
  EXPECT_EQ(DEFAULT_API_VERSION, version);
}

TEST_F(NdkVersionTest, GetParamApiVersion_Underflow) {
  EXPECT_CALL(*mock_system_properties_, GetDeviceInfoApiVersion())
      .WillOnce(Return(UNDERFLOW_STRING));

  int version = GetParamApiVersion();
  EXPECT_EQ(DEFAULT_API_VERSION, version);
}

TEST_F(NdkVersionTest, CheckTargetApiVersion_GreaterThan) {
  EXPECT_CALL(*mock_system_properties_, GetDeviceInfoApiVersion())
      .WillOnce(Return(API_VERSION_STRING_60000));

  bool result = CheckTargetApiVersion(TEST_API_VERSION_70000);
  EXPECT_TRUE(result);
}

TEST_F(NdkVersionTest, CheckTargetApiVersion_Equal) {
  EXPECT_CALL(*mock_system_properties_, GetDeviceInfoApiVersion())
      .WillOnce(Return(API_VERSION_STRING_60000));

  bool result = CheckTargetApiVersion(TEST_API_VERSION_60000);
  EXPECT_FALSE(result);
}

TEST_F(NdkVersionTest, CheckTargetApiVersion_LessThan) {
  EXPECT_CALL(*mock_system_properties_, GetDeviceInfoApiVersion())
      .WillOnce(Return(API_VERSION_STRING_70000));

  bool result = CheckTargetApiVersion(TEST_API_VERSION_60000);
  EXPECT_FALSE(result);
}