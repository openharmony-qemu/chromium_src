// Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

#ifndef SERVICES_NETWORK_ARKWEB_NETWORK_SERVICE_EXT_H_
#define SERVICES_NETWORK_ARKWEB_NETWORK_SERVICE_EXT_H_

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "base/component_export.h"
#include "base/containers/flat_map.h"
#include "base/containers/flat_set.h"
#include "base/containers/unique_ptr_adapters.h"
#include "base/feature_list.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "components/ip_protection/common/masked_domain_list_manager.h"
#include "components/privacy_sandbox/masked_domain_list/masked_domain_list.pb.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/base/address_list.h"
#include "net/base/schemeful_site.h"
#include "net/dns/host_resolver.h"
#include "net/dns/host_resolver_manager.h"
#include "net/dns/public/dns_over_https_config.h"
#include "net/dns/public/secure_dns_mode.h"
#include "net/first_party_sets/global_first_party_sets.h"
#include "net/http/transport_security_state.h"
#include "net/log/net_log.h"
#include "net/log/trace_net_log_observer.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "services/network/first_party_sets/first_party_sets_manager.h"
#include "services/network/keepalive_statistics_recorder.h"
#include "services/network/network_change_manager.h"
#include "services/network/network_quality_estimator_manager.h"
#include "services/network/public/cpp/network_service_buildflags.h"
#include "services/network/public/mojom/host_resolver.mojom.h"
#include "services/network/public/mojom/key_pinning.mojom.h"
#include "services/network/public/mojom/net_log.mojom.h"
#include "services/network/public/mojom/network_annotation_monitor.mojom.h"
#include "services/network/public/mojom/network_change_manager.mojom.h"
#include "services/network/public/mojom/network_quality_estimator_manager.mojom.h"
#include "services/network/public/mojom/network_service.mojom.h"
#include "services/network/public/mojom/system_dns_resolution.mojom.h"
#include "services/network/public/mojom/trust_tokens.mojom.h"
#include "services/network/public/mojom/url_loader_network_service_observer.mojom.h"
#include "services/network/restricted_cookie_manager.h"
#include "services/network/tpcd/metadata/manager.h"
#include "services/network/trust_tokens/trust_token_key_commitments.h"
#include "services/service_manager/public/cpp/binder_registry.h"

#if BUILDFLAG(IS_CT_SUPPORTED)
#include "services/network/public/mojom/ct_log_info.mojom.h"
#endif  // BUILDFLAG(IS_CT_SUPPORTED)

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
#include "arkweb/chromium_ext/servieces/network/public/mojom/network_config_ohos.mojom.h"
#endif

namespace network {

class NetworkService;

class COMPONENT_EXPORT(NETWORK_SERVICE) ArkWebNetworkServiceExt
    : public NetworkService {
 public:
  explicit ArkWebNetworkServiceExt(
      std::unique_ptr<service_manager::BinderRegistry> registry,
      mojo::PendingReceiver<mojom::NetworkService> receiver =
          mojo::NullReceiver(),
      bool delay_initialization_until_set_client = false);

  ArkWebNetworkServiceExt(const ArkWebNetworkServiceExt&) = delete;
  ArkWebNetworkServiceExt& operator=(const ArkWebNetworkServiceExt&) = delete;

  ~ArkWebNetworkServiceExt() override;

  ArkWebNetworkServiceExt* AsArkWebNetworkServiceExt() override { return this; }

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
  void SetURLRequestContext(NetworkContext* network_context);
  void SetConnectTimeout(int seconds) override;
  void BindDnsToNetwork(int network) override;
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
  void SetHttpsDnsFallbackData(
      mojom::HttpsDnsFallbackConfigPtr config) override;
#endif

#if BUILDFLAG(ARKWEB_HTTP_DNS)
  net::DnsConfigOverrides ConfigureStubHostResolverExt(
      net::SecureDnsMode secure_dns_mode,
      const net::DnsOverHttpsConfig& dns_over_https_config);
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
  void NetworkContextSetHostIP(NetworkContext* network_context);
  void DeregisterNetworkContextExt(NetworkContext* network_context);
#endif

 private:
#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
  void SetHttpsDnsHostResolver(bool enabled,
                               const std::string& server_template);
#endif

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
  int timeout_override_ = 0;
  net::handles::NetworkHandle network_for_dns_{
      net::handles::kInvalidNetworkHandle};

  int connect_job_with_secure_dns_only_timeout_{15};
  bool cfg_https_dns_fallback_enabled_ = false;
  std::string cfg_http_dns_server_template_;
  bool real_https_dns_fallback_enabled_ = false;
  std::string real_http_dns_server_template_;
#endif
};

}  // namespace network

#endif  // SERVICES_NETWORK_ARKWEB_NETWORK_SERVICE_EXT_H_
