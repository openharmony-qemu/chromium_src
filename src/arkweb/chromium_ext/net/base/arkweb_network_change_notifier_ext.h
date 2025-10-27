// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef NET_BASE_ARKWEB_NETWORK_CHANGE_NOTIFIER_EXT_H_
#define NET_BASE_ARKWEB_NETWORK_CHANGE_NOTIFIER_EXT_H_

#include <stdint.h>

#include <memory>
#include <vector>

#include "net/base/network_change_notifier.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/observer_list_threadsafe.h"
#include "base/strings/cstring_view.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "net/base/net_export.h"
#include "net/base/network_handle.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace net {

class NET_EXPORT ArkwebNetworkChangeNotifierExt : public NetworkChangeNotifier {
 public:
  explicit ArkwebNetworkChangeNotifierExt(
      const NetworkChangeCalculatorParams& params =
          NetworkChangeCalculatorParams(),
      SystemDnsConfigChangeNotifier* system_dns_config_notifier = nullptr,
      bool omit_observers_in_constructor_for_testing = false)
      :NetworkChangeNotifier(params, system_dns_config_notifier, omit_observers_in_constructor_for_testing) {}
  ArkwebNetworkChangeNotifierExt(const ArkwebNetworkChangeNotifierExt&) = delete;
  ArkwebNetworkChangeNotifierExt& operator=(const ArkwebNetworkChangeNotifierExt&) =
      delete;

  ArkwebNetworkChangeNotifierExt* AsArkwebNetworkChangeNotifierExt() {
    return this;
  }

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
  static const std::vector<std::string> GetDnsServers();
#endif
#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
  static void BindToNetwork(int32_t network_for_dns);
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
  virtual const std::vector<std::string> GetCurrentDnsServers();
#endif
#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
  virtual void BindDnsToNetwork(int32_t network_for_dns);
#endif
};

}  // namespace net

#endif  // NET_BASE_NETWORK_CHANGE_NOTIFIER_H_