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
#include "arkweb/chromium_ext/ohos_test/fuzztest/ohos_adapter_ndk/net_connect_adapter_fuzzer/net_connect_fuzz_mock.h"
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
    MOCK_METHOD(CommonEvent_ErrCode, OH_CommonEvent_UnSubscribe, (const CommonEvent_Subscriber *subscriber));
    MOCK_METHOD(const char *, OH_CommonEvent_GetEventFromRcvData, (const CommonEvent_RcvData *rcvData));
    MOCK_METHOD(const CommonEvent_Parameters *, OH_CommonEvent_GetParametersFromRcvData, (const CommonEvent_RcvData *rcvData));
    MOCK_METHOD(bool, OH_CommonEvent_HasKeyInParameters, (const CommonEvent_Parameters *para, const char *key));
    MOCK_METHOD(int, OH_CommonEvent_GetIntFromParameters, (const CommonEvent_Parameters *para, const char *key, int defaultValue));
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
  return MockCommonEventSupport::getInstance().OH_CommonEvent_DestroySubscribeInfo(info);
}

void __wrap_OH_CommonEvent_DestroySubscriber(CommonEvent_Subscriber* subscriber) {
  return MockCommonEventSupport::getInstance().OH_CommonEvent_DestroySubscriber(subscriber);
}

int32_t __wrap_OH_BatteryInfo_GetCapacity() {
  return MockCommonEventSupport::getInstance().OH_BatteryInfo_GetCapacity();
}

BatteryInfo_BatteryPluggedType __wrap_OH_BatteryInfo_GetPluggedType() {
  return MockCommonEventSupport::getInstance().OH_BatteryInfo_GetPluggedType();
}

CommonEvent_ErrCode __wrap_OH_CommonEvent_UnSubscribe(const CommonEvent_Subscriber *subscriber) {
  return MockCommonEventSupport::getInstance().OH_CommonEvent_UnSubscribe(subscriber);
}

const char * __wrap_OH_CommonEvent_GetEventFromRcvData(const CommonEvent_RcvData *rcvData) {
  return MockCommonEventSupport::getInstance().OH_CommonEvent_GetEventFromRcvData(rcvData);
}

const CommonEvent_Parameters * __wrap_OH_CommonEvent_GetParametersFromRcvData(const CommonEvent_RcvData *rcvData) {
  return MockCommonEventSupport::getInstance().OH_CommonEvent_GetParametersFromRcvData(rcvData);
}

bool __wrap_OH_CommonEvent_HasKeyInParameters(const CommonEvent_Parameters *para, const char *key) {
  return MockCommonEventSupport::getInstance().OH_CommonEvent_HasKeyInParameters(para, key);
}

int __wrap_OH_CommonEvent_GetIntFromParameters(const CommonEvent_Parameters *para, const char *key, int defaultValue) {
  return MockCommonEventSupport::getInstance().OH_CommonEvent_GetIntFromParameters(para, key, defaultValue);
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
    CommonEvent_RcvData data_{
      .event = "dummy_event",
      .bundleName = "test.bundle",
      .code = 0,
      .data = "{}",
      .parameters = nullptr 
  };
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

TEST_F(BatteryMgrClientAdapterImplTest, StartListen_Fail_OH_CommonEvent_Subscribe) {
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

TEST_F(BatteryMgrClientAdapterImplTest, RequestBatteryInfo_ChargingScenario) {
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_BatteryInfo_GetCapacity())
    .WillOnce(Return(85));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_BatteryInfo_GetPluggedType())
    .WillOnce(Return(PLUGGED_TYPE_AC));
  
  auto batteryInfo = adapter_->RequestBatteryInfo();

  EXPECT_NEAR(batteryInfo.get()->GetLevel(), 0.85, 0.001);
  EXPECT_TRUE(batteryInfo.get()->IsCharging());
}

TEST_F(BatteryMgrClientAdapterImplTest, RequestBatteryInfo_ChargingScenario2) {
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_BatteryInfo_GetCapacity())
    .WillOnce(Return(85));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_BatteryInfo_GetPluggedType())
    .WillOnce(Return(PLUGGED_TYPE_NONE));
  
  auto batteryInfo = adapter_->RequestBatteryInfo();

  EXPECT_NEAR(batteryInfo.get()->GetLevel(), 0.85, 0.001);
  EXPECT_FALSE(batteryInfo.get()->IsCharging());
}

TEST_F(BatteryMgrClientAdapterImplTest, RequestBatteryInfo_ChargingScenario3) {
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_BatteryInfo_GetCapacity())
    .WillOnce(Return(85));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_BatteryInfo_GetPluggedType())
    .WillOnce(Return(PLUGGED_TYPE_BUTT));
  
  auto batteryInfo = adapter_->RequestBatteryInfo();

  EXPECT_NEAR(batteryInfo.get()->GetLevel(), 0.85, 0.001);
  EXPECT_FALSE(batteryInfo.get()->IsCharging());
}

TEST_F(BatteryMgrClientAdapterImplTest, StopListen_NoActionTaken) {
  adapter_->callbackSet_.clear();
  adapter_->commonEventSubscriber_ = reinterpret_cast<void*>(0x1234);
  adapter_->StopListen();
  ASSERT_NE(adapter_->commonEventSubscriber_, nullptr);
  ASSERT_TRUE(adapter_->callbackSet_.empty());
}


TEST_F(BatteryMgrClientAdapterImplTest, StopListen_WithCallbackButNotStarted) {
  auto anotherCb = std::make_shared<MockWebBatteryEventCallback>();
  adapter_->callbackSet_.insert(adapter_->cb_);
  adapter_->callbackSet_.insert(anotherCb);
  adapter_->commonEventSubscriber_ = reinterpret_cast<void*>(0x1234);
  adapter_->StopListen();
  ASSERT_NE(adapter_->commonEventSubscriber_, nullptr);
  ASSERT_FALSE(adapter_->callbackSet_.empty());
}


TEST_F(BatteryMgrClientAdapterImplTest, StopListen_CommonEventSubscriber_IsNull) {
  adapter_->callbackSet_.insert(adapter_->cb_);
  adapter_->commonEventSubscriber_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter_->StopListen());
}

TEST_F(BatteryMgrClientAdapterImplTest, StopListen_UnSubscribe_Success) {
  adapter_->callbackSet_.insert(adapter_->cb_);
  adapter_->commonEventSubscriber_ = reinterpret_cast<CommonEvent_Subscriber*>(0x1234);
  adapter_->commonEventSubscriberInfo_ = reinterpret_cast<CommonEvent_SubscribeInfo*>(0x5678);

  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_UnSubscribe(_))
    .WillOnce(Return(COMMONEVENT_ERR_PERMISSION_ERROR));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_DestroySubscriber(_));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_DestroySubscribeInfo(_));
  
  adapter_->StopListen();
  
  ASSERT_EQ(adapter_->commonEventSubscriber_, nullptr);
  ASSERT_EQ(adapter_->commonEventSubscriberInfo_, nullptr);
}


TEST_F(BatteryMgrClientAdapterImplTest, StopListen_Success) {  
  adapter_->callbackSet_.insert(adapter_->cb_);
  adapter_->commonEventSubscriber_ = reinterpret_cast<CommonEvent_Subscriber*>(0x1234);
  adapter_->commonEventSubscriberInfo_ = reinterpret_cast<CommonEvent_SubscribeInfo*>(0x5678);

  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_UnSubscribe(_))
    .WillOnce(Return(COMMONEVENT_ERR_OK));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_DestroySubscriber(_));
  EXPECT_CALL(MockCommonEventSupport::getInstance(), OH_CommonEvent_DestroySubscribeInfo(_));
  
  adapter_->StopListen();
  
  ASSERT_EQ(adapter_->commonEventSubscriber_, nullptr);
  ASSERT_EQ(adapter_->commonEventSubscriberInfo_, nullptr);
}

TEST_F(BatteryMgrClientAdapterImplTest, OnBatteryEvent_Datanullptr) {
  ASSERT_NO_FATAL_FAILURE(adapter_->OnBatteryEvent(nullptr));
}
