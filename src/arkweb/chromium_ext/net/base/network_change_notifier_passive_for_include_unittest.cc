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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/functional/callback_forward.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/no_destructor.h"
#include "base/run_loop.h"
#include "base/test/task_environment.h"

#define private public
#define BIND_DNS_TEST
#include "net/base/network_change_notifier_passive.h"
#include "net/base/network_change_notifier_passive_for_include.cc"
#undef BIND_DNS_TEST
#undef private
#include "base/test/task_environment.h"
#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "net/base/network_change_notifier.h"
#include "net/dns/dns_config.h"
#include "net/dns/system_dns_config_change_notifier.h"
#include "net/dns/test_dns_config_service.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/net_connect_adapter.h"

using net::NetworkChangeNotifier;
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

namespace OHOS::NWeb {
class MockNetCapabilitiesAdapter : public NetCapabilitiesAdapter {
 public:
  MOCK_METHOD(int32_t, GetNetId, (), (override));
  MOCK_METHOD(NetConnectType, GetConnectType, (), (override));
  MOCK_METHOD(NetConnectSubtype, GetConnectSubtype, (), (override));
};

class MockNetConnectionPropertiesAdapter
    : public NetConnectionPropertiesAdapter {
 public:
  MOCK_METHOD(int32_t, GetNetId, (), (override));
};

class MockNetConnectAdapter : public NetConnectAdapter {
 public:
  MockNetConnectAdapter() = default;
  ~MockNetConnectAdapter() override = default;

  MOCK_METHOD(int32_t,
              RegisterNetConnCallback,
              (std::shared_ptr<NetConnCallback>),
              (override));
  MOCK_METHOD(int32_t, UnregisterNetConnCallback, (int32_t), (override));
  MOCK_METHOD(int32_t,
              GetDefaultNetConnect,
              (NetConnectType & type, NetConnectSubtype& netConnectSubtype),
              (override));
  MOCK_METHOD(std::vector<std::string>, GetDnsServers, (), (override));
  MOCK_METHOD(std::vector<std::string>,
              GetDnsServersByNetId,
              (int32_t),
              (override));
  std::vector<std::string> GetDnsServersForVpn() { return {}; }
  void RegisterVpnListener(std::shared_ptr<VpnListener>) {}
  void UnRegisterVpnListener() {}
};
}  // namespace OHOS::NWeb

namespace net {

class TestDnsObserver : public NetworkChangeNotifier::DNSObserver {
 public:
  void OnDNSChanged() override { dns_changes_++; }

  int GetDnsChanges() const { return dns_changes_; }

 private:
  int dns_changes_ = 0;
};

class MockIPAddressObserver : public NetworkChangeNotifier::IPAddressObserver {
 public:
  void OnIPAddressChanged() { changes_++; }
  int GetIPChanges() { return changes_; }

 private:
  int changes_ = 0;
};

class MockNetworkChangeObserver
    : public NetworkChangeNotifier::NetworkChangeObserver {
 public:
  void OnNetworkChanged(NetworkChangeNotifier::ConnectionType) { changes_++; }
  int GetNetworkChanges() { return changes_; }

 private:
  int changes_ = 0;
};

class MockMaxBandwidthObserver
    : public NetworkChangeNotifier::MaxBandwidthObserver {
 public:
  void OnMaxBandwidthChanged(double, NetworkChangeNotifier::ConnectionType) {
    changes_++;
  }
  int GetMaxBandwidthChanges() { return changes_; }

 private:
  int changes_ = 0;
};

class NetworkChangeNotifierPassiveExtTest : public testing::Test {
 public:
  NetworkChangeNotifierPassiveExtTest()
      : task_environment_(base::test::TaskEnvironment::TimeSource::MOCK_TIME) {
    notifier_ = base::WrapUnique(new NetworkChangeNotifierPassive(
        NetworkChangeNotifier::CONNECTION_UNKNOWN,
        NetworkChangeNotifier::SUBTYPE_UNKNOWN));
  }

  void FastForwardUntilIdle() {
    task_environment_.FastForwardUntilNoTasksRemain();
  }

  net::NetworkChangeNotifierPassive* GetNotifier() { return notifier_.get(); }

 private:
  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<NetworkChangeNotifierPassive> notifier_;
};

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
TEST_F(NetworkChangeNotifierPassiveExtTest,
       ConvertOhosConnTypeToNetBaseConnType) {
  std::map<net::NetworkChangeNotifier::ConnectionType,
           OHOS::NWeb::NetConnectType>
      connection_type_map = {
          {net::NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN,
           OHOS::NWeb::NetConnectType::CONNECTION_UNKNOWN},
          {net::NetworkChangeNotifier::ConnectionType::CONNECTION_ETHERNET,
           OHOS::NWeb::NetConnectType::CONNECTION_ETHERNET},
          {net::NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI,
           OHOS::NWeb::NetConnectType::CONNECTION_WIFI},
          {net::NetworkChangeNotifier::ConnectionType::CONNECTION_2G,
           OHOS::NWeb::NetConnectType::CONNECTION_2G},
          {net::NetworkChangeNotifier::ConnectionType::CONNECTION_3G,
           OHOS::NWeb::NetConnectType::CONNECTION_3G},
          {net::NetworkChangeNotifier::ConnectionType::CONNECTION_4G,
           OHOS::NWeb::NetConnectType::CONNECTION_4G},
          {net::NetworkChangeNotifier::ConnectionType::CONNECTION_NONE,
           OHOS::NWeb::NetConnectType::CONNECTION_NONE},
          {net::NetworkChangeNotifier::ConnectionType::CONNECTION_BLUETOOTH,
           OHOS::NWeb::NetConnectType::CONNECTION_BLUETOOTH},
          {net::NetworkChangeNotifier::ConnectionType::CONNECTION_5G,
           OHOS::NWeb::NetConnectType::CONNECTION_5G},
      };
  for (auto [notifier_type, nweb_type] : connection_type_map) {
    auto type = ConvertOhosConnTypeToNetBaseConnType(nweb_type);
    EXPECT_EQ(type, notifier_type);
  }
}

TEST_F(NetworkChangeNotifierPassiveExtTest,
       ConvertOhosConnSubtypeToNetBaseConnSubtype) {
  std::map<net::NetworkChangeNotifier::ConnectionSubtype,
           OHOS::NWeb::NetConnectSubtype>
      connection_type_map = {
          {net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_UNKNOWN,
           OHOS::NWeb::NetConnectSubtype::SUBTYPE_UNKNOWN},
          {net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_NONE,
           OHOS::NWeb::NetConnectSubtype::SUBTYPE_NONE},
          {net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_OTHER,
           OHOS::NWeb::NetConnectSubtype::SUBTYPE_OTHER},
          {net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_GSM,
           OHOS::NWeb::NetConnectSubtype::SUBTYPE_GSM},
          {net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_IDEN,
           OHOS::NWeb::NetConnectSubtype::SUBTYPE_IDEN},
          {net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_CDMA,
           OHOS::NWeb::NetConnectSubtype::SUBTYPE_CDMA},
          {net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_1XRTT,
           OHOS::NWeb::NetConnectSubtype::SUBTYPE_1XRTT},
          {net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_GPRS,
           OHOS::NWeb::NetConnectSubtype::SUBTYPE_GPRS},
          {net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_EDGE,
           OHOS::NWeb::NetConnectSubtype::SUBTYPE_EDGE},
      };
  for (auto [notifier_type, nweb_type] : connection_type_map) {
    auto type = ConvertOhosConnSubtypeToNetBaseConnSubtype(nweb_type);
    EXPECT_EQ(type, notifier_type);
  }
}

// network_change_notifier_posix_ is null
TEST_F(NetworkChangeNotifierPassiveExtTest, NullNotifier) {
  testing::StrictMock<MockNetworkChangeObserver> observer;
  NetworkChangeNotifier::AddNetworkChangeObserver(&observer);
  NetConnCallbackImpl callback_null(nullptr);
  auto result = callback_null.NetAvailable();
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(observer.GetNetworkChanges(), 0);
  result = callback_null.NetUnavailable();
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(observer.GetNetworkChanges(), 0);
  result = callback_null.NetCapabilitiesChange(
      OHOS::NWeb::NetConnectType::CONNECTION_5G,
      OHOS::NWeb::NetConnectSubtype::SUBTYPE_LTE);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(observer.GetNetworkChanges(), 0);
  result = callback_null.NetConnectionPropertiesChange();
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(observer.GetNetworkChanges(), 0);
  NetworkChangeNotifier::RemoveNetworkChangeObserver(&observer);
}

TEST_F(NetworkChangeNotifierPassiveExtTest, NetCapabilitiesChange) {
  testing::StrictMock<MockNetworkChangeObserver> observer;
  NetworkChangeNotifier::AddNetworkChangeObserver(&observer);

  NetConnCallbackImpl callback(GetNotifier());
  auto result = callback.NetCapabilitiesChange(
      OHOS::NWeb::NetConnectType::CONNECTION_5G,
      OHOS::NWeb::NetConnectSubtype::SUBTYPE_LTE);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(observer.GetNetworkChanges(), 2);
  NetworkChangeNotifier::RemoveNetworkChangeObserver(&observer);
}

TEST_F(NetworkChangeNotifierPassiveExtTest, NetConnectionPropertiesChange) {
  testing::StrictMock<MockNetworkChangeObserver> observer;
  NetworkChangeNotifier::AddNetworkChangeObserver(&observer);
  testing::StrictMock<TestDnsObserver> dns_observer;
  NetworkChangeNotifier::AddDNSObserver(&dns_observer);
  testing::StrictMock<MockIPAddressObserver> ip_observer;
  NetworkChangeNotifier::AddIPAddressObserver(&ip_observer);

  NetConnCallbackImpl callback(GetNotifier());
  auto result = callback.NetConnectionPropertiesChange();
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(observer.GetNetworkChanges(), 2);
  EXPECT_EQ(dns_observer.GetDnsChanges(), 0);
  EXPECT_EQ(ip_observer.GetIPChanges(), 1);
  NetworkChangeNotifier::RemoveNetworkChangeObserver(&observer);
  NetworkChangeNotifier::RemoveDNSObserver(&dns_observer);
  NetworkChangeNotifier::RemoveIPAddressObserver(&ip_observer);
}

TEST_F(NetworkChangeNotifierPassiveExtTest, NetUnavailable) {
  NetConnCallbackImpl callback(GetNotifier());
  testing::StrictMock<MockNetworkChangeObserver> observer;
  NetworkChangeNotifier::AddNetworkChangeObserver(&observer);
  auto result = callback.NetUnavailable();
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(observer.GetNetworkChanges(), 1);
  NetworkChangeNotifier::RemoveNetworkChangeObserver(&observer);
}

TEST_F(NetworkChangeNotifierPassiveExtTest, OnNetCapabilitiesChanged) {
  auto capabilities =
      std::make_shared<NiceMock<OHOS::NWeb::MockNetCapabilitiesAdapter>>();
  auto properties = std::make_shared<
      NiceMock<OHOS::NWeb::MockNetConnectionPropertiesAdapter>>();
  EXPECT_CALL(*properties, GetNetId()).WillRepeatedly(Return(42));
  EXPECT_CALL(*capabilities, GetNetId()).WillRepeatedly(Return(42));
  EXPECT_CALL(*capabilities, GetConnectType())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectType::CONNECTION_5G));
  EXPECT_CALL(*capabilities, GetConnectSubtype())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectSubtype::SUBTYPE_LTE));

  testing::StrictMock<MockNetworkChangeObserver> observer;
  NetworkChangeNotifier::AddNetworkChangeObserver(&observer);
  testing::StrictMock<MockMaxBandwidthObserver> cap_observer;
  NetworkChangeNotifier::AddMaxBandwidthObserver(&cap_observer);

  NetConnCallbackImpl callback(GetNotifier());

  // no capabilities
  auto result = callback.OnNetCapabilitiesChanged(nullptr);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 0);

  // no properties_
  result = callback.OnNetCapabilitiesChanged(capabilities);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 0);

  // set properties
  result = callback.OnNetConnectionPropertiesChanged(properties);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 1);

  // capabilities, same properties, do not change
  result = callback.OnNetCapabilitiesChanged(capabilities);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 1);
  NetworkChangeNotifier::RemoveNetworkChangeObserver(&observer);
  NetworkChangeNotifier::RemoveMaxBandwidthObserver(&cap_observer);
}

TEST_F(NetworkChangeNotifierPassiveExtTest, OnNetConnectionPropertiesChanged) {
  auto capabilities =
      std::make_shared<NiceMock<OHOS::NWeb::MockNetCapabilitiesAdapter>>();
  auto properties = std::make_shared<
      NiceMock<OHOS::NWeb::MockNetConnectionPropertiesAdapter>>();
  EXPECT_CALL(*properties, GetNetId()).WillRepeatedly(Return(42));
  EXPECT_CALL(*capabilities, GetNetId()).WillRepeatedly(Return(42));
  EXPECT_CALL(*capabilities, GetConnectType())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectType::CONNECTION_5G));
  EXPECT_CALL(*capabilities, GetConnectSubtype())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectSubtype::SUBTYPE_LTE));

  testing::StrictMock<MockNetworkChangeObserver> observer;
  NetworkChangeNotifier::AddNetworkChangeObserver(&observer);
  testing::StrictMock<MockMaxBandwidthObserver> cap_observer;
  NetworkChangeNotifier::AddMaxBandwidthObserver(&cap_observer);

  NetConnCallbackImpl callback(GetNotifier());

  // no properties_
  auto result = callback.OnNetConnectionPropertiesChanged(nullptr);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 0);

  // no capabilities
  result = callback.OnNetConnectionPropertiesChanged(properties);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 0);

  // set capabilities
  result = callback.OnNetCapabilitiesChanged(capabilities);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 1);

  // properties, same properties, do not change
  result = callback.OnNetConnectionPropertiesChanged(properties);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 1);
  NetworkChangeNotifier::RemoveNetworkChangeObserver(&observer);
  NetworkChangeNotifier::RemoveMaxBandwidthObserver(&cap_observer);
}

TEST_F(NetworkChangeNotifierPassiveExtTest,
       ConnectionTypeChangedTo_NetIdChanged) {
  const int net_id = 42;
  auto capabilities =
      std::make_shared<NiceMock<OHOS::NWeb::MockNetCapabilitiesAdapter>>();
  auto properties = std::make_shared<
      NiceMock<OHOS::NWeb::MockNetConnectionPropertiesAdapter>>();
  EXPECT_CALL(*properties, GetNetId()).WillRepeatedly(Return(net_id));
  EXPECT_CALL(*capabilities, GetNetId()).WillRepeatedly(Return(net_id + 1));
  EXPECT_CALL(*capabilities, GetConnectType())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectType::CONNECTION_5G));
  EXPECT_CALL(*capabilities, GetConnectSubtype())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectSubtype::SUBTYPE_LTE));

  testing::StrictMock<MockNetworkChangeObserver> observer;
  NetworkChangeNotifier::AddNetworkChangeObserver(&observer);
  testing::StrictMock<MockMaxBandwidthObserver> cap_observer;
  NetworkChangeNotifier::AddMaxBandwidthObserver(&cap_observer);

  NetConnCallbackImpl callback(GetNotifier());
  // set dns id
  callback.BindDnsToNetwork(net_id);

  auto result = callback.OnNetConnectionPropertiesChanged(properties);
  EXPECT_EQ(result, 0);
  // net_id not eq，do not notifier
  result = callback.OnNetCapabilitiesChanged(capabilities);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 0);

  // net_id eq，notifier
  result = callback.OnNetConnectionPropertiesChanged(properties);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 1);
  EXPECT_EQ(observer.GetNetworkChanges(), 2);
  NetworkChangeNotifier::RemoveNetworkChangeObserver(&observer);
  NetworkChangeNotifier::RemoveMaxBandwidthObserver(&cap_observer);
}

TEST_F(NetworkChangeNotifierPassiveExtTest,
       ConnectionTypeChangedTo_NetworkChanged) {
  const int net_id = 42;
  auto capabilities =
      std::make_shared<NiceMock<OHOS::NWeb::MockNetCapabilitiesAdapter>>();
  auto properties = std::make_shared<
      NiceMock<OHOS::NWeb::MockNetConnectionPropertiesAdapter>>();
  EXPECT_CALL(*properties, GetNetId()).WillRepeatedly(Return(net_id));
  EXPECT_CALL(*capabilities, GetNetId()).WillRepeatedly(Return(net_id + 1));
  EXPECT_CALL(*capabilities, GetConnectType())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectType::CONNECTION_5G));
  EXPECT_CALL(*capabilities, GetConnectSubtype())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectSubtype::SUBTYPE_LTE));

  testing::StrictMock<MockNetworkChangeObserver> observer;
  NetworkChangeNotifier::AddNetworkChangeObserver(&observer);
  testing::StrictMock<MockMaxBandwidthObserver> cap_observer;
  NetworkChangeNotifier::AddMaxBandwidthObserver(&cap_observer);

  NetConnCallbackImpl callback(GetNotifier());
  auto result = callback.OnNetConnectionPropertiesChanged(properties);
  EXPECT_EQ(result, 0);
  result = callback.OnNetCapabilitiesChanged(capabilities);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 1);
  EXPECT_EQ(observer.GetNetworkChanges(), 2);

  EXPECT_CALL(*properties, GetNetId()).WillRepeatedly(Return(net_id));
  EXPECT_CALL(*capabilities, GetNetId()).WillRepeatedly(Return(net_id + 1));
  EXPECT_CALL(*capabilities, GetConnectType())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectType::CONNECTION_ETHERNET));
  EXPECT_CALL(*capabilities, GetConnectSubtype())
      .WillRepeatedly(Return(OHOS::NWeb::NetConnectSubtype::SUBTYPE_ETHERNET));
  result = callback.OnNetCapabilitiesChanged(capabilities);
  EXPECT_EQ(result, 0);
  FastForwardUntilIdle();
  EXPECT_EQ(observer.GetNetworkChanges(), 4);
  EXPECT_EQ(cap_observer.GetMaxBandwidthChanges(), 2);
  NetworkChangeNotifier::RemoveNetworkChangeObserver(&observer);
  NetworkChangeNotifier::RemoveMaxBandwidthObserver(&cap_observer);
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
TEST_F(NetworkChangeNotifierPassiveExtTest,
       NetworkChangeNotifierPassiveUtils_RegisterOhosNetConnCallback) {
  *g_net_connect_callback = nullptr;
  g_callback_id = -1;

  // 替换ohos_net_conn_adapter_为MockNetConnectAdapter
  auto adapter = new OHOS::NWeb::MockNetConnectAdapter();
  GetNotifier()->ohos_net_conn_adapter_.reset(adapter);
  int callback_id = -2;
  EXPECT_CALL(*adapter, RegisterNetConnCallback(testing::_))
      .WillOnce(Return(callback_id));
  EXPECT_CALL(*adapter, UnregisterNetConnCallback(testing::_))
      .WillOnce(Return(callback_id));
  EXPECT_NO_FATAL_FAILURE(
      net::NetworkChangeNotifierPassiveUtils::RegisterOhosNetConnCallback(
          GetNotifier()));
  EXPECT_NE(*g_net_connect_callback, nullptr);
  EXPECT_GE(g_callback_id, callback_id);
  EXPECT_NO_FATAL_FAILURE(
      net::NetworkChangeNotifierPassiveUtils::UnRegisterOhosNetConnCallback(
          GetNotifier()));

  callback_id = 10;
  EXPECT_CALL(*adapter, RegisterNetConnCallback(testing::_))
      .WillOnce(Return(callback_id));
  EXPECT_CALL(*adapter, UnregisterNetConnCallback(testing::_))
      .WillOnce(Return(callback_id));
  EXPECT_NO_FATAL_FAILURE(
      net::NetworkChangeNotifierPassiveUtils::RegisterOhosNetConnCallback(
          GetNotifier()));
  EXPECT_NE(*g_net_connect_callback, nullptr);
  EXPECT_GE(g_callback_id, callback_id);

  // unregister
  EXPECT_NO_FATAL_FAILURE(
      net::NetworkChangeNotifierPassiveUtils::UnRegisterOhosNetConnCallback(
          GetNotifier()));

  // ohos_net_conn_adapter_ null
  *g_net_connect_callback = nullptr;
  g_callback_id = -1;
  GetNotifier()->ohos_net_conn_adapter_ = nullptr;
  EXPECT_NO_FATAL_FAILURE(
      net::NetworkChangeNotifierPassiveUtils::RegisterOhosNetConnCallback(
          GetNotifier()));
  EXPECT_NE(*g_net_connect_callback, nullptr);
  EXPECT_EQ(g_callback_id, -1);
  EXPECT_NO_FATAL_FAILURE(
      net::NetworkChangeNotifierPassiveUtils::UnRegisterOhosNetConnCallback(
          GetNotifier()));
  *g_net_connect_callback = nullptr;
  g_callback_id = 0;
}
#endif

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
TEST_F(NetworkChangeNotifierPassiveExtTest,
       NetworkChangeNotifierPassive_BindDnsToNetwork) {
  // no register
  const int dns_id = 100;
  ASSERT_NO_FATAL_FAILURE(GetNotifier()->BindDnsToNetwork(dns_id));
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  *g_net_connect_callback = nullptr;
  g_callback_id = 0;
  net::NetworkChangeNotifierPassiveUtils::RegisterOhosNetConnCallback(
      GetNotifier());
  EXPECT_NE(*g_net_connect_callback, nullptr);
#endif
  ASSERT_NO_FATAL_FAILURE(GetNotifier()->BindDnsToNetwork(dns_id));
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  EXPECT_EQ((*g_net_connect_callback)->network_for_dns_, dns_id);
#endif
}
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
TEST_F(NetworkChangeNotifierPassiveExtTest,
       NetworkChangeNotifierPassiveUtils_SetDnsServers) {
  // no switch kEnableNwebExHttpDnsFallback
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableNwebExHttpDnsFallback);
  ASSERT_NO_FATAL_FAILURE(
      NetworkChangeNotifierPassiveUtils::SetDnsServers(GetNotifier()));

  // 替换ohos_net_conn_adapter_为MockNetConnectAdapter
  auto adapter = new OHOS::NWeb::MockNetConnectAdapter();
  GetNotifier()->ohos_net_conn_adapter_.reset(adapter);
  std::vector<std::string> dns_servers = {"dns server1"};
  EXPECT_CALL(*adapter, GetDnsServersByNetId(testing::_))
      .WillOnce(Return(dns_servers));
  EXPECT_CALL(*adapter, UnregisterNetConnCallback(testing::_))
      .WillOnce(Return(0));
  // open switch
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableNwebExHttpDnsFallback);
  ASSERT_NO_FATAL_FAILURE(
      NetworkChangeNotifierPassiveUtils::SetDnsServers(GetNotifier()));

  auto result = GetNotifier()->GetCurrentDnsServers();
  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result[0], dns_servers[0]);
}
#endif
}  // namespace net
