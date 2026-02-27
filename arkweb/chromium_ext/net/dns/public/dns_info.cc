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

#include "arkweb/chromium_ext/net/dns/public/dns_info.h"

namespace net {

DnsInfo::DnsInfo() {}

DnsInfo::DnsInfo(const DnsInfo& dns_info) = default;

DnsInfo::DnsInfo(DnsInfo&& other) = default;

DnsInfo::~DnsInfo() = default;

DnsInfo& DnsInfo::operator=(const DnsInfo& other) = default;

DnsInfo& DnsInfo::operator=(DnsInfo&& other) = default;

bool DnsInfo::UsedHttpDns() const {
  return dns_status == kDnsResolvedFromSecureCache ||
         dns_status == kDnsResolvedByHttpsDns;
}

bool DnsInfo::UsedInsecureDns() const {
  return dns_status == kDnsResolvedFromHosts ||
         dns_status == kDnsResolvedFromInsecureCache ||
         dns_status == kDnsResolvedByLocalDns ||
         dns_status == kDnsResolvedBySystemDns;
}

}  // namespace net
