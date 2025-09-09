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
#include "arkweb/ohos_adapter_ndk/net_connect_adapter/net_connect_adapter_impl.h"
#undef private

using namespace testing;
using namespace OHOS::NWeb;

class NetConnectAdapterImplTest : public ::testing::Test {};

class MockNetConnectAdapterImpl : public NetConnectAdapterImpl {
 public:
  MOCK_METHOD(std::vector<std::string>, GetDnsServersForVpn, (), (override));
  MOCK_METHOD(void, RegisterVpnListener, (std::shared_ptr<VpnListener>), (override));
  MOCK_METHOD(void, UnRegisterVpnListener, (), (override));
};


class MockNetConnCallback : public NetConnCallback {
 public:
  MOCK_METHOD(int32_t, NetAvailable, (), (override));
  MOCK_METHOD(int32_t, NetCapabilitiesChange, (const NetConnectType&, const NetConnectSubtype&), (override));
  MOCK_METHOD(int32_t, NetConnectionPropertiesChange, (), (override));
  MOCK_METHOD(int32_t, NetUnavailable, (), (override));
  MOCK_METHOD(int32_t, OnNetCapabilitiesChanged, (const std::shared_ptr<NetCapabilitiesAdapter>), (override));
  MOCK_METHOD(int32_t, OnNetConnectionPropertiesChanged,
      (const std::shared_ptr<NetConnectionPropertiesAdapter>), (override));
};

/**
 * @tc.name: NetConnectAdapterImplTest_NetAvailable_001.
 * @tc.desc: test of NetAvailable in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_NetAvailable_001) {
  auto cb = std::make_shared<MockNetConnCallback>();
  NetConn_NetHandle netHandle;
  EXPECT_CALL(*cb, NetAvailable())
      .Times(1)
      .WillOnce(Return(0));
  auto result = NetConnectAdapterImpl::NetAvailable(cb, &netHandle);
  EXPECT_EQ(result, 0);
}

/**
 * @tc.name: NetConnectAdapterImplTest_NetCapabilitiesChange_001.
 * @tc.desc: test of NetCapabilitiesChange in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_NetCapabilitiesChange_001) {
  std::shared_ptr<MockNetConnCallback> cb = nullptr;
  NetConn_NetHandle netHandle;
  NetConn_NetCapabilities netCapabilities;
  netCapabilities.bearerTypesSize = 1;
  netCapabilities.bearerTypes[0] = NETCONN_BEARER_CELLULAR;
  auto result = NetConnectAdapterImpl::NetCapabilitiesChange(cb, &netHandle, &netCapabilities);
  EXPECT_EQ(result, 0);

  cb = std::make_shared<MockNetConnCallback>();
  EXPECT_CALL(*cb, OnNetCapabilitiesChanged(::testing::_))
      .Times(1)
      .WillOnce(Return(-1));
  result = NetConnectAdapterImpl::NetCapabilitiesChange(cb, &netHandle, &netCapabilities);
  EXPECT_EQ(result, -1);
}

/**
 * @tc.name: NetConnectAdapterImplTest_NetConnectionPropertiesChange_001.
 * @tc.desc: test of NetConnectionPropertiesChange in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_NetConnectionPropertiesChange_001) {
  std::shared_ptr<MockNetConnCallback> cb = nullptr;
  NetConn_NetHandle netHandle;
  NetConn_ConnectionProperties connConnetionProperties;
  auto result = NetConnectAdapterImpl::NetConnectionPropertiesChange(cb, &netHandle, &connConnetionProperties);
  EXPECT_EQ(result, 0);

  cb = std::make_shared<MockNetConnCallback>();
  EXPECT_CALL(*cb, OnNetConnectionPropertiesChanged(::testing::_))
      .Times(1)
      .WillOnce(Return(-1));
  result = NetConnectAdapterImpl::NetConnectionPropertiesChange(cb, &netHandle, &connConnetionProperties);
  EXPECT_EQ(result, -1);
}

/**
 * @tc.name: NetConnectAdapterImplTest_RegisterNetConnCallback_001.
 * @tc.desc: test of RegisterNetConnCallback in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_RegisterNetConnCallback_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  std::shared_ptr<MockNetConnCallback> cb = nullptr;
  auto result = net_connect_adapter_impl.RegisterNetConnCallback(cb);
  EXPECT_EQ(result, -1);

  cb = std::make_shared<MockNetConnCallback>();
  result = net_connect_adapter_impl.RegisterNetConnCallback(cb);
  EXPECT_EQ(result, -1);
}

/**
 * @tc.name: NetConnectAdapterImplTest_UnregisterNetConnCallback_001.
 * @tc.desc: test of UnregisterNetConnCallback in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_UnregisterNetConnCallback_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  int32_t id = -1;
  auto result = net_connect_adapter_impl.UnregisterNetConnCallback(id);
  EXPECT_EQ(result, -1);

  id = 1000;
  NetConnectAdapterImpl::netConnCallbackMap_[id] = std::make_shared<MockNetConnCallback>();
  result = net_connect_adapter_impl.UnregisterNetConnCallback(id);
  EXPECT_EQ(result, -1);
}

/**
 * @tc.name: NetConnectAdapterImplTest_GetDefaultNetConnect_001.
 * @tc.desc: test of GetDefaultNetConnect in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_GetDefaultNetConnect_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  NetConnectType type;
  NetConnectSubtype subtype;
  auto result = net_connect_adapter_impl.GetDefaultNetConnect(type, subtype);
  EXPECT_EQ(result, -1);
}

/**
 * @tc.name: NetConnectAdapterImplTest_GetDnsServersInternal_001.
 * @tc.desc: test of GetDnsServersInternal in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_GetDnsServersInternal_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  NetConn_NetHandle netHandle;
  auto result = net_connect_adapter_impl.GetDnsServersInternal(netHandle);
  EXPECT_EQ(result.empty(), true);
}

/**
 * @tc.name: NetConnectAdapterImplTest_GetDnsServers_001.
 * @tc.desc: test of GetDnsServers in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_GetDnsServers_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  auto result = net_connect_adapter_impl.GetDnsServers();
  EXPECT_EQ(result.empty(), true);
}

/**
 * @tc.name: NetConnectAdapterImplTest_GetDnsServersByNetId_001.
 * @tc.desc: test of GetDnsServersByNetId in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_GetDnsServersByNetId_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  int32_t netId = -1;
  auto result = net_connect_adapter_impl.GetDnsServersByNetId(netId);
  EXPECT_EQ(result.empty(), true);

  netId = 0;
  result = net_connect_adapter_impl.GetDnsServersByNetId(netId);
  EXPECT_EQ(result.empty(), true);
}
