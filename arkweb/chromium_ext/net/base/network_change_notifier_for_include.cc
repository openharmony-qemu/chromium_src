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

namespace net {
// The process-wide singleton notifier.
namespace {
  extern NetworkChangeNotifier* g_network_change_notifier;
}

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
// static
const std::vector<std::string> NetworkChangeNotifier::GetDnsServers() {
  std::vector<std::string> dns_servers;
  if (!g_network_change_notifier) {
    return dns_servers;
  }
  dns_servers = g_network_change_notifier->GetCurrentDnsServers();
  return dns_servers;
}

const std::vector<std::string> NetworkChangeNotifier::GetCurrentDnsServers() {
  return std::vector<std::string>();
}
#endif

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
const std::vector<std::string> NetworkChangeNotifier::GetCurrentNetAddrList() {
  return std::vector<std::string>();
}
const std::vector<std::string> NetworkChangeNotifier::GetNetAddrListByNetId(int32_t netId) {
  return std::vector<std::string>();
}

void NetworkChangeNotifier::SetNetAddrList(std::vector<std::string> newNetAddrList) {
  return;
}

// static
void NetworkChangeNotifier::BindToNetwork(int32_t network_for_dns) {
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

void NetworkChangeNotifier::BindDnsToNetwork(int32_t network_for_dns) {
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
}