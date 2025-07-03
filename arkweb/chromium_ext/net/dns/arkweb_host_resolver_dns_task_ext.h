// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef NET_DNS_ARKWEB_HOST_RESOLVER_DNS_TASK_EXT_H_
#define NET_DNS_ARKWEB_HOST_RESOLVER_DNS_TASK_EXT_H_

#include <initializer_list>
#include <memory>
#include <optional>
#include <set>
#include <vector>

#include "base/containers/circular_deque.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/safe_ref.h"
#include "base/memory/weak_ptr.h"
#include "base/numerics/safe_conversions.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "base/values.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_export.h"
#include "net/base/request_priority.h"
#include "net/dns/host_cache.h"
#include "net/dns/host_resolver.h"
#include "net/dns/httpssvc_metrics.h"
#include "net/dns/public/secure_dns_mode.h"
#include "net/dns/resolve_context.h"
#include "net/log/net_log_with_source.h"
#include "third_party/abseil-cpp/absl/types/variant.h"

namespace net {

class HostResolverDnsTask;

class NET_EXPORT_PRIVATE ArkWebHostResolverDnsTaskExt final {
 public:
  friend class HostResolverDnsTask;
  raw_ptr<HostResolverDnsTask> hostResolverDnsTask;
  ArkWebHostResolverDnsTaskExt(HostResolverDnsTask *resolver) { hostResolverDnsTask = resolver; }
  ~ArkWebHostResolverDnsTaskExt() = default;

  ArkWebHostResolverDnsTaskExt(const ArkWebHostResolverDnsTaskExt&) = delete;
  ArkWebHostResolverDnsTaskExt& operator=(const ArkWebHostResolverDnsTaskExt&) = delete;

 private:
#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
  void ArkWebSetNotNeedQueryType(int legacy_results_error, DnsQueryType dns_query_type);
  void ArkWebFailedTransaction(int net_error, std::optional<DnsQueryType> failed_transaction_type);
  bool AnyAOrAAAATransactionRemain();
  void RecordFailedTransactionInfo(int index,
                                   int net_error,
                                   DnsQueryType dns_query_type);
  void SetNotNeedMoreAttemptIPQueryType(DnsQueryType dns_query_type);
#endif  // BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
};

}  // namespace net

#endif  // NET_DNS_ARKWEB_HOST_RESOLVER_DNS_TASK_EXT_H_
