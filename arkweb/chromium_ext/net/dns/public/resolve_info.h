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

#ifndef ARKWEB_CHROMIUM_EXT_NET_DNS_PUBLIC_RESOLVE_INFO_H_
#define ARKWEB_CHROMIUM_EXT_NET_DNS_PUBLIC_RESOLVE_INFO_H_

#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"

namespace net {
static const int kDnsResolvedUndefined = 0;
static const int kDnsResolvedFromHosts =
    1;  // IP address resolved from /etc/hosts
static const int kDnsResolvedFromInsecureCache =
    2;  // IP address resolved from insecure dns cache
static const int kDnsResolvedFromSecureCache =
    3;  // IP address resolved from secure dns cache
static const int kDnsResolvedByLocalDns = 4;   // host resolved by local dns
static const int kDnsResolvedBySystemDns = 5;  // hosts resolved by system dns
static const int kDnsResolvedByHttpsDns = 6;   // hosts resolved by https dns

// Host resolution info.
struct NET_EXPORT ResolveInfo {
  ResolveInfo();
  ResolveInfo(int error_code, int dns_status);
  ResolveInfo(int error_code,
              int dns_status,
              uint32_t dns_transition,
              const std::string& query_host);

  ResolveInfo(const ResolveInfo& resolve_info);
  ResolveInfo(ResolveInfo&& other);
  ~ResolveInfo();

  ResolveInfo& operator=(const ResolveInfo& other);
  ResolveInfo& operator=(ResolveInfo&& other);

  bool operator==(const ResolveInfo& other) const;
  bool operator!=(const ResolveInfo& other) const;

  static bool IsResolvedByHttpsDns(int dns_status);
  void SetIPEndPoints(const std::vector<IPEndPoint>& endpoints);
  void AppendAddress(const IPEndPoint& endpoint);
  void AppendTruncationAddress(const IPEndPoint& endpoint);
  void AddTransitionType(int type);

  int error_code = net::OK;
  int dns_status = kDnsResolvedUndefined;
  uint32_t dns_transition = 0;
  std::string query_host;
  std::vector<std::string> ip_endpoints = {};
  std::vector<std::string> truncation_ips = {};
};

}  // namespace net

#endif  // ARKWEB_CHROMIUM_EXT_NET_DNS_PUBLIC_RESOLVE_INFO_H_
