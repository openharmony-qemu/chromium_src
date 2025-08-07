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

#ifndef OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_PROXY_TYPES_H
#define OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_PROXY_TYPES_H

#include <optional>
#include <string>
#include <vector>

enum NWebExtensionProxyMode {
  DIRECT = 0,
  AUTO_DETECT = 1,
  PAC_SCRIPT = 2,
  FIXED_SERVERS = 3,
  SYSTEM = 4,
};

enum NWebExtensionScheme {
  INVALID = 0,
  HTTP = 1,
  HTTPS = 2,
  QUIC = 3,
  SOCKS4 = 4,
  SOCKS5 = 5,
};

struct NWebExtensionPacScript {
  bool mandatory = false;
  std::string url;
};

struct NWebExtensionProxyServer {
  std::string host;
  std::optional<int> port = std::nullopt;
  std::optional<NWebExtensionScheme> scheme = std::nullopt;
};

struct NWebExtensionProxyRules {
  std::vector<std::string> bypassList;
  std::optional<NWebExtensionProxyServer> fallbackProxy = std::nullopt;
  std::optional<NWebExtensionProxyServer> proxyForFtp = std::nullopt;
  std::optional<NWebExtensionProxyServer> proxyForHttp = std::nullopt;
  std::optional<NWebExtensionProxyServer> proxyForHttps = std::nullopt;
  std::optional<NWebExtensionProxyServer> singleProxy = std::nullopt;
};

struct NWebExtensionEffectiveProxyInfo {
  std::string extensionId;
  NWebExtensionProxyMode mode;
  std::optional<NWebExtensionPacScript> pacScript = std::nullopt;
  std::optional<NWebExtensionProxyRules> rules = std::nullopt;
};

struct NWebExtensionProxyInfo {
  bool enable = false;
  std::optional<NWebExtensionEffectiveProxyInfo> effective = std::nullopt;
};

#endif // OHOS_NWEB_SRC_CAPI_BROWSER_SERVICE_NWEB_EXTENSION_PROXY_TYPES_H
