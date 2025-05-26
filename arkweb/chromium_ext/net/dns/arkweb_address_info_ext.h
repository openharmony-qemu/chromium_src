// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef NET_DNS_ARKWEB_ADDRESS_INFO_EXT_H_
#define NET_DNS_ARKWEB_ADDRESS_INFO_EXT_H_

#include <memory>
#include <optional>
#include <string>
#include <tuple>

#include "base/memory/raw_ptr.h"
#include "build/build_config.h"
#include "net/base/address_family.h"
#include "net/base/net_export.h"
#include "net/base/network_handle.h"
#include "net/base/sys_addrinfo.h"

namespace net {

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
using OHGetAddrInfoForNetwork = int32_t (*)(char* host,
                                            char* serv,
                                            struct addrinfo* hints,
                                            struct addrinfo** res,
                                            int32_t net_id);

using OHFreeDnsResult = int32_t (*)(struct addrinfo* res);

OHGetAddrInfoForNetwork GetOHGetAddrInfoForNetwork();

OHFreeDnsResult GetOHFreeDnsResult();

namespace ohos {

int GetAddrInfoForNetwork(char* host,
                          char* serv,
                          struct addrinfo* hints,
                          struct addrinfo** res,
                          int32_t network);

NO_SANITIZE("cfi-icall") int FreeDnsResult(struct addrinfo* res);

}  // namespace ohos

#endif  // ARKWEB_EXT_NETWORK_CONNECTION

}  // namespace net

#endif  // NET_DNS_ARKWEB_ADDRESS_INFO_EXT_H_
