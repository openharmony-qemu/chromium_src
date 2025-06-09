/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
