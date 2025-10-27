// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef_dns_data_base.h"

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_PER_DFX)
#include <set>
#endif
#include "base/logging.h"
#include "net/base/ip_endpoint.h"
#include "nweb_pre_dns_adapter.h"
#include "ohos_adapter_helper.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

#if BUILDFLAG(ARKWEB_PER_DFX)
namespace {
std::set<const std::string> g_hostname;
}
#endif
void CacheHostName(const std::string& hostname) {
#if BUILDFLAG(ARKWEB_PER_DFX)
  if (g_hostname.count(hostname) == 0) {
    g_hostname.insert(hostname);
    auto& dnsDatabaseAdapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                   .GetWebDnsDataBaseInstance();
    dnsDatabaseAdapter.InsertHostname(hostname);
  }
#endif
}

net::AddressList GetAddrList(const std::string& hostname) {
  using OHOS::NWeb::g_AddrInfoMap;
  auto it = g_AddrInfoMap.find(hostname);
  if (it == g_AddrInfoMap.end()) {
    return net::AddressList();
  }

  net::AddressList addr_list;
  addrinfo* addrInfo = it->second;
  auto canonical_name =
      addrInfo->ai_canonname
          ? absl::make_optional(std::string(addrInfo->ai_canonname))
          : absl::nullopt;
  if (canonical_name) {
    addr_list.SetDnsAliases({*canonical_name});
  }
  for (auto ai = addrInfo; ai != nullptr; ai = ai->ai_next) {
    net::IPEndPoint ipe;
    // NOTE: Ignoring non-INET* families.
    if (ipe.FromSockAddr(ai->ai_addr, ai->ai_addrlen)) {
      addr_list.push_back(ipe);
    } else {
      LOG(INFO) << "Unknown family found in addrinfo: " << ai->ai_family;
    }
  }
  return addr_list;
}
