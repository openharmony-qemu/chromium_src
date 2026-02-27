/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/overrides/net/dns/secure_dns_fallback_utils.h"
#endif  // BUILDFLAG(IS_ARKWEB_EXT)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "arkweb/chromium_ext/net/dns/secure_dns_fallback_utils.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
#include "net/base/features.h"
#include "services/network/public/cpp/features.h"
#endif

namespace net {

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
void HostResolverManager::ServiceEndpointRequestImpl::
    MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
        HostCache::Entry& out_results,
        int dns_status) {
  std::vector<IPEndPoint> truncation_results;
  MaybeModifyResolveLocallyResults(out_results, truncation_results);

  for (auto ip : truncation_results) {
    resolve_info_.AppendTruncationAddress(ip);
  }

  for (auto ip : out_results.ip_endpoints()) {
    resolve_info_.ip_endpoints.emplace_back(ip.ToStringWithoutPort());
  }

  resolve_info_.dns_status = dns_status;
  resolve_info_.error_code = out_results.error();
}

void HostResolverManager::ServiceEndpointRequestImpl::
    MaybeModifyResolveLocallyResults(
        HostCache::Entry& out_results,
        std::vector<IPEndPoint>& truncation_results) {
  if (out_results.error() != OK) {
    return;
  }
  if (out_results.ip_endpoints().empty()) {
    return;
  }

  std::string host(host_.GetHostnameWithoutBrackets());
  bool secure_dns_fallback_available = false;
  if (manager_) {
    secure_dns_fallback_available =
        manager_->CanUseSecureDnsFallback(resolve_context_.get());
  }
  bool need_to_modify_resolve_result = false;
  std::vector<IPEndPoint> ip_endpoints_modified;
  bool need_to_replace_address = MaybeNeedToProcessAddressList(
      host, out_results.ip_endpoints(), secure_dns_fallback_available,
      ip_endpoints_modified, need_to_modify_resolve_result, truncation_results);

  if (!need_to_replace_address) {
    return;
  }

#if BUILDFLAG(IS_ARKWEB_EXT)
  ReportDnsHijackHitInfo(host, RecordQueryType::LOCALLY,
                         out_results.ip_endpoints());
#endif  // BUILDFLAG(IS_ARKWEB_EXT)

  out_results.set_ip_endpoints(ip_endpoints_modified);
  if (need_to_modify_resolve_result) {
    out_results.set_error(ERR_DNS_CACHE_MISS);
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
void HostResolverManager::ServiceEndpointRequestImpl::set_resolve_info(
    int error_code,
    int dns_status,
    const std::vector<TaskType> finished_tasks,
    const std::vector<IPEndPoint>& truncation_results) {
  resolve_info_.error_code = error_code;
  resolve_info_.dns_status = dns_status;
  if (finalized_result_.has_value()) {
    for (const auto& ip : finalized_result_->endpoints) {
      for (const auto& ipv4 : ip.ipv4_endpoints) {
        resolve_info_.AppendAddress(ipv4);
      }
      for (const auto& ipv6 : ip.ipv6_endpoints) {
        resolve_info_.AppendAddress(ipv6);
      }
    }
  }

  for (const auto& ip : truncation_results) {
    resolve_info_.truncation_ips.emplace_back(ip.ToStringWithoutPort());
  }

  for (auto task : finished_tasks) {
    resolve_info_.AddTransitionType(
        static_cast<int>(HostResolverManager::ConvertFromTaskType(task)));
  }
}
#endif

}  // namespace net
