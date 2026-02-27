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

#include "arkweb/chromium_ext/net/dns/public/resolve_info.h"

namespace net {

ResolveInfo::ResolveInfo() {}

ResolveInfo::ResolveInfo(int error_code, int dns_status)
    : error_code(error_code), dns_status(dns_status) {}

ResolveInfo::ResolveInfo(int error_code,
                         int dns_status,
                         uint32_t dns_transition,
                         const std::string& query_host)
    : error_code(error_code),
      dns_status(dns_status),
      dns_transition(dns_transition),
      query_host(query_host) {}

ResolveInfo::ResolveInfo(const ResolveInfo& resolve_info) = default;

ResolveInfo::ResolveInfo(ResolveInfo&& other) = default;

ResolveInfo::~ResolveInfo() = default;

ResolveInfo& ResolveInfo::operator=(const ResolveInfo& other) = default;

ResolveInfo& ResolveInfo::operator=(ResolveInfo&& other) = default;

bool ResolveInfo::operator==(const ResolveInfo& other) const {
  return error_code == other.error_code && dns_status == other.dns_status;
}

bool ResolveInfo::operator!=(const ResolveInfo& other) const {
  return !(*this == other);
}

// static
bool ResolveInfo::IsResolvedByHttpsDns(int dns_status) {
  return dns_status == kDnsResolvedByHttpsDns ||
         dns_status == kDnsResolvedFromSecureCache;
}

void ResolveInfo::SetIPEndPoints(const std::vector<IPEndPoint>& endpoints) {
  for (const auto& ip : endpoints) {
    ip_endpoints.emplace_back(ip.ToStringWithoutPort());
  }
}

void ResolveInfo::AppendAddress(const IPEndPoint& endpoint) {
  ip_endpoints.emplace_back(endpoint.ToStringWithoutPort());
}

void ResolveInfo::AppendTruncationAddress(const IPEndPoint& endpoint) {
  truncation_ips.emplace_back(endpoint.ToStringWithoutPort());
}

void ResolveInfo::AddTransitionType(int type) {
  dns_transition |= type;
}

}  // namespace net
