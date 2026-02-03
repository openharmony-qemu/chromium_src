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

#ifndef ARKWEB_NET_DNS_SECURE_DNS_FALLBACK_UTILS_H_
#define ARKWEB_NET_DNS_SECURE_DNS_FALLBACK_UTILS_H_

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "net/base/address_list.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_export.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif  // BUILDFLAG(IS_ARKWEB_EXT)

namespace net {
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
enum RecordQueryType { LOCALLY, UDP, PROC };

NET_EXPORT_PRIVATE void StoreSuspectIPListAndSourceHostList(
    const std::vector<std::string>& ip_list,
    const std::vector<std::string>& host_list);
NET_EXPORT_PRIVATE bool MaybeNeedToProcessAddressList(
    const std::string& host,
    const std::vector<IPEndPoint>& legacy_addresses,
    bool doh_fallback_available,
    std::vector<IPEndPoint>& out_addresses,
    bool& need_to_modify_result,
    std::vector<IPEndPoint>& truncation_address);
#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
}  // namespace net
#endif  // ARKWEB_NET_DNS_SECURE_DNS_FALLBACK_UTILS_H_
