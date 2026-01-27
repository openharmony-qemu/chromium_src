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

#include "base/logging.h"
#include "arkweb/build/features/features.h"

#if BUILDFLAG(IS_LINUX)
#include <linux/rtnetlink.h>

#include "net/base/network_change_notifier_linux.h"
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "net/dns/public/dns_protocol.h"
#endif

#include "base/no_destructor.h"
#include "arkweb/chromium_ext/net/base/network_change_notifier_passive_for_include.cc"

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
    : ArkwebNetworkChangeNotifierExt(NetworkChangeCalculatorParamsPassive(),
                                     system_dns_config_notifier),
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
      ohos_net_conn_adapter_(OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                 .CreateNetConnectAdapter()),
#endif
      connection_type_(initial_connection_type),
      max_bandwidth_mbps_(
          NetworkChangeNotifier::GetMaxBandwidthMbpsForConnectionSubtype(
              initial_connection_subtype)) {
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  NetworkChangeNotifierPassiveUtils::RegisterOhosNetConnCallback(this);
#endif
}


NetworkChangeNotifierPassive::~NetworkChangeNotifierPassive() {
  ClearGlobalPointer();
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  NetworkChangeNotifierPassiveUtils::UnRegisterOhosNetConnCallback(this);
#endif
}

void NetworkChangeNotifierPassive::OnDNSChanged() {
  GetCurrentSystemDnsConfigNotifier()->RefreshConfig();
}

void NetworkChangeNotifierPassive::OnIPAddressChanged() {
#if !BUILDFLAG(ARKWEB_NETWORK_BASE)
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
#endif
  NetworkChangeNotifier::NotifyObserversOfIPAddressChange();
}

void NetworkChangeNotifierPassive::OnConnectionChanged(
    NetworkChangeNotifier::ConnectionType connection_type) {
#if !BUILDFLAG(ARKWEB_NETWORK_BASE)
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
#endif
  {
    base::AutoLock scoped_lock(lock_);
    connection_type_ = connection_type;
  }

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
  NetworkChangeNotifierPassiveUtils::SetDnsServers(this);
#endif

  NetworkChangeNotifier::NotifyObserversOfConnectionTypeChange();
}

void NetworkChangeNotifierPassive::OnConnectionSubtypeChanged(
    NetworkChangeNotifier::ConnectionType connection_type,
    NetworkChangeNotifier::ConnectionSubtype connection_subtype) {
#if !BUILDFLAG(ARKWEB_NETWORK_BASE)
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
#endif
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

}  // namespace net
