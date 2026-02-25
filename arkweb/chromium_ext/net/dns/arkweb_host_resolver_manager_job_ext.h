// Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

#ifndef NET_DNS_ARKWEB_HOST_RESOLVER_MANAGER_JOB_EXT_H_
#define NET_DNS_ARKWEB_HOST_RESOLVER_MANAGER_JOB_EXT_H_

#ifdef UNSAFE_BUFFERS_BUILD
// TODO(crbug.com/40284755): Remove this and spanify to fix the errors.
#pragma allow_unsafe_buffers
#endif

#include <deque>
#include <memory>
#include <optional>
#include <vector>

#include "base/containers/linked_list.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/safe_ref.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "net/base/address_family.h"
#include "net/base/network_anonymization_key.h"
#include "net/base/network_handle.h"
#include "net/base/prioritized_dispatcher.h"
#include "net/dns/dns_task_results_manager.h"
#include "net/dns/host_cache.h"
#include "net/dns/host_resolver.h"
#include "net/dns/host_resolver_dns_task.h"
#include "net/dns/host_resolver_manager.h"
#include "net/dns/public/dns_query_type.h"
#include "net/dns/public/secure_dns_mode.h"
#include "net/log/net_log_with_source.h"
#include "third_party/abseil-cpp/absl/types/variant.h"

namespace net {

class ResolveContext;
class HostResolverMdnsTask;
class HostResolverNat64Task;

class ArkWebHostResolverManagerJobExt : public HostResolverManager::Job {
 public:
  ArkWebHostResolverManagerJobExt(
      const base::WeakPtr<HostResolverManager>& resolver,
      HostResolverManager::JobKey key,
      HostResolver::ResolveHostParameters::CacheUsage cache_usage,
      HostCache* host_cache,
      std::deque<HostResolverManager::TaskType> tasks,
      RequestPriority priority,
      const NetLogWithSource& source_net_log,
      const base::TickClock* tick_clock,
      const HostResolver::HttpsSvcbOptions& https_svcb_options);
  ~ArkWebHostResolverManagerJobExt() override;
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  ArkWebHostResolverManagerJobExt* AsArkWebHostResolverManagerJobExt()
      override {
    return this;
  }
  bool CanUseSecureDnsFallback(ResolveContext* resolve_context) override;
#endif

 private:
  friend class Job;
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  bool CheckDnsFallBackTask(int net_error);
  void ReportDnsFallBackTaskResult(HostCache::Entry results,
                                   base::TimeDelta duration);
  using DnsTransactionAddressFailedType =
      HostResolverManager::DnsTransactionAddressFailedType;
  void AddTransactionResultForReport(const DnsQueryType query_type,
                                     int net_error) override;
  void InitReportInfoForDohFallback() override;
  void InSecureCacheLookupWithoutRunTask(
      std::optional<HostCache::Entry>& resolved);
  void MaybeModifyProcResolveResults(
      const std::string& host,
      bool secure_dns_fallback_available,
      int& net_error,
      AddressList& out_addr_list,
      std::vector<IPEndPoint>& truncation_results);
  void RecordIllegalIPAddrToLog(const std::string& host,
                               const AddressList& addrlist);

  int resolved_result_for_ipv4_{0};
  int resolved_result_for_ipv6_{0};
  HostResolverManager::DnsTransactionAddressFailedType
      failed_transactions_type_{DnsTransactionAddressFailedType::BOTH_OK};
#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
};

}  // namespace net

#endif  // NET_DNS_ARKWEB_HOST_RESOLVER_MANAGER_JOB_EXT_H_
