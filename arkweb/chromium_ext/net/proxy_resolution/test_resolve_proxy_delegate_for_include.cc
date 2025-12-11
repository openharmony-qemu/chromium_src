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

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
  void OnTunnelConnectResult(
      const net::ProxyChain& proxy_chain,
      const std::string& host,
      const net::HttpResponseHeaders& response_headers,
      const net::HttpRequestHeaders& request_headers) override {}
  void AddSuccessMainFrameHosts(const std::string& host) override {}
  bool IsFallbackProxyFailedHost(const std::string& host) override {
    return false;
  }
  bool IsFallbackProxySuccessMainFrameHost(const std::string& host) override {
    return false;
  }
  void AddByPassRuleWithHost(const std::string& host) override {}
  void OnProxyConnectResult(const net::ProxyChain& proxy_chain,
                            int net_error) override {}
  bool GetUrlMaliciousTypeAndHwCode(const std::vector<GURL>& url_chain,
                                    int* malicious_type,
                                    int* hw_code) override {
    return false;
  }
  bool IsFallbackProxyMaliciousType(int malicious_type) override {
    return false;
  }
  bool IsFallbackProxyHwCode(int hw_code) override { return false; }
  bool IsFallbackProxyRetryErrorCode(int net_error) override { return false; }
  bool IsFallbackProxyBlockHost(const std::string& host) override {
    return false;
  }
  int GetMaliciousUrlCheckWaitTime() override { return 0; }
  int GetProxyConnectTimeout() override { return 0; }
  int GetProxyTunnelTimeout() override { return 0; }
  net::FallbackProxyStatus GetFallbackProxyStatus() override {
    return net::FallbackProxyStatus::NONE;
  }
  bool IsFallbackProxyServer(const net::ProxyChain& proxy_chain) override {
    return false;
  }
#endif
