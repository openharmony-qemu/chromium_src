/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

class MockProxyDelegate : public ProxyDelegate {
 public:
  MOCK_METHOD(void, OnResolveProxy,
              (const GURL& url,
               const NetworkAnonymizationKey& network_anonymization_key,
               const std::string& method,
               const ProxyRetryInfoMap& proxy_retry_info,
               ProxyInfo* result), (override));

  MOCK_METHOD(void, OnFallback,
              (const ProxyChain& bad_chain, int net_error), (override));

  MOCK_METHOD(void, OnSuccessfulRequestAfterFailures,
              (const ProxyRetryInfoMap& proxy_retry_info), (override));

  MOCK_METHOD(Error, OnBeforeTunnelRequest,
              (const ProxyChain& proxy_chain,
               size_t chain_index,
               HttpRequestHeaders* extra_headers), (override));

  MOCK_METHOD(Error, OnTunnelHeadersReceived,
              (const net::ProxyChain& proxy_chain,
               size_t chain_index,
               const HttpResponseHeaders& response_headers), (override));

  MOCK_METHOD(void, SetProxyResolutionService,
              (ProxyResolutionService* proxy_resolution_service), (override));

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
  MOCK_METHOD(void, OnTunnelConnectResult,
              (const net::ProxyChain& proxy_chain,
               const std::string& host,
               const net::HttpResponseHeaders& response_headers,
               const net::HttpRequestHeaders& request_headers), (override));

  MOCK_METHOD(void, AddSuccessMainFrameHosts,
              (const std::string& host), (override));

  MOCK_METHOD(bool, IsFallbackProxyFailedHost,
              (const std::string& host), (override));

  MOCK_METHOD(bool, IsFallbackProxySuccessMainFrameHost,
              (const std::string& host), (override));

  MOCK_METHOD(void, AddByPassRuleWithHost,
              (const std::string& host), (override));

  MOCK_METHOD(void, OnProxyConnectResult,
              (const net::ProxyChain& proxy_chain, int net_error), (override));

  MOCK_METHOD(bool, GetUrlMaliciousTypeAndHwCode,
              (const std::vector<GURL>& url_chain,
               int* malicious_type,
               int* hw_code), (override));

  MOCK_METHOD(bool, IsFallbackProxyMaliciousType,
              (int malicious_type), (override));

  MOCK_METHOD(bool, IsFallbackProxyHwCode,
              (int hw_code), (override));

  MOCK_METHOD(bool, IsFallbackProxyRetryErrorCode,
              (int net_error), (override));

  MOCK_METHOD(bool, IsFallbackProxyBlockHost,
              (const std::string& host), (override));

  MOCK_METHOD(int, GetMaliciousUrlCheckWaitTime, (), (override));

  MOCK_METHOD(int, GetProxyConnectTimeout, (), (override));

  MOCK_METHOD(int, GetProxyTunnelTimeout, (), (override));

  MOCK_METHOD(net::FallbackProxyStatus, GetFallbackProxyStatus, (), (override));

  MOCK_METHOD(bool, IsFallbackProxyServer,
              (const net::ProxyChain& proxy_chain), (override));
#endif
};
#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
TEST_F(URLRequestTest, HandleFallbackProxyResult_001) {
  auto context_builder = CreateTestURLRequestContextBuilder();
  auto host_resolver = std::make_unique<MockHostResolver>();
  host_resolver->rules()->AddRule("example.com", "127.0.0.1");
  context_builder->set_host_resolver(std::move(host_resolver));
  auto context = context_builder->Build();

  TestDelegate d;
  std::unique_ptr<URLRequest> req(
      context->CreateRequest(GURL("http://example.com"), DEFAULT_PRIORITY, &d,
                             TRAFFIC_ANNOTATION_FOR_TESTS));
  req->HandleFallbackProxyResult();
  req->Start();
  d.RunUntilComplete();
  EXPECT_FALSE(req->response_info_.used_fallback_proxy);
}

TEST_F(URLRequestTest, HandleFallbackProxyResult_002) {
  auto context_builder = CreateTestURLRequestContextBuilder();
  auto host_resolver = std::make_unique<MockHostResolver>();
  host_resolver->rules()->AddRule("example.com", "127.0.0.1");
  context_builder->set_host_resolver(std::move(host_resolver));
  auto context = context_builder->Build();

  TestDelegate d;
  std::unique_ptr<URLRequest> req(
      context->CreateRequest(GURL("http://example.com"), DEFAULT_PRIORITY, &d,
                             TRAFFIC_ANNOTATION_FOR_TESTS));
  req->response_info_.used_fallback_proxy = true;
  req->HandleFallbackProxyResult();
  req->Start();
  d.RunUntilComplete();
  EXPECT_FALSE(req->response_info_.used_fallback_proxy);

}

TEST_F(URLRequestTest, HandleFallbackProxyResult_003) {
  auto context_builder = CreateTestURLRequestContextBuilder();
  auto host_resolver = std::make_unique<MockHostResolver>();
  host_resolver->rules()->AddRule("example.com", "127.0.0.1");
  context_builder->set_host_resolver(std::move(host_resolver));
  auto context = context_builder->Build();
  context->proxy_delegate_ = std::make_unique<MockProxyDelegate>();
  ASSERT_NE(context->proxy_delegate_, nullptr);

  TestDelegate d;
  std::unique_ptr<URLRequest> req(
      context->CreateRequest(GURL("http://example.com"), DEFAULT_PRIORITY, &d,
                             TRAFFIC_ANNOTATION_FOR_TESTS));
  req->response_info_.used_fallback_proxy = true;
  req->HandleFallbackProxyResult();
  req->Start();
  d.RunUntilComplete();
  EXPECT_FALSE(req->response_info_.used_fallback_proxy);

}

TEST_F(URLRequestTest, HandleFallbackProxyResult_004) {
  auto context_builder = CreateTestURLRequestContextBuilder();
  auto host_resolver = std::make_unique<MockHostResolver>();
  host_resolver->rules()->AddRule("example.com", "127.0.0.1");
  context_builder->set_host_resolver(std::move(host_resolver));
  auto context = context_builder->Build();
  context->proxy_delegate_ = std::make_unique<MockProxyDelegate>();
  ASSERT_NE(context->proxy_delegate_, nullptr);

  TestDelegate d;
  std::unique_ptr<URLRequest> req(
      context->CreateRequest(GURL("http://example.com"), DEFAULT_PRIORITY, &d,
                             TRAFFIC_ANNOTATION_FOR_TESTS));
  req->response_info_.used_fallback_proxy = true;
  req->fallback_proxy_error_code_ = OK;
  req->HandleFallbackProxyResult();
  req->Start();
  d.RunUntilComplete();
  EXPECT_FALSE(req->response_info_.used_fallback_proxy);
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)