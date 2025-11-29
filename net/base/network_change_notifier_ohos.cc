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

#include "net/base/network_change_notifier_ohos.h"

#include <string>
#include <unordered_set>
#include <utility>

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/task/task_traits.h"
#include "BasicServicesKit/oh_commonevent_support.h"
#include "BasicServicesKit/oh_commonevent.h"
#include "build/build_config.h"
#include "net/dns/dns_config_service_ohos.h"
#include "net/dns/system_dns_config_change_notifier.h"
#include "network/netmanager/net_connection_type.h"
#include "network/netmanager/net_connection.h"

namespace net {

const int kNetDelayMilliseconds = 500;

NetworkChangeNotifier::ConnectionType ConnTypeConvert(NetConn_NetBearerType connection_type) {
  NetworkChangeNotifier::ConnectionType native_conn_type;
  switch (connection_type) {
    case NetConn_NetBearerType::NETCONN_BEARER_ETHERNET:
      native_conn_type =
          NetworkChangeNotifier::ConnectionType::CONNECTION_ETHERNET;
      break;

    case NetConn_NetBearerType::NETCONN_BEARER_WIFI:
      native_conn_type = NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI;
      break;

    case NetConn_NetBearerType::NETCONN_BEARER_BLUETOOTH:
      native_conn_type =
          NetworkChangeNotifier::ConnectionType::CONNECTION_BLUETOOTH;
      break;

    case NetConn_NetBearerType::NETCONN_BEARER_CELLULAR:
      native_conn_type = NetworkChangeNotifier::ConnectionType::CONNECTION_4G;
      break;

    default:
      native_conn_type =
          NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN;
      LOG(WARNING) << "NetworkChange ConnectionType Unknown";
  }
  return native_conn_type;
}

void NetAvailable(NetConn_NetHandle* net_handle) {
  LOG(INFO) << "Triggered when the network is available.";
  NOTIMPLEMENTED();
}

void NetCapabilitiesChange(NetConn_NetHandle* net_handle,
                           NetConn_NetCapabilities* net_capabilities) {
  if (net_capabilities == nullptr) {
    LOG(ERROR) << "NetCapabilitiesChange, net_capabilities is nullptr";
    return;
  }
  LOG(INFO) << "NetCapabilitiesChange, net_capabilities: " << ToString(net_capabilities);
  NetworkChangeNotifier::ConnectionType connection_type =
    NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN;
  if (net_capabilities->bearerTypesSize > 0) {
    connection_type = ConnTypeConvert(net_capabilities->bearerTypes[0]);
    LOG(INFO) << "NetCapabilitiesChange, connection_type: " << static_cast<int>(connection_type);
  }

  auto notifier_ohos = net::NetworkChangeNotifierOhos::GetInstance();
  if (notifier_ohos == nullptr) {
    LOG(ERROR) << "NetCapabilitiesChange, notifier_ohos is nullptr";
    return;
  }
  notifier_ohos->OnConnectionChanged(connection_type);
  notifier_ohos->OnConnectionSubtypeChanged(
      connection_type,
      NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_UNKNOWN);
}

void NetConnectionPropertiesChange(NetConn_NetHandle* net_handle,
                                   NetConn_ConnectionProperties* conn_connetion_properties) {
  LOG(INFO) << "NetConnectionPropertiesChange";
  if (conn_connetion_properties == nullptr) {
    LOG(ERROR) << "NetConnectionPropertiesChange, conn_connetion_properties is nullptr";
    return;
  }
  auto notifier_ohos = net::NetworkChangeNotifierOhos::GetInstance();
  if (notifier_ohos == nullptr) {
    LOG(ERROR) << "NetConnectionPropertiesChange, notifier_ohos is nullptr";
    return;
  }
  notifier_ohos->OnDNSChanged();
  notifier_ohos->OnIPAddressChanged();
}

void NetUnavailable() {
  LOG(INFO) << "NetUnavailable";
  auto notifier_ohos = net::NetworkChangeNotifierOhos::GetInstance();
  if (notifier_ohos == nullptr) {
    LOG(ERROR) << "NetUnavailable, notifier_ohos is nullptr";
    return;
  }
  notifier_ohos->ConnectionUnavailable();
}

void NetLost(NetConn_NetHandle* net_handle) {
  LOG(INFO) << "NetLost";
  NetUnavailable();
}

void OnCommonEventConnectiveChange(const CommonEvent_RcvData* data) {
  LOG(INFO) << "OnCommonEventConnectiveChange";
  auto notifier_ohos = net::NetworkChangeNotifierOhos::GetInstance();
  notifier_ohos->OnDNSChanged();
  notifier_ohos->OnIPAddressChanged();
}

static NetworkChangeNotifierOhos* g_notifier_ohos = nullptr;

NetworkChangeNotifierOhos::NetworkChangeNotifierOhos(
    NetworkChangeNotifier::ConnectionType initial_connection_type,
    NetworkChangeNotifier::ConnectionSubtype initial_connection_subtype)
    : NetworkChangeNotifierOhos(initial_connection_type,
                                initial_connection_subtype,
                                /*system_dns_config_notifier=*/nullptr) {}

NetworkChangeNotifierOhos::NetworkChangeNotifierOhos(
    NetworkChangeNotifier::ConnectionType initial_connection_type,
    NetworkChangeNotifier::ConnectionSubtype initial_connection_subtype,
    SystemDnsConfigChangeNotifier* system_dns_config_notifier)
    : NetworkChangeNotifier(NetworkChangeCalculatorParamsOhos(),
                            system_dns_config_notifier),
      connection_type_(initial_connection_type),
      max_bandwidth_mbps_(
          NetworkChangeNotifier::GetMaxBandwidthMbpsForConnectionSubtype(
              initial_connection_subtype)) {
  g_notifier_ohos = this;
  if (!NetConnectionInit()) {
    LOG(ERROR) << "NetConnectionInit Failed";
  }
}

NetworkChangeNotifierOhos::~NetworkChangeNotifierOhos() {
  NetConnectionDestroy();
  g_notifier_ohos = nullptr;
}

bool NetworkChangeNotifierOhos::NetConnectionInit() {
  callbacks_.onNetworkAvailable = &NetAvailable;
  callbacks_.onNetCapabilitiesChange= &NetCapabilitiesChange;
  callbacks_.onConnetionProperties = &NetConnectionPropertiesChange;
  callbacks_.onNetLost = &NetLost;
  callbacks_.onNetUnavailable = &NetUnavailable;

  int32_t result =
      OH_NetConn_RegisterDefaultNetConnCallback(&callbacks_, &callbackId_);
  if (result != 0) {
    LOG(ERROR) << "OH_NetConn_RegisterDefaultNetConnCallback ERROR, error code: " << result;
    return false;
  }
  bool subscribe_result = SubscribeNetChangeEvent();
  if (!subscribe_result) {
    LOG(ERROR) << "SubscribeNetChangeEvent FAIL";
    return false;
  }
  return true;
}

void NetworkChangeNotifierOhos::NetConnectionDestroy() {
  int32_t result = OH_NetConn_UnregisterNetConnCallback(callbackId_);
  if (result != 0) {
    LOG(ERROR) << "OH_NetConn_UnregisterNetConnCallback ERROR, error code: " << result;
  }
  UnSubscribeNetChangeEvent();
}

bool NetworkChangeNotifierOhos::SubscribeNetChangeEvent() {
  CommonEvent_SubscribeInfo* subscriber_info = OH_CommonEvent_CreateSubscribeInfo(common_events_, events_num_);
  if (subscriber_info == nullptr) {
    LOG(ERROR) << "OH_CommonEvent_CreateSubscribeInfo error";
    return false;
  }
  subscriber_ = OH_CommonEvent_CreateSubscriber(subscriber_info, OnCommonEventConnectiveChange);
  OH_CommonEvent_DestroySubscribeInfo(subscriber_info);
  if (subscriber_ == nullptr) {
    LOG(ERROR) << "OH_CommonEvent_CreateSubscriber error";
    return false;
  }
  CommonEvent_ErrCode err_code = OH_CommonEvent_Subscribe(subscriber_);
  if (err_code != COMMONEVENT_ERR_OK) {
    LOG(ERROR) << "OH_CommonEvent_Subscribe error, error code: " << static_cast<int>(err_code);
    return false;
  }
  LOG(INFO) << "SubscribeNetChangeEvent SUCCESSFULLY";
  return true;
}

void NetworkChangeNotifierOhos::UnSubscribeNetChangeEvent() {
  CommonEvent_ErrCode err_code = OH_CommonEvent_UnSubscribe(subscriber_);
  if (err_code != COMMONEVENT_ERR_OK) {
    LOG(ERROR) << "OH_CommonEvent_UnSubscribe error, error code: " << static_cast<int>(err_code);
  }
  OH_CommonEvent_DestroySubscriber(subscriber_);
}

//static
NetworkChangeNotifierOhos* NetworkChangeNotifierOhos::GetInstance() {
  return g_notifier_ohos;
}

void NetworkChangeNotifierOhos::OnDNSChanged() {
  GetCurrentSystemDnsConfigNotifier()->RefreshConfig();
}

void NetworkChangeNotifierOhos::OnIPAddressChanged() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  NetworkChangeNotifier::NotifyObserversOfIPAddressChange();
}

void NetworkChangeNotifierOhos::OnConnectionChanged(
    NetworkChangeNotifier::ConnectionType connection_type) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  {
    base::AutoLock scoped_lock(lock_);
    connection_type_ = connection_type;
  }
  NetworkChangeNotifier::NotifyObserversOfConnectionTypeChange();
}

void NetworkChangeNotifierOhos::OnConnectionSubtypeChanged(
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

void NetworkChangeNotifierOhos::ConnectionUnavailable() {
  OnConnectionChanged(NetworkChangeNotifier::ConnectionType::CONNECTION_NONE);
  OnConnectionSubtypeChanged(
      NetworkChangeNotifier::ConnectionType::CONNECTION_NONE,
      NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_NONE);
}

NetworkChangeNotifier::ConnectionType
NetworkChangeNotifierOhos::GetCurrentConnectionType() const {
  base::AutoLock scoped_lock(lock_);
  return connection_type_;
}

void NetworkChangeNotifierOhos::GetCurrentMaxBandwidthAndConnectionType(
    double* max_bandwidth_mbps,
    ConnectionType* connection_type) const {
  base::AutoLock scoped_lock(lock_);
  *connection_type = connection_type_;
  *max_bandwidth_mbps = max_bandwidth_mbps_;
}

// static
NetworkChangeNotifier::NetworkChangeCalculatorParams
NetworkChangeNotifierOhos::NetworkChangeCalculatorParamsOhos() {
  NetworkChangeCalculatorParams params;
  params.ip_address_offline_delay_ = base::Milliseconds(kNetDelayMilliseconds);
  params.ip_address_online_delay_ = base::Milliseconds(kNetDelayMilliseconds);
  params.connection_type_offline_delay_ =
      base::Milliseconds(kNetDelayMilliseconds);
  params.connection_type_online_delay_ =
      base::Milliseconds(kNetDelayMilliseconds);
  return params;
}

}  // namespace net
