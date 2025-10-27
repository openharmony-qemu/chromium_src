// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_NETWORK_CHANGE_NOTIFIER_OHOS_H_
#define NET_BASE_NETWORK_CHANGE_NOTIFIER_OHOS_H_

#include "base/gtest_prod_util.h"
#include "base/sequence_checker.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread.h"
#include "base/threading/thread_checker.h"
#include "net/base/net_export.h"
#include "net/base/network_change_notifier.h"

#include "ohos/adapter/net/connection_adapter.h"

namespace net {

class NET_EXPORT NetworkChangeNotifierOhos : public NetworkChangeNotifier {
 public:
  NetworkChangeNotifierOhos(
      NetworkChangeNotifier::ConnectionType initial_connection_type,
      NetworkChangeNotifier::ConnectionSubtype initial_connection_subtype);
  NetworkChangeNotifierOhos(const NetworkChangeNotifierOhos&) = delete;
  NetworkChangeNotifierOhos& operator=(const NetworkChangeNotifierOhos&) =
      delete;
  ~NetworkChangeNotifierOhos() override;

  // These methods are used to notify this object that a network property has
  // changed. These must be called from the thread that owns this object.
  void OnDNSChanged();
  void OnIPAddressChanged();
  void OnConnectionChanged(
      NetworkChangeNotifier::ConnectionType connection_type);
  void OnConnectionSubtypeChanged(
      NetworkChangeNotifier::ConnectionType connection_type,
      NetworkChangeNotifier::ConnectionSubtype connection_subtype);
  std::shared_ptr<ohos::adapter::net::NetConnectionCallback> connectionCallback() {
    return net_connection_callback_;
  }

 protected:
  // NetworkChangeNotifier overrides.
  NetworkChangeNotifier::ConnectionType GetCurrentConnectionType()
      const override;
  void GetCurrentMaxBandwidthAndConnectionType(
      double* max_bandwidth_mbps,
      ConnectionType* connection_type) const override;

 private:
  friend class NetworkChangeNotifierOhosTest;

  // For testing purposes, allows specifying a SystemDnsConfigChangeNotifier.
  // If |system_dns_config_notifier| is nullptr, NetworkChangeNotifier create a
  // global one.
  NetworkChangeNotifierOhos(
      NetworkChangeNotifier::ConnectionType initial_connection_type,
      NetworkChangeNotifier::ConnectionSubtype initial_connection_subtype,
      SystemDnsConfigChangeNotifier* system_dns_config_notifier);

  // Calculates parameters used for network change notifier online/offline
  // signals.
  static NetworkChangeNotifier::NetworkChangeCalculatorParams
  NetworkChangeCalculatorParamsOhos();

  THREAD_CHECKER(thread_checker_);

  mutable base::Lock lock_;
  NetworkChangeNotifier::ConnectionType
      connection_type_;        // Guarded by |lock_|.

  std::unique_ptr<ohos::adapter::net::NetConnectionAdapter> net_connection_adapter_ = nullptr;
  std::shared_ptr<ohos::adapter::net::NetConnectionCallback> net_connection_callback_ = nullptr;

  double max_bandwidth_mbps_;  // Guarded by |lock_|.
};

}  // namespace net

#endif  // NET_BASE_NETWORK_CHANGE_NOTIFIER_OHOS_H_
