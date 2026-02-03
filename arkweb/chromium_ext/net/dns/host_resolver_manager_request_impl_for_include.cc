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

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "arkweb/chromium_ext/net/dns/secure_dns_fallback_utils.h"
#include "base/base_switches.h"
#include "base/command_line.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/overrides/net/dns/secure_dns_fallback_utils.h"
#endif  // BUILDFLAG(IS_ARKWEB_EXT)

namespace net {

void HostResolverManager::RequestImpl::
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

void HostResolverManager::RequestImpl::MaybeModifyResolveLocallyResults(
    HostCache::Entry& out_results,
    std::vector<IPEndPoint>& truncation_results) {
  if (out_results.error() != OK) {
    return;
  }
  if (out_results.ip_endpoints().empty()) {
    return;
  }

  std::string host(request_host_.GetHostnameWithoutBrackets());
  bool secure_dns_fallback_available = false;
  if (resolver_) {
    secure_dns_fallback_available =
        resolver_->CanUseSecureDnsFallback(resolve_context());
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

}  // namespace net
#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
