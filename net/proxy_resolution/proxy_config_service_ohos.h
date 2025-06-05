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
 
#ifndef NET_PROXY_RESOLUTION_PROXY_CONFIG_SERVICE_OHOS_H
#define NET_PROXY_RESOLUTION_PROXY_CONFIG_SERVICE_OHOS_H
 
#include "net/proxy_resolution/polling_proxy_config_service.h"
 
#include <network/netmanager/net_connection.h>
 
#include "net/proxy_resolution/proxy_config.h"
#include "net/proxy_resolution/proxy_config_with_annotation.h"
 
namespace net {
 
ProxyConfig CreateProxyConfig(NetConn_HttpProxy& http_proxy);
 
void GetCurrentProxyConfig(const NetworkTrafficAnnotationTag traffic_annotation,
                           ProxyConfigWithAnnotation* config);
 
class ProxyConfigServiceOhos : public PollingProxyConfigService {
 public:
  // Constructs a ProxyConfigService
  // that watches the OHOS system proxy settings.
  explicit ProxyConfigServiceOhos(
      const NetworkTrafficAnnotationTag& traffic_annotation);
 
  ProxyConfigServiceOhos(const NetworkTrafficAnnotationTag& traffic_annotation,
                         GetConfigFunction get_config_func);
 
  ProxyConfigServiceOhos(const ProxyConfigServiceOhos&) = delete;
  ProxyConfigServiceOhos& operator=(const ProxyConfigServiceOhos&) = delete;
 
  ~ProxyConfigServiceOhos() override;
};
 
}  // namespace net
 
#endif  // NET_PROXY_RESOLUTION_PROXY_CONFIG_SERVICE_OHOS_H
