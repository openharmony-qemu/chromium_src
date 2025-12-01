// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
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