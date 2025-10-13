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
#include <gtest/gtest.h>

#include "interfaces/mock/mock_ohos_adapter_ndk_refs.h"
using namespace OHOS::NWeb;
using namespace OHOS::ArkWeb;
using namespace testing;

class OhosAdapterHelperDecoupleTest : public ::testing::Test {
 protected:
  void SetUp() override { mock_helper_ = new MockArkOhosAdapterHelper(); }

  void TearDown() override { delete mock_helper_; }

  MockArkOhosAdapterHelper* mock_helper_;
};

TEST_F(OhosAdapterHelperDecoupleTest, CreateAafwkAdapterWhenCtocppIsNull) {
  ArkOhosAdapterHelperWrapper wrapper(nullptr);
  auto result = wrapper.CreateAafwkAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateAafwkAdapterWhenAdapterIsNull) {
  EXPECT_CALL(*mock_helper_, CreateAafwkAdapter())
      .WillOnce(
          Return(ArkWebRefPtr<OHOS::ArkWeb::ArkAafwkAppMgrClientAdapter>()));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateAafwkAdapter();
  EXPECT_EQ(result, nullptr);
}
TEST_F(OhosAdapterHelperDecoupleTest, CreateAafwkAdapterSuccess) {
  auto mock_adapter = new MockArkAafwkAppMgrClientAdapter();
  EXPECT_CALL(*mock_helper_, CreateAafwkAdapter())
      .WillOnce(Return(static_cast<ArkWebRefPtr<ArkAafwkAppMgrClientAdapter>>(
          mock_adapter)));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateAafwkAdapter();
  EXPECT_NE(result, nullptr);
}
TEST_F(OhosAdapterHelperDecoupleTest, CreateDisplayMgrAdapterWhenCtocppIsNull) {
  ArkOhosAdapterHelperWrapper wrapper(nullptr);
  auto result = wrapper.CreateDisplayMgrAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest,
       CreateDisplayMgrAdapterWhenAdapterIsNull) {
  EXPECT_CALL(*mock_helper_, CreateDisplayMgrAdapter())
      .WillOnce(Return(ArkWebRefPtr<OHOS::ArkWeb::ArkDisplayManagerAdapter>()));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateDisplayMgrAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateDisplayMgrAdapterSuccess) {
  auto mock_display_manager = new MockArkDisplayManagerAdapter();

  EXPECT_CALL(*mock_helper_, CreateDisplayMgrAdapter())
      .WillOnce(Return(ArkWebRefPtr<ArkDisplayManagerAdapter>(
          static_cast<ArkDisplayManagerAdapter*>(mock_display_manager))));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateDisplayMgrAdapter();
  EXPECT_NE(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateMMIAdapterWhenCtocppIsNull) {
  ArkOhosAdapterHelperWrapper wrapper(nullptr);
  auto result = wrapper.CreateMMIAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateMMIAdapterWhenAdapterIsNull) {
  EXPECT_CALL(*mock_helper_, CreateMMIAdapter())
      .WillOnce(Return(ArkWebRefPtr<OHOS::ArkWeb::ArkMMIAdapter>()));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateMMIAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateMMIAdapterSuccess) {
  auto mock_mmi = new MockArkMMIAdapter();

  EXPECT_CALL(*mock_helper_, CreateMMIAdapter())
      .WillOnce(Return(
          ArkWebRefPtr<ArkMMIAdapter>(static_cast<ArkMMIAdapter*>(mock_mmi))));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateMMIAdapter();
  EXPECT_NE(result, nullptr);
}
TEST_F(OhosAdapterHelperDecoupleTest,
       CreateSocPerfClientAdapterWhenCtocppIsNull) {
  ArkOhosAdapterHelperWrapper wrapper(nullptr);
  auto result = wrapper.CreateSocPerfClientAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest,
       CreateSocPerfClientAdapterWhenAdapterIsNull) {
  EXPECT_CALL(*mock_helper_, CreateSocPerfClientAdapter())
      .WillOnce(Return(ArkWebRefPtr<OHOS::ArkWeb::ArkSocPerfClientAdapter>()));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateSocPerfClientAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateSocPerfClientAdapterSuccess) {
  auto mock_soc_perf = new MockArkSocPerfClientAdapter();

  EXPECT_CALL(*mock_helper_, CreateSocPerfClientAdapter())
      .WillOnce(Return(ArkWebRefPtr<ArkSocPerfClientAdapter>(
          static_cast<ArkSocPerfClientAdapter*>(mock_soc_perf))));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateSocPerfClientAdapter();
  EXPECT_NE(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateIMFAdapterWhenCtocppIsNull) {
  ArkOhosAdapterHelperWrapper wrapper(nullptr);
  auto result = wrapper.CreateIMFAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateIMFAdapterWhenAdapterIsNull) {
  EXPECT_CALL(*mock_helper_, CreateIMFAdapter())
      .WillOnce(Return(ArkWebRefPtr<OHOS::ArkWeb::ArkIMFAdapter>()));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateIMFAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateIMFAdapterSuccess) {
  auto mock_imf = new MockArkIMFAdapter();

  EXPECT_CALL(*mock_helper_, CreateIMFAdapter())
      .WillOnce(Return(
          ArkWebRefPtr<ArkIMFAdapter>(static_cast<ArkIMFAdapter*>(mock_imf))));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateIMFAdapter();
  EXPECT_NE(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreatePlayerAdapterWhenAdapterIsNull) {
  EXPECT_CALL(*mock_helper_, CreatePlayerAdapter())
      .WillOnce(Return(ArkWebRefPtr<OHOS::ArkWeb::ArkPlayerAdapter>()));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreatePlayerAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreatePlayerAdapterSuccess) {
  auto mock_player = new MockArkPlayerAdapter();

  EXPECT_CALL(*mock_helper_, CreatePlayerAdapter())
      .WillOnce(Return(ArkWebRefPtr<ArkPlayerAdapter>(
          static_cast<ArkPlayerAdapter*>(mock_player))));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreatePlayerAdapter();
  EXPECT_NE(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateFlowbufferAdapterWhenCtocppIsNull) {
  ArkOhosAdapterHelperWrapper wrapper(nullptr);
  auto result = wrapper.CreateFlowbufferAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest,
       CreateFlowbufferAdapterWhenAdapterIsNull) {
  EXPECT_CALL(*mock_helper_, CreateFlowbufferAdapter())
      .WillOnce(Return(ArkWebRefPtr<OHOS::ArkWeb::ArkFlowbufferAdapter>()));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateFlowbufferAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateFlowbufferAdapterSuccess) {
  auto mock_flowbuffer = new MockArkFlowbufferAdapter();

  EXPECT_CALL(*mock_helper_, CreateFlowbufferAdapter())
      .WillOnce(Return(ArkWebRefPtr<ArkFlowbufferAdapter>(
          static_cast<ArkFlowbufferAdapter*>(mock_flowbuffer))));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateFlowbufferAdapter();
  EXPECT_NE(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest,
       SetArkWebCoreHapPathOverrideWhenCtocppIsNull) {
  ArkOhosAdapterHelperWrapper wrapper(nullptr);
  EXPECT_NO_FATAL_FAILURE(wrapper.SetArkWebCoreHapPathOverride("test_path"));
}

TEST_F(OhosAdapterHelperDecoupleTest,
       CreateMigrationMgrAdapterWhenCtocppIsNull) {
  ArkOhosAdapterHelperWrapper wrapper(nullptr);
  auto result = wrapper.CreateMigrationMgrAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest,
       CreateMigrationMgrAdapterWhenAdapterIsNull) {
  EXPECT_CALL(*mock_helper_, CreateMigrationMgrAdapter())
      .WillOnce(
          Return(ArkWebRefPtr<OHOS::ArkWeb::ArkMigrationManagerAdapter>()));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateMigrationMgrAdapter();
  EXPECT_EQ(result, nullptr);
}

TEST_F(OhosAdapterHelperDecoupleTest, CreateMigrationMgrAdapterSuccess) {
  auto mock_migration = new MockArkMigrationManagerAdapter();

  EXPECT_CALL(*mock_helper_, CreateMigrationMgrAdapter())
      .WillOnce(Return(ArkWebRefPtr<ArkMigrationManagerAdapter>(
          static_cast<ArkMigrationManagerAdapter*>(mock_migration))));

  ArkOhosAdapterHelperWrapper wrapper(mock_helper_);
  auto result = wrapper.CreateMigrationMgrAdapter();
  EXPECT_NE(result, nullptr);
}