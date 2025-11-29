/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include <sstream>
#include <string>

#include "base/gtest_prod_util.h"
#include "base/sequence_checker.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread.h"
#include "base/threading/thread_checker.h"
#include "net/base/net_export.h"
#include "net/base/network_change_notifier.h"

#include "network/netmanager/net_connection_type.h"
#include "network/netmanager/net_connection.h"
#include "BasicServicesKit/oh_commonevent_support.h"
#include "BasicServicesKit/oh_commonevent.h"

#include "ohos/adapter/net/connection_adapter.h"

namespace net {

inline std::string ToString(NetConn_NetCapabilities* net_capabilities);
inline std::string ToString(const NetConn_NetCap* net_capabilities, int32_t net_caps_size);
inline std::string ToString(const NetConn_NetBearerType* bearerTypes, int32_t bearer_types_size);

inline std::string ToString(NetConn_NetCapabilities* net_capabilities) {
  std::stringstream stream;
  stream << "{linkUpBandwidthKbps: " << net_capabilities->linkUpBandwidthKbps
         << ", linkDownBandwidthKbps: " << net_capabilities->linkDownBandwidthKbps
         << ", netCapsSize: " << net_capabilities->netCapsSize
         << ", netCaps: " << ToString(net_capabilities->netCaps, net_capabilities->netCapsSize)
         << ", bearerTypesSize: " << net_capabilities->bearerTypesSize
         << ", bearerTypes: " << ToString(net_capabilities->bearerTypes, net_capabilities->bearerTypesSize);
  return stream.str();
}

inline std::string ToString(const NetConn_NetCap* net_capabilities, int32_t net_caps_size) {
  if (net_caps_size == 0)
    return " ";
  std::stringstream stream;
  stream << "{";
  for (int index = 0; index < net_caps_size - 1; index++) {
    stream << static_cast<int32_t>(net_capabilities[index]) << ", ";
  }
  stream << static_cast<int32_t>(net_capabilities[net_caps_size - 1]) << "}";
  return stream.str();
}

inline std::string ToString(const NetConn_NetBearerType* bearer_types, int32_t bearer_types_size) {
  if (bearer_types_size == 0)
    return " ";
  std::stringstream stream;
  stream << "{";
  for (int index = 0; index < bearer_types_size - 1; index++) {
    stream << static_cast<int32_t>(bearer_types[index]) << ", ";
  }
  stream << static_cast<int32_t>(bearer_types[bearer_types_size - 1]) << "}";
  return stream.str();
}

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
  void ConnectionUnavailable();

  static NetworkChangeNotifierOhos* GetInstance();

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

  double max_bandwidth_mbps_;  // Guarded by |lock_|.

  //watch net connection
  bool NetConnectionInit();
  void NetConnectionDestroy();
  bool SubscribeNetChangeEvent();
  void UnSubscribeNetChangeEvent();
  NetConn_NetConnCallback callbacks_;
  uint32_t callbackId_ = 0;
  const char* common_events_[1] = {COMMON_EVENT_CONNECTIVITY_CHANGE};
  int32_t events_num_ = std::size(common_events_);
  CommonEvent_Subscriber* subscriber_ = nullptr;
  ConnectionType ConnTypeConvert(NetConn_NetBearerType connection_type);
};

}  // namespace net

#endif  // NET_BASE_NETWORK_CHANGE_NOTIFIER_OHOS_H_
