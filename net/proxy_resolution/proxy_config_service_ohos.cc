/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "net/proxy_resolution/proxy_config_service_ohos.h"

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/logging.h"
#include "net/base/proxy_server.h"
 
namespace net {
 
static const int kPollIntervalSec = 10;
static const std::string kAsterisk = "*";
 
ProxyConfig CreateProxyConfig(NetConn_HttpProxy& http_proxy) {
  if (std::string{http_proxy.host}.empty() && http_proxy.port == 0) {
    return ProxyConfig::CreateDirect();
  }
 
  std::string host = http_proxy.host;
  int32_t port = http_proxy.port;
  std::vector<std::string> exclusions{http_proxy.exclusionListSize};
  if (http_proxy.exclusionListSize != 0) {
    for (int i = 0; i < http_proxy.exclusionListSize; i++) {
      exclusions[i] = std::string{http_proxy.exclusionList[i]};
    }
  }
 
  // see
  // https://docs.openharmony.cn/pages/v4.0/zh-cn/application-dev/reference/
  // apis/js-apis-net-connection.md/#httpproxy10
  // A single asterisk (*) is the only valid wildcard. If the list contains only
  // wildcards, the wildcards match all host names; that is, the HTTP proxy is
  // disabled. A wildcard can only be added independently. It cannot be added to
  // the list together with other domain names or IP addresses. Otherwise, the
  // wildcard does not take effect.
  if (exclusions.size() == 1 && exclusions[0] == kAsterisk) {
    return ProxyConfig::CreateDirect();
  }
 
  ProxyBypassRules proxy_bypass_rules;
  for (const std::string& exclusion : exclusions) {
    if (exclusion == kAsterisk) {
      continue;
    }
    proxy_bypass_rules.AddRuleFromString(exclusion);
  }
 
  ProxyConfig proxy_config;
  proxy_config.proxy_rules().bypass_rules = std::move(proxy_bypass_rules);
  proxy_config.proxy_rules().type = ProxyConfig::ProxyRules::Type::PROXY_LIST;
  ProxyServer proxy_server = ProxyServer::FromSchemeHostAndPort(
      ProxyServer::Scheme::SCHEME_HTTP, host, port);
  proxy_config.proxy_rules().single_proxies.SetSingleProxyServer(proxy_server);
  proxy_config.set_from_system(true);
  return proxy_config;
}
 
void GetCurrentProxyConfig(const NetworkTrafficAnnotationTag traffic_annotation,
                           ProxyConfigWithAnnotation* config) {
  NetConn_HttpProxy http_proxy;
  int32_t result = OH_NetConn_GetDefaultHttpProxy(&http_proxy);
  ProxyConfig proxy_config;
  if (result != 0) {
    LOG(ERROR) << "OH_NetConn_GetDefaultHttpProxy Failed.";
    proxy_config = ProxyConfig::CreateDirect();
  } else {
    proxy_config = CreateProxyConfig(http_proxy);
  }
  *config = ProxyConfigWithAnnotation(proxy_config, traffic_annotation);
}
 
ProxyConfigServiceOhos::ProxyConfigServiceOhos(
    const NetworkTrafficAnnotationTag& traffic_annotation)
    : PollingProxyConfigService(base::Seconds(kPollIntervalSec),
                                base::BindRepeating(GetCurrentProxyConfig),
                                traffic_annotation) {}
 
ProxyConfigServiceOhos::ProxyConfigServiceOhos(
    const NetworkTrafficAnnotationTag& traffic_annotation,
    GetConfigFunction get_config_func)
    : PollingProxyConfigService(base::Seconds(kPollIntervalSec),
                                get_config_func,
                                traffic_annotation) {}
 
ProxyConfigServiceOhos::~ProxyConfigServiceOhos() = default;
 
}  // namespace net
