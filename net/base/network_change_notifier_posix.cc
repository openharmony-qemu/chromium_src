// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <utility>

#include "base/bind.h"
#include "base/logging.h"
#include "base/task/post_task.h"
#include "base/task/task_traits.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "net/base/network_change_notifier_posix.h"
#include "net/dns/dns_config_service_posix.h"
#include "net/dns/system_dns_config_change_notifier.h"

#if BUILDFLAG(IS_ANDROID)
#include "net/android/network_change_notifier_android.h"
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

class NetConnCallbackImpl : public OHOS::NWeb::NetConnCallback {
 public:
  NetConnCallbackImpl(
      net::NetworkChangeNotifierPosix* network_change_notifier_posix)
      : network_change_notifier_posix_(network_change_notifier_posix) {}
  virtual ~NetConnCallbackImpl() = default;
  int32_t NetAvailable() override;
  int32_t NetCapabilitiesChange(
      const OHOS::NWeb::NetConnectType& netConnectType,
      const OHOS::NWeb::NetConnectSubtype& netConnectSubtype) override;
  int32_t NetConnectionPropertiesChange() override;
  int32_t NetUnavailable() override;

 private:
  net::NetworkChangeNotifierPosix* network_change_notifier_posix_ = nullptr;
};

int32_t NetConnCallbackImpl::NetAvailable() {
  return 0;
}

int32_t NetConnCallbackImpl::NetCapabilitiesChange(
    const OHOS::NWeb::NetConnectType& netConnectType,
    const OHOS::NWeb::NetConnectSubtype& netConnectSubtype) {
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
  if (network_change_notifier_posix_) {
    network_change_notifier_posix_->OnConnectionChanged(
        net::NetworkChangeNotifier::ConnectionType::CONNECTION_NONE);
    network_change_notifier_posix_->OnConnectionSubtypeChanged(
        net::NetworkChangeNotifier::ConnectionType::CONNECTION_NONE,
        net::NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_NONE);
  }
  return 0;
}

std::shared_ptr<NetConnCallbackImpl> g_net_connect_callback = nullptr;
#endif
}  // namespace

namespace net {

NetworkChangeNotifierPosix::NetworkChangeNotifierPosix(
    NetworkChangeNotifier::ConnectionType initial_connection_type,
    NetworkChangeNotifier::ConnectionSubtype initial_connection_subtype)
    : NetworkChangeNotifierPosix(initial_connection_type,
                                 initial_connection_subtype,
                                 /*system_dns_config_notifier=*/nullptr) {}

NetworkChangeNotifierPosix::NetworkChangeNotifierPosix(
    NetworkChangeNotifier::ConnectionType initial_connection_type,
    NetworkChangeNotifier::ConnectionSubtype initial_connection_subtype,
    SystemDnsConfigChangeNotifier* system_dns_config_notifier)
    : NetworkChangeNotifier(NetworkChangeCalculatorParamsPosix(),
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
  if (ohos_net_conn_adapter_) {
    int32_t ret =
        ohos_net_conn_adapter_->RegisterNetConnCallback(g_net_connect_callback);
    if (ret != 0) {
      LOG(ERROR) << "register ohos net connect callback failed.";
    }
  }
#endif
}

NetworkChangeNotifierPosix::~NetworkChangeNotifierPosix() {
  ClearGlobalPointer();
#if BUILDFLAG(IS_OHOS)
  if (ohos_net_conn_adapter_) {
    int32_t ret = ohos_net_conn_adapter_->UnregisterNetConnCallback(
        g_net_connect_callback);
    if (ret != 0) {
      LOG(ERROR) << "unregister ohos net connect callback failed.";
    }
  }
#endif
}

void NetworkChangeNotifierPosix::OnDNSChanged() {
  GetCurrentSystemDnsConfigNotifier()->RefreshConfig();
}

void NetworkChangeNotifierPosix::OnIPAddressChanged() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  NetworkChangeNotifier::NotifyObserversOfIPAddressChange();
}

void NetworkChangeNotifierPosix::OnConnectionChanged(
    NetworkChangeNotifier::ConnectionType connection_type) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  {
    base::AutoLock scoped_lock(lock_);
    connection_type_ = connection_type;
  }
  NetworkChangeNotifier::NotifyObserversOfConnectionTypeChange();
}

void NetworkChangeNotifierPosix::OnConnectionSubtypeChanged(
    NetworkChangeNotifier::ConnectionType connection_type,
    NetworkChangeNotifier::ConnectionSubtype connection_subtype) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
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
NetworkChangeNotifierPosix::GetCurrentConnectionType() const {
  base::AutoLock scoped_lock(lock_);
  return connection_type_;
}

void NetworkChangeNotifierPosix::GetCurrentMaxBandwidthAndConnectionType(
    double* max_bandwidth_mbps,
    ConnectionType* connection_type) const {
  base::AutoLock scoped_lock(lock_);
  *connection_type = connection_type_;
  *max_bandwidth_mbps = max_bandwidth_mbps_;
}

// static
NetworkChangeNotifier::NetworkChangeCalculatorParams
NetworkChangeNotifierPosix::NetworkChangeCalculatorParamsPosix() {
  NetworkChangeCalculatorParams params;
#if BUILDFLAG(IS_CHROMEOS_ASH)
  // Delay values arrived at by simple experimentation and adjusted so as to
  // produce a single signal when switching between network connections.
  params.ip_address_offline_delay_ = base::Milliseconds(4000);
  params.ip_address_online_delay_ = base::Milliseconds(1000);
  params.connection_type_offline_delay_ = base::Milliseconds(500);
  params.connection_type_online_delay_ = base::Milliseconds(500);
#elif BUILDFLAG(IS_ANDROID)
  params =
      net::NetworkChangeNotifierAndroid::NetworkChangeCalculatorParamsAndroid();
#else
  NOTIMPLEMENTED();
#endif
  return params;
}

}  // namespace net
