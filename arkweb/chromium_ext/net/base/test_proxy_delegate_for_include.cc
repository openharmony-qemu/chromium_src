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
void TestProxyDelegate::OnTunnelConnectResult(
    const net::ProxyChain& proxy_chain,
    const std::string& host,
    const net::HttpResponseHeaders& response_headers,
    const net::HttpRequestHeaders& request_headers) {}

void TestProxyDelegate::AddSuccessMainFrameHosts(const std::string& host) {}

bool TestProxyDelegate::IsFallbackProxyFailedHost(const std::string& host) {
  return false;
}

bool TestProxyDelegate::IsFallbackProxySuccessMainFrameHost(
    const std::string& host) {
  return false;
}
void TestProxyDelegate::AddByPassRuleWithHost(const std::string& host) {}

void TestProxyDelegate::OnProxyConnectResult(const net::ProxyChain& proxy_chain,
                                             int net_error) {}

bool TestProxyDelegate::GetUrlMaliciousTypeAndHwCode(
    const std::vector<GURL>& url_chain,
    int* malicious_type,
    int* hw_code) {
  return false;
}

bool TestProxyDelegate::IsFallbackProxyMaliciousType(int malicious_type) {
  return false;
}

bool TestProxyDelegate::IsFallbackProxyHwCode(int hw_code) {
  return false;
}

bool TestProxyDelegate::IsFallbackProxyRetryErrorCode(int net_error) {
  return false;
}

bool TestProxyDelegate::IsFallbackProxyBlockHost(const std::string& host) {
  return false;
}

int TestProxyDelegate::GetMaliciousUrlCheckWaitTime() {
  return 0;
}

int TestProxyDelegate::GetProxyConnectTimeout() {
  return 0;
}

int TestProxyDelegate::GetProxyTunnelTimeout() {
  return 0;
}

net::FallbackProxyStatus TestProxyDelegate::GetFallbackProxyStatus() {
  return net::FallbackProxyStatus::NONE;
}

bool TestProxyDelegate::IsFallbackProxyServer(
    const net::ProxyChain& proxy_chain) {
  return false;
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
