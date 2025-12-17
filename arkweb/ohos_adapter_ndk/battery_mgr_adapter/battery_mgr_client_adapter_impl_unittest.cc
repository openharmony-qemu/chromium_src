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
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"
#include "arkweb/chromium_ext/ohos_test/fuzztest/ohos_adapter_ndk/net_connect_adapter_fuzzer/net_connect_fuzz_mock.h"
#define private public
#include "battery_mgr_client_adapter_impl.h"
#undef private

using namespace testing;
using namespace OHOS::NWeb;
using namespace MockNdkApi;

class MockWebBatteryEventCallback : public WebBatteryEventCallback {
  public:
    MOCK_METHOD(void, BatteryInfoChanged, (std::shared_ptr<WebBatteryInfo>), (override));
};

class BatteryMgrClientAdapterImplTest : public ::testing::Test {
  protected:
    void SetType(bool type) {
      MockNetCommonEventSupport::bGetCapacity = type;
      MockNetCommonEventSupport::bGetPluggedType = type;
    }
    void SetUp() override {
      adapter_ = std::make_shared<BatteryMgrClientAdapterImpl>();
      batteryInfo_ = std::make_shared<WebBatteryInfoImpl>(50.0, true, 60, 30);
    }

    void TearDown() override {
      g_mock_OH_CommonEvent_CreateSubscribeInfo = nullptr;
      g_mock_OH_CommonEvent_CreateSubscriber = nullptr;
      g_mock_OH_CommonEvent_Subscribe = nullptr;
      g_mock_OH_CommonEvent_UnSubscribe = nullptr;
      g_mock_OH_CommonEvent_GetEventFromRcvData = nullptr;
      g_mock_OH_CommonEvent_DestroySubscribeInfo = nullptr;
      g_mock_OH_CommonEvent_DestroySubscriber = nullptr;
      SetType(false);
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
  g_mock_OH_CommonEvent_CreateSubscribeInfo = [](const char* [], int32_t) {
    return nullptr;
  };

  auto callback = std::make_shared<MockWebBatteryEventCallback>();
  adapter_->RegBatteryEvent(callback);

  bool result = adapter_->StartListen();
  EXPECT_FALSE(result);
  g_mock_OH_CommonEvent_CreateSubscribeInfo = nullptr;
}

TEST_F(BatteryMgrClientAdapterImplTest, StartListen_Fail_OH_CommonEvent_CreateSubscriber) {
  adapter_->commonEventSubscriber_ = nullptr;
  auto* mockInfo = reinterpret_cast<CommonEvent_SubscribeInfo*>(0x1234);
  auto* mockSubscriber = reinterpret_cast<CommonEvent_Subscriber*>(0x5678);

  g_mock_OH_CommonEvent_CreateSubscribeInfo = [&mockInfo](const char* [], int32_t) {
    return mockInfo;
  };
  g_mock_OH_CommonEvent_CreateSubscriber = [](const CommonEvent_SubscribeInfo* info,
      CommonEvent_ReceiveCallback callback) {
    return nullptr;
  };

  g_mock_OH_CommonEvent_DestroySubscribeInfo = [](CommonEvent_SubscribeInfo* info) {};

  auto callback = std::make_shared<MockWebBatteryEventCallback>();
  adapter_->RegBatteryEvent(callback);
  bool result = adapter_->StartListen();
  EXPECT_FALSE(result);
  g_mock_OH_CommonEvent_DestroySubscribeInfo = nullptr;
}

TEST_F(BatteryMgrClientAdapterImplTest, StartListen_Fail_OH_CommonEvent_Subscribe) {
  adapter_->commonEventSubscriber_ = nullptr;
  auto* mockInfo = reinterpret_cast<CommonEvent_SubscribeInfo*>(0x1234);
  auto* mockSubscriber = reinterpret_cast<CommonEvent_Subscriber*>(0x5678);
  g_mock_OH_CommonEvent_CreateSubscribeInfo = [&mockInfo](const char* [], int32_t) {
    return mockInfo;
  };
  g_mock_OH_CommonEvent_CreateSubscriber = [&mockSubscriber](const CommonEvent_SubscribeInfo* info,
      CommonEvent_ReceiveCallback callback) {
    return mockSubscriber;
  };
  g_mock_OH_CommonEvent_Subscribe = [](const CommonEvent_Subscriber* subscriber) {
    return COMMONEVENT_ERR_INVALID_PARAMETER;
  };

  g_mock_OH_CommonEvent_DestroySubscribeInfo = [](CommonEvent_SubscribeInfo* info) {};
  g_mock_OH_CommonEvent_DestroySubscriber = [](CommonEvent_Subscriber* subscriber) {};
  auto callback = std::make_shared<MockWebBatteryEventCallback>();
  adapter_->RegBatteryEvent(callback);
  bool result = adapter_->StartListen();
  EXPECT_FALSE(result);

  g_mock_OH_CommonEvent_DestroySubscribeInfo = nullptr;
  g_mock_OH_CommonEvent_DestroySubscriber = nullptr;
}

TEST_F(BatteryMgrClientAdapterImplTest, RequestBatteryInfo_ChargingScenario) {
  SetType(true);
  EXPECT_CALL(MockNetCommonEventSupport::GetInstance(), OH_BatteryInfo_GetCapacity())
    .WillOnce(Return(85));
  EXPECT_CALL(MockNetCommonEventSupport::GetInstance(), OH_BatteryInfo_GetPluggedType())
    .WillOnce(Return(PLUGGED_TYPE_AC));
  
  auto batteryInfo = adapter_->RequestBatteryInfo();

  EXPECT_NEAR(batteryInfo.get()->GetLevel(), 0.85, 0.001);
  EXPECT_TRUE(batteryInfo.get()->IsCharging());
}

TEST_F(BatteryMgrClientAdapterImplTest, RequestBatteryInfo_ChargingScenario2) {
  SetType(true);
  EXPECT_CALL(MockNetCommonEventSupport::GetInstance(), OH_BatteryInfo_GetCapacity())
    .WillOnce(Return(85));
  EXPECT_CALL(MockNetCommonEventSupport::GetInstance(), OH_BatteryInfo_GetPluggedType())
    .WillOnce(Return(PLUGGED_TYPE_NONE));
  
  auto batteryInfo = adapter_->RequestBatteryInfo();

  EXPECT_NEAR(batteryInfo.get()->GetLevel(), 0.85, 0.001);
  EXPECT_FALSE(batteryInfo.get()->IsCharging());
}

TEST_F(BatteryMgrClientAdapterImplTest, RequestBatteryInfo_ChargingScenario3) {
  SetType(true);
  EXPECT_CALL(MockNetCommonEventSupport::GetInstance(), OH_BatteryInfo_GetCapacity())
    .WillOnce(Return(85));
  EXPECT_CALL(MockNetCommonEventSupport::GetInstance(), OH_BatteryInfo_GetPluggedType())
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
  adapter_->callbackSet_.clear();
  adapter_->callbackSet_.insert(adapter_->cb_);
  adapter_->callbackSet_.insert(anotherCb);
  adapter_->commonEventSubscriber_ = reinterpret_cast<void*>(0x1234);
  adapter_->StopListen();
  ASSERT_NE(adapter_->commonEventSubscriber_, nullptr);
  ASSERT_FALSE(adapter_->callbackSet_.empty());
}


TEST_F(BatteryMgrClientAdapterImplTest, StopListen_CommonEventSubscriber_IsNull) {
  adapter_->callbackSet_.clear();
  adapter_->callbackSet_.insert(adapter_->cb_);
  adapter_->commonEventSubscriber_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter_->StopListen());
}

TEST_F(BatteryMgrClientAdapterImplTest, StopListen_UnSubscribe_Success) {
  adapter_->callbackSet_.clear();
  adapter_->callbackSet_.insert(adapter_->cb_);
  adapter_->commonEventSubscriber_ = reinterpret_cast<CommonEvent_Subscriber*>(0x1234);
  adapter_->commonEventSubscriberInfo_ = reinterpret_cast<CommonEvent_SubscribeInfo*>(0x5678);

  g_mock_OH_CommonEvent_UnSubscribe = [](const CommonEvent_Subscriber*) {
    return COMMONEVENT_ERR_PERMISSION_ERROR;
  };

  g_mock_OH_CommonEvent_DestroySubscribeInfo = [](CommonEvent_SubscribeInfo* info) {};
  g_mock_OH_CommonEvent_DestroySubscriber = [](CommonEvent_Subscriber* subscriber) {};

  adapter_->StopListen();
  
  ASSERT_EQ(adapter_->commonEventSubscriber_, nullptr);
  ASSERT_EQ(adapter_->commonEventSubscriberInfo_, nullptr);
  g_mock_OH_CommonEvent_DestroySubscriber = nullptr;
  g_mock_OH_CommonEvent_DestroySubscribeInfo = nullptr;
}

TEST_F(BatteryMgrClientAdapterImplTest, StopListen_Success) {
  adapter_->callbackSet_.clear();
  adapter_->callbackSet_.insert(adapter_->cb_);
  adapter_->commonEventSubscriber_ = reinterpret_cast<CommonEvent_Subscriber*>(0x1234);
  adapter_->commonEventSubscriberInfo_ = reinterpret_cast<CommonEvent_SubscribeInfo*>(0x5678);
  g_mock_OH_CommonEvent_UnSubscribe = [](const CommonEvent_Subscriber*) {
    return COMMONEVENT_ERR_OK;
  };

  g_mock_OH_CommonEvent_DestroySubscribeInfo = [](CommonEvent_SubscribeInfo* info) {};
  g_mock_OH_CommonEvent_DestroySubscriber = [](CommonEvent_Subscriber* subscriber) {};

  adapter_->StopListen();
  
  ASSERT_EQ(adapter_->commonEventSubscriber_, nullptr);
  ASSERT_EQ(adapter_->commonEventSubscriberInfo_, nullptr);

  g_mock_OH_CommonEvent_DestroySubscribeInfo = nullptr;
  g_mock_OH_CommonEvent_DestroySubscriber = nullptr;
}

TEST_F(BatteryMgrClientAdapterImplTest, OnBatteryEvent_Datanullptr) {
  ASSERT_NO_FATAL_FAILURE(adapter_->OnBatteryEvent(nullptr));
}
