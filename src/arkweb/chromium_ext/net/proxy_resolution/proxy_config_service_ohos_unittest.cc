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

#include "net/base/proxy_server.h"

#define private public
#include "net/proxy_resolution/proxy_config_service_ohos.cc"
#undef private
#include "base/task/thread_pool.h"
#include "base/test/task_environment.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "net/proxy_resolution/proxy_config_service.h"

namespace net {
class MockObserver : public ProxyConfigService::Observer {
 public:
  MOCK_METHOD(void,
              OnProxyConfigChanged,
              (const ProxyConfigWithAnnotation& config,
               ProxyConfigService::ConfigAvailability availability),
              (override));
};

class TestDelegate : public ProxyConfigServiceOHOS::Delegate {
 public:
  using Delegate::Delegate;

  void SetHasProxyOverride(bool value) { has_proxy_override_ = value; }
  bool GetHasProxyOverride() { return has_proxy_override_; }
  void TestSetNewConfigInMainSequence(
      const ProxyConfigWithAnnotation& proxy_config) {
    SetNewConfigInMainSequence(proxy_config);
  }
};

class ProxyConfigServiceOHOSTest : public ::testing::Test {
 protected:
  void SetUp() {
    task_runner_ = base::ThreadPool::CreateSequencedTaskRunner(
        {base::TaskPriority::USER_VISIBLE, base::MayBlock(),
         base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN});
    service_ = std::make_shared<ProxyConfigServiceOHOS>(
        task_runner_,
        base::BindRepeating([](const std::string& property) -> std::string {
          if (property == "test_prefix.proxyHost") {
            return "proxy1.example.com";
          }
          if (property == "test_prefix.proxyPort") {
            return "6060";
          }
          return std::string();
        }));
    delegate_ = base::MakeRefCounted<TestDelegate>(
        task_runner_,
        base::BindRepeating([](const std::string& property) -> std::string {
          if (property == "test_prefix.proxyHost") {
            return "proxy1.example.com";
          }
          if (property == "test_prefix.proxyPort") {
            return "8080";
          }
          return std::string();
        }));
    service_->delegate_->has_proxy_override_ = true;
    callback_ = NetProxyEventCallback::GetInstance();
  }

  void TearDown() {
    task_environment_.RunUntilIdle();
    service_.reset();
    delegate_.reset();
    callback_.reset();
    task_runner_ = nullptr;
  }

  base::test::TaskEnvironment task_environment_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;
  std::shared_ptr<ProxyConfigServiceOHOS> service_;
  scoped_refptr<TestDelegate> delegate_;
  std::shared_ptr<NetProxyEventCallback> callback_;
};

TEST_F(ProxyConfigServiceOHOSTest, ConvertStringToPort_001) {
  int output = 0;
  bool result = false;
  result = ConvertStringToPort("80", &output);
  EXPECT_TRUE(result);
  EXPECT_EQ(output, 80);
  result = ConvertStringToPort("0", &output);
  EXPECT_TRUE(result);
  EXPECT_EQ(output, 0);
  result = ConvertStringToPort("65535", &output);
  EXPECT_TRUE(result);
  EXPECT_EQ(output, 65535);
  result = ConvertStringToPort("0080", &output);
  EXPECT_TRUE(result);
  EXPECT_EQ(output, 80);
}

TEST_F(ProxyConfigServiceOHOSTest, ConvertStringToPort_002) {
  int output;
  bool result;
  result = ConvertStringToPort("65536", &output);
  EXPECT_FALSE(result);
  result = ConvertStringToPort("-1", &output);
  EXPECT_FALSE(result);
  result = ConvertStringToPort("80a", &output);
  EXPECT_FALSE(result);
  result = ConvertStringToPort("80.5", &output);
  EXPECT_FALSE(result);
  result = ConvertStringToPort("", &output);
  EXPECT_FALSE(result);
}

TEST_F(ProxyConfigServiceOHOSTest, ConstructProxyServer_001) {
  std::string valid_host = "proxy.example.com";
  std::string valid_port = "8080";
  ProxyServer::Scheme valid_scheme = ProxyServer::SCHEME_HTTP;
  ProxyServer result =
      ConstructProxyServer(valid_scheme, valid_host, valid_port);
  EXPECT_TRUE(result.is_valid());
  EXPECT_EQ(result.scheme(), valid_scheme);
  EXPECT_EQ(result.host_port_pair().host(), valid_host);
  EXPECT_EQ(result.host_port_pair().port(), 8080);
}

TEST_F(ProxyConfigServiceOHOSTest, ConstructProxyServer_002) {
  std::string valid_host = "proxy.example.com";
  ProxyServer::Scheme valid_scheme = ProxyServer::SCHEME_HTTP;
  ProxyServer http_result = ConstructProxyServer(valid_scheme, valid_host, "");
  EXPECT_TRUE(http_result.is_valid());
  EXPECT_EQ(http_result.host_port_pair().port(), 80);
}

TEST_F(ProxyConfigServiceOHOSTest, ConstructProxyServer_003) {
  std::string valid_host = "proxy.example.com";
  ProxyServer::Scheme valid_scheme = ProxyServer::SCHEME_HTTP;
  ProxyServer result1 = ConstructProxyServer(valid_scheme, valid_host, "abc");
  EXPECT_FALSE(result1.is_valid());
}

TEST_F(ProxyConfigServiceOHOSTest, LookupProxy_001) {
  std::string prefix = "test_prefix";
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        if (property == "test_prefix.proxyHost") {
          return "proxy1.example.com";
        }
        if (property == "test_prefix.proxyPort") {
          return "8080";
        }
        return std::string();
      });
  ProxyServer result =
      LookupProxy(prefix, get_property, ProxyServer::SCHEME_HTTP);
  EXPECT_TRUE(result.is_valid());
  EXPECT_EQ(result.host_port_pair().host(), "proxy1.example.com");
  EXPECT_EQ(result.host_port_pair().port(), 8080);
}

TEST_F(ProxyConfigServiceOHOSTest, LookupProxy_002) {
  std::string prefix = "test_prefix";
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        if (property == "proxyHost") {
          return "default.example.com";
        }
        if (property == "proxyPort") {
          return "3128";
        }
        return std::string();
      });
  ProxyServer result =
      LookupProxy(prefix, get_property, ProxyServer::SCHEME_HTTP);
  EXPECT_TRUE(result.is_valid());
  EXPECT_EQ(result.host_port_pair().host(), "default.example.com");
  EXPECT_EQ(result.host_port_pair().port(), 3128);
}

TEST_F(ProxyConfigServiceOHOSTest, LookupProxy_003) {
  std::string prefix = "test_prefix";
  GetPropertyCallback get_property = base::BindRepeating(
      [](const std::string& property) -> std::string { return std::string(); });
  ProxyServer result =
      LookupProxy(prefix, get_property, ProxyServer::SCHEME_HTTP);
  EXPECT_FALSE(result.is_valid());
}

TEST_F(ProxyConfigServiceOHOSTest, LookupSocksProxy_001) {
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        if (property == "socksProxyHost") {
          return "socks.example.com";
        }
        if (property == "socksProxyPort") {
          return "1080";
        }
        return std::string();
      });
  ProxyServer result = LookupSocksProxy(get_property);
  EXPECT_TRUE(result.is_valid());
  EXPECT_EQ(result.scheme(), ProxyServer::SCHEME_SOCKS5);
  EXPECT_EQ(result.host_port_pair().host(), "socks.example.com");
  EXPECT_EQ(result.host_port_pair().port(), 1080);
}

TEST_F(ProxyConfigServiceOHOSTest, LookupSocksProxy_002) {
  GetPropertyCallback get_property = base::BindRepeating(
      [](const std::string& property) -> std::string { return std::string(); });
  ProxyServer result = LookupSocksProxy(get_property);
  EXPECT_FALSE(result.is_valid());
}

TEST_F(ProxyConfigServiceOHOSTest, AddBypassRules_001) {
  GetPropertyCallback get_property = base::BindRepeating(
      [](const std::string& property) -> std::string { return std::string(); });
  net::ProxyBypassRules bypass_rules;
  AddBypassRules("http", get_property, &bypass_rules);
  EXPECT_TRUE(bypass_rules.rules().empty());
}

TEST_F(ProxyConfigServiceOHOSTest, AddBypassRules_002) {
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        return "*.example.com";
      });
  net::ProxyBypassRules bypass_rules;
  AddBypassRules("https", get_property, &bypass_rules);
  ASSERT_EQ(1u, bypass_rules.rules().size());
  GURL test_url("https://test.example.com");
  EXPECT_TRUE(bypass_rules.Matches(test_url));
}

TEST_F(ProxyConfigServiceOHOSTest, AddBypassRules_003) {
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        return " ,, *.valid.com,  , ";
      });
  net::ProxyBypassRules bypass_rules;
  AddBypassRules("ftp", get_property, &bypass_rules);
  ASSERT_EQ(1u, bypass_rules.rules().size());
  EXPECT_TRUE(bypass_rules.Matches(GURL("ftp://test.valid.com")));
}

std::string GetPropertyString(const std::string& property) {
  if (property == "http.proxyHost") {
    return "http.proxy.com";
  }
  if (property == "http.proxyPort") {
    return "8080";
  }
  if (property == "https.proxyHost") {
    return "https.proxy.com";
  }
  if (property == "https.proxyPort") {
    return "8443";
  }
  if (property == "ftp.proxyHost") {
    return "ftp.proxy.com";
  }
  if (property == "ftp.proxyPort") {
    return "2121";
  }
  if (property == "socksProxyHost") {
    return "socks.proxy.com";
  }
  if (property == "socksProxyPort") {
    return "1080";
  }
  if (property == "http.nonProxyHosts") {
    return "*.noproxy.com";
  }
  return std::string();
}

TEST_F(ProxyConfigServiceOHOSTest, GetProxyRules_001) {
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        return GetPropertyString(property);
      });
  ProxyConfig::ProxyRules rules;
  bool result = GetProxyRules(get_property, &rules);
  EXPECT_TRUE(result);
  EXPECT_EQ(rules.type, ProxyConfig::ProxyRules::Type::PROXY_LIST_PER_SCHEME);

  EXPECT_FALSE(rules.proxies_for_http.IsEmpty());
  net::ProxyChain http_chain = rules.proxies_for_http.First();
  ASSERT_EQ(http_chain.length(), 1u);
  net::ProxyServer http_proxy = http_chain.GetProxyServer(0);
  EXPECT_EQ(http_proxy.scheme(), net::ProxyServer::SCHEME_HTTP);
  EXPECT_EQ(http_proxy.host_port_pair().host(), "http.proxy.com");
  EXPECT_EQ(http_proxy.host_port_pair().port(), 8080);

  EXPECT_FALSE(rules.proxies_for_https.IsEmpty());
  net::ProxyChain https_chain = rules.proxies_for_https.First();
  ASSERT_EQ(https_chain.length(), 1u);
  net::ProxyServer https_proxy = https_chain.GetProxyServer(0);
  EXPECT_EQ(https_proxy.scheme(), net::ProxyServer::SCHEME_HTTP);
  EXPECT_EQ(https_proxy.host_port_pair().host(), "https.proxy.com");
  EXPECT_EQ(https_proxy.host_port_pair().port(), 8443);

  EXPECT_FALSE(rules.proxies_for_ftp.IsEmpty());
  net::ProxyChain ftp_chain = rules.proxies_for_ftp.First();
  ASSERT_EQ(ftp_chain.length(), 1u);
  net::ProxyServer ftp_proxy = ftp_chain.GetProxyServer(0);
  EXPECT_EQ(ftp_proxy.scheme(), net::ProxyServer::SCHEME_HTTP);
  EXPECT_EQ(ftp_proxy.host_port_pair().host(), "ftp.proxy.com");
  EXPECT_EQ(ftp_proxy.host_port_pair().port(), 2121);

  EXPECT_FALSE(rules.fallback_proxies.IsEmpty());
  net::ProxyChain socks_chain = rules.fallback_proxies.First();
  ASSERT_EQ(socks_chain.length(), 1u);
  net::ProxyServer socks_proxy = socks_chain.GetProxyServer(0);
  EXPECT_EQ(socks_proxy.scheme(), net::ProxyServer::SCHEME_SOCKS5);
  EXPECT_EQ(socks_proxy.host_port_pair().host(), "socks.proxy.com");
  EXPECT_EQ(socks_proxy.host_port_pair().port(), 1080);

  EXPECT_FALSE(rules.bypass_rules.rules().empty());
  GURL noproxy_url("http://test.noproxy.com");
  EXPECT_TRUE(rules.bypass_rules.Matches(noproxy_url));
  GURL proxy_url("http://test.proxy.com");
  EXPECT_FALSE(rules.bypass_rules.Matches(proxy_url));
}

TEST_F(ProxyConfigServiceOHOSTest, GetProxyRules_002) {
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        if (property == "http.proxyHost") {
          return "http.proxy.com";
        }
        if (property == "http.proxyPort") {
          return "8080";
        }
        return std::string();
      });
  ProxyConfig::ProxyRules rules;
  bool result = GetProxyRules(get_property, &rules);
  EXPECT_TRUE(result);
}

TEST_F(ProxyConfigServiceOHOSTest, GetProxyRules_003) {
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        if (property == "https.proxyHost") {
          return "https.proxy.com";
        }
        if (property == "https.proxyPort") {
          return "8443";
        }
        return std::string();
      });
  ProxyConfig::ProxyRules rules;
  bool result = GetProxyRules(get_property, &rules);
  EXPECT_TRUE(result);
}

TEST_F(ProxyConfigServiceOHOSTest, GetProxyRules_004) {
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        if (property == "ftp.proxyHost") {
          return "ftp.proxy.com";
        }
        if (property == "ftp.proxyPort") {
          return "2121";
        }
        return std::string();
      });
  ProxyConfig::ProxyRules rules;
  bool result = GetProxyRules(get_property, &rules);
  EXPECT_TRUE(result);
}

TEST_F(ProxyConfigServiceOHOSTest, GetProxyRules_005) {
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        if (property == "socksProxyHost") {
          return "socks.proxy.com";
        }
        if (property == "socksProxyPort") {
          return "1080";
        }
        return std::string();
      });
  ProxyConfig::ProxyRules rules;
  bool result = GetProxyRules(get_property, &rules);
  EXPECT_TRUE(result);
}

TEST_F(ProxyConfigServiceOHOSTest, GetProxyRules_006) {
  GetPropertyCallback get_property = base::BindRepeating(
      [](const std::string& property) -> std::string { return std::string(); });
  ProxyConfig::ProxyRules rules;
  bool result = GetProxyRules(get_property, &rules);
  EXPECT_FALSE(result);
}

TEST_F(ProxyConfigServiceOHOSTest, GetLatestProxyConfigInternal_001) {
  GetPropertyCallback get_property =
      base::BindRepeating([](const std::string& property) -> std::string {
        if (property == "http.proxyHost") {
          return "http.proxy.com";
        }
        if (property == "http.proxyPort") {
          return "8080";
        }
        return std::string();
      });
  ProxyConfigWithAnnotation config;
  GetLatestProxyConfigInternal(get_property, &config);
  EXPECT_FALSE(config.value().proxy_rules().empty());
  EXPECT_TRUE(config.value().from_system());

  const auto& rules = config.value().proxy_rules();
  EXPECT_FALSE(rules.proxies_for_http.IsEmpty());
  net::ProxyChain http_chain = rules.proxies_for_http.First();
  ASSERT_EQ(http_chain.length(), 1u);
  net::ProxyServer http_proxy = http_chain.GetProxyServer(0);
  EXPECT_EQ(http_proxy.host_port_pair().host(), "http.proxy.com");
  EXPECT_EQ(http_proxy.host_port_pair().port(), 8080);
}

TEST_F(ProxyConfigServiceOHOSTest, GetLatestProxyConfigInternal_002) {
  GetPropertyCallback get_property = base::BindRepeating(
      [](const std::string&) -> std::string { return std::string(); });
  ProxyConfigWithAnnotation config;
  GetLatestProxyConfigInternal(get_property, &config);
  EXPECT_TRUE(config.value().proxy_rules().empty());
  EXPECT_TRUE(config.value().proxy_rules().type ==
              net::ProxyConfig::ProxyRules::Type::EMPTY);
  EXPECT_FALSE(config.value().from_system());
}

TEST_F(ProxyConfigServiceOHOSTest, FixupProxyHostScheme_001) {
  std::string input = "http://proxy.example.com";
  std::string result = FixupProxyHostScheme(input);
  EXPECT_EQ(result, "proxy.example.com");
}

TEST_F(ProxyConfigServiceOHOSTest, FixupProxyHostScheme_002) {
  std::string input = "user:pass@proxy.example.com";
  std::string result = FixupProxyHostScheme(input);
  EXPECT_EQ(result, "proxy.example.com");
}

TEST_F(ProxyConfigServiceOHOSTest, FixupProxyHostScheme_003) {
  std::string input = "https://admin:secret@secure-proxy.example.com";
  std::string result = FixupProxyHostScheme(input);
  EXPECT_EQ(result, "secure-proxy.example.com");
}

TEST_F(ProxyConfigServiceOHOSTest, FixupProxyHostScheme_004) {
  std::string input = "plain.proxy.example.com";
  std::string result = FixupProxyHostScheme(input);
  EXPECT_EQ(result, "plain.proxy.example.com");
}

TEST_F(ProxyConfigServiceOHOSTest, GetProperty_001) {
  std::string http_host = GetProperty("http.proxyHost");
  EXPECT_EQ(http_host, "");
  std::string https_host = GetProperty("https.proxyHost");
  EXPECT_EQ(https_host, "");
}

TEST_F(ProxyConfigServiceOHOSTest, GetProperty_002) {
  std::string http_port = GetProperty("http.proxyPort");
  EXPECT_EQ(http_port, "0");
  std::string https_port = GetProperty("https.proxyPort");
  EXPECT_EQ(https_port, "0");
}

TEST_F(ProxyConfigServiceOHOSTest, GetProperty_003) {
  std::string http_nonProxyHosts = GetProperty("http.nonProxyHosts");
  EXPECT_EQ(http_nonProxyHosts, "");
  std::string https_nonProxyHosts = GetProperty("https.nonProxyHosts");
  EXPECT_EQ(https_nonProxyHosts, "");
  std::string ws_nonProxyHosts = GetProperty("ws.nonProxyHosts");
  EXPECT_EQ(ws_nonProxyHosts, "");
  std::string wss_nonProxyHosts = GetProperty("wss.nonProxyHosts");
  EXPECT_EQ(wss_nonProxyHosts, "");
}

TEST_F(ProxyConfigServiceOHOSTest, GetProperty_004) {
  EXPECT_EQ(GetProperty("unknown.property"), "");
  EXPECT_EQ(GetProperty("ftp.proxyHost"), "");
  EXPECT_EQ(GetProperty(""), "");
}

TEST_F(ProxyConfigServiceOHOSTest, CreateStaticProxyConfig_001) {
  ProxyConfigWithAnnotation config;
  CreateStaticProxyConfig("proxy.example.com", 8080, "http://pac.example.com",
                          {"*.example.com"}, &config);
  EXPECT_EQ(config.value().pac_url(), GURL("http://pac.example.com"));
  EXPECT_FALSE(config.value().pac_mandatory());
}

TEST_F(ProxyConfigServiceOHOSTest, CreateStaticProxyConfig_002) {
  ProxyConfigWithAnnotation config;
  CreateStaticProxyConfig("proxy.example.com", 8080, "",
                          {"localhost", "*.internal"}, &config);
  const auto& rules = config.value().proxy_rules();
  EXPECT_EQ(rules.bypass_rules.rules().size(), 2u);
  EXPECT_TRUE(rules.bypass_rules.Matches(GURL("http://localhost")));
  EXPECT_TRUE(rules.bypass_rules.Matches(GURL("http://server.internal")));
}

TEST_F(ProxyConfigServiceOHOSTest, CreateStaticProxyConfig_003) {
  ProxyConfigWithAnnotation config;
  CreateStaticProxyConfig("", 0, "", {}, &config);
  EXPECT_TRUE(config.value().proxy_rules().empty());
  CreateStaticProxyConfig("", 8080, "", {}, &config);
  EXPECT_TRUE(config.value().proxy_rules().empty());
  CreateStaticProxyConfig("proxy.example.com", 0, "", {}, &config);
  EXPECT_TRUE(config.value().proxy_rules().empty());
}

TEST_F(ProxyConfigServiceOHOSTest, CreateStaticProxyConfig_004) {
  ProxyConfigWithAnnotation config;
  CreateStaticProxyConfig("proxy.example.com", 8080, "", {}, &config);
  EXPECT_TRUE(config.value().proxy_rules().bypass_rules.rules().empty());
}

TEST_F(ProxyConfigServiceOHOSTest, ParseOverrideRules_001) {
  ProxyConfig::ProxyRules proxy_rules;
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> rules;
  std::string result = ParseOverrideRules(rules, &proxy_rules);
  EXPECT_TRUE(result.empty());
  EXPECT_TRUE(proxy_rules.empty());
}

TEST_F(ProxyConfigServiceOHOSTest, ParseOverrideRules_002) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> rules = {
      {"http", "invalid:proxy:url"}};
  ProxyConfig::ProxyRules proxy_rules;
  std::string result = ParseOverrideRules(rules, &proxy_rules);
  EXPECT_EQ(result, "Invalid Proxy URL: invalid:proxy:url");
}

TEST_F(ProxyConfigServiceOHOSTest, ParseOverrideRules_003) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> rules = {
      {"http", "http://proxy1.com,http://proxy2.com"}};
  ProxyConfig::ProxyRules proxy_rules;
  std::string result = ParseOverrideRules(rules, &proxy_rules);
  EXPECT_EQ(result, "Invalid Proxy URL: http://proxy1.com,http://proxy2.com");
}

TEST_F(ProxyConfigServiceOHOSTest, ParseOverrideRules_004) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> rules = {
      {"https", "quic://quic-proxy.com:443"}};
  ProxyConfig::ProxyRules proxy_rules;
  std::string result = ParseOverrideRules(rules, &proxy_rules);
  EXPECT_EQ(result, "Invalid Proxy URL: quic://quic-proxy.com:443");
}

TEST_F(ProxyConfigServiceOHOSTest, ParseOverrideRules_005) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> rules = {
      {"ftp", "http://proxy.com"}};
  ProxyConfig::ProxyRules proxy_rules;
  std::string result = ParseOverrideRules(rules, &proxy_rules);
  EXPECT_EQ(result, "Unsupported URL scheme: ftp");
}

TEST_F(ProxyConfigServiceOHOSTest, ParseOverrideRules_006) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> rules = {
      {"http", "http://http-proxy.com:8080"}};
  ProxyConfig::ProxyRules proxy_rules;
  std::string result = ParseOverrideRules(rules, &proxy_rules);
  EXPECT_TRUE(result.empty());
  EXPECT_EQ(proxy_rules.type,
            ProxyConfig::ProxyRules::Type::PROXY_LIST_PER_SCHEME);
  EXPECT_FALSE(proxy_rules.proxies_for_http.IsEmpty());
  EXPECT_TRUE(proxy_rules.proxies_for_https.IsEmpty());
  EXPECT_TRUE(proxy_rules.fallback_proxies.IsEmpty());
}

TEST_F(ProxyConfigServiceOHOSTest, ParseOverrideRules_007) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> rules = {
      {"https", "https://https-proxy.com:8443"}};
  ProxyConfig::ProxyRules proxy_rules;
  std::string result = ParseOverrideRules(rules, &proxy_rules);
  EXPECT_TRUE(result.empty());
  EXPECT_EQ(proxy_rules.type,
            ProxyConfig::ProxyRules::Type::PROXY_LIST_PER_SCHEME);
  EXPECT_TRUE(proxy_rules.proxies_for_http.IsEmpty());
  EXPECT_FALSE(proxy_rules.proxies_for_https.IsEmpty());
  EXPECT_TRUE(proxy_rules.fallback_proxies.IsEmpty());
}

TEST_F(ProxyConfigServiceOHOSTest, ParseOverrideRules_008) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> rules = {
      {"*", "http://fallback-proxy.com:8080"}};
  ProxyConfig::ProxyRules proxy_rules;
  std::string result = ParseOverrideRules(rules, &proxy_rules);
  EXPECT_TRUE(result.empty());
  EXPECT_EQ(proxy_rules.type, ProxyConfig::ProxyRules::Type::PROXY_LIST);
  EXPECT_TRUE(proxy_rules.fallback_proxies.IsEmpty());
  EXPECT_FALSE(proxy_rules.single_proxies.IsEmpty());
}

TEST_F(ProxyConfigServiceOHOSTest, CreateOverrideProxyConfig_001) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> proxy_rules = {
      {"http", "invalid:proxy:url"}};
  std::vector<std::string> bypass_rules = {};
  bool reverse_bypass = false;
  ProxyConfigWithAnnotation config;
  std::string result = CreateOverrideProxyConfig(proxy_rules, bypass_rules,
                                                 reverse_bypass, &config);
  EXPECT_EQ(result, "Invalid Proxy URL: invalid:proxy:url");
}

TEST_F(ProxyConfigServiceOHOSTest, CreateOverrideProxyConfig_002) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> proxy_rules = {
      {"http", "http://valid-proxy.com:8080"}};
  std::vector<std::string> bypass_rules = {"valid.rule", "      "};
  bool reverse_bypass = true;
  ProxyConfigWithAnnotation config;
  std::string result = CreateOverrideProxyConfig(proxy_rules, bypass_rules,
                                                 reverse_bypass, &config);
  EXPECT_EQ(result, "Invalid bypass rule       ");
}

TEST_F(ProxyConfigServiceOHOSTest, CreateOverrideProxyConfig_003) {
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> proxy_rules = {
      {"http", "http://proxy.com:8080"}, {"*", "socks5://fallback.com:1080"}};
  std::vector<std::string> bypass_rules = {"localhost", "*.internal"};
  bool reverse_bypass = false;
  ProxyConfigWithAnnotation config;
  std::string result = CreateOverrideProxyConfig(proxy_rules, bypass_rules,
                                                 reverse_bypass, &config);
  EXPECT_TRUE(result.empty());
  EXPECT_EQ(config.value().proxy_rules().reverse_bypass, reverse_bypass);
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_GetLatestProxyConfig_001) {
  ASSERT_NE(delegate_, nullptr);
  auto result = delegate_->GetLatestProxyConfig(nullptr);
  EXPECT_EQ(result, ProxyConfigService::CONFIG_UNSET);
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_GetLatestProxyConfig_002) {
  ASSERT_NE(delegate_, nullptr);
  ProxyConfigWithAnnotation expected_config =
      ProxyConfigWithAnnotation::CreateDirect();
  delegate_->AddObserver(new MockObserver());
  delegate_->TestSetNewConfigInMainSequence(expected_config);
  ProxyConfigWithAnnotation actual_config;
  auto result = delegate_->GetLatestProxyConfig(&actual_config);
  EXPECT_EQ(result, ProxyConfigService::CONFIG_VALID);
  EXPECT_TRUE(actual_config.value().Equals(ProxyConfig::CreateDirect()));
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_ProxySettingsChanged_001) {
  ASSERT_NE(delegate_, nullptr);
  delegate_->SetHasProxyOverride(true);
  delegate_->ProxySettingsChanged();
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_ProxySettingsChanged_002) {
  ASSERT_NE(delegate_, nullptr);
  delegate_->SetHasProxyOverride(false);
  delegate_->ProxySettingsChanged();
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_ProxySettingsChangedTo_001) {
  ASSERT_NE(delegate_, nullptr);
  delegate_->SetHasProxyOverride(true);
  delegate_->ProxySettingsChangedTo("example.com", 8080, "http://pac",
                                    {"*.example.com"});
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_ProxySettingsChangedTo_002) {
  ASSERT_NE(delegate_, nullptr);
  delegate_->SetHasProxyOverride(false);
  delegate_->set_exclude_pac_url(true);
  const std::string host = "proxy.example.com";
  const int port = 8888;
  const std::string pac_url = "http://example.com/pac.js";
  delegate_->ProxySettingsChangedTo(host, port, pac_url, {"*.example.com"});
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_ProxySettingsChangedTo_003) {
  ASSERT_NE(delegate_, nullptr);
  delegate_->SetHasProxyOverride(false);
  delegate_->set_exclude_pac_url(false);
  const std::string host = "proxy.example.com";
  const int port = 8888;
  const std::string pac_url = "http://example.com/pac.js";
  delegate_->ProxySettingsChangedTo(host, port, pac_url, {"*.example.com"});
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_SetProxyOverride_001) {
  ASSERT_NE(delegate_, nullptr);
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> proxy_rules = {
      {"http", "invalid:proxy:url"}};
  std::vector<std::string> bypass_rules = {};
  bool reverse_bypass = false;
  bool callback_called = false;
  base::OnceClosure callback = base::BindOnce(
      [](bool* callback_called) { *callback_called = true; }, &callback_called);
  std::string result = delegate_->SetProxyOverride(
      proxy_rules, bypass_rules, reverse_bypass, std::move(callback));
  EXPECT_EQ(result, "Invalid Proxy URL: invalid:proxy:url");
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_SetProxyOverride_002) {
  ASSERT_NE(delegate_, nullptr);
  std::vector<ProxyConfigServiceOHOS::ProxyOverrideRule> proxy_rules = {
      {"http", "http://proxy.com:8080"}, {"*", "socks5://fallback.com:1080"}};
  std::vector<std::string> bypass_rules = {"localhost", "*.internal"};
  bool reverse_bypass = false;
  bool callback_called = false;
  base::OnceClosure callback = base::BindOnce(
      [](bool* callback_called) { *callback_called = true; }, &callback_called);
  std::string result = delegate_->SetProxyOverride(
      proxy_rules, bypass_rules, reverse_bypass, std::move(callback));
  EXPECT_EQ(result, "");
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_ClearProxyOverride_001) {
  delegate_->SetHasProxyOverride(false);
  bool callback_called = false;
  base::OnceClosure callback = base::BindOnce(
      [](bool* callback_called) { *callback_called = true; }, &callback_called);
  delegate_->ClearProxyOverride(std::move(callback));
  EXPECT_TRUE(callback_called);
}

TEST_F(ProxyConfigServiceOHOSTest, Delegate_ClearProxyOverride_002) {
  delegate_->SetHasProxyOverride(true);
  bool callback_called = false;
  base::OnceClosure callback = base::BindOnce(
      [](bool* callback_called) { *callback_called = true; }, &callback_called);
  delegate_->ClearProxyOverride(std::move(callback));
  EXPECT_FALSE(delegate_->GetHasProxyOverride());
}

TEST_F(ProxyConfigServiceOHOSTest, NetProxyEventCallback_GetInstance_001) {
  NetProxyEventCallback::GetInstance().reset();
  auto instance = NetProxyEventCallback::GetInstance();
  EXPECT_NE(instance, nullptr);
}

TEST_F(ProxyConfigServiceOHOSTest, NetProxyEventCallback_GetInstance_002) {
  auto first_instance = NetProxyEventCallback::GetInstance();
  EXPECT_NE(first_instance, nullptr);
  auto second_instance = NetProxyEventCallback::GetInstance();
  EXPECT_EQ(first_instance, second_instance);
}

TEST_F(ProxyConfigServiceOHOSTest, NetProxyEventCallback_Changed_001) {
  callback_->AddObserver(service_.get());
  callback_->AddObserver(nullptr);
  callback_->Changed("example.com", 8080, "http://pac", {"*.example.com"});
}
}  // namespace net
