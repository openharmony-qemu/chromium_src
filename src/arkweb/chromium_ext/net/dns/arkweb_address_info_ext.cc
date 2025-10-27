// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifdef UNSAFE_BUFFERS_BUILD
// TODO(crbug.com/40284755): Remove this and spanify to fix the errors.
#pragma allow_unsafe_buffers
#endif

#include "net/dns/address_info.h"

#include <memory>
#include <optional>

#include "base/logging.h"
#include "base/notreached.h"
#include "base/sys_byteorder.h"
#include "build/build_config.h"
#include "net/base/address_list.h"
#include "net/base/net_errors.h"
#include "net/base/sys_addrinfo.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(IS_ANDROID)
#include "net/android/network_library.h"
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
#include <dlfcn.h>
#include <netdb.h>

#include "base/files/file.h"
#include "base/native_library.h"
#include "net/base/network_handle.h"
#include "arkweb/chromium_ext/net/dns/arkweb_address_info_ext.h"
#endif

namespace net {

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
using OHGetAddrInfoForNetwork = int32_t (*)(char* host,
                                            char* serv,
                                            struct addrinfo* hints,
                                            struct addrinfo** res,
                                            int32_t net_id);

using OHFreeDnsResult = int32_t (*)(struct addrinfo* res);

OHGetAddrInfoForNetwork GetOHGetAddrInfoForNetwork() {
#if defined(ARKWEB_ARM64)
  base::FilePath file("/system/lib64/ndk/libnet_connection.so");
#else
  base::FilePath file("/system/lib/ndk/libnet_connection.so");
#endif
  void* dl = dlopen(file.value().c_str(), RTLD_NOW);
  return dl == nullptr ? nullptr
                       : reinterpret_cast<OHGetAddrInfoForNetwork>(
                             dlsym(dl, "OH_NetConn_GetAddrInfo"));
}

OHFreeDnsResult GetOHFreeDnsResult() {
#if defined(ARKWEB_ARM64)
  base::FilePath file("/system/lib64/ndk/libnet_connection.so");
#else
  base::FilePath file("/system/lib/ndk/libnet_connection.so");
#endif
  void* dl = dlopen(file.value().c_str(), RTLD_NOW);
  return dl == nullptr ? nullptr
                       : reinterpret_cast<OHFreeDnsResult>(
                             dlsym(dl, "OH_NetConn_FreeDnsResult"));
}

namespace ohos {

NO_SANITIZE("cfi-icall")
int GetAddrInfoForNetwork(char* host,
                          char* serv,
                          struct addrinfo* hints,
                          struct addrinfo** res,
                          int32_t network) {
  if (network == handles::kInvalidNetworkHandle) {
    errno = EINVAL;
    return EAI_SYSTEM;
  }

  static OHGetAddrInfoForNetwork get_addrinfo_for_network =
      GetOHGetAddrInfoForNetwork();
  if (!get_addrinfo_for_network) {
    errno = ENOSYS;
    return EAI_SYSTEM;
  }

  return get_addrinfo_for_network(host, serv, hints, res, network);
}

NO_SANITIZE("cfi-icall") int FreeDnsResult(struct addrinfo* res) {
  static OHFreeDnsResult free_dns_result = GetOHFreeDnsResult();
  if (!free_dns_result) {
    errno = ENOSYS;
    return EAI_SYSTEM;
  }

  return free_dns_result(res);
}

}  // namespace ohos
#endif  // ARKWEB_EXT_NETWORK_CONNECTION

}  // namespace net
