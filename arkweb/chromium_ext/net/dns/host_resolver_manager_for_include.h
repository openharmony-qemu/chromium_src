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

#ifndef NET_DNS_HOST_RESOLVER_MANAGER_FOR_INCLUDE_H
#define NET_DNS_HOST_RESOLVER_MANAGER_FOR_INCLUDE_H

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
public:
// Return true if Doh fallback server(s) exist and it/they can resolve
// successfully
bool CanUseSecureDnsFallback(ResolveContext* context) const;
void SetHttpsDnsFallbackData(bool enabled, const std::string& server_template);
void SetSuspectIpListAndSourceHostList(
    const std::vector<std::string>& ip_list,
    const std::vector<std::string>& host_list);

private:
friend class ArkWebHostResolverManagerJobExt;
class WarmUpHttpDnsFallbackImpl;

enum class DnsTransactionAddressFailedType {
  BOTH_OK,
  IPV4_ADDRESS_FAILED,
  IPV6_ADDRESS_FAILED,
  BOTH_FAILED,
};

void ReportSecureFallbackDnsResult(
    const std::optional<HostCache::Entry> insecure_results,
    const HostCache::Entry& secure_fallback_results,
    const std::string& host,
    const int index,
    const base::TimeDelta& duration);
void ReportDnsTransactionResult(int index,
                                const std::string& host,
                                int result_for_ipv4,
                                int result_for_ipv6);
void WarmUpHttpsDnsFallback(ResolveContext* context);
bool https_dns_fallback_enabled_{false};
std::string doh_fallback_server_template_;
std::vector<std::unique_ptr<WarmUpHttpDnsFallbackImpl>>
    warmup_httpdns_fallback_list_;
#endif

#endif  // NET_DNS_HOST_RESOLVER_MANAGER_FOR_INCLUDE_H