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

#include "net/base/network_change_notifier_ohos.h"

#include <string>
#include <unordered_set>
#include <utility>

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/task/task_traits.h"
#include "build/build_config.h"
#include "net/dns/dns_config_service_ohos.h"
#include "net/dns/system_dns_config_change_notifier.h"

namespace {
using namespace ohos::adapter::net;
using namespace net;

const int kNetDelayMilliseconds = 500;

class NetConnectionCallbackImpl : public NetConnectionCallback {
 public:
  explicit NetConnectionCallbackImpl(
      NetworkChangeNotifierOhos* network_change_notifier_ohos)
      : notifier_ohos_(network_change_notifier_ohos) {}
  virtual ~NetConnectionCallbackImpl() = default;
  int32_t NetAvailable() override;
  int32_t NetCapabilitiesChange(
      const NetConnectionType& netConnectType,
      const NetConnectionSubtype& netConnectSubtype) override;
  int32_t NetConnectionPropertiesChange() override;
  int32_t NetUnavailable() override;
  int32_t NetLost() override;

 private:
  raw_ptr<NetworkChangeNotifierOhos> notifier_ohos_ = nullptr;
};

int32_t NetConnectionCallbackImpl::NetAvailable() {
  NOTIMPLEMENTED();
  return 0;
}

int32_t NetConnectionCallbackImpl::NetCapabilitiesChange(
    const NetConnectionType& netConnectType,
    const NetConnectionSubtype& netConnectSubtype) {
  if (notifier_ohos_) {
    notifier_ohos_->OnConnectionChanged(
        ConvertOhosConnTypeToNativeConnType<
            NetworkChangeNotifier::ConnectionType, NetConnectionType>(
            netConnectType));

    notifier_ohos_->OnConnectionSubtypeChanged(
        ConvertOhosConnTypeToNativeConnType<
            NetworkChangeNotifier::ConnectionType, NetConnectionType>(
            netConnectType),
        ConvertOhosConnTypeToNativeConnType<
            NetworkChangeNotifier::ConnectionSubtype, NetConnectionSubtype>(
            netConnectSubtype));
  }
  return 0;
}

int32_t NetConnectionCallbackImpl::NetConnectionPropertiesChange() {
  if (notifier_ohos_) {
    notifier_ohos_->OnDNSChanged();
    notifier_ohos_->OnIPAddressChanged();
  }
  return 0;
}

int32_t NetConnectionCallbackImpl::NetUnavailable() {
  if (notifier_ohos_) {
    notifier_ohos_->OnConnectionChanged(
        NetworkChangeNotifier::ConnectionType::CONNECTION_NONE);
    notifier_ohos_->OnConnectionSubtypeChanged(
        NetworkChangeNotifier::ConnectionType::CONNECTION_NONE,
        NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_NONE);
  }
  return 0;
}

int32_t NetConnectionCallbackImpl::NetLost() {
  if (notifier_ohos_) {
    notifier_ohos_->OnConnectionChanged(
        NetworkChangeNotifier::ConnectionType::CONNECTION_NONE);
    notifier_ohos_->OnConnectionSubtypeChanged(
        NetworkChangeNotifier::ConnectionType::CONNECTION_NONE,
        NetworkChangeNotifier::ConnectionSubtype::SUBTYPE_NONE);
  }
  return 0;
}
}  // namespace

namespace net {

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
      net_connection_adapter_(NetConnectionAdapterHelper::GetInstance()
                                  .CreateNetConnectionAdapter()),
      max_bandwidth_mbps_(
          NetworkChangeNotifier::GetMaxBandwidthMbpsForConnectionSubtype(
              initial_connection_subtype)) {
  net_connection_callback_ = std::make_shared<NetConnectionCallbackImpl>(this);
  if (net_connection_adapter_) {
    int32_t ret = net_connection_adapter_->RegisterNetConnectionCallback(
        net_connection_callback_);
    if (ret != 0) {
      LOG(ERROR) << "register ohos net connection callback failed.";
    }
  }
}

NetworkChangeNotifierOhos::~NetworkChangeNotifierOhos() {
  ClearGlobalPointer();
  if (net_connection_adapter_) {
    int32_t ret = net_connection_adapter_->UnregisterNetConnectionCallback(
        net_connection_callback_);
    if (ret != 0) {
      LOG(ERROR) << "unregister ohos net connection callback failed.";
    }
    net_connection_adapter_.reset();
  }
  if (net_connection_callback_ != nullptr) {
    net_connection_callback_.reset();
  }
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
