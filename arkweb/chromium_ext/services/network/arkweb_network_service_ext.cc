// Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

#include "services/network/network_service.h"
#include "arkweb/chromium_ext/services/network/arkweb_network_service_ext.h"

#include <algorithm>
#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "arkweb/chromium_ext/net/url_request/url_request_context_ext.h"
#include "base/check.h"
#include "base/check_op.h"
#include "base/debug/crash_logging.h"
#include "base/debug/dump_without_crashing.h"
#include "base/environment.h"
#include "base/feature_list.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/raw_ptr.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/not_fatal_until.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "base/timer/timer.h"
#include "base/types/pass_key.h"
#include "base/values.h"
#include "build/build_config.h"
#include "build/chromecast_buildflags.h"
#include "build/chromeos_buildflags.h"
#include "components/ip_protection/common/masked_domain_list_manager.h"
#include "components/network_session_configurator/common/network_features.h"
#include "components/os_crypt/sync/os_crypt.h"
#include "components/privacy_sandbox/masked_domain_list/masked_domain_list.pb.h"
#include "mojo/public/cpp/base/proto_wrapper.h"
#include "mojo/public/cpp/bindings/callback_helpers.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/scoped_message_error_crash_key.h"
#include "mojo/public/cpp/bindings/shared_remote.h"
#include "mojo/public/cpp/system/functions.h"
#include "net/base/address_list.h"
#include "net/base/features.h"
#include "net/base/logging_network_change_observer.h"
#include "net/base/network_change_notifier.h"
#include "net/base/network_change_notifier_passive.h"
#include "net/base/port_util.h"
#include "net/cert/cert_database.h"
#include "net/cert/ct_log_response_parser.h"
#include "net/cert/internal/system_trust_store.h"
#include "net/cert/signed_tree_head.h"
#include "net/cookies/cookie_util.h"
#include "net/dns/host_resolver.h"
#include "net/dns/host_resolver_manager.h"
#include "net/dns/host_resolver_proc.h"
#include "net/dns/public/dns_config_overrides.h"
#include "net/dns/public/dns_over_https_config.h"
#include "net/dns/public/doh_provider_entry.h"
#include "net/dns/system_dns_config_change_notifier.h"
#include "net/dns/test_dns_config_service.h"
#include "net/first_party_sets/global_first_party_sets.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/log/file_net_log_observer.h"
#include "net/log/net_log.h"
#include "net/log/net_log_capture_mode.h"
#include "net/log/net_log_util.h"
#include "net/nqe/network_quality_estimator.h"
#include "net/socket/client_socket_pool_manager.h"
#include "net/ssl/ssl_key_logger_impl.h"
#include "net/url_request/url_request_context.h"
#include "services/network/dns_config_change_manager.h"
#include "services/network/first_party_sets/first_party_sets_manager.h"
#include "services/network/http_auth_cache_copier.h"
#include "services/network/net_log_exporter.h"
#include "services/network/net_log_proxy_sink.h"
#include "services/network/network_context.h"
#include "services/network/public/cpp/crash_keys.h"
#include "services/network/public/cpp/features.h"
#include "services/network/public/cpp/initiator_lock_compatibility.h"
#include "services/network/public/cpp/load_info_util.h"
#include "services/network/public/cpp/network_switches.h"
#include "services/network/public/cpp/parsed_headers.h"
#include "services/network/public/mojom/key_pinning.mojom.h"
#include "services/network/public/mojom/network_service_test.mojom.h"
#include "services/network/public/mojom/system_dns_resolution.mojom-forward.h"
#include "services/network/restricted_cookie_manager.h"
#include "services/network/tpcd/metadata/manager.h"
#include "services/network/url_loader.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(IS_ANDROID) && defined(ARCH_CPU_ARMEL)
#include "third_party/boringssl/src/include/openssl/cpu.h"
#endif

#if (BUILDFLAG(IS_LINUX) && !BUILDFLAG(IS_CASTOS)) || \
    BUILDFLAG(IS_CHROMEOS_LACROS)

#include "components/os_crypt/sync/key_storage_config_linux.h"
#endif

#if BUILDFLAG(IS_LINUX)
#include "services/network/network_change_notifier_passive_factory.h"
#endif

#if BUILDFLAG(IS_ANDROID)
#include "base/android/application_status_listener.h"
#include "net/android/http_auth_negotiate_android.h"
#endif

#if BUILDFLAG(IS_CT_SUPPORTED)
#include "services/network/sct_auditing/sct_auditing_cache.h"
#endif

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
#include "net/socket/client_socket_pool.h"
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#endif

namespace net {
class FirstPartySetEntry;
}

namespace network {

ArkWebNetworkServiceExt::ArkWebNetworkServiceExt(
    std::unique_ptr<service_manager::BinderRegistry> registry,
    mojo::PendingReceiver<mojom::NetworkService> receiver,
    bool delay_initialization_until_set_client)
    : NetworkService(std::move(registry),
                     std::move(receiver),
                     delay_initialization_until_set_client) {}

ArkWebNetworkServiceExt::~ArkWebNetworkServiceExt() = default;

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
void ArkWebNetworkServiceExt::SetURLRequestContext(
    NetworkContext* network_context) {
  net::URLRequestContext* url_request_context =
      network_context->url_request_context();
  if (url_request_context) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO) << "Register network context and set network timeout "
                       << timeout_override_ << " second(s)";
#endif
    url_request_context->AsURLRequestContextExt()->SetConnectTimeout(
        timeout_override_);
    url_request_context->AsURLRequestContextExt()->BindDnsToNetwork(
        network_for_dns_);
#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
    url_request_context->AsURLRequestContextExt()
        ->SetConnectJobWithSecureDnsOnlyTimeout(
            connect_job_with_secure_dns_only_timeout_);
#endif
  }
}

void ArkWebNetworkServiceExt::SetConnectTimeout(int seconds) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO, kNetwork)
      << "SetConnectTimeout timeout:" << seconds << "s";
#endif
  timeout_override_ = seconds;
  for (NetworkContext* network_context : network_contexts_) {
    net::URLRequestContext* url_request_context =
        network_context->url_request_context();
    if (url_request_context) {
      url_request_context->AsURLRequestContextExt()->SetConnectTimeout(seconds);
    }
  }
}

void ArkWebNetworkServiceExt::BindDnsToNetwork(int network) {
  if (network_for_dns_ == network) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO) << "bind dns to network return for network is same with "
                       << network_for_dns_;
#endif
    return;
  }
  network_for_dns_ = network;
  if (host_resolver_manager_) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO) << "bind dns to network " << network
                       << " invalid dns cache.";
#endif
    host_resolver_manager_->InvalidateCachesForTesting();

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
    bool http_dns_enabled = false;
    std::string http_dns_servers_template;
    if (network == -1) {
      http_dns_enabled = cfg_https_dns_fallback_enabled_;
      http_dns_servers_template = cfg_http_dns_server_template_;
    }
    SetHttpsDnsHostResolver(http_dns_enabled, http_dns_servers_template);
#endif
  }
  for (auto network_context : network_contexts_) {
    net::URLRequestContext* url_request_context =
        network_context->url_request_context();
    if (url_request_context) {
      url_request_context->AsURLRequestContextExt()->BindDnsToNetwork(
          network_for_dns_);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
void ArkWebNetworkServiceExt::SetHttpsDnsFallbackData(
    mojom::HttpsDnsFallbackConfigPtr config) {
  bool https_dns_fallback_enabled = false;
  std::string http_dns_server_template;
  std::vector<std::string> host_list;
  std::vector<std::string> ip_list;
  if (config) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO, kNetwork)
        << "SetHttpsDnsFallbackData enabled:" << config->enabled
        << " httpsDnsServerTemplate:"
        << url::LogUtils::ConvertUrlWithMask(config->https_dns_server_template)
        << " connectJobWithDnsOnlyTimeout:"
        << config->connect_job_with_dns_only_timeout
        << " sourceHostListSize:" << config->source_host_list.size()
        << " suspectIPListSize:" << config->suspect_ip_list.size();
#endif
    https_dns_fallback_enabled = config->enabled;
    http_dns_server_template = config->https_dns_server_template;
    connect_job_with_secure_dns_only_timeout_ =
        config->connect_job_with_dns_only_timeout;
    host_list = std::move(config->source_host_list);
    ip_list = std::move(config->suspect_ip_list);
  }

  SetHttpsDnsHostResolver(https_dns_fallback_enabled, http_dns_server_template);
  cfg_https_dns_fallback_enabled_ = https_dns_fallback_enabled;
  cfg_http_dns_server_template_ = std::move(http_dns_server_template);

  for (auto network_context : network_contexts_) {
    net::URLRequestContext* url_request_context =
        network_context->url_request_context();
    if (url_request_context) {
      url_request_context->AsURLRequestContextExt()
          ->SetConnectJobWithSecureDnsOnlyTimeout(
              connect_job_with_secure_dns_only_timeout_);
    }
  }

  host_resolver_manager_->SetSuspectIpListAndSourceHostList(ip_list, host_list);
}

void ArkWebNetworkServiceExt::SetHttpsDnsHostResolver(
    bool enabled,
    const std::string& server_template) {
  if (enabled == real_https_dns_fallback_enabled_ &&
      server_template == real_http_dns_server_template_) {
    return;
  }
  LOG_FEEDBACK(INFO, kNetwork)
      << "SetHttpsDnsHostResolver enabled:" << enabled
      << " realHttpsDnsFallbackEnabled:" << real_https_dns_fallback_enabled_
      << " serverTemplate:"
      << url::LogUtils::ConvertUrlWithMask(server_template)
      << " realHttpDnsServerTemplate:"
      << url::LogUtils::ConvertUrlWithMask(real_http_dns_server_template_)
      << " networkForDns:" << network_for_dns_;
  real_https_dns_fallback_enabled_ = enabled;
  real_http_dns_server_template_ = server_template;

  bool allow_enable_http_dns_fallback = false;
  net::DnsOverHttpsConfig doh_fallback_config;
  if (enabled) {
    doh_fallback_config =
        net::DnsOverHttpsConfig::FromStringLax(server_template);
    if (doh_fallback_config.servers().size() > 0) {
      allow_enable_http_dns_fallback = true;
    }
  }

  host_resolver_manager_->SetHttpsDnsFallbackData(
      allow_enable_http_dns_fallback, server_template);

  // Enable or disable the insecure part of DnsClient. "DnsClient" is the class
  // that implements the stub resolver.
  host_resolver_manager_->SetInsecureDnsClientEnabled(
      allow_enable_http_dns_fallback, false);

  net::DnsConfigOverrides overrides;
  // 如果HTTP DNS FALLBACK去使能，则不需要Dns name servers
  if (!allow_enable_http_dns_fallback) {
    overrides =
        net::DnsConfigOverrides::CreateOverridingEverythingWithDefaults();
  }
  overrides.secure_dns_mode = net::SecureDnsMode::kOff;
  overrides.dns_over_https_config = std::move(doh_fallback_config);
  overrides.allow_dns_over_https_upgrade = false;

  host_resolver_manager_->SetDnsConfigOverrides(overrides);
}
#endif

#if BUILDFLAG(ARKWEB_HTTP_DNS)
net::DnsConfigOverrides ArkWebNetworkServiceExt::ConfigureStubHostResolverExt(
    net::SecureDnsMode secure_dns_mode,
    const net::DnsOverHttpsConfig& dns_over_https_config) {
  net::DnsConfigOverrides overrides;
#if !BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
  // Since the system dnsconfig is not obtained and null in OHOS, so override
  // the full config with default.
  overrides = net::DnsConfigOverrides::CreateOverridingEverythingWithDefaults();
#else
  // 如果是webview或使能Doh，则不需要获取Dns name servers
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          ::switches::kEnableNwebExHttpDnsFallback) ||
      secure_dns_mode != net::SecureDnsMode::kOff) {
    overrides =
        net::DnsConfigOverrides::CreateOverridingEverythingWithDefaults();
  }
#endif
  overrides.secure_dns_mode = secure_dns_mode;
  overrides.dns_over_https_config = dns_over_https_config;

  // Keep the dns_over_https_upgrade disabled in OHOS since we don't have
  // available update providers.
  overrides.allow_dns_over_https_upgrade = false;
  return overrides;
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
void ArkWebNetworkServiceExt::NetworkContextSetHostIP(
    NetworkContext* network_context) {
  std::string hostName = "";
  std::vector<std::string> address = {};
  int32_t ttl = 0;
  auto host_map = net_service::NetHelpers::GetHostIP();
  for (auto& it : host_map) {
    hostName = it.first;
    address = it.second.address;
    ttl = it.second.ttl;
    network_context->SetHostIP(hostName, address, ttl);
  }
}

void ArkWebNetworkServiceExt::DeregisterNetworkContextExt(
    NetworkContext* network_context) {
  auto host_map = net_service::NetHelpers::GetHostIP();
  for (auto& it : host_map) {
    auto hostName = it.first;
    network_context->ClearHostIP(hostName);
  }
  net_service::NetHelpers::ClearHostIP();
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
void ArkWebNetworkServiceExt::SetSocketIdleTimeout(int32_t timeout) {
  LOG(INFO) << "Network service set socket idle timeout " << timeout
            << " second(s)";
  net::ClientSocketPool::set_used_idle_socket_timeout(base::Seconds(timeout));

  for (NetworkContext* network_context : network_contexts_) {
    net::URLRequestContext* url_request_context =
        network_context->url_request_context();
    if (url_request_context && url_request_context->AsURLRequestContextExt()) {
      url_request_context->AsURLRequestContextExt()->SetSocketIdleTimeout(
          timeout);
    }
  }
}
#endif

}  // namespace network
