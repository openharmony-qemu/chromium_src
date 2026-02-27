/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/net/dns/secure_dns_fallback_utils.h"

#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "base/logging.h"
#include "base/no_destructor.h"

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

namespace net {
namespace {
base::NoDestructor<std::set<IPAddress>> g_suspect_ip_list;
base::NoDestructor<std::set<std::string>> g_source_host_list;
}  // namespace

void StoreSuspectIPListAndSourceHostList(
    const std::vector<std::string>& ip_list,
    const std::vector<std::string>& host_list) {
  g_source_host_list->clear();
  g_suspect_ip_list->clear();
  for (auto& host : host_list) {
    g_source_host_list->insert(host);
  }
  for (size_t i = 0; i < ip_list.size(); ++i) {
    net::IPAddress ip_address;
    if (ip_address.AssignFromIPLiteral(ip_list[i])) {
      g_suspect_ip_list->insert(ip_address);
    }
  }
  LOG(INFO) << "StoreSuspectIPListAndSourceHostList, ip_list.size "
            << ip_list.size() << ", host_list.size " << host_list.size()
            << ", g_source_host_list.size " << g_source_host_list->size()
            << ", g_suspect_ip_list.size " << g_suspect_ip_list->size();
}

bool MaybeNeedToProcessAddressList(
    const std::string& host,
    const std::vector<IPEndPoint>& legacy_addresses,
    bool doh_fallback_available,
    std::vector<IPEndPoint>& out_addresses,
    bool& need_to_modify_result,
    std::vector<IPEndPoint>& truncation_address) {
  if (legacy_addresses.empty()) {
    return false;
  }
  need_to_modify_result = false;

  if (g_source_host_list->find(host) == g_source_host_list->end()) {
    return false;
  }

  bool matched_suspect_ip = false;
  for (const IPEndPoint& endpoint : legacy_addresses) {
    if (g_suspect_ip_list->find(endpoint.address()) ==
        g_suspect_ip_list->end()) {
      out_addresses.push_back(endpoint);
    } else {
      truncation_address.push_back(endpoint);
      matched_suspect_ip = true;
    }
  }

  if (!matched_suspect_ip) {
    return false;
  }

  if (out_addresses.empty()) {
    if (doh_fallback_available) {
      need_to_modify_result = true;
    } else {
      out_addresses = legacy_addresses;
    }
  }

  return true;
}

}  // namespace net

#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
