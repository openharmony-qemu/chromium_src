// Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

#include "services/network/network_context.h"
#include "arkweb/chromium_ext/services/network/arkweb_network_context_ext.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/barrier_closure.h"
#include "base/base64.h"
#include "base/build_time.h"
#include "base/callback_list.h"
#include "base/check.h"
#include "base/check_op.h"
#include "base/command_line.h"
#include "base/containers/unique_ptr_adapters.h"
#include "base/dcheck_is_on.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/ref_counted.h"
#include "base/metrics/histogram_functions.h"
#include "base/not_fatal_until.h"
#include "base/ranges/algorithm.h"
#include "base/sequence_checker.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/current_thread.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "base/time/time.h"
#include "base/types/optional_util.h"
#include "build/build_config.h"
#include "build/chromecast_buildflags.h"
#include "build/chromeos_buildflags.h"
#include "components/cookie_config/cookie_store_util.h"
#include "components/domain_reliability/monitor.h"
#include "components/ip_protection/common/ip_protection_config_getter_mojo_impl.h"
#include "components/ip_protection/common/ip_protection_control_mojo.h"
#include "components/ip_protection/common/ip_protection_core_impl.h"
#include "components/ip_protection/common/ip_protection_proxy_delegate.h"
#include "components/network_session_configurator/browser/network_session_configurator.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "components/os_crypt/async/common/encryptor.h"
#include "components/prefs/json_pref_store.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/pref_service_factory.h"
#include "components/url_matcher/url_matcher.h"
#include "components/url_matcher/url_util.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "net/base/features.h"
#include "net/base/isolation_info.h"
#include "net/base/load_flags.h"
#include "net/base/net_errors.h"
#include "net/base/network_anonymization_key.h"
#include "net/base/network_delegate.h"
#include "net/base/network_isolation_key.h"
#include "net/base/port_util.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "net/cert/caching_cert_verifier.h"
#include "net/cert/cert_verifier.h"
#include "net/cert/coalescing_cert_verifier.h"
#include "net/cookies/cookie_access_delegate.h"
#include "net/cookies/cookie_constants.h"
#include "net/cookies/cookie_monster.h"
#include "net/cookies/cookie_setting_override.h"
#include "net/dns/host_cache.h"
#include "net/dns/mapped_host_resolver.h"
#include "net/extras/sqlite/cookie_crypto_delegate.h"
#include "net/extras/sqlite/sqlite_persistent_cookie_store.h"
#include "net/first_party_sets/first_party_set_metadata.h"
#include "net/http/http_auth.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_auth_preferences.h"
#include "net/http/http_auth_scheme.h"
#include "net/http/http_cache.h"
#include "net/http/http_network_session.h"
#include "net/http/http_request_headers.h"
#include "net/http/http_server_properties.h"
#include "net/http/http_transaction_factory.h"
#include "net/log/net_log_source_type.h"
#include "net/net_buildflags.h"
#include "net/proxy_resolution/configured_proxy_resolution_service.h"
#include "net/proxy_resolution/proxy_config.h"
#include "net/shared_dictionary/shared_dictionary_isolation_key.h"
#include "net/storage_access_api/status.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "net/url_request/static_http_user_agent_settings.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_builder.h"
#include "services/network/brokered_client_socket_factory.h"
#include "services/network/cookie_manager.h"
#include "services/network/data_remover_util.h"
#include "services/network/disk_cache/mojo_backend_file_operations_factory.h"
#include "services/network/host_resolver.h"
#include "services/network/http_auth_cache_copier.h"
#include "services/network/http_server_properties_pref_delegate.h"
#include "services/network/ignore_errors_cert_verifier.h"
#include "services/network/is_browser_initiated.h"
#include "services/network/net_log_exporter.h"
#include "services/network/network_service.h"
#include "services/network/network_service_network_delegate.h"
#include "services/network/network_service_proxy_delegate.h"
#include "services/network/oblivious_http_request_handler.h"
#include "services/network/prefetch_cache.h"
#include "services/network/prefetch_matching_url_loader_factory.h"
#include "services/network/prefetch_url_loader_client.h"
#include "services/network/proxy_config_service_mojo.h"
#include "services/network/proxy_lookup_request.h"
#include "services/network/proxy_resolving_socket_factory_mojo.h"
#include "services/network/public/cpp/cert_verifier/mojo_cert_verifier.h"
#include "services/network/public/cpp/content_security_policy/content_security_policy.h"
#include "services/network/public/cpp/features.h"
#include "services/network/public/cpp/network_switches.h"
#include "services/network/public/cpp/parsed_headers.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/simple_host_resolver.h"
#include "services/network/public/mojom/clear_data_filter.mojom.h"
#include "services/network/public/mojom/cookie_encryption_provider.mojom.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "services/network/public/mojom/reporting_service.mojom.h"
#include "services/network/public/mojom/trust_tokens.mojom-forward.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/resolve_host_request.h"
#include "services/network/resource_scheduler/resource_scheduler_client.h"
#include "services/network/restricted_cookie_manager.h"
#include "services/network/session_cleanup_cookie_store.h"
#include "services/network/shared_dictionary/shared_dictionary_constants.h"
#include "services/network/shared_dictionary/shared_dictionary_manager.h"
#include "services/network/shared_dictionary/shared_dictionary_storage.h"
#include "services/network/ssl_config_service_mojo.h"
#include "services/network/throttling/network_conditions.h"
#include "services/network/throttling/throttling_controller.h"
#include "services/network/throttling/throttling_network_transaction_factory.h"
#include "services/network/trust_tokens/expiry_inspecting_record_expiry_delegate.h"
#include "services/network/trust_tokens/in_memory_trust_token_persister.h"
#include "services/network/trust_tokens/pending_trust_token_store.h"
#include "services/network/trust_tokens/sqlite_trust_token_persister.h"
#include "services/network/trust_tokens/suitable_trust_token_origin.h"
#include "services/network/trust_tokens/trust_token_parameterization.h"
#include "services/network/trust_tokens/trust_token_query_answerer.h"
#include "services/network/trust_tokens/trust_token_store.h"
#include "services/network/url_loader.h"
#include "services/network/url_request_context_builder_mojo.h"
#include "services/network/web_transport.h"
#include "url/gurl.h"

#if BUILDFLAG(IS_CT_SUPPORTED)
// gn check does not account for BUILDFLAG(). So, for iOS builds, it will
// complain about a missing dependency on the target exposing this header. Add a
// nogncheck to stop it from yelling.
#include "components/certificate_transparency/chrome_require_ct_delegate.h"  // nogncheck
#include "services/network/sct_auditing/sct_auditing_cache.h"
#include "services/network/sct_auditing/sct_auditing_handler.h"
#endif  // BUILDFLAG(IS_CT_SUPPORTED)

#if BUILDFLAG(IS_CHROMEOS)
#include "services/network/cert_verifier_with_trust_anchors.h"
#endif  // BUILDFLAG(IS_CHROMEOS)

#if BUILDFLAG(ENABLE_WEBSOCKETS)
#include "services/network/websocket_factory.h"
#endif  // BUILDFLAG(ENABLE_WEBSOCKETS)

#if BUILDFLAG(ENABLE_REPORTING)
#include "net/base/http_user_agent_settings.h"
#include "net/extras/sqlite/sqlite_persistent_reporting_and_nel_store.h"
#include "net/network_error_logging/network_error_logging_service.h"
#include "net/reporting/reporting_browsing_data_remover.h"
#include "net/reporting/reporting_policy.h"
#include "net/reporting/reporting_service.h"
#endif  // BUILDFLAG(ENABLE_REPORTING)

#if BUILDFLAG(ENABLE_MDNS)
#include "services/network/mdns_responder.h"
#endif  // BUILDFLAG(ENABLE_MDNS)

#if BUILDFLAG(IS_P2P_ENABLED)
#include "services/network/p2p/socket_manager.h"
#endif  // BUILDFLAG(IS_P2P_ENABLED)

#if BUILDFLAG(IS_ANDROID)
#include "base/android/application_status_listener.h"
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(ENABLE_DEVICE_BOUND_SESSIONS)
#include "net/device_bound_sessions/session_service.h"
#endif  // BUILDFLAG(ENABLE_DEVICE_BOUND_SESSIONS)

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
#include "arkweb/chromium_ext/services/network/prp_preload/include/page_res_parallel_preload_mgr.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#endif // BUILDFLAG(ARKWEB_PRP_PRELOAD)

namespace network {

ArkWebNetworkContextExt::ArkWebNetworkContextExt(
    NetworkService* network_service,
    mojo::PendingReceiver<mojom::NetworkContext> receiver,
    mojom::NetworkContextParamsPtr params,
    OnConnectionCloseCallback on_connection_close_callback)
    : NetworkContext(network_service,
                     std::move(receiver),
                     std::move(params),
                     std::move(on_connection_close_callback)) {}

ArkWebNetworkContextExt::ArkWebNetworkContextExt(
    base::PassKey<NetworkContext> pass_key,
    NetworkService* network_service,
    mojo::PendingReceiver<mojom::NetworkContext> receiver,
    mojom::NetworkContextParamsPtr params,
    OnConnectionCloseCallback on_connection_close_callback,
    OnURLRequestContextBuilderConfiguredCallback
        on_url_request_context_builder_configured)
    : NetworkContext(pass_key,
                     network_service,
                     std::move(receiver),
                     std::move(params),
                     std::move(on_connection_close_callback),
                     std::move(on_url_request_context_builder_configured)) {}

ArkWebNetworkContextExt::ArkWebNetworkContextExt(
    NetworkService* network_service,
    mojo::PendingReceiver<mojom::NetworkContext> receiver,
    net::URLRequestContext* url_request_context,
    const std::vector<std::string>& cors_exempt_header_list)
    : NetworkContext(network_service,
                     std::move(receiver),
                     url_request_context,
                     cors_exempt_header_list) {}

ArkWebNetworkContextExt::~ArkWebNetworkContextExt() {}

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
void ArkWebNetworkContextExt::SetHostIP(const std::string& host_name,
                               const std::vector<std::string>& address,
                               uint32_t alive_time) {
  const uint16_t host_port_80 = 80;
  const uint16_t host_port_443 = 443;
  auto host_cache = url_request_context_->host_resolver()->GetHostCache();
  if (host_cache) {
    std::vector<net::IPEndPoint> expected;
    for (auto& it : address) {
      net::IPAddress ip;
      bool result = ip.AssignFromIPLiteral(it);
      DCHECK(result);
      expected.push_back(net::IPEndPoint(ip, 0));
    }
    host_cache->Set(
        net::HostCache::Key(url::SchemeHostPort("http", host_name, host_port_80),
                            net::DnsQueryType::UNSPECIFIED, 0,
                            net::HostResolverSource::ANY,
                            net::NetworkAnonymizationKey(false),
                            false, true),
        net::HostCache::Entry(
            net::OK, expected, std::set<std::string>({host_name}),
            net::HostCache::Entry::SOURCE_UNKNOWN, base::Seconds(alive_time)),
        base::TimeTicks::Now(), base::Seconds(alive_time));
    host_cache->Set(
        net::HostCache::Key(url::SchemeHostPort("https", host_name, host_port_443),
                            net::DnsQueryType::UNSPECIFIED, 0,
                            net::HostResolverSource::ANY,
                            net::NetworkAnonymizationKey(false),
                            false, true),
        net::HostCache::Entry(
            net::OK, expected, std::set<std::string>({host_name}),
            net::HostCache::Entry::SOURCE_UNKNOWN, base::Seconds(alive_time)),
        base::TimeTicks::Now(), base::Seconds(alive_time));
  }
}

// Predicate function to determine if the given |domain| matches the
// |filter_type| and |filter_domains| from a |mojom::ClearDataFilter|.
bool MatchesDomainFilter(mojom::ClearDataFilter_Type filter_type,
                         std::set<std::string> filter_domains,
                         const std::string& domain) {
  bool found_domain = filter_domains.find(domain) != filter_domains.end();
  return (filter_type == mojom::ClearDataFilter_Type::DELETE_MATCHES) ==
         found_domain;
}

void ArkWebNetworkContextExt::ClearHostIP(const std::string& host_name) {
  net::HostCache* host_cache =
      url_request_context_->host_resolver()->GetHostCache();
  DCHECK(host_cache);
  if (host_name == "") {
    return;
  }

  std::set<std::string> filter_domains;
  filter_domains.insert(host_name);
  host_cache->ClearForHosts(base::BindRepeating(
      &MatchesDomainFilter, mojom::ClearDataFilter_Type::DELETE_MATCHES,
      std::move(filter_domains)));
}
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
// static
void ArkWebNetworkContextExt::PRPPreloadCreateURLLoaderFactory(
    mojo::PendingReceiver<mojom::URLLoaderFactory> receiver,
    mojom::URLLoaderFactoryParamsPtr params,
    scoped_refptr<ResourceSchedulerClient> resource_scheduler_client,
    NetworkContext *network_context) {
  auto main_url = params->main_url;
  auto addr_web_handle = params->addr_web_handle;
  net::IsolationInfo isolation_info(params->isolation_info);
  auto url_loader_factory = std::make_unique<PrefetchMatchingURLLoaderFactory>(
      network_context, std::move(params), std::move(resource_scheduler_client),
      std::move(receiver), &network_context->cors_origin_access_list_, network_context->prefetch_cache_.get());
  network_context->url_loader_factories_.emplace(std::move(url_loader_factory));
  if (addr_web_handle != 0 && !main_url.empty()) {
    if (isolation_info.frame_origin().has_value()) {
      ohos_prp_preload::PRParallelPreloadMgr::GetInstance().SetPageOrigin(main_url, isolation_info);
    }
  }
  return;
}

void ArkWebNetworkContextExt::InitPRParallelPreloadMgr() {
  ohos_prp_preload::PRParallelPreloadMgr::GetInstance().Init(base::SingleThreadTaskRunner::GetCurrentDefault());
}
 
void ArkWebNetworkContextExt::StartPage(const std::string& url,
    const net::NetworkAnonymizationKey& networkAnonymizationKey, uint64_t addr_web_handle,
    StartPageCallback page_origin_cb) {
  ohos_prp_preload::PRParallelPreloadMgr::GetInstance().StartPage(url, networkAnonymizationKey,
      url_request_context()->GetWeakPtr(), addr_web_handle, std::move(page_origin_cb),
      net_service::NetHelpers::GetNoUseCache());
}
 
void ArkWebNetworkContextExt::StopPage(uint64_t addr_web_handle) {
  ohos_prp_preload::PRParallelPreloadMgr::GetInstance().StopPage(addr_web_handle);
}
 
void ArkWebNetworkContextExt::SetURLLoaderFactoryParam(mojom::URLLoaderFactoryParamsPtr params) {
  ohos_prp_preload::PRParallelPreloadMgr::GetInstance().SetURLLoaderFactoryParam(std::move(params));
}
#endif

}  // namespace network
