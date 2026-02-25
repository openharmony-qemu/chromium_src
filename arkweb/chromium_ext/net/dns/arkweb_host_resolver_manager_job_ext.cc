// Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

#include "net/dns/host_resolver_manager_job.h"
#include "arkweb/chromium_ext/net/dns/arkweb_host_resolver_manager_job_ext.h"

#include <deque>
#include <memory>
#include <optional>
#include <vector>

#include "base/containers/linked_list.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/safe_ref.h"
#include "base/memory/weak_ptr.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/not_fatal_until.h"
#include "base/task/sequenced_task_runner.h"
#include "base/time/time.h"
#include "net/base/address_family.h"
#include "net/base/features.h"
#include "net/base/network_anonymization_key.h"
#include "net/base/network_handle.h"
#include "net/base/prioritized_dispatcher.h"
#include "net/base/url_util.h"
#include "net/dns/dns_client.h"
#include "net/dns/dns_task_results_manager.h"
#include "net/dns/host_cache.h"
#include "net/dns/host_resolver.h"
#include "net/dns/host_resolver_dns_task.h"
#include "net/dns/host_resolver_manager.h"
#include "net/dns/host_resolver_manager_request_impl.h"
#include "net/dns/host_resolver_manager_service_endpoint_request_impl.h"
#include "net/dns/host_resolver_mdns_task.h"
#include "net/dns/host_resolver_nat64_task.h"
#include "net/dns/host_resolver_system_task.h"
#include "net/dns/public/dns_query_type.h"
#include "net/dns/public/secure_dns_mode.h"
#include "net/log/net_log_with_source.h"
#include "third_party/abseil-cpp/absl/types/variant.h"
#include "url/url_constants.h"

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "arkweb/chromium_ext/net/dns/secure_dns_fallback_utils.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/overrides/net/dns/secure_dns_fallback_utils.h"
#endif  // BUILDFLAG(IS_ARKWEB_EXT)
#endif

namespace net {

ArkWebHostResolverManagerJobExt::ArkWebHostResolverManagerJobExt(
    const base::WeakPtr<HostResolverManager>& resolver,
    HostResolverManager::JobKey key,
    HostResolver::ResolveHostParameters::CacheUsage cache_usage,
    HostCache* host_cache,
    std::deque<HostResolverManager::TaskType> tasks,
    RequestPriority priority,
    const NetLogWithSource& source_net_log,
    const base::TickClock* tick_clock,
    const HostResolver::HttpsSvcbOptions& https_svcb_options)
    : HostResolverManager::Job(resolver,
                               std::move(key),
                               cache_usage,
                               host_cache,
                               tasks,
                               priority,
                               source_net_log,
                               tick_clock,
                               https_svcb_options) {}

ArkWebHostResolverManagerJobExt::~ArkWebHostResolverManagerJobExt() = default;

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
bool ArkWebHostResolverManagerJobExt::CheckDnsFallBackTask(int net_error) {
  if (dns_task_error_ != OK && net_error != OK && !tasks_.empty() &&
      tasks_.back() == HostResolverManager::TaskType::SECURE_DNS_FALLBACK) {
    KillDnsTask();
    RunNextTask();
    return true;
  }
  return false;
}

void ArkWebHostResolverManagerJobExt::ReportDnsFallBackTaskResult(
    HostCache::Entry results,
    base::TimeDelta duration) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExHttpDnsFallback)) {
    if (dns_task_->secure_fallback()) {
      HostResolverManager::RequestImpl* req = requests_.head()->value();
      int index = req->parameters().only_use_secure_fallback ? 1 : 0;
      std::optional<HostCache::Entry> insecure_resolved;
      InSecureCacheLookupWithoutRunTask(insecure_resolved);
      resolver_->ReportSecureFallbackDnsResult(
          insecure_resolved, results,
          std::string(key_.host.GetHostnameWithoutBrackets()), index, duration);
    }

    if ((dns_task_->need_to_sniff_ip_result()) &&
        (failed_transactions_type_ ==
             DnsTransactionAddressFailedType::IPV4_ADDRESS_FAILED ||
         failed_transactions_type_ ==
             DnsTransactionAddressFailedType::IPV6_ADDRESS_FAILED)) {
      int index = dns_task_->secure() ? 1 : 0;
      resolver_->ReportDnsTransactionResult(
          index, std::string(key_.host.GetHostnameWithoutBrackets()),
          resolved_result_for_ipv4_, resolved_result_for_ipv6_);
    }
  }
}

void ArkWebHostResolverManagerJobExt::InSecureCacheLookupWithoutRunTask(
    std::optional<HostCache::Entry>& resolved) {
  std::optional<HostCache::EntryStaleness> stale_info;
  resolved = resolver_->MaybeServeFromCache(
      host_cache_, key_.ToCacheKey(/*secure=*/false),
      HostResolver::ResolveHostParameters::CacheUsage::STALE_ALLOWED, false,
      net_log_, &stale_info);
}

void ArkWebHostResolverManagerJobExt::AddTransactionResultForReport(
    const DnsQueryType query_type,
    int net_error) {
  if (query_type == DnsQueryType::A) {
    resolved_result_for_ipv4_ = net_error;
    failed_transactions_type_ =
        (failed_transactions_type_ ==
         DnsTransactionAddressFailedType::IPV6_ADDRESS_FAILED)
            ? DnsTransactionAddressFailedType::BOTH_FAILED
            : DnsTransactionAddressFailedType::IPV4_ADDRESS_FAILED;
  } else if (query_type == DnsQueryType::AAAA) {
    resolved_result_for_ipv6_ = net_error;
    failed_transactions_type_ =
        (failed_transactions_type_ ==
         DnsTransactionAddressFailedType::IPV4_ADDRESS_FAILED)
            ? DnsTransactionAddressFailedType::BOTH_FAILED
            : DnsTransactionAddressFailedType::IPV6_ADDRESS_FAILED;
  }
}

void ArkWebHostResolverManagerJobExt::InitReportInfoForDohFallback() {
  failed_transactions_type_ = DnsTransactionAddressFailedType::BOTH_OK;
  resolved_result_for_ipv4_ = 0;
  resolved_result_for_ipv6_ = 0;
}

bool ArkWebHostResolverManagerJobExt::CanUseSecureDnsFallback(
    ResolveContext* resolve_context) {
  if (resolver_) {
    return resolver_->CanUseSecureDnsFallback(resolve_context);
  }
  return false;
}

void ArkWebHostResolverManagerJobExt::RecordIllegalIPAddrToLog(
    const std::string& host,
    const AddressList& addrlist) {
  if (addrlist.empty()) {
    return;
  }

  const IPAddress zeroIPv4 = net::IPAddress::IPv4AllZeros();
  const IPAddress zeroIPv6 = net::IPAddress::IPv6AllZeros();

  for (const auto& endpoint : addrlist) {
    const IPAddress& addr = endpoint.address();
    if ((addr.IsIPv4() && zeroIPv4 == addr) ||
        (addr.IsIPv6() && zeroIPv6 == addr)) {
      LOG(INFO) << "The IP address resolved from dns cache with host "
                << url::LogUtils::ConvertUrlWithMask(host)
                << " maybe illegal, and maybe we can't connect IP successful.";
    }
  }
}

void ArkWebHostResolverManagerJobExt::MaybeModifyProcResolveResults(
    const std::string& host,
    bool secure_dns_fallback_available,
    int& net_error,
    AddressList& out_addr_list,
    std::vector<IPEndPoint>& truncation_results) {
  if (net_error != OK) {
    return;
  }

  bool need_to_modify_resolve_result = false;
  // AddressList addresses;
  std::vector<IPEndPoint> addresses;
  bool need_to_replace_address = MaybeNeedToProcessAddressList(
      host, out_addr_list.endpoints(), secure_dns_fallback_available, addresses,
      need_to_modify_resolve_result, truncation_results);
  if (!need_to_replace_address) {
    return;
  }

#if BUILDFLAG(IS_ARKWEB_EXT)
  ReportDnsHijackHitInfo(host, RecordQueryType::PROC,
                         out_addr_list.endpoints());
#endif  // BUILDFLAG(IS_ARKWEB_EXT)

  out_addr_list = AddressList(addresses);
  if (need_to_modify_resolve_result) {
    net_error = ERR_NAME_NOT_RESOLVED;
  }
}
#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

}  // namespace net
