/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "mmi_device_info_adapter_impl.h"
#include <memory>
#include "mmi_adapter.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace OHOS::NWeb;

class MockMMIDeviceInfoAdapter : public OHOS::NWeb::MMIDeviceInfoAdapter {
 public:
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

TEST(MMIDeviceInfoAdapterTest, TransformToMMIDeviceInfoTest001) {
  auto adapter = std::shared_ptr<OHOS::NWeb::MMIDeviceInfoAdapter>();
  auto info = TransformToMMIDeviceInfo(adapter);
  EXPECT_EQ(info.id, -1);
  EXPECT_EQ(info.type, 0);
  EXPECT_EQ(info.bus, 0);
  EXPECT_EQ(info.version, 0);
  EXPECT_EQ(info.product, 0);
  EXPECT_EQ(info.vendor, 0);
  EXPECT_EQ(info.name.length(), 0);
  EXPECT_EQ(info.phys.length(), 0);
  EXPECT_EQ(info.uniq.length(), 0);
}

TEST(MMIDeviceInfoAdapterTest, TransformToMMIDeviceInfoTest002) {
  auto mockadapter = std::make_shared<MockMMIDeviceInfoAdapter>();
  EXPECT_CALL(*mockadapter, GetId()).WillOnce(testing::Return(1));
  EXPECT_CALL(*mockadapter, GetType()).WillOnce(testing::Return(2));
  EXPECT_CALL(*mockadapter, GetBus()).WillOnce(testing::Return(3));
  EXPECT_CALL(*mockadapter, GetVersion()).WillOnce(testing::Return(4));
  EXPECT_CALL(*mockadapter, GetProduct()).WillOnce(testing::Return(5));
  EXPECT_CALL(*mockadapter, GetVendor()).WillOnce(testing::Return(6));
  EXPECT_CALL(*mockadapter, GetName()).WillOnce(testing::Return("TestName"));
  EXPECT_CALL(*mockadapter, GetPhys()).WillOnce(testing::Return("TestPhys"));
  EXPECT_CALL(*mockadapter, GetUniq()).WillOnce(testing::Return("TestUniq"));
  auto info = TransformToMMIDeviceInfo(mockadapter);
  EXPECT_EQ(info.id, 1);
  EXPECT_EQ(info.type, 2);
  EXPECT_EQ(info.bus, 3);
  EXPECT_EQ(info.version, 4);
  EXPECT_EQ(info.product, 5);
  EXPECT_EQ(info.vendor, 6);
  EXPECT_EQ(info.name, "TestName");
  EXPECT_EQ(info.phys, "TestPhys");
  EXPECT_EQ(info.uniq, "TestUniq");
}
