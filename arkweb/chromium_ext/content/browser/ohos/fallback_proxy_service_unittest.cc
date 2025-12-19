/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "content/browser/ohos/fallback_proxy_service.h"

#include "arkweb/chromium_ext/net/base/fallback_proxy_constants.h"
#include "base/containers/lru_cache.h"
#include "base/logging.h"
#include "base/run_loop.h"
#include "base/test/task_environment.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/test_content_client_initializer.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/base/proxy_string_util.h"
#include "services/network/public/mojom/hw_network_config.mojom.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "services/network/test/test_network_connection_tracker.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace fallback_proxy {
namespace {
const char kTestHeaderValue[] = "This_is_test_tunnel_header";
const int kMaxUrlSafeBrowingInfoEntriesForTest = 3;

class TestCustomProxyConfigClient
    : public network::mojom::CustomProxyConfigClient {
 public:
  explicit TestCustomProxyConfigClient(
      mojo::PendingReceiver<network::mojom::CustomProxyConfigClient>
          pending_receiver)
      : receiver_(this, std::move(pending_receiver)),
        url_sb_info_map_(kMaxUrlSafeBrowingInfoEntriesForTest) {}

  void OnCustomProxyConfigUpdated(
      network::mojom::CustomProxyConfigPtr proxy_config,
      OnCustomProxyConfigUpdatedCallback callback) override {
    config_ = std::move(proxy_config);
    std::move(callback).Run();
  }
  void MarkProxiesAsBad(base::TimeDelta bypass_duration,
                        const net::ProxyList& bad_proxies,
                        MarkProxiesAsBadCallback callback) override {}
  void ClearBadProxiesCache() override {}
  void SaveURLMaliciousTypeAndHwCode(const std::string& url,
                                     int type,
                                     int hw_code) override {
    PageSafeBrowsingInfo info(type, hw_code);
    url_sb_info_map_.Put(url, info);
  }

  void OnHostBlockListUpdated(
      const std::vector<std::string>& block_list) override {
    host_block_list_ =
        std::set<std::string>(block_list.begin(), block_list.end());
  }

  bool GetURLMaliciousTypeAndHwCode(const std::string& url,
                                    int* type,
                                    int* code) {
    auto it = url_sb_info_map_.Get(url);
    if (it == url_sb_info_map_.end()) {
      return false;
    }

    *type = it->second.malicious_type;
    *code = it->second.hw_code;
    return true;
  }

  int GetUrlSbInfoMapSize() { return url_sb_info_map_.size(); }

  void OnFallbackProxyInfoConfigUpdated(
      network::mojom::HwFallbackProxyInfoConfigPtr fallback_proxy_info_config)
      override {}
  void UpdateFallbackProxyAuthHeaders(
      const net::HttpRequestHeaders& tunnel_headers) override {
    config_->connect_tunnel_headers = tunnel_headers;
  }
  void UpdateFallbackProxyStatus(int status) override {
    status_ = static_cast<net::FallbackProxyStatus>(status);
  }

  net::FallbackProxyStatus GetFallbackProxyStatus() { return status_; }

  const std::set<std::string>& GetHostBlockList() const {
    return host_block_list_;
  }

  network::mojom::CustomProxyConfigPtr config_;

 private:
  struct PageSafeBrowsingInfo {
    PageSafeBrowsingInfo(int type, int code)
        : malicious_type(type), hw_code(code) {}
    ~PageSafeBrowsingInfo() {}

    int malicious_type;
    int hw_code;
  };

  mojo::Receiver<network::mojom::CustomProxyConfigClient> receiver_;
  net::FallbackProxyStatus status_ = net::FallbackProxyStatus::NONE;
  base::LRUCache<std::string, PageSafeBrowsingInfo> url_sb_info_map_;
  std::set<std::string> host_block_list_;
};

class FallbackProxyServiceTest : public testing::Test {
 public:
  FallbackProxyServiceTest()
      : task_environment_(base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  void SetUp() override {
    test_content_client_initializer_ =
        new content::TestContentClientInitializer();
    AddCustProxyConfigClient();
    InitializeBypassRules();
  }

  void TearDown() override {
    RunUntilIdle();
    FallbackProxyService::GetInstance()->RemoveNetworkConnectionObserver();
    delete test_content_client_initializer_;
  }

  network::mojom::CustomProxyConfigPtr LatestProxyConfig() {
    return std::move(config_client_->config_);
  }

  net::FallbackProxyStatus GetFallbackProxyStatus() {
    return config_client_->GetFallbackProxyStatus();
  }

  const std::set<std::string>& GetHostBlockList() const {
    return config_client_->GetHostBlockList();
  }

  bool GetURLMaliciousTypeAndHwCode(const std::string& url,
                                    int* type,
                                    int* code) {
    return config_client_->GetURLMaliciousTypeAndHwCode(url, type, code);
  }

  int GetUrlSbInfoMapSize() { return config_client_->GetUrlSbInfoMapSize(); }

  const std::string& proxy_url() const { return proxy_url_; }

  void VerifyHasProxyServerConfig(const std::string& proxy_url,
                                  const std::string& header) {
    auto config = LatestProxyConfig();
    ASSERT_TRUE(config);

    EXPECT_EQ(config->rules.type,
              net::ProxyConfig::ProxyRules::Type::PROXY_LIST);
    EXPECT_FALSE(config->should_override_existing_config);
    EXPECT_FALSE(config->allow_non_idempotent_methods);

    net::HttpRequestHeaders headers;
    headers.SetHeader(net::kFallbackProxyTunnelHeaderKey, header);
    EXPECT_EQ(config->connect_tunnel_headers.ToString(), headers.ToString());

    EXPECT_EQ(config->rules.single_proxies.size(), 1U);
    EXPECT_TRUE(config->rules.reverse_bypass);
    EXPECT_EQ(config->rules.bypass_rules.rules().size(), 4U);

    EXPECT_EQ(
        GURL(net::ProxyServerToProxyUri(config->rules.single_proxies.Get())),
        GURL(proxy_url));
  }

  void VerifyProxyServerEmptyConfig() {
    auto config = LatestProxyConfig();
    ASSERT_TRUE(config);

    EXPECT_EQ(config->rules.type, net::ProxyConfig::ProxyRules::Type::EMPTY);
    EXPECT_FALSE(config->should_override_existing_config);
    EXPECT_FALSE(config->allow_non_idempotent_methods);

    EXPECT_EQ(config->rules.single_proxies.size(), 0U);
    EXPECT_FALSE(config->rules.reverse_bypass);
    EXPECT_EQ(config->rules.bypass_rules.rules().size(), 0U);
  }

  void RunUntilIdle() { task_environment_.RunUntilIdle(); }

  void AddCustProxyConfigClient() {
    mojo::Remote<network::mojom::CustomProxyConfigClient> client_remote;
    config_client_ = std::make_unique<TestCustomProxyConfigClient>(
        client_remote.BindNewPipeAndPassReceiver());

    FallbackProxyService::GetInstance()->AddCustomProxyConfigClient(
        std::move(client_remote));
  }

  void InitializeBypassRules() {
    const std::vector<std::string> hostList = {
        "kilgwyyy.xyz", "www.xiaohongshu.com", "www.baidu.com", "m.baidu.com"};
    std::string bypass_rules;
    for (auto host : hostList) {
      bypass_rules.append(host);
      bypass_rules.append(",");
    }
    FallbackProxyService::GetInstance()->config()->SetBypassRulesForTesting(
        bypass_rules);
  }

  void FastForwardBy(base::TimeDelta delta) {
    task_environment_.FastForwardBy(delta);
  }

 private:
  std::string proxy_url_ = "https://wisewanconnect.hwcloudtest.cn:30010";
  raw_ptr<content::TestContentClientInitializer>
      test_content_client_initializer_;
  std::unique_ptr<TestCustomProxyConfigClient> config_client_;
  content::BrowserTaskEnvironment task_environment_{
      base::test::TaskEnvironment::TimeSource::MOCK_TIME};
};

TEST_F(FallbackProxyServiceTest, DisabledToEnabled) {
  EXPECT_FALSE(LatestProxyConfig());

  FallbackProxyService::GetInstance()->EnableProxyForTesting(proxy_url(),
                                                             kTestHeaderValue);
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), kTestHeaderValue);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);
}

TEST_F(FallbackProxyServiceTest, EnabledToDisabled) {
  FallbackProxyService::GetInstance()->EnableProxyForTesting(proxy_url(),
                                                             kTestHeaderValue);
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), kTestHeaderValue);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);

  FallbackProxyService::GetInstance()->DisableProxyForTesting();
  RunUntilIdle();
  VerifyProxyServerEmptyConfig();
  EXPECT_EQ(GetFallbackProxyStatus(),
            net::FallbackProxyStatus::DISABLE_BY_CLOUD_CONTROL);
}

TEST_F(FallbackProxyServiceTest, ProxyServerChanged) {
  FallbackProxyService::GetInstance()->EnableProxyForTesting(proxy_url(),
                                                             kTestHeaderValue);
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), kTestHeaderValue);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);

  std::string url1 = "https://connect.hwcloudtest.cn:30010";
  std::string header1 = "test1_header";
  FallbackProxyService::GetInstance()->EnableProxyForTesting(url1, header1);
  RunUntilIdle();
  VerifyHasProxyServerConfig(url1, header1);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);

  std::string url2 = "http://wisewanconnect.hwcloudtest.cn:30010";
  std::string header2 = "test2_header";
  FallbackProxyService::GetInstance()->EnableProxyForTesting(url2, header2);
  RunUntilIdle();
  VerifyProxyServerEmptyConfig();
  EXPECT_EQ(GetFallbackProxyStatus(),
            net::FallbackProxyStatus::DISABLE_BY_CLOUD_CONTROL);
}

TEST_F(FallbackProxyServiceTest, TunnelHeaders_600AuthFailed) {
  FallbackProxyService::GetInstance()->EnableProxyForTesting(proxy_url(),
                                                             kTestHeaderValue);
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), kTestHeaderValue);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);

  GURL proxy(proxy_url());
  net::ProxyServer proxy_server(net::GetSchemeFromUriScheme(proxy.scheme()),
                                net::HostPortPair::FromURL(proxy));
  FallbackProxyService::GetInstance()->OnTunnelHeadersReceivedWithToken(
      proxy_server,
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 600"),
      kTestHeaderValue);
  RunUntilIdle();
  VerifyProxyServerEmptyConfig();
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::AUTH_FAILED);

  std::string token = "test_new_header";
  std::string tokenInfo;
  FallbackProxyService::GetInstance()->UpdateProxyToken(token, tokenInfo);
  RunUntilIdle();
  auto config = LatestProxyConfig();
  net::HttpRequestHeaders headers;
  headers.SetHeader(net::kFallbackProxyTunnelHeaderKey, token);
  EXPECT_EQ(config->connect_tunnel_headers.ToString(), headers.ToString());
  EXPECT_EQ(config->rules.single_proxies.size(), 1U);
  EXPECT_EQ(
      GURL(net::ProxyServerToProxyUri(config->rules.single_proxies.Get())),
      GURL(proxy_url()));
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);

  FallbackProxyService::GetInstance()->OnTunnelHeadersReceivedWithToken(
      proxy_server,
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 600"),
      kTestHeaderValue);
  EXPECT_EQ(config->connect_tunnel_headers.ToString(), headers.ToString());
  EXPECT_EQ(config->rules.single_proxies.size(), 1U);
  EXPECT_EQ(
      GURL(net::ProxyServerToProxyUri(config->rules.single_proxies.Get())),
      GURL(proxy_url()));
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);
}

TEST_F(FallbackProxyServiceTest,
       TunnelHeaders_600AuthFailedWhenNetworkChanged) {
  network::TestNetworkConnectionTracker::GetInstance()->SetConnectionType(
      network::mojom::ConnectionType::CONNECTION_WIFI);
  RunUntilIdle();

  FallbackProxyService::GetInstance()->EnableProxyForTesting(proxy_url(),
                                                             kTestHeaderValue);
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), kTestHeaderValue);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);

  GURL proxy(proxy_url());
  net::ProxyServer proxy_server(net::GetSchemeFromUriScheme(proxy.scheme()),
                                net::HostPortPair::FromURL(proxy));
  FallbackProxyService::GetInstance()->OnTunnelHeadersReceivedWithToken(
      proxy_server,
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 600"),
      kTestHeaderValue);
  RunUntilIdle();
  VerifyProxyServerEmptyConfig();
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::AUTH_FAILED);

  network::TestNetworkConnectionTracker::GetInstance()->SetConnectionType(
      network::mojom::ConnectionType::CONNECTION_4G);
  RunUntilIdle();
  EXPECT_FALSE(LatestProxyConfig());

  std::string token = "test_new_header";
  std::string tokenInfo;
  FallbackProxyService::GetInstance()->UpdateProxyToken(token, tokenInfo);
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), token);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);
}

TEST_F(FallbackProxyServiceTest, TunnelHeaders_InsuffientResourceFailed) {
  FallbackProxyService::GetInstance()->EnableProxyForTesting(proxy_url(),
                                                             kTestHeaderValue);
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), kTestHeaderValue);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);

  GURL proxy(proxy_url());
  net::ProxyServer proxy_server(net::GetSchemeFromUriScheme(proxy.scheme()),
                                net::HostPortPair::FromURL(proxy));
  FallbackProxyService::GetInstance()->OnTunnelHeadersReceivedWithToken(
      proxy_server,
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 601"),
      kTestHeaderValue);
  RunUntilIdle();
  VerifyProxyServerEmptyConfig();
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::UNAVAILABLE);

  FastForwardBy(base::Seconds(60));
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::UNAVAILABLE);

  FallbackProxyService::GetInstance()->OnTunnelHeadersReceivedWithToken(
      proxy_server,
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 602"),
      kTestHeaderValue);

  int wait_time =
      FallbackProxyService::GetInstance()->config()->GetRestartProxyInternal() +
      1;
  FastForwardBy(base::Seconds(wait_time - 60));
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), kTestHeaderValue);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);
}

TEST_F(FallbackProxyServiceTest, ProxyConnectFailed) {
  network::TestNetworkConnectionTracker::GetInstance()->SetConnectionType(
      network::mojom::ConnectionType::CONNECTION_WIFI);
  RunUntilIdle();

  FallbackProxyService::GetInstance()->EnableProxyForTesting(proxy_url(),
                                                             kTestHeaderValue);
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), kTestHeaderValue);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);

  GURL proxy(proxy_url());
  net::ProxyServer proxy_server(net::GetSchemeFromUriScheme(proxy.scheme()),
                                net::HostPortPair::FromURL(proxy));
  FallbackProxyService::GetInstance()->OnProxyConnectResult(
      proxy_server, net::ERR_PROXY_CONNECTION_FAILED);
  FallbackProxyService::GetInstance()->OnProxyConnectResult(proxy_server,
                                                            net::ERR_TIMED_OUT);
  FallbackProxyService::GetInstance()->OnProxyConnectResult(proxy_server,
                                                            net::ERR_TIMED_OUT);
  RunUntilIdle();
  VerifyProxyServerEmptyConfig();
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::UNAVAILABLE);

  network::TestNetworkConnectionTracker::GetInstance()->SetConnectionType(
      network::mojom::ConnectionType::CONNECTION_WIFI);
  RunUntilIdle();
  EXPECT_FALSE(LatestProxyConfig());

  network::TestNetworkConnectionTracker::GetInstance()->SetConnectionType(
      network::mojom::ConnectionType::CONNECTION_NONE);
  RunUntilIdle();
  EXPECT_FALSE(LatestProxyConfig());

  network::TestNetworkConnectionTracker::GetInstance()->SetConnectionType(
      network::mojom::ConnectionType::CONNECTION_4G);
  RunUntilIdle();
  VerifyHasProxyServerConfig(proxy_url(), kTestHeaderValue);
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::NORMAL);

  FallbackProxyService::GetInstance()->OnProxyConnectResult(
      proxy_server, net::ERR_PROXY_CERTIFICATE_INVALID);
  RunUntilIdle();
  VerifyProxyServerEmptyConfig();
  EXPECT_EQ(GetFallbackProxyStatus(), net::FallbackProxyStatus::UNAVAILABLE);

  network::TestNetworkConnectionTracker::GetInstance()->SetConnectionType(
      network::mojom::ConnectionType::CONNECTION_WIFI);
  RunUntilIdle();
  EXPECT_FALSE(LatestProxyConfig());
}

TEST_F(FallbackProxyServiceTest, CheckURLMaliciousTypeAndHwCodePart1) {
  // Enable fallback proxy
  FallbackProxyService::GetInstance()->EnableProxyForTesting(proxy_url(),
                                                             kTestHeaderValue);
  RunUntilIdle();

  std::string url1 = "http://www.test1.com";
  int type1 = 0;
  int code1 = 0;
  FallbackProxyService::GetInstance()->SaveURLMaliciousTypeAndHwCode(
      url1, type1, code1);
  RunUntilIdle();
  EXPECT_EQ(1, GetUrlSbInfoMapSize());

  std::string url2 = "http://www.test2.com";
  int type2 = 1;
  int code2 = 1;
  FallbackProxyService::GetInstance()->SaveURLMaliciousTypeAndHwCode(
      url2, type2, code2);
  RunUntilIdle();
  EXPECT_EQ(2, GetUrlSbInfoMapSize());

  // Disable fallback proxy
  FallbackProxyService::GetInstance()->DisableProxyForTesting();
  RunUntilIdle();

  std::string url3 = "http://www.test3.com";
  int type3 = 2;
  int code3 = 2;
  FallbackProxyService::GetInstance()->SaveURLMaliciousTypeAndHwCode(
      url3, type3, code3);
  RunUntilIdle();
  EXPECT_EQ(2, GetUrlSbInfoMapSize());
}

TEST_F(FallbackProxyServiceTest, CheckURLMaliciousTypeAndHwCodePart2) {
  // Enable fallback proxy
  FallbackProxyService::GetInstance()->EnableProxyForTesting(proxy_url(),
                                                             kTestHeaderValue);
  RunUntilIdle();

  std::string url4 = "http://www.test4.com";
  int type4 = 3;
  int code4 = 3;
  FallbackProxyService::GetInstance()->SaveURLMaliciousTypeAndHwCode(
      url4, type4, code4);
  RunUntilIdle();
  EXPECT_EQ(kMaxUrlSafeBrowingInfoEntriesForTest, GetUrlSbInfoMapSize());
  int save_type1;
  int save_code1;
  EXPECT_TRUE(GetURLMaliciousTypeAndHwCode(url1, &save_type1, &save_code1));

  int save_type2;
  int save_code2;
  EXPECT_TRUE(GetURLMaliciousTypeAndHwCode(url2, &save_type2, &save_code2));
  EXPECT_EQ(type2, save_type2);
  EXPECT_EQ(code2, save_code2);

  std::string url5 = "http://www.test5.com";
  int type5 = 4;
  int code5 = 4;
  FallbackProxyService::GetInstance()->SaveURLMaliciousTypeAndHwCode(
      url5, type5, code5);
  RunUntilIdle();
  EXPECT_EQ(kMaxUrlSafeBrowingInfoEntriesForTest, GetUrlSbInfoMapSize());
  int save_type3;
  int save_code3;
  EXPECT_FALSE(GetURLMaliciousTypeAndHwCode(url4, &save_type3, &save_code3));
  int save_type4;
  int save_code4;
  EXPECT_TRUE(GetURLMaliciousTypeAndHwCode(url1, &save_type4, &save_code4));
  EXPECT_EQ(type1, save_type4);
  EXPECT_EQ(code1, save_code4);
  int save_type5;
  int save_code5;
  EXPECT_TRUE(GetURLMaliciousTypeAndHwCode(url5, &save_type5, &save_code5));
  EXPECT_EQ(type5, save_type5);
  EXPECT_EQ(code5, save_code5);
}

TEST_F(FallbackProxyServiceTest, CheckHostBlockList) {
  std::vector<std::string> block_list1 = {"www.test.com", "www.baidu.com",
                                          "m.baidu.com"};
  FallbackProxyService::GetInstance()->OnHostBlockListUpdatedForTesting(
      block_list1);
  RunUntilIdle();
  std::set<std::string> block_set1 =
      std::set<std::string>(block_list1.begin(), block_list1.end());
  EXPECT_EQ(block_set1, GetHostBlockList());

  std::vector<std::string> block_list2 = {"www.test.com", "www.test2.com",
                                          "www.test3.com", "www.test4.com"};
  FallbackProxyService::GetInstance()->OnHostBlockListUpdatedForTesting(
      block_list2);
  RunUntilIdle();
  std::set<std::string> block_set2 =
      std::set<std::string>(block_list2.begin(), block_list2.end());
  EXPECT_EQ(block_set2, GetHostBlockList());

  std::vector<std::string> block_list_empty;
  FallbackProxyService::GetInstance()->OnHostBlockListUpdatedForTesting(
      block_list_empty);
  RunUntilIdle();
  std::set<std::string> block_set_empty;
  EXPECT_EQ(block_set_empty, GetHostBlockList());

  std::vector<std::string> block_list3 = {"www.test5.com", "www.test6.com"};
  FallbackProxyService::GetInstance()->OnHostBlockListUpdatedForTesting(
      block_list3);
  RunUntilIdle();
  std::set<std::string> block_set3 =
      std::set<std::string>(block_list3.begin(), block_list3.end());
  EXPECT_EQ(block_set3, GetHostBlockList());
}

}  // namespace

}  // namespace fallback_proxy
