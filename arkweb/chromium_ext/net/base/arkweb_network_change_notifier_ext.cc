// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "arkweb/chromium_ext/net/base/arkweb_network_change_notifier_ext.h"
#include "net/base/network_change_notifier.h"

#include <limits>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>

#include "base/memory/ref_counted.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/no_destructor.h"
#include "base/observer_list.h"
#include "base/sequence_checker.h"
#include "base/strings/string_util.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread_checker.h"
#include "base/timer/timer.h"
#include "build/build_config.h"
#include "net/base/network_change_notifier_factory.h"
#include "net/base/network_interfaces.h"
#include "net/base/url_util.h"
#include "net/dns/dns_config.h"
#include "net/dns/dns_config_service.h"
#include "net/dns/system_dns_config_change_notifier.h"
#include "net/url_request/url_request.h"
#include "url/gurl.h"

namespace net {

ArkwebNetworkChangeNotifierExt* g_network_change_notifier = nullptr;

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
// static
const std::vector<std::string> ArkwebNetworkChangeNotifierExt::GetDnsServers()
{
  std::vector<std::string> dns_servers;
  if (!g_network_change_notifier) {
    return dns_servers;
  }
  dns_servers = g_network_change_notifier->GetCurrentDnsServers();
  return dns_servers;
}

const std::vector<std::string> ArkwebNetworkChangeNotifierExt::GetCurrentDnsServers()
{
  return std::vector<std::string>();
}
#endif

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
// static
void ArkwebNetworkChangeNotifierExt::BindToNetwork(int32_t network_for_dns)
{
  if (g_network_change_notifier) {
    g_network_change_notifier->BindDnsToNetwork(network_for_dns);
  } else {
    LOG(ERROR) << "NetworkChangeNotifier bindDnsToNetwork failed, "
                  "network_for_dns "
               << network_for_dns;
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "NetworkChangeNotifier bindDnsToNetwork failed, "
                           "network_for_dns "
                        << network_for_dns;
#endif
  }
}

void ArkwebNetworkChangeNotifierExt::BindDnsToNetwork(int32_t network_for_dns)
{
  LOG(INFO) << "NetworkChangeNotifier bindDnsToNetwork, network_for_dns "
            << network_for_dns;

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO)
      << "NetworkChangeNotifier bindDnsToNetwork, network_for_dns "
      << network_for_dns;
#endif
  return;
}
#endif

}  // namespace net
