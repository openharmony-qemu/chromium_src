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
#define private public
#include "arkweb/ohos_adapter_ndk/net_proxy_adapter_impl/net_proxy_adapter_impl.h"
#undef private

using namespace testing;
using namespace OHOS::NWeb;

class NetProxyAdapterImplTest : public ::testing::Test {};

namespace OHOS::NWeb::Base64 {
extern std::string Encode(const std::string& source);
extern std::string Decode(const std::string& source);
}

struct CArrParameters {
  int32_t netType;
};

typedef struct CommonEvent_RcvData {
  std::string event;
  std::string bundleName;
  int32_t code;
  std::string data;
  CArrParameters* parameters = nullptr;
} CommonEvent_RcvData;

class MockNetProxyEventCallbackAdapter : public NetProxyEventCallbackAdapter {
 public:
  MOCK_METHOD(void, Changed, (const std::string& host, const uint16_t& port, const std::string& pacUrl,
      const std::vector<std::string>& exclusionList), (override));
};

/**
 * @tc.name: NetProxyAdapterImplTest_Encode_001.
 * @tc.desc: test of Encode in NetProxyAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetProxyAdapterImplTest, NetProxyAdapterImplTest_Encode_001) {
  std::string source;
  EXPECT_EQ(Base64::Encode(source), source);
  source = "test";
  std::string expect = "dGVzdA==";
  EXPECT_EQ(Base64::Encode(source), expect);
}

/**
 * @tc.name: NetProxyAdapterImplTest_Decode_001.
 * @tc.desc: test of Decode in NetProxyAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetProxyAdapterImplTest, NetProxyAdapterImplTest_Decode_001) {
  std::string source;
  EXPECT_EQ(Base64::Decode(source), source);
  source = "dGVzdA==";
  std::string expect = "test";
  EXPECT_EQ(Base64::Decode(source), expect);
}

/**
 * @tc.name: NetProxyAdapterImplTest_RegNetProxyEvent_001.
 * @tc.desc: test of RegNetProxyEvent in NetProxyAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetProxyAdapterImplTest, NetProxyAdapterImplTest_RegNetProxyEvent_001) {
  std::shared_ptr<NetProxyEventCallbackAdapter> event_callback = nullptr;
  NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(event_callback); 
  EXPECT_EQ(NetProxyAdapterImpl::cb_, nullptr);
}

/**
 * @tc.name: NetProxyAdapterImplTest_Changed_001.
 * @tc.desc: test of Changed in NetProxyAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetProxyAdapterImplTest, NetProxyAdapterImplTest_Changed_001) {
  auto event_callback = std::make_shared<MockNetProxyEventCallbackAdapter>();
  NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(event_callback); 
  bool callback_called = false;
  EXPECT_CALL(*event_callback, Changed(::testing::_, ::testing::_, ::testing::_, ::testing::_))
      .WillRepeatedly(::testing::Assign(&callback_called, true));
  NetProxyAdapterImpl::GetInstance().Changed(); 
  EXPECT_EQ(callback_called, true);
  callback_called = false;
  NetProxyAdapterImpl::GetInstance().Changed(); 
  EXPECT_EQ(callback_called, false);
}

/**
 * @tc.name: NetProxyAdapterImplTest_StartListen_001.
 * @tc.desc: test of StartListen in NetProxyAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetProxyAdapterImplTest, NetProxyAdapterImplTest_StartListen_001) {
  NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(nullptr); 
  EXPECT_EQ(NetProxyAdapterImpl::GetInstance().StartListen(), false);
  auto event_callback = std::make_shared<MockNetProxyEventCallbackAdapter>();
  NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(event_callback); 
  EXPECT_EQ(NetProxyAdapterImpl::GetInstance().StartListen(), true);
}

/**
 * @tc.name: NetProxyAdapterImplTest_AppProxyChange_001.
 * @tc.desc: test of AppProxyChange in NetProxyAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetProxyAdapterImplTest, NetProxyAdapterImplTest_AppProxyChange_001) {
  NetConn_HttpProxy receive_http_proxy;
  auto event_callback = std::make_shared<MockNetProxyEventCallbackAdapter>();
  NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(event_callback); 
  bool callback_called = false;
  EXPECT_CALL(*event_callback, Changed(::testing::_, ::testing::_, ::testing::_, ::testing::_))
      .WillRepeatedly(::testing::Assign(&callback_called, true));
  NetProxyAdapterImpl::AppProxyChange(&receive_http_proxy);
  EXPECT_EQ(callback_called, true);
  callback_called = false;
  NetProxyAdapterImpl::AppProxyChange(nullptr);
  EXPECT_EQ(callback_called, false);
}

/**
 * @tc.name: NetProxyAdapterImplTest_StartListenAppProxy_001.
 * @tc.desc: test of StartListenAppProxy in NetProxyAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetProxyAdapterImplTest, NetProxyAdapterImplTest_StartListenAppProxy_001) {
  NetProxyAdapterImpl::GetInstance().appProxyCallbackId_ = 0;
  NetProxyAdapterImpl::GetInstance().StartListenAppProxy();
  uint32_t expect = 1;
  EXPECT_EQ(NetProxyAdapterImpl::GetInstance().appProxyCallbackId_, expect);
}

/**
 * @tc.name: NetProxyAdapterImplTest_StopListen_001.
 * @tc.desc: test of StopListen in NetProxyAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetProxyAdapterImplTest, NetProxyAdapterImplTest_StopListen_001) {
  auto event_callback = std::make_shared<testing::NiceMock<MockNetProxyEventCallbackAdapter>>();
  NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(event_callback); 
  NetProxyAdapterImpl::GetInstance().StartListen();
  NetProxyAdapterImpl::GetInstance().StopListen();
  EXPECT_EQ(NetProxyAdapterImpl::commonEventSubscribeInfo_, nullptr);
  EXPECT_EQ(NetProxyAdapterImpl::commonEventSubscriber_, nullptr);
}

/**
 * @tc.name: NetProxyAdapterImplTest_OnReceiveEvent_001.
 * @tc.desc: test of OnReceiveEvent in NetProxyAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetProxyAdapterImplTest, NetProxyAdapterImplTest_OnReceiveEvent_001) {
  CommonEvent_RcvData data;
  data.code = 0;
  data.parameters = nullptr;
  auto event_callback = std::make_shared<MockNetProxyEventCallbackAdapter>();
  NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(event_callback); 
  bool callback_called = false;
  EXPECT_CALL(*event_callback, Changed(::testing::_, ::testing::_, ::testing::_, ::testing::_))
      .WillRepeatedly(::testing::Assign(&callback_called, true));
  NetProxyAdapterImpl::OnReceiveEvent(&data);
  EXPECT_EQ(callback_called, false);
}
