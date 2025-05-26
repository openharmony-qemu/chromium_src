// Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

#ifndef SERVICES_NETWORK_ARKWEB_NETWORK_CONTEXT_EXT_H_
#define SERVICES_NETWORK_ARKWEB_NETWORK_CONTEXT_EXT_H_

#include <stdint.h>

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
#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/sequence_checker.h"
#include "base/time/time.h"
#include "base/types/pass_key.h"
#include "base/unguessable_token.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "components/ip_protection/common/ip_protection_core.h"
#include "mojo/public/cpp/base/big_buffer.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "mojo/public/cpp/bindings/remote_set.h"
#include "mojo/public/cpp/bindings/unique_receiver_set.h"
#include "net/base/network_isolation_key.h"
#include "net/cert/cert_verifier.h"
#include "net/cert/cert_verify_result.h"
#include "net/cookies/cookie_setting_override.h"
#include "net/dns/host_resolver.h"
#include "net/dns/public/dns_config_overrides.h"
#include "net/first_party_sets/first_party_set_metadata.h"
#include "net/http/http_auth_preferences.h"
#include "net/net_buildflags.h"
#include "net/reporting/reporting_target_type.h"
#include "net/storage_access_api/status.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "services/network/cors/preflight_controller.h"
#include "services/network/first_party_sets/first_party_sets_access_delegate.h"
#include "services/network/http_cache_data_counter.h"
#include "services/network/http_cache_data_remover.h"
#include "services/network/network_qualities_pref_delegate.h"
#include "services/network/oblivious_http_request_handler.h"
#include "services/network/public/cpp/cors/origin_access_list.h"
#include "services/network/public/cpp/network_service_buildflags.h"
#include "services/network/public/cpp/transferable_directory.h"
#include "services/network/public/mojom/clear_data_filter.mojom-forward.h"
#include "services/network/public/mojom/cookie_access_observer.mojom.h"
#include "services/network/public/mojom/cookie_manager.mojom-shared.h"
#include "services/network/public/mojom/host_resolver.mojom.h"
#include "services/network/public/mojom/network_context.mojom-forward.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "services/network/public/mojom/network_context_client.mojom.h"
#include "services/network/public/mojom/network_service.mojom-forward.h"
#include "services/network/public/mojom/proxy_lookup_client.mojom.h"
#include "services/network/public/mojom/proxy_resolving_socket.mojom.h"
#include "services/network/public/mojom/restricted_cookie_manager.mojom.h"
#include "services/network/public/mojom/restricted_udp_socket.mojom.h"
#include "services/network/public/mojom/tcp_socket.mojom.h"
#include "services/network/public/mojom/udp_socket.mojom.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/websocket.mojom.h"
#include "services/network/resource_scheduler/resource_scheduler.h"
#include "services/network/restricted_cookie_manager.h"
#include "services/network/socket_factory.h"
#include "services/network/url_request_context_owner.h"
#include "services/network/web_bundle/web_bundle_manager.h"

#if BUILDFLAG(ENABLE_REPORTING)
#include "net/reporting/reporting_cache_observer.h"
#include "net/reporting/reporting_report.h"
#endif  // BUILDFLAG(ENABLE_REPORTING)

#if BUILDFLAG(IS_CT_SUPPORTED)
#include "services/network/public/mojom/ct_log_info.mojom-forward.h"
#endif

namespace net {
class NetworkAnonymizationKey;
class URLRequestContext;
}  // namespace net

namespace network {
class NetworkService;

class COMPONENT_EXPORT(NETWORK_SERVICE) ArkWebNetworkContextExt : public NetworkContext {
 public:
  ArkWebNetworkContextExt(NetworkService* network_service,
                          mojo::PendingReceiver<mojom::NetworkContext> receiver,
                          mojom::NetworkContextParamsPtr params,
                          OnConnectionCloseCallback on_connection_close_callback =
                              OnConnectionCloseCallback());

  ArkWebNetworkContextExt(NetworkService* network_service,
                          mojo::PendingReceiver<mojom::NetworkContext> receiver,
                          net::URLRequestContext* url_request_context,
                          const std::vector<std::string>& cors_exempt_header_list);

  ArkWebNetworkContextExt(base::PassKey<NetworkContext> pass_key,
                          NetworkService* network_service,
                          mojo::PendingReceiver<mojom::NetworkContext> receiver,
                          mojom::NetworkContextParamsPtr params,
                          OnConnectionCloseCallback on_connection_close_callback,
                          OnURLRequestContextBuilderConfiguredCallback
                              on_url_request_context_builder_configured);

  ArkWebNetworkContextExt(const ArkWebNetworkContextExt&) = delete;
  ArkWebNetworkContextExt& operator=(const ArkWebNetworkContextExt&) = delete;

  ~ArkWebNetworkContextExt() override;

  ArkWebNetworkContextExt *AsArkWebNetworkContextExt() override { return this; }

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
  void SetHostIP(const std::string& host_name,
                 const std::vector<std::string>& dddress,
                 uint32_t alive_time) override;
  void ClearHostIP(const std::string& host_name) override;
#endif
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  static void PRPPreloadCreateURLLoaderFactory(
      mojo::PendingReceiver<mojom::URLLoaderFactory> receiver,
      mojom::URLLoaderFactoryParamsPtr params,
      scoped_refptr<ResourceSchedulerClient> resource_scheduler_client,
      NetworkContext *network_context);
  void InitPRParallelPreloadMgr() override;
  void StartPage(const std::string& url,
      const net::NetworkAnonymizationKey& networkAnonymizationKey, uint64_t addr_web_handle,
      StartPageCallback page_origin_cb) override;
  void StopPage(uint64_t addr_web_handle) override;
  void SetURLLoaderFactoryParam(mojom::URLLoaderFactoryParamsPtr params) override;
#endif
};

}  // namespace network

#endif  // SERVICES_NETWORK_ARKWEB_NETWORK_CONTEXT_EXT_H_
