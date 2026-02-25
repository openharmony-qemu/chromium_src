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

#ifndef ARKWEB_CHROMIUM_EXT_NET_DNS_PUBLIC_DNS_INFO_H_
#define ARKWEB_CHROMIUM_EXT_NET_DNS_PUBLIC_DNS_INFO_H_

#include "arkweb/chromium_ext/net/dns/public/resolve_info.h"
#include "base/values.h"
#include "net/base/net_errors.h"

namespace net {

// Host resolution info.
struct NET_EXPORT DnsInfo {
  DnsInfo();

  DnsInfo(const DnsInfo& dns_info);
  DnsInfo(DnsInfo&& other);
  ~DnsInfo();

  DnsInfo& operator=(const DnsInfo& other);
  DnsInfo& operator=(DnsInfo&& other);

  enum DnsTaskTransitionType : uint32_t {
    DEFAULT = 0,
    SYSTEM = 0x00000001,
    DNS = 0x00000010,
    SECURE_DNS = 0x00000100,
    CACHE_LOOKUP = 0x00001000,
    INSECURE_CACHE = 0x00010000,
    SECURE_CACHE = 0x00100000,
    SECURE_DNS_FALLBACK = 0x01000000
  };

  bool UsedHttpDns() const;
  bool UsedInsecureDns() const;

  int result = net::OK;
  uint32_t dns_transition = 0;
  int dns_status = kDnsResolvedUndefined;
  std::string host;
  std::vector<std::string> address_list = {};
  std::vector<std::string> truncation_ips = {};
};

}  // namespace net

#endif  // ARKWEB_CHROMIUM_EXT_NET_DNS_PUBLIC_DNS_INFO_H_
