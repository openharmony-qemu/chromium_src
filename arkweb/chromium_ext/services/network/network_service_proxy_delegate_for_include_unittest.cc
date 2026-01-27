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

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
TEST_F(NetworkServiceProxyDelegateTest, IsFallbackProxyServerTest)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    auto proxy_chain = net::ProxyChain(net::PacResultElementToProxyServer("HTTPS proxy"));
    bool result = delegate->IsFallbackProxyServer(proxy_chain);
    EXPECT_FALSE(result);

    delegate->fallback_proxy_status_ = net::FallbackProxyStatus::NORMAL;
    result = delegate->IsFallbackProxyServer(proxy_chain);
    EXPECT_TRUE(result);

    net::ProxyChain proxy_chain_null;
    result = delegate->IsFallbackProxyServer(proxy_chain_null);
    EXPECT_FALSE(result);
}

TEST_F(NetworkServiceProxyDelegateTest, AddSuccessMainFrameHostsTest)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    const std::string test_host = "www.example.com";
    delegate->AddSuccessMainFrameHosts(test_host);
    
    delegate->AddSuccessMainFrameHosts(test_host);
    EXPECT_EQ(delegate->main_frame_using_proxy_success_list_.size(), 1);
    EXPECT_NE(delegate->main_frame_using_proxy_success_list_.find(test_host),
              delegate->main_frame_using_proxy_success_list_.end());
}

TEST_F(NetworkServiceProxyDelegateTest, ProcessFailedResultTest_001)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));
    const std::string test_host = "www.example.com";

    delegate->AddSuccessMainFrameHosts(test_host);
    ASSERT_EQ(delegate->main_frame_using_proxy_success_list_.size(), 1);

    delegate->ProcessFailedResult(test_host);

    EXPECT_EQ(delegate->main_frame_using_proxy_success_list_.size(), 0);
    EXPECT_EQ(delegate->main_frame_using_proxy_success_list_.find(test_host),
              delegate->main_frame_using_proxy_success_list_.end());
}

TEST_F(NetworkServiceProxyDelegateTest, ProcessFailedResultTest_002)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));
    const std::string test_host = "www.example.com";

    ASSERT_TRUE(delegate->using_proxy_failed_hosts_map_.empty());
    delegate->ProcessFailedResult(test_host);

    EXPECT_EQ(delegate->using_proxy_failed_hosts_map_.size(), 1);
    EXPECT_EQ(delegate->using_proxy_failed_hosts_map_[test_host], 1);
}

TEST_F(NetworkServiceProxyDelegateTest, ProcessFailedResultTest_003)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));
    const std::string test_host = "www.example.com";

    delegate->ProcessFailedResult(test_host);
    ASSERT_EQ(delegate->using_proxy_failed_hosts_map_[test_host], 1);

    delegate->ProcessFailedResult(test_host);
    EXPECT_EQ(delegate->using_proxy_failed_hosts_map_[test_host], 2);
}

TEST_F(NetworkServiceProxyDelegateTest, ProcessFailedResultTest_004)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    const std::string host1 = "www.example.com";
    const std::string host2 = "www.test.com";
    const std::string host3 = "www.another.com";
    delegate->using_proxy_failed_hosts_map_[host1] = 2;
    delegate->using_proxy_failed_hosts_map_[host3] = 1;
    delegate->ProcessSuccessResult(host1);
    delegate->ProcessSuccessResult(host2);
    delegate->ProcessSuccessResult(host3);

    EXPECT_EQ(delegate->using_proxy_failed_hosts_map_.size(), 0);
}

TEST_F(NetworkServiceProxyDelegateTest, IsFallbackProxyFailedHostTest)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    const std::string host1 = "www.example.com";
    const std::string host2 = "www.test.com";
    const std::string host3 = "www.another.com";

    delegate->using_proxy_failed_hosts_map_[host1] = 1;
    delegate->using_proxy_failed_hosts_map_[host2] = 3;
    delegate->using_proxy_failed_hosts_map_[host3] = 5;

    EXPECT_FALSE(delegate->IsFallbackProxyFailedHost(host1));
    EXPECT_TRUE(delegate->IsFallbackProxyFailedHost(host2));
    EXPECT_TRUE(delegate->IsFallbackProxyFailedHost(host3));
    EXPECT_FALSE(delegate->IsFallbackProxyFailedHost("www.new.com"));
}

TEST_F(NetworkServiceProxyDelegateTest, IsFallbackProxySuccessMainFrameHostTest)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    const std::string host1 = "www.example.com";
    const std::string host2 = "www.test.com";
    const std::string host3 = "www.another.com";

    delegate->AddSuccessMainFrameHosts(host1);
    delegate->AddSuccessMainFrameHosts(host3);

    EXPECT_TRUE(delegate->IsFallbackProxySuccessMainFrameHost(host1));
    EXPECT_FALSE(delegate->IsFallbackProxySuccessMainFrameHost(host2));
    EXPECT_TRUE(delegate->IsFallbackProxySuccessMainFrameHost(host3));
    EXPECT_FALSE(delegate->IsFallbackProxySuccessMainFrameHost("www.new.com"));
}

TEST_F(NetworkServiceProxyDelegateTest, GetUrlMaliciousTypeAndHwCodeTest_001)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    std::vector<GURL> empty_url_chain;
    int malicious_type = 0;
    int hw_code = 0;
    auto proxy_chain = net::ProxyChain(net::PacResultElementToProxyServer("HTTPS proxy"));
    delegate->OnProxyConnectResult(proxy_chain, 0);
    EXPECT_FALSE(delegate->GetUrlMaliciousTypeAndHwCode(empty_url_chain, &malicious_type, &hw_code));
}

TEST_F(NetworkServiceProxyDelegateTest, GetUrlMaliciousTypeAndHwCodeTest_002)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    std::vector<GURL> url_chain = {GURL("http://www.example.com")};
    int malicious_type = 0;
    int hw_code = 0;

    ASSERT_TRUE(delegate->url_sb_info_map_.empty());
    EXPECT_FALSE(delegate->GetUrlMaliciousTypeAndHwCode(url_chain, &malicious_type, &hw_code));
}

TEST_F(NetworkServiceProxyDelegateTest, GetUrlMaliciousTypeAndHwCodeTest_003)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    const std::string url1 = "http://www.example.com";
    const std::string url2 = "http://www.test.com";
    std::vector<GURL> url_chain = {GURL(url1), GURL(url2)};
    int malicious_type = 0;
    int hw_code = 0;
    int test_type = 3;
    int test_code = 300;
    GURL gurl(url2);
    delegate->url_sb_info_map_.Put(gurl.spec(),
                                   NetworkServiceProxyDelegate::PageSafeBrowsingInfo(test_type, test_code));

    EXPECT_TRUE(delegate->GetUrlMaliciousTypeAndHwCode(url_chain, &malicious_type, &hw_code));
    EXPECT_EQ(test_type, malicious_type);
    EXPECT_EQ(test_code, hw_code);
}

TEST_F(NetworkServiceProxyDelegateTest, IsFallbackProxyMaliciousTypeTest)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    int test_type = 100;
    ASSERT_TRUE(delegate->safe_browsing_malicious_type_list_.empty());
    EXPECT_FALSE(delegate->IsFallbackProxyMaliciousType(test_type));

    delegate->safe_browsing_malicious_type_list_.insert(test_type);
    EXPECT_TRUE(delegate->IsFallbackProxyMaliciousType(test_type));
}

TEST_F(NetworkServiceProxyDelegateTest, IsFallbackProxyHwCodeTest)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));
    int test_code = 100;
    ASSERT_TRUE(delegate->safe_browsing_hw_code_list_.empty());
    EXPECT_FALSE(delegate->IsFallbackProxyHwCode(test_code));

    delegate->safe_browsing_hw_code_list_.insert(test_code);
    EXPECT_TRUE(delegate->IsFallbackProxyHwCode(test_code));
}

TEST_F(NetworkServiceProxyDelegateTest, OnTunnelConnectResult_001)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    std::string host = "www.example.com";
    auto proxy_chain = net::ProxyChain(net::PacResultElementToProxyServer("HTTPS proxy"));
    net::ProxyChain proxy_chain_null;
    scoped_refptr<net::HttpResponseHeaders> responseHeaders =
        base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 200\nHello: World\n\n");
    net::HttpRequestHeaders requestHeaders;
    responseHeaders->response_code_ = net::FallbackProxyResponseCode::TOKEN_AUTH_FAILED;
    EXPECT_NO_FATAL_FAILURE(delegate->OnTunnelConnectResult(proxy_chain_null, host, *responseHeaders, requestHeaders));
    EXPECT_NO_FATAL_FAILURE(delegate->OnTunnelConnectResult(proxy_chain, host, *responseHeaders, requestHeaders));
}

TEST_F(NetworkServiceProxyDelegateTest, OnTunnelConnectResult_002)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("Authentication", "baz");
    auto delegate = CreateDelegate(std::move(config));

    std::string host = "www.example.com";
    auto proxy_chain = net::ProxyChain(net::PacResultElementToProxyServer("HTTPS proxy"));
    scoped_refptr<net::HttpResponseHeaders> responseHeaders =
        base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 200\nHello: World\n\n");
    net::HttpRequestHeaders requestHeaders;
    responseHeaders->response_code_ = net::FallbackProxyResponseCode::TOKEN_AUTH_FAILED;
    delegate->OnBeforeTunnelRequest(proxy_chain, /*chain_index=*/0, &requestHeaders);
    delegate->OnTunnelConnectResult(proxy_chain, host, *responseHeaders, requestHeaders);
    EXPECT_THAT(requestHeaders, Contain("Authentication", "baz"));
}

TEST_F(NetworkServiceProxyDelegateTest, OnTunnelConnectResult_003)
{
    auto config = mojom::CustomProxyConfig::New();
    config->rules.ParseFromString("https://proxy");
    config->connect_tunnel_headers.SetHeader("connect", "baz");
    auto delegate = CreateDelegate(std::move(config));

    std::string host = "www.example.com";
    auto proxy_chain = net::ProxyChain(net::PacResultElementToProxyServer("HTTPS proxy"));
    scoped_refptr<net::HttpResponseHeaders> responseHeaders =
        base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 200\nHello: World\n\n");
    net::HttpRequestHeaders requestHeaders;
    int times = 1;
    responseHeaders->response_code_ = net::FallbackProxyResponseCode::DEST_SERVER_TIME_OUT;
    delegate->OnTunnelConnectResult(proxy_chain, host, *responseHeaders, requestHeaders);
    EXPECT_EQ(delegate->using_proxy_failed_hosts_map_[host], times);

    responseHeaders->response_code_ = net::FallbackProxyResponseCode::DEST_SERVER_DISCONNECT;
    delegate->OnTunnelConnectResult(proxy_chain, host, *responseHeaders, requestHeaders);
    EXPECT_EQ(delegate->using_proxy_failed_hosts_map_[host], ++times);

    responseHeaders->response_code_ = net::FallbackProxyResponseCode::DEST_HOST_RESOLVED_FAILED;
    delegate->OnTunnelConnectResult(proxy_chain, host, *responseHeaders, requestHeaders);
    EXPECT_EQ(delegate->using_proxy_failed_hosts_map_[host], ++times);

    responseHeaders->response_code_ = net::FallbackProxyResponseCode::DEST_PORT_DENY_BY_PROXY;
    delegate->OnTunnelConnectResult(proxy_chain, host, *responseHeaders, requestHeaders);
    EXPECT_EQ(delegate->using_proxy_failed_hosts_map_[host], ++times);

    responseHeaders->response_code_ = net::HTTP_OK;
    delegate->OnTunnelConnectResult(proxy_chain, host, *responseHeaders, requestHeaders);
    EXPECT_EQ(delegate->using_proxy_failed_hosts_map_.size(), 0);
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)