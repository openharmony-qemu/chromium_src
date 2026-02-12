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
#define private public
#include "arkweb/ohos_adapter_ndk/net_connect_adapter/net_connect_adapter_impl.h"
#undef private

using namespace testing;
using namespace OHOS::NWeb;
using namespace MockNdkApi;

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
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_NetAvailable_001) {
  auto cb = std::make_shared<MockNetConnCallback>();
  NetConn_NetHandle netHandle;
  EXPECT_CALL(*cb, NetAvailable())
      .Times(1)
      .WillOnce(Return(0));
  auto result = NetConnectAdapterImpl::NetAvailable(cb, &netHandle);
  EXPECT_EQ(result, 0);

  cb = nullptr;
  result = NetConnectAdapterImpl::NetAvailable(cb, &netHandle);
  EXPECT_EQ(result, 0);
}

/**
 * @tc.name: NetConnectAdapterImplTest_NetCapabilitiesChange_001.
 * @tc.desc: test of NetCapabilitiesChange in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_NetCapabilitiesChange_001) {
  std::shared_ptr<MockNetConnCallback> cb = nullptr;
  NetConn_NetHandle netHandle;
  NetConn_NetCapabilities netCapabilities;
  netCapabilities.bearerTypesSize = -1;
  auto result = NetConnectAdapterImpl::NetCapabilitiesChange(cb, &netHandle, &netCapabilities);
  EXPECT_EQ(result, 0);

  netCapabilities.bearerTypesSize = NETCONN_MAX_BEARER_TYPE_SIZE + 1;
  result = NetConnectAdapterImpl::NetCapabilitiesChange(cb, &netHandle, &netCapabilities);
  EXPECT_EQ(result, 0);

  netCapabilities.bearerTypesSize = 2;
  netCapabilities.bearerTypes[0] = NETCONN_BEARER_CELLULAR;
  netCapabilities.bearerTypes[1] = NETCONN_BEARER_WIFI;
  result = NetConnectAdapterImpl::NetCapabilitiesChange(cb, &netHandle, &netCapabilities);
  EXPECT_EQ(result, 0);

  cb = std::make_shared<MockNetConnCallback>();
  EXPECT_CALL(*cb, OnNetCapabilitiesChanged(::testing::_))
      .Times(1)
      .WillOnce(Return(-1));
  g_mock_OH_Telephony_GetDefaultCellularDataSlotId = []() {;
    return -1;
  };
  g_mock_OH_Telephony_GetNetworkStateForSlot = [](int32_t, Telephony_NetworkState *) {
    return TEL_RADIO_SUCCESS;
  };
  result = NetConnectAdapterImpl::NetCapabilitiesChange(cb, &netHandle, &netCapabilities);
  EXPECT_EQ(result, -1);
  g_mock_OH_Telephony_GetDefaultCellularDataSlotId = nullptr;
  g_mock_OH_Telephony_GetNetworkStateForSlot = nullptr;
}

/**
 * @tc.name: NetConnectAdapterImplTest_NetConnectionPropertiesChange_001.
 * @tc.desc: test of NetConnectionPropertiesChange in NetConnectAdapterImplTest
 * @tc.type: FUNC.
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
 * @tc.name: NetConnectAdapterImplTest_InitNetConnCallback_001.
 * @tc.desc: test of InitNetConnCallback in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_InitNetConnCallback_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  NetConn_NetConnCallback netConnCallback;
  NetConn_ConnectionProperties connProp;
  NetConn_NetHandle netHandle;
  NetConn_NetCapabilities netCap;
  netCap.bearerTypesSize = 1;
  netCap.bearerTypes[0] = NETCONN_BEARER_CELLULAR;
  auto cb = std::make_shared<MockNetConnCallback>();
  NetConnectAdapterImpl::netConnCallbackMap_[0] = cb;
  NetConnectAdapterImpl::InitNetConnCallback(&netConnCallback);

  bool callback_called = false;
  EXPECT_CALL(*cb, NetAvailable()).WillRepeatedly(::testing::Invoke([&callback_called]() {
    callback_called = true;
    return 0;
  }));
  netConnCallback.onNetworkAvailable(nullptr);
  EXPECT_EQ(callback_called, false);
  netConnCallback.onNetworkAvailable(&netHandle);
  EXPECT_EQ(callback_called, true);

  callback_called = false;
  EXPECT_CALL(*cb, OnNetCapabilitiesChanged(::testing::_)).WillRepeatedly(::testing::Invoke([&callback_called]() {
    callback_called = true;
    return 0;
  }));
  netConnCallback.onNetCapabilitiesChange(nullptr, &netCap);
  EXPECT_EQ(callback_called, false);
  netConnCallback.onNetCapabilitiesChange(&netHandle, nullptr);
  EXPECT_EQ(callback_called, false);
  netConnCallback.onNetCapabilitiesChange(&netHandle, &netCap);
  EXPECT_EQ(callback_called, true);

  callback_called = false;
  EXPECT_CALL(*cb, OnNetConnectionPropertiesChanged(::testing::_)).
      WillRepeatedly(::testing::Invoke([&callback_called]() {
    callback_called = true;
    return 0;
  }));
  netConnCallback.onConnetionProperties(nullptr, &connProp);
  EXPECT_EQ(callback_called, false);
  netConnCallback.onConnetionProperties(&netHandle, nullptr);
  EXPECT_EQ(callback_called, false);
  netConnCallback.onConnetionProperties(&netHandle, &connProp);
  EXPECT_EQ(callback_called, true);
  NetConnectAdapterImpl::netConnCallbackMap_.erase(0);
}

/**
 * @tc.name: NetConnectAdapterImplTest_InitNetConnCallback_002.
 * @tc.desc: test of InitNetConnCallback in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_InitNetConnCallback_002) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  NetConn_NetConnCallback netConnCallback;
  NetConn_NetHandle netHandle;
  auto cb = std::make_shared<MockNetConnCallback>();
  NetConnectAdapterImpl::netConnCallbackMap_[0] = cb;
  NetConnectAdapterImpl::netConnCallbackMap_[1000] = nullptr;
  NetConnectAdapterImpl::InitNetConnCallback(&netConnCallback);

  bool callback_called = false;
  EXPECT_CALL(*cb, NetUnavailable()).WillRepeatedly(::testing::Invoke([&callback_called]() {
    callback_called = true;
    return 0;
  }));
  netConnCallback.onNetBlockStatusChange(&netHandle, false);
  EXPECT_EQ(callback_called, false);

  callback_called = false;
  netConnCallback.onNetLost(&netHandle);
  EXPECT_EQ(callback_called, true);

  callback_called = false;
  netConnCallback.onNetUnavailable();
  EXPECT_EQ(callback_called, true);
  NetConnectAdapterImpl::netConnCallbackMap_.erase(0);
  NetConnectAdapterImpl::netConnCallbackMap_.erase(1000);
}

/**
 * @tc.name: NetConnectAdapterImplTest_InitNetConnCallback_003.
 * @tc.desc: test of InitNetConnCallback in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_InitNetConnCallback_003) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  NetConn_NetConnCallback netConnCallback;
  NetConn_ConnectionProperties connProp;
  NetConn_NetHandle netHandle;
  NetConn_NetCapabilities netCap;
  netCap.bearerTypesSize = 1;
  netCap.bearerTypes[0] = NETCONN_BEARER_CELLULAR;
  auto cb = std::make_shared<MockNetConnCallback>();
  g_mock_OH_NetConn_RegisterDefaultNetConnCallback = [](NetConn_NetConnCallback *, uint32_t *) {
    return -1;
  };
  net_connect_adapter_impl.RegisterNetConnCallback(cb);
  g_mock_OH_NetConn_RegisterDefaultNetConnCallback = nullptr;
  NetConnectAdapterImpl::netConnCallbackMap_[0] = cb;
  NetConnectAdapterImpl::InitNetConnCallback(&netConnCallback);

  bool callback_called = false;
  EXPECT_CALL(*cb, NetAvailable()).WillRepeatedly(::testing::Invoke([&callback_called]() {
    callback_called = true;
    return 0;
  }));
  netConnCallback.onNetworkAvailable(&netHandle);
  EXPECT_EQ(callback_called, true);

  callback_called = false;
  EXPECT_CALL(*cb, OnNetCapabilitiesChanged(::testing::_)).WillRepeatedly(::testing::Invoke([&callback_called]() {
    callback_called = true;
    return 0;
  }));
  netConnCallback.onNetCapabilitiesChange(&netHandle, &netCap);
  EXPECT_EQ(callback_called, true);

  callback_called = false;
  EXPECT_CALL(*cb, OnNetConnectionPropertiesChanged(::testing::_)).
      WillRepeatedly(::testing::Invoke([&callback_called]() {
    callback_called = true;
    return 0;
  }));
  netConnCallback.onConnetionProperties(&netHandle, &connProp);
  EXPECT_EQ(callback_called, true);

  callback_called = false;
  EXPECT_CALL(*cb, NetUnavailable()).WillRepeatedly(::testing::Invoke([&callback_called]() {
    callback_called = true;
    return 0;
  }));
  callback_called = false;
  netConnCallback.onNetLost(&netHandle);
  EXPECT_EQ(callback_called, true);

  callback_called = false;
  netConnCallback.onNetUnavailable();
  EXPECT_EQ(callback_called, true);
  NetConnectAdapterImpl::netConnCallbackMap_.erase(0);
}

/**
 * @tc.name: NetConnectAdapterImplTest_RegisterNetConnCallback_001.
 * @tc.desc: test of RegisterNetConnCallback in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_RegisterNetConnCallback_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  std::shared_ptr<MockNetConnCallback> cb = nullptr;
  auto result = net_connect_adapter_impl.RegisterNetConnCallback(cb);
  EXPECT_EQ(result, -1);

  cb = std::make_shared<MockNetConnCallback>();
  result = net_connect_adapter_impl.RegisterNetConnCallback(cb);
  EXPECT_EQ(result, -1);

  g_mock_OH_NetConn_RegisterDefaultNetConnCallback = [](NetConn_NetConnCallback *, uint32_t *callbackId) {
    *callbackId = 0;
    return 0;
  };
  result = net_connect_adapter_impl.RegisterNetConnCallback(cb);
  EXPECT_EQ(result, 0);
  g_mock_OH_NetConn_RegisterDefaultNetConnCallback = nullptr;
}

/**
 * @tc.name: NetConnectAdapterImplTest_UnregisterNetConnCallback_001.
 * @tc.desc: test of UnregisterNetConnCallback in NetConnectAdapterImplTest
 * @tc.type: FUNC.
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

  g_mock_OH_NetConn_UnregisterNetConnCallback = [](uint32_t) {
    return 0;
  };
  result = net_connect_adapter_impl.UnregisterNetConnCallback(id);
  EXPECT_EQ(result, 0);
  g_mock_OH_NetConn_UnregisterNetConnCallback = nullptr;
}

/**
 * @tc.name: NetConnectAdapterImplTest_GetDefaultNetConnect_001.
 * @tc.desc: test of GetDefaultNetConnect in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_GetDefaultNetConnect_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  NetConnectType type;
  NetConnectSubtype subtype;
  EXPECT_EQ(net_connect_adapter_impl.GetDefaultNetConnect(type, subtype), -1);

  g_mock_OH_NetConn_GetDefaultNet = [](NetConn_NetHandle *) {
    return 0;
  };
  EXPECT_EQ(net_connect_adapter_impl.GetDefaultNetConnect(type, subtype), -1);

  g_mock_OH_NetConn_GetNetCapabilities = [](NetConn_NetHandle *, NetConn_NetCapabilities *netCapabilities) {
    netCapabilities->bearerTypes[0] = NETCONN_BEARER_WIFI;
    return 0;
  };
  EXPECT_EQ(net_connect_adapter_impl.GetDefaultNetConnect(type, subtype), 0);

  g_mock_OH_NetConn_GetNetCapabilities = [](NetConn_NetHandle *, NetConn_NetCapabilities *netCapabilities) {
    netCapabilities->bearerTypes[0] = NETCONN_BEARER_CELLULAR;
    return 0;
  };
  EXPECT_EQ(net_connect_adapter_impl.GetDefaultNetConnect(type, subtype), 0);

  g_mock_OH_Telephony_GetDefaultCellularDataSlotId = []() {;
    return -1;
  };
  g_mock_OH_Telephony_GetNetworkStateForSlot = [](int32_t, Telephony_NetworkState *) {
    return TEL_RADIO_SUCCESS;
  };
  EXPECT_EQ(net_connect_adapter_impl.GetDefaultNetConnect(type, subtype), 0);
  g_mock_OH_Telephony_GetNetworkStateForSlot = nullptr;
  g_mock_OH_Telephony_GetDefaultCellularDataSlotId = nullptr;
  g_mock_OH_NetConn_GetNetCapabilities = nullptr;
  g_mock_OH_NetConn_GetDefaultNet = nullptr;
}

/**
 * @tc.name: NetConnectAdapterImplTest_GetDnsServersInternal_001.
 * @tc.desc: test of GetDnsServersInternal in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_GetDnsServersInternal_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  NetConn_NetHandle netHandle;
  auto result = net_connect_adapter_impl.GetDnsServersInternal(netHandle);
  EXPECT_EQ(result.empty(), true);

  g_mock_OH_NetConn_GetConnectionProperties = [](NetConn_NetHandle*, NetConn_ConnectionProperties* properties) {
    properties->dnsListSize = 1;
    properties->dnsList[0].address[0] = 0;
    return 0;
  };
  result = net_connect_adapter_impl.GetDnsServersInternal(netHandle);
  EXPECT_EQ(result.empty(), false);
  g_mock_OH_NetConn_GetConnectionProperties = nullptr;
}

/**
 * @tc.name: NetConnectAdapterImplTest_GetDnsServers_001.
 * @tc.desc: test of GetDnsServers in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_GetDnsServers_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  auto result = net_connect_adapter_impl.GetDnsServers();
  EXPECT_EQ(result.empty(), true);

  g_mock_OH_NetConn_GetDefaultNet = [](NetConn_NetHandle *) {
    return 0;
  };
  result = net_connect_adapter_impl.GetDnsServers();
  EXPECT_EQ(result.empty(), true);
  g_mock_OH_NetConn_GetDefaultNet = nullptr;
}

/**
 * @tc.name: NetConnectAdapterImplTest_GetDnsServersByNetId_001.
 * @tc.desc: test of GetDnsServersByNetId in NetConnectAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_GetDnsServersByNetId_001) {
  MockNetConnectAdapterImpl net_connect_adapter_impl;
  int32_t netId = -1;
  auto result = net_connect_adapter_impl.GetDnsServersByNetId(netId);
  EXPECT_EQ(result.empty(), true);

  netId = 0;
  result = net_connect_adapter_impl.GetDnsServersByNetId(netId);
  EXPECT_EQ(result.empty(), true);

  g_mock_OH_NetConn_GetAllNets = [](NetConn_NetHandleList *netHandleList) {
    return 0;
  };
  result = net_connect_adapter_impl.GetDnsServersByNetId(netId);
  EXPECT_EQ(result.empty(), true);

  g_mock_OH_NetConn_GetAllNets = [](NetConn_NetHandleList *netHandleList) {
    netHandleList->netHandleListSize = 2;
    netHandleList->netHandles[0].netId = 1;
    netHandleList->netHandles[1].netId = 0;
    return 0;
  };
  result = net_connect_adapter_impl.GetDnsServersByNetId(netId);
  EXPECT_EQ(result.empty(), true);
  g_mock_OH_NetConn_GetAllNets = nullptr;
}