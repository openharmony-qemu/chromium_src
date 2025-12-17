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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"
#include "arkweb/ohos_adapter_ndk/multimodalinputnew_adapter/mmi_new_adapter_impl.h"

using namespace testing;
using namespace OHOS::NWeb;
using namespace MockNdkApi;

class MMINewAdapterImplTest : public ::testing::Test {};

namespace OHOS::NWeb {
void OnDeviceAdded(int32_t deviceId);
void OnDeviceRemoved(int32_t deviceId);
}

class MockMMIListenerAdapter : public MMIListenerAdapter {
 public:
  MOCK_METHOD(void, OnDeviceAdded, (int32_t deviceId, const std::string& type), (override));
  MOCK_METHOD(void, OnDeviceRemoved, (int32_t deviceId, const std::string& type), (override));
};

class MockMMIDeviceInfoAdapter : public MMIDeviceInfoAdapter {
  MOCK_METHOD(int32_t, GetId, (), (override));
  MOCK_METHOD(int32_t, GetType, (), (override));
  MOCK_METHOD(int32_t, GetBus, (), (override));
  MOCK_METHOD(int32_t, GetVersion, (), (override));
  MOCK_METHOD(int32_t, GetProduct, (), (override));
  MOCK_METHOD(int32_t, GetVendor, (), (override));
  MOCK_METHOD(std::string, GetName, (), (override));
  MOCK_METHOD(std::string, GetPhys, (), (override));
  MOCK_METHOD(std::string, GetUniq, (), (override));
  MOCK_METHOD(void, SetId, (int32_t id), (override));
  MOCK_METHOD(void, SetType, (int32_t type), (override));
  MOCK_METHOD(void, SetBus, (int32_t bus), (override));
  MOCK_METHOD(void, SetVersion, (int32_t version), (override));
  MOCK_METHOD(void, SetProduct, (int32_t product), (override));
  MOCK_METHOD(void, SetVendor, (int32_t vendor), (override));
  MOCK_METHOD(void, SetName, (std::string name), (override));
  MOCK_METHOD(void, SetPhys, (std::string phys), (override));
  MOCK_METHOD(void, SetUniq, (std::string uniq), (override));
};

/**
 * @tc.name: MMINewAdapterImplTest_OnDeviceAdded_001.
 * @tc.desc: test of OnDeviceAdded in MMINewAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(MMINewAdapterImplTest, MMINewAdapterImplTest_OnDeviceAdded_001) {
  int32_t deviceId = 0;
  OnDeviceAdded(deviceId);
  OnDeviceRemoved(deviceId);
  std::string type;
  auto listener = std::make_shared<MockMMIListenerAdapter>();
  MMINewAdapterImpl::GetInstance().RegisterDevListener(type, listener);
  bool callback_called = false;
  EXPECT_CALL(*listener, OnDeviceAdded(::testing::_, ::testing::_))
      .WillRepeatedly(::testing::Assign(&callback_called, true));
  OnDeviceAdded(deviceId);
  EXPECT_EQ(callback_called, true);
}

/**
 * @tc.name: MMINewAdapterImplTest_OnDeviceRemoved_001.
 * @tc.desc: test of OnDeviceRemoved in MMINewAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(MMINewAdapterImplTest, MMINewAdapterImplTest_OnDeviceRemoved_001) {
  std::string type;
  auto listener = std::make_shared<MockMMIListenerAdapter>();
  MMINewAdapterImpl::GetInstance().RegisterDevListener(type, listener);
  int32_t deviceId = 0;
  bool callback_called = false;
  EXPECT_CALL(*listener, OnDeviceRemoved(::testing::_, ::testing::_))
      .WillRepeatedly(::testing::Assign(&callback_called, true));
  OnDeviceRemoved(deviceId);
  EXPECT_EQ(callback_called, true);
}

/**
 * @tc.name: MMINewAdapterImplTest_RegisterDevListener_001.
 * @tc.desc: test of RegisterDevListener in MMINewAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(MMINewAdapterImplTest, MMINewAdapterImplTest_RegisterDevListener_001) {
  std::string type;
  std::shared_ptr<MockMMIListenerAdapter> listener = nullptr;
  auto result = MMINewAdapterImpl::GetInstance().RegisterDevListener(type, listener);
  EXPECT_EQ(result, -1);

  listener = std::make_shared<MockMMIListenerAdapter>();
  result = MMINewAdapterImpl::GetInstance().RegisterDevListener(type, listener);
  EXPECT_EQ(result, 0);
}

/**
 * @tc.name: MMINewAdapterImplTest_UnregisterDevListener_001.
 * @tc.desc: test of UnregisterDevListener in MMINewAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(MMINewAdapterImplTest, MMINewAdapterImplTest_UnregisterDevListener_001) {
  std::string type = MMINewAdapterImpl::GetInstance().GetType();
  auto result = MMINewAdapterImpl::GetInstance().UnregisterDevListener(type);
  EXPECT_EQ(result, 0);

  type = "test";
  result = MMINewAdapterImpl::GetInstance().UnregisterDevListener(type);
  EXPECT_EQ(result, -1);
}

/**
 * @tc.name: MMINewAdapterImplTest_GetDeviceIds_001.
 * @tc.desc: test of GetDeviceIds in MMINewAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(MMINewAdapterImplTest, MMINewAdapterImplTest_GetDeviceIds_001) {
  std::vector<int32_t> ids;

  g_mock_OH_Input_GetDeviceIds = [](int32_t *deviceIds, int32_t inSize, int32_t *outSize) {
    return INPUT_PERMISSION_DENIED;
  };
  auto result = MMINewAdapterImpl::GetInstance().GetDeviceIds(ids);
  EXPECT_EQ(result, -1);

  g_mock_OH_Input_GetDeviceIds = [](int32_t *deviceIds, int32_t inSize, int32_t *outSize) {
    *outSize = 0;
    return INPUT_SUCCESS;
  };
  result = MMINewAdapterImpl::GetInstance().GetDeviceIds(ids);
  EXPECT_EQ(result, 0);

  g_mock_OH_Input_GetDeviceIds = nullptr;
  result = MMINewAdapterImpl::GetInstance().GetDeviceIds(ids);
  EXPECT_EQ(result, 0);
}

/**
 * @tc.name: MMINewAdapterImplTest_GetDeviceInfo_001.
 * @tc.desc: test of GetDeviceInfo in MMINewAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(MMINewAdapterImplTest, MMINewAdapterImplTest_GetDeviceInfo_001) {
  int32_t deviceId = 0;
  std::shared_ptr<MockMMIDeviceInfoAdapter> info = nullptr;
  auto result = MMINewAdapterImpl::GetInstance().GetDeviceInfo(deviceId, info);
  EXPECT_EQ(result, -1);

  info = std::make_shared<testing::NiceMock<MockMMIDeviceInfoAdapter>>();
  g_mock_OH_Input_CreateDeviceInfo = []() {
    return nullptr;
  };
  result = MMINewAdapterImpl::GetInstance().GetDeviceInfo(deviceId, info);
  EXPECT_EQ(result, -1);

  g_mock_OH_Input_CreateDeviceInfo = nullptr;
  result = MMINewAdapterImpl::GetInstance().GetDeviceInfo(deviceId, info);
  EXPECT_EQ(result, 0);
}
