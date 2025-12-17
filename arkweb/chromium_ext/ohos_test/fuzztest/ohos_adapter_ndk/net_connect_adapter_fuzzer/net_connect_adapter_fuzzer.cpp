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
#include "net_connect_adapter.h"

#include <fuzzer/FuzzedDataProvider.h>
#include "net_connect_fuzz_mock.h"
#include "net_connect_adapter/net_capabilities_adapter_impl.h"
#include "net_connect_adapter/net_connect_adapter_impl.h"
#include "net_connect_adapter/net_connect_utils.h"
#include "net_connect_adapter/net_connection_properties_adapter_impl.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::NiceMock;

namespace OHOS::NWeb {
bool LLVMFuzzerTestSetup() {
  return true;
}

class NetConnCallbackMock : public NetConnCallback {
 public:
  NetConnCallbackMock() = default;
  ~NetConnCallbackMock() override = default;
  MOCK_METHOD(int32_t, NetAvailable, (), (override));
  MOCK_METHOD(int32_t,
              NetCapabilitiesChange,
              (const NetConnectType& netConnectType,
               const NetConnectSubtype& netConnectSubtype),
              (override));
  MOCK_METHOD(int32_t, NetConnectionPropertiesChange, (), (override));
  MOCK_METHOD(int32_t, NetUnavailable, (), (override));
  MOCK_METHOD(int32_t,
              OnNetCapabilitiesChanged,
              (const std::shared_ptr<NetCapabilitiesAdapter> capabilities),
              (override));
  MOCK_METHOD(
      int32_t,
      OnNetConnectionPropertiesChanged,
      (const std::shared_ptr<NetConnectionPropertiesAdapter> properties),
      (override));
};

class VpnListenerMock : public VpnListener {
 public:
  VpnListenerMock() = default;
  ~VpnListenerMock() override = default;
  MOCK_METHOD(void, OnAvailable, (), (override));
  MOCK_METHOD(void, OnLost, (), (override));
};

void FuzzNetConnectUtils(FuzzedDataProvider& fdp) {
  Telephony_RadioTechnology subtype = static_cast<Telephony_RadioTechnology>(
      fdp.ConsumeIntegralInRange<uint32_t>(0, 12));
  NetConnectUtils::ConvertToConnectsubtype(subtype);
  NetConn_NetBearerType bearType = static_cast<NetConn_NetBearerType>(
      fdp.ConsumeIntegralInRange<uint32_t>(0, 4));
  NetConnectUtils::ConvertToConnectType(bearType, subtype);
  NetConnectType type =
      static_cast<NetConnectType>(fdp.ConsumeIntegralInRange<uint32_t>(0, 9));
  NetConnectUtils::ConnectTypeToString(type);
}

void FuzzOnReceiveEvent(FuzzedDataProvider& fdp) {
  CArrParameters parameters;
  parameters.netType = fdp.ConsumeIntegralInRange<int32_t>(-1, 5);
  CommonEvent_RcvData eventData;
  eventData.event = "usual.event.CONNECTIVITY_CHANGE";
  eventData.code = fdp.ConsumeIntegralInRange<int32_t>(2, 6);
  eventData.parameters = &parameters;
  NetConnectAdapterImpl::OnReceiveEvent(&eventData);
}

void FuzzApi(const uint8_t* data, size_t size) {
  FuzzedDataProvider fdp(data, size);
  NetConnectAdapterImpl ins;
  std::shared_ptr<NiceMock<NetConnCallbackMock>> callback =
      std::make_shared<NiceMock<NetConnCallbackMock>>();
  ins.RegisterNetConnCallback(nullptr);
  int32_t id = ins.RegisterNetConnCallback(callback);
  ins.GetDnsServers();
  int32_t netId = fdp.ConsumeIntegral<int32_t>();
  ins.GetDnsServersByNetId(netId);
  ins.GetDnsServersForVpn();
  std::shared_ptr<NiceMock<VpnListenerMock>> vpnListener =
      std::make_shared<NiceMock<VpnListenerMock>>();
  ins.RegisterVpnListener(vpnListener);
  ins.GetNetAddrListByNetId(netId);
  ins.GetNetAddrListForVpn();
  NetCapabilitiesAdapterImpl cap;
  cap.SetNetId(netId);
  cap.SetConnectType(NetConnectType::CONNECTION_UNKNOWN);
  cap.SetConnectSubtype(NetConnectSubtype::SUBTYPE_UNKNOWN);
  cap.GetNetId();
  cap.GetConnectType();
  cap.GetConnectSubtype();
  NetConnectionPropertiesAdapterImpl conProb;
  conProb.SetNetId(netId);
  conProb.GetNetId();
  FuzzNetConnectUtils(fdp);
  FuzzOnReceiveEvent(fdp);
  ins.UnRegisterVpnListener();
  ins.UnregisterNetConnCallback(id);
  NetConnectType type =
      static_cast<NetConnectType>(fdp.ConsumeIntegralInRange<uint32_t>(0, 9));
  NetConnectSubtype netConnectSubtype = static_cast<NetConnectSubtype>(
      fdp.ConsumeIntegralInRange<uint32_t>(0, 32));
  ins.GetDefaultNetConnect(type, netConnectSubtype);
}
}  // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size < sizeof(int32_t))) {
    return 0;
  }
  static bool init = OHOS::NWeb::LLVMFuzzerTestSetup();
  if (init) {
    OHOS::NWeb::FuzzApi(data, size);
  }
  return 0;
}
