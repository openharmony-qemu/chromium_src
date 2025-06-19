// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/base/network_change_notifier_passive.h"

#include <string>
#include <unordered_set>
#include <utility>

#include "base/functional/bind.h"
#include "base/task/task_traits.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "net/dns/dns_config_service_posix.h"
#include "net/dns/system_dns_config_change_notifier.h"

#if BUILDFLAG(IS_ANDROID)
#include "net/android/network_change_notifier_android.h"
#endif

#if BUILDFLAG(IS_LINUX)
#include <linux/rtnetlink.h>

#include "net/base/network_change_notifier_linux.h"
#endif

#if BUILDFLAG(IS_OHOS)
#include "base/logging.h"
#ifdef OHOS_EX_HTTP_DNS_FALLBACK
#include "base/base_switches.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include "net/dns/public/dns_protocol.h"
#endif
#endif

namespace {
#if BUILDFLAG(IS_OHOS)
net::NetworkChangeNotifier::ConnectionType ConvertOhosConnTypeToNetBaseConnType(
    const OHOS::NWeb::NetConnectType& netConnectType) {
  return static_cast<net::NetworkChangeNotifier::ConnectionType>(
      netConnectType);
}

net::NetworkChangeNotifier::ConnectionSubtype
ConvertOhosConnSubtypeToNetBaseConnSubtype(
    const OHOS::NWeb::NetConnectSubtype& subtype) {
  return static_cast<net::NetworkChangeNotifier::ConnectionSubtype>(subtype);
}

class VpnListenerImpl : public OHOS::NWeb::VpnListener {
 public:
  VpnListenerImpl(
      net::NetworkChangeNotifierPassive* network_change_notifier)
      : network_change_notifier_(network_change_notifier) {}
  virtual ~VpnListenerImpl() = default;
  void OnAvailable() override;
  void OnLost() override;

 private:
  net::NetworkChangeNotifierPassive* network_change_notifier_ = nullptr;
};

void VpnListenerImpl::OnAvailable() {
  LOG(INFO) << "ohos_network Vpn OnAvailable";
  network_change_notifier_->OnVpnAvailable();
}

void VpnListenerImpl::OnLost() {
  LOG(INFO) << "ohos_network Vpn OnLost";
  network_change_notifier_->OnVpnLost();
}

class NetConnCallbackImpl : public OHOS::NWeb::NetConnCallback {
 public:
  NetConnCallbackImpl(
      net::NetworkChangeNotifierPassive* network_change_notifier_posix)
      : network_change_notifier_posix_(network_change_notifier_posix) {}
  virtual ~NetConnCallbackImpl() = default;
  int32_t NetAvailable() override;
  int32_t NetCapabilitiesChange(
      const OHOS::NWeb::NetConnectType& netConnectType,
      const OHOS::NWeb::NetConnectSubtype& netConnectSubtype) override;
  int32_t NetConnectionPropertiesChange() override;
  int32_t NetUnavailable() override;
  int32_t OnNetCapabilitiesChanged(
      const std::shared_ptr<OHOS::NWeb::NetCapabilitiesAdapter> capabilities)
      override;
  int32_t OnNetConnectionPropertiesChanged(
      const std::shared_ptr<OHOS::NWeb::NetConnectionPropertiesAdapter>
          properties) override;
  void BindDnsToNetwork(int32_t network_for_dns);

 private:
  void ConnectionTypeChangedTo(int32_t net_id,
                               OHOS::NWeb::NetConnectType type,
                               OHOS::NWeb::NetConnectSubtype subtype);

  net::NetworkChangeNotifierPassive* network_change_notifier_posix_ = nullptr;
  std::shared_ptr<OHOS::NWeb::NetCapabilitiesAdapter> capabilities_ = nullptr;
  std::shared_ptr<OHOS::NWeb::NetConnectionPropertiesAdapter> properties_ =
      nullptr;
  int32_t net_id_ = -1;
  int32_t network_for_dns_ = -1;
  OHOS::NWeb::NetConnectType type_ =
      OHOS::NWeb::NetConnectType::CONNECTION_UNKNOWN;
  OHOS::NWeb::NetConnectSubtype subtype_ =
      OHOS::NWeb::NetConnectSubtype::SUBTYPE_UNKNOWN;
};

int32_t NetConnCallbackImpl::NetAvailable() {
  LOG(INFO) << "ohos_network NetAvailable";
  capabilities_ = nullptr;
  properties_ = nullptr;
  return 0;
}

int32_t NetConnCallbackImpl::NetCapabilitiesChange(
    const OHOS::NWeb::NetConnectType& netConnectType,
    const OHOS::NWeb::NetConnectSubtype& netConnectSubtype) {
  LOG(INFO) << "ohos_network NetCapabilitiesChange " << static_cast<int>(netConnectType);
  if (network_change_notifier_posix_) {
    network_change_notifier_posix_->OnConnectionChanged(
        ConvertOhosConnTypeToNetBaseConnType(netConnectType));
    network_change_notifier_posix_->OnConnectionSubtypeChanged(
        ConvertOhosConnTypeToNetBaseConnType(netConnectType),
        ConvertOhosConnSubtypeToNetBaseConnSubtype(netConnectSubtype));
  }
  return 0;
}

int32_t NetConnCallbackImpl::NetConnectionPropertiesChange() {
  if (network_change_notifier_posix_) {
    network_change_notifier_posix_->OnDNSChanged();
    network_change_notifier_posix_->OnIPAddressChanged();
  }
  return 0;
}

int32_t NetConnCallbackImpl::NetUnavailable() {
  LOG(INFO) << "ohos_network NetUnavailable";
  capabilities_ = nullptr;
  properties_ = nullptr;
  if (network_change_notifier_posix_) {
      ConnectionTypeChangedTo(-1, OHOS::NWeb::NetConnectType::CONNECTION_NONE,
                              OHOS::NWeb::NetConnectSubtype::SUBTYPE_NONE);
  }
  return 0;
}


int32_t NetConnCallbackImpl::OnNetCapabilitiesChanged(
      const std::shared_ptr<OHOS::NWeb::NetCapabilitiesAdapter> capabilities) {
  capabilities_ = capabilities;
  if (capabilities_) {
    LOG(INFO) << "ohos_network NetCapabilitiesChange, net_id "
      << capabilities_->GetNetId() << ", connectType "
      << (int)capabilities_->GetConnectType() << ", subtype "
      << (int)capabilities_->GetConnectSubtype();
  }
  if (network_change_notifier_posix_ && capabilities_ && properties_) {
    ConnectionTypeChangedTo(capabilities_->GetNetId(),
                            capabilities_->GetConnectType(),
                            capabilities_->GetConnectSubtype());
  }
  return 0;
}

int32_t NetConnCallbackImpl::OnNetConnectionPropertiesChanged(
      const std::shared_ptr<OHOS::NWeb::NetConnectionPropertiesAdapter>
          properties) {
  properties_ = properties;
  if (properties_) {
    LOG(INFO) << "ohos_network NetConnectionPropertiesChange, net_id "
              << properties_->GetNetId();
  }
  if (network_change_notifier_posix_ && capabilities_ && properties_) {
    ConnectionTypeChangedTo(properties_->GetNetId(),
                            capabilities_->GetConnectType(),
                            capabilities_->GetConnectSubtype());
  }
  return 0;
}

void NetConnCallbackImpl::BindDnsToNetwork(int32_t network_for_dns) {
  LOG(INFO)
      << "NetConnCallbackImpl::BindDnsToNetwork, network_for_dns "
      << network_for_dns;
  network_for_dns_ = network_for_dns;
}

void NetConnCallbackImpl::ConnectionTypeChangedTo(
    int32_t net_id,
    OHOS::NWeb::NetConnectType type,
    OHOS::NWeb::NetConnectSubtype subtype) {
  if (network_for_dns_ != -1 && net_id != network_for_dns_) {
    LOG(INFO)
        << "ohos_network ConnectionTypeChangedTo ret, net_id "
        << net_id << ", network_for_dns_ " << network_for_dns_;
    return;
  }

  if (net_id_ != net_id || type_ != type) {
    LOG(INFO) << "ohos_network ConnectionTypeChangedTo, net_id_ " << net_id_
              << ", net_id " << net_id << ", type_ " << (int)type_ << ", type "
              << (int)type << ", network_for_dns_ " << network_for_dns_;
    network_change_notifier_posix_->OnIPAddressChanged();
    network_change_notifier_posix_->OnConnectionChanged(
        ConvertOhosConnTypeToNetBaseConnType(type));
  }

  if (subtype_ != subtype) {
    LOG(INFO) << "ohos_network ConnectionTypeChangedTo, net_id_ " << net_id_
              << ", net_id " << net_id << ", subtype_ " << (int)subtype_
              << ", subtype " << (int)subtype;
    network_change_notifier_posix_->OnConnectionSubtypeChanged(
        ConvertOhosConnTypeToNetBaseConnType(type),
        ConvertOhosConnSubtypeToNetBaseConnSubtype(subtype));
  }

  net_id_ = net_id;
  type_ = type;
  subtype_ = subtype;
}

std::shared_ptr<NetConnCallbackImpl> g_net_connect_callback = nullptr;
std::shared_ptr<VpnListenerImpl> g_vpn_listener = nullptr;
int32_t g_callback_id = -1;
#endif
}  // namespace

namespace net {

NetworkChangeNotifierPassive::NetworkChangeNotifierPassive(
    NetworkChangeNotifier::ConnectionType initial_connection_type,
    NetworkChangeNotifier::ConnectionSubtype initial_connection_subtype)
    : NetworkChangeNotifierPassive(initial_connection_type,
                                   initial_connection_subtype,
                                   /*system_dns_config_notifier=*/nullptr) {}

NetworkChangeNotifierPassive::NetworkChangeNotifierPassive(
    NetworkChangeNotifier::ConnectionType initial_connection_type,
    NetworkChangeNotifier::ConnectionSubtype initial_connection_subtype,
    SystemDnsConfigChangeNotifier* system_dns_config_notifier)
    : NetworkChangeNotifier(NetworkChangeCalculatorParamsPassive(),
                            system_dns_config_notifier),
      connection_type_(initial_connection_type),
#if BUILDFLAG(IS_OHOS)
      ohos_net_conn_adapter_(OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                 .CreateNetConnectAdapter()),
#endif
      max_bandwidth_mbps_(
          NetworkChangeNotifier::GetMaxBandwidthMbpsForConnectionSubtype(
              initial_connection_subtype)) {
#if BUILDFLAG(IS_OHOS)
  g_net_connect_callback = std::make_shared<NetConnCallbackImpl>(this);
  g_vpn_listener = std::make_shared<VpnListenerImpl>(this);
  if (ohos_net_conn_adapter_) {
    g_callback_id =
        ohos_net_conn_adapter_->RegisterNetConnCallback(g_net_connect_callback);
    if (g_callback_id < 0) {
      LOG(ERROR) << "register ohos net connect callback failed.";
    }
    ohos_net_conn_adapter_->RegisterVpnListener(g_vpn_listener);
  }
#endif
}

NetworkChangeNotifierPassive::~NetworkChangeNotifierPassive() {
  ClearGlobalPointer();
#if BUILDFLAG(IS_OHOS)
  if (ohos_net_conn_adapter_) {
    int32_t ret = ohos_net_conn_adapter_->UnregisterNetConnCallback(
        g_callback_id);
    if (ret != 0) {
      LOG(ERROR) << "unregister ohos net connect callback failed.";
    }
    ohos_net_conn_adapter_->UnRegisterVpnListener();
  }
#endif
}

void NetworkChangeNotifierPassive::OnDNSChanged() {
  GetCurrentSystemDnsConfigNotifier()->RefreshConfig();
}

void NetworkChangeNotifierPassive::OnIPAddressChanged() {
  // DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  NetworkChangeNotifier::NotifyObserversOfIPAddressChange();
}

#if BUILDFLAG(IS_OHOS)
void NetworkChangeNotifierPassive::OnVpnAvailable() {
  LOG(INFO) << "ohos_network on vpn available";
  vpn_in_place_ = true;
  OnIPAddressChanged();
  OnConnectionChanged(connection_type_);
}

void NetworkChangeNotifierPassive::OnVpnLost() {
  LOG(INFO) << "ohos_network on vpn lost";
  vpn_in_place_ = false;
  OnIPAddressChanged();
  OnConnectionChanged(connection_type_);
}
#endif

void NetworkChangeNotifierPassive::OnConnectionChanged(
    NetworkChangeNotifier::ConnectionType connection_type) {
  // DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  {
    base::AutoLock scoped_lock(lock_);
    connection_type_ = connection_type;
  }

#if BUILDFLAG(IS_OHOS) && defined(OHOS_EX_HTTP_DNS_FALLBACK)
  std::vector<std::string> dns_servers;
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
        switches::kForBrowser)) {
    if (ohos_net_conn_adapter_) {
      if (vpn_in_place_) {
        dns_servers = ohos_net_conn_adapter_->GetDnsServersForVpn();
      } else {
        dns_servers = ohos_net_conn_adapter_->GetDnsServers();
      }
    }
  }
  {
    base::AutoLock scoped_lock(dns_server_lock_);
    if (!dns_servers.empty()) {
      dns_servers_ = std::move(dns_servers);
    } else {
      LOG(ERROR) << "OnConnectionChanged, ohos_network dns server is empty.";
    }
  }
#endif

  NetworkChangeNotifier::NotifyObserversOfConnectionTypeChange();
}

void NetworkChangeNotifierPassive::OnConnectionSubtypeChanged(
    NetworkChangeNotifier::ConnectionType connection_type,
    NetworkChangeNotifier::ConnectionSubtype connection_subtype) {
  // DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  double max_bandwidth_mbps =
      GetMaxBandwidthMbpsForConnectionSubtype(connection_subtype);
  {
    base::AutoLock scoped_lock(lock_);
    max_bandwidth_mbps_ = max_bandwidth_mbps;
  }
  NetworkChangeNotifier::NotifyObserversOfMaxBandwidthChange(max_bandwidth_mbps,
                                                             connection_type);
}

NetworkChangeNotifier::ConnectionType
NetworkChangeNotifierPassive::GetCurrentConnectionType() const {
  base::AutoLock scoped_lock(lock_);
  return connection_type_;
}

void NetworkChangeNotifierPassive::GetCurrentMaxBandwidthAndConnectionType(
    double* max_bandwidth_mbps,
    ConnectionType* connection_type) const {
  base::AutoLock scoped_lock(lock_);
  *connection_type = connection_type_;
  *max_bandwidth_mbps = max_bandwidth_mbps_;
}

#if BUILDFLAG(IS_LINUX)
AddressMapOwnerLinux*
NetworkChangeNotifierPassive::GetAddressMapOwnerInternal() {
  return &address_map_cache_;
}
#endif

// static
NetworkChangeNotifier::NetworkChangeCalculatorParams
NetworkChangeNotifierPassive::NetworkChangeCalculatorParamsPassive() {
  NetworkChangeCalculatorParams params;
#if BUILDFLAG(IS_CHROMEOS)
  // Delay values arrived at by simple experimentation and adjusted so as to
  // produce a single signal when switching between network connections.
  params.ip_address_offline_delay_ = base::Milliseconds(4000);
  params.ip_address_online_delay_ = base::Milliseconds(1000);
  params.connection_type_offline_delay_ = base::Milliseconds(500);
  params.connection_type_online_delay_ = base::Milliseconds(500);
#elif BUILDFLAG(IS_ANDROID)
  params = NetworkChangeNotifierAndroid::NetworkChangeCalculatorParamsAndroid();
#elif BUILDFLAG(IS_LINUX)
  params = NetworkChangeNotifierLinux::NetworkChangeCalculatorParamsLinux();
#else
  NOTIMPLEMENTED();
#endif
  return params;
}

#if BUILDFLAG(IS_OHOS) && defined(OHOS_EX_HTTP_DNS_FALLBACK)
const std::vector<std::string>
NetworkChangeNotifierPassive::GetCurrentDnsServers() {
  base::AutoLock scoped_lock(dns_server_lock_);
  return dns_servers_;
}
#endif

#if BUILDFLAG(IS_OHOS) && defined(OHOS_EX_NETWORK_CONNECTION)
void NetworkChangeNotifierPassive::BindDnsToNetwork(int32_t network_for_dns) {
  if (g_net_connect_callback) {
    g_net_connect_callback->BindDnsToNetwork(network_for_dns);
  }
}
#endif

}  // namespace net
