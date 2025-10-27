/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef CEF_LIBCEF_BROWSER_NET_SERVICE_PROXY_CONFIG_MONITOR_H_
#define CEF_LIBCEF_BROWSER_NET_SERVICE_PROXY_CONFIG_MONITOR_H_

#include <memory>
#include <string>
#include <vector>

#include "base/no_destructor.h"
#include "mojo/public/cpp/bindings/remote_set.h"
#include "net/proxy_resolution/proxy_config_service_ohos.h"
#include "services/network/public/mojom/network_service.mojom.h"

namespace network {
namespace mojom {
class ProxyConfigClient;
}
}  // namespace network

namespace NWEB {

// This class configures proxy settings for NetworkContext if network service
// is enabled.
class ProxyConfigMonitor : public net::ProxyConfigService::Observer {
 public:
  ProxyConfigMonitor(const ProxyConfigMonitor&) = delete;
  ProxyConfigMonitor& operator=(const ProxyConfigMonitor&) = delete;

  static ProxyConfigMonitor* GetInstance();

  void AddProxyToNetworkContextParams(
      network::mojom::NetworkContextParams* network_context_params);
  std::string SetProxyOverride(
      const std::vector<net::ProxyConfigServiceOHOS::ProxyOverrideRule>&
          proxy_rules,
      const std::vector<std::string>& bypass_rules,
      const bool reverse_bypass,
      base::OnceClosure callback);
  void ClearProxyOverride(base::OnceClosure callback);

 private:
  ProxyConfigMonitor();
  ~ProxyConfigMonitor() override;

  friend class base::NoDestructor<ProxyConfigMonitor>;
  // net::ProxyConfigService::Observer implementation:
  void OnProxyConfigChanged(
      const net::ProxyConfigWithAnnotation& config,
      net::ProxyConfigService::ConfigAvailability availability) override;

  void FlushProxyConfig(base::OnceClosure callback);

  std::unique_ptr<net::ProxyConfigServiceOHOS> proxy_config_service_;
  mojo::RemoteSet<network::mojom::ProxyConfigClient> proxy_config_client_set_;
};

}  // namespace NWEB

#endif  // CEF_LIBCEF_BROWSER_NET_SERVICE_PROXY_CONFIG_MONITOR_H_
