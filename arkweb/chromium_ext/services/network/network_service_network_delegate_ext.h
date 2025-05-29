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
#ifndef SERVICES_NETWORK_NETWORK_SERVICE_NETWORK_DELEGATE_EXT_H_
#define SERVICES_NETWORK_NETWORK_SERVICE_NETWORK_DELEGATE_EXT_H_

#include <optional>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/component_export.h"
#include "base/memory/raw_ptr.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/base/completion_once_callback.h"
#include "net/base/network_delegate_impl.h"
#include "net/cookies/cookie_setting_override.h"
#include "net/first_party_sets/first_party_set_metadata.h"
#include "net/first_party_sets/first_party_sets_cache_filter.h"
#include "net/url_request/url_request.h"
#include "services/network/cookie_settings.h"
#include "services/network/network_context.h"
#include "services/network/network_service_network_delegate.h"

namespace network {

class COMPONENT_EXPORT(NETWORK_SERVICE) NetworkServiceNetworkDelegateExt
    : public NetworkServiceNetworkDelegate {
 public:
  explicit NetworkServiceNetworkDelegateExt(
      bool enable_referrers,
      bool validate_referrer_policy_on_initial_request,
      mojo::PendingRemote<mojom::ProxyErrorClient> proxy_error_client_remote,
      NetworkContext* network_context)
      : NetworkServiceNetworkDelegate(
            enable_referrers,
            validate_referrer_policy_on_initial_request,
            std::move(proxy_error_client_remote),  // 关键修改：添加 std::move
            network_context) {}

  NetworkServiceNetworkDelegateExt(const NetworkServiceNetworkDelegateExt&) =
      delete;
  NetworkServiceNetworkDelegateExt& operator=(
      const NetworkServiceNetworkDelegateExt&) = delete;

  NetworkServiceNetworkDelegateExt* AsNetworkServiceNetworkDelegateExt()
      override {
    return this;
  }

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
  net::AddressList ResolveFromCacheInternal(net::HostCache::Key& key);
  void RecordErrorInfo(net::URLRequest* request, int net_error);
  int32_t GetDownStreamThroughputKbps();
#endif
};

}  // namespace network

#endif  // SERVICES_NETWORK_NETWORK_SERVICE_NETWORK_DELEGATE_EXT_H_