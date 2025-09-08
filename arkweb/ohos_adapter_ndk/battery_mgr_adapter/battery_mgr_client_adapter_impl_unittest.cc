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
#include <gmock/gmock.h>
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include <BasicServicesKit/oh_commonevent_support.h>
#include <BasicServicesKit/ohbattery_info.h>
#include "base/logging.h"
#define private public
#include "battery_mgr_client_adapter_impl.h"
#undef private

using namespace testing;
using namespace OHOS::NWeb;

class MockWebBatteryEventCallback : public WebBatteryEventCallback {
  public:
    MOCK_METHOD(void, BatteryInfoChanged, (std::shared_ptr<WebBatteryInfo>), (override));
};

class MockCommonEventSupport {
  public:
    static MockCommonEventSupport& getInstance() {
      static MockCommonEventSupport instance;
      return instance;
    }
    
    MOCK_METHOD(CommonEvent_SubscribeInfo*, OH_CommonEvent_CreateSubscribeInfo, (const char* events[], int count));
    MOCK_METHOD(CommonEvent_Subscriber*, OH_CommonEvent_CreateSubscriber, (CommonEvent_SubscribeInfo* info, CommonEvent_ReceiveCallback callback));
    MOCK_METHOD(CommonEvent_ErrCode, OH_CommonEvent_Subscribe, (CommonEvent_Subscriber* subscriber));
    MOCK_METHOD(void, OH_CommonEvent_DestroySubscribeInfo, (CommonEvent_SubscribeInfo* info));
    MOCK_METHOD(void, OH_CommonEvent_DestroySubscriber, (CommonEvent_Subscriber* subscriber));
    MOCK_METHOD(int32_t, OH_BatteryInfo_GetCapacity, ());
    MOCK_METHOD(BatteryInfo_BatteryPluggedType, OH_BatteryInfo_GetPluggedType, ());
};

#ifdef __cplusplus
extern "C" {
#endif
CommonEvent_SubscribeInfo* __wrap_OH_CommonEvent_CreateSubscribeInfo(const char* events[], int count) {
  return MockCommonEventSupport::getInstance().OH_CommonEvent_CreateSubscribeInfo(events, count);
}

CommonEvent_Subscriber* __wrap_OH_CommonEvent_CreateSubscriber(CommonEvent_SubscribeInfo* info, CommonEvent_ReceiveCallback callback) {
  return MockCommonEventSupport::getInstance().OH_CommonEvent_CreateSubscriber(info, callback);
}

CommonEvent_ErrCode __wrap_OH_CommonEvent_Subscribe(CommonEvent_Subscriber* subscriber) {
  return MockCommonEventSupport::getInstance().OH_CommonEvent_Subscribe(subscriber);
}

void __wrap_OH_CommonEvent_DestroySubscribeInfo(CommonEvent_SubscribeInfo* info) {
  MockCommonEventSupport::getInstance().OH_CommonEvent_DestroySubscribeInfo(info);
}

void __wrap_OH_CommonEvent_DestroySubscriber(CommonEvent_Subscriber* subscriber) {
  MockCommonEventSupport::getInstance().OH_CommonEvent_DestroySubscriber(subscriber);
}

int32_t __wrap_OH_BatteryInfo_GetCapacity() {
  MockCommonEventSupport::getInstance().OH_BatteryInfo_GetCapacity();
}

BatteryInfo_BatteryPluggedType __wrap_OH_BatteryInfo_GetPluggedType() {
  MockCommonEventSupport::getInstance().OH_BatteryInfo_GetPluggedType();
}
#ifdef __cplusplus
}
#endif

class BatteryMgrClientAdapterImplTest : public ::testing::Test {
  protected:
    void SetUp() override {
      adapter_ = std::make_shared<BatteryMgrClientAdapterImpl>();
      batteryInfo_ = std::make_shared<WebBatteryInfoImpl>(50.0, true, 60, 30);
    }

    std::shared_ptr<BatteryMgrClientAdapterImpl> adapter_;
    std::shared_ptr<WebBatteryInfoImpl> batteryInfo_;

};

TEST_F(BatteryMgrClientAdapterImplTest, RegBatteryEvent_ValidCallback) {
  auto mockCallback = std::make_shared<MockWebBatteryEventCallback>();
  adapter_->RegBatteryEvent(mockCallback);
  EXPECT_EQ(mockCallback, adapter_->cb_);
}

TEST_F(BatteryMgrClientAdapterImplTest, RegBatteryEvent_NullCallback) {
  std::shared_ptr<WebBatteryEventCallback> actualCallback = adapter_->cb_;
  adapter_->RegBatteryEvent(nullptr);
  EXPECT_EQ(actualCallback, adapter_->cb_);
}


TEST_F(BatteryMgrClientAdapterImplTest, StartListen_Fail_CreateSubscribeInfoFails) {
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_CreateSubscribeInfo(_, _))
    .WillOnce(Return(nullptr));

  auto callback = std::make_shared<MockWebBatteryEventCallback>();
  adapter_->RegBatteryEvent(callback);

  bool result = adapter_->StartListen();
  EXPECT_FALSE(result);
}

TEST_F(BatteryMgrClientAdapterImplTest, StartListen_Fail_OH_CommonEvent_CreateSubscriber) {
  adapter_->commonEventSubscriber_ = nullptr;
  auto* mockInfo = reinterpret_cast<CommonEvent_SubscribeInfo*>(0x1234);
  auto* mockSubscriber = reinterpret_cast<CommonEvent_Subscriber*>(0x5678);
  
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_CreateSubscribeInfo(_, _))
    .WillOnce(Return(mockInfo));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_CreateSubscriber(mockInfo, _))
    .WillOnce(Return(nullptr));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_DestroySubscribeInfo(mockInfo));

  auto callback = std::make_shared<MockWebBatteryEventCallback>();
  adapter_->RegBatteryEvent(callback);
  bool result = adapter_->StartListen();
  EXPECT_FALSE(result);
}

TEST_F(BatteryMgrClientAdapterImplTest, StartListen_Success_OH_CommonEvent_Subscribe) {
  adapter_->commonEventSubscriber_ = nullptr;
  auto* mockInfo = reinterpret_cast<CommonEvent_SubscribeInfo*>(0x1234);
  auto* mockSubscriber = reinterpret_cast<CommonEvent_Subscriber*>(0x5678);
    
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_CreateSubscribeInfo(_, _))
    .WillOnce(Return(mockInfo));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_CreateSubscriber(mockInfo, _))
    .WillOnce(Return(mockSubscriber));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_Subscribe(mockSubscriber))
    .WillOnce(Return(COMMONEVENT_ERR_INVALID_PARAMETER));

  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_DestroySubscribeInfo(mockInfo));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_DestroySubscriber(mockSubscriber));
  auto callback = std::make_shared<MockWebBatteryEventCallback>();
  adapter_->RegBatteryEvent(callback);
  bool result = adapter_->StartListen();
  EXPECT_FALSE(result);
}
