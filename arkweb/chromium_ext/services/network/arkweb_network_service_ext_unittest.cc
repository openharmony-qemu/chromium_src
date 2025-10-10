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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "base/test/task_environment.h"
#include "base/test/test_future.h"
#include "services/network/test/test_utils.h"
#define private public
#include "services/network/network_context.h"
#include "services/network/network_service.h"
#include "arkweb/chromium_ext/services/network/arkweb_network_service_ext.h"
#undef private
#include "services/network/test/fake_test_cert_verifier_params_factory.h"
#include "services/network/test/test_network_context_client.h"

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
#include "net/socket/client_socket_pool.h"
#endif

#if (BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK) || BUILDFLAG(ARKWEB_HTTP_DNS))
#include "base/base_switches.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#endif

namespace network {
namespace {
const size_t kMaxJobs = 10u;
const size_t kMaxRetryAttempts = 4u;
const base::FilePath::CharType kServicesTestData[] =
    FILE_PATH_LITERAL("services/test/data");
}  // namespace

class MockHostResolverManager : public net::HostResolverManager {
 public:
  MockHostResolverManager(const net::HostResolver::ManagerOptions& options,
                          net::SystemDnsConfigChangeNotifier* notifier,
                          net::NetLog* net_log)
      : net::HostResolverManager(options, notifier, net_log) {}

  ~MockHostResolverManager() override = default;

  virtual void SetInsecureDnsClientEnabled(bool enabled,
                                           bool additional_dns_types_enabled) {
    enabled_ = enabled;
  }
  bool enabled() { return enabled_; }

 private:
  bool enabled_{false};
};

class MockArkWebNetworkContextExt : public ArkWebNetworkContextExt {
 public:
  MockArkWebNetworkContextExt(
      NetworkService* network_service,
      mojo::PendingReceiver<mojom::NetworkContext> receiver,
      mojom::NetworkContextParamsPtr params)
      : ArkWebNetworkContextExt(network_service,
                                std::move(receiver),
                                std::move(params)) {}

  MockArkWebNetworkContextExt(const MockArkWebNetworkContextExt&) = delete;
  MockArkWebNetworkContextExt& operator=(const MockArkWebNetworkContextExt&) =
      delete;

  ~MockArkWebNetworkContextExt() override = default;

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
  MOCK_METHOD(void,
              SetHostIP,
              (const std::string& host_name,
               const std::vector<std::string>& dddress,
               uint32_t alive_time),
              (override));
  MOCK_METHOD(void, ClearHostIP, (const std::string& host_name), (override));
#endif
};

class ArkWebNetworkServiceExtTest : public testing::Test {
 public:
  explicit ArkWebNetworkServiceExtTest(
      base::test::TaskEnvironment::TimeSource time_source =
          base::test::TaskEnvironment::TimeSource::MOCK_TIME)
      : task_environment_(base::test::TaskEnvironment::MainThreadType::IO,
                          time_source),
        service_(NetworkService::CreateForTesting()) {}
  ~ArkWebNetworkServiceExtTest() override = default;

  void SetUp() override {
#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
    network_context_ = std::make_unique<MockArkWebNetworkContextExt>(
#else
    network_context_ = std::make_unique<NetworkContext>(
#endif
        service(), network_context_remote_.BindNewPipeAndPassReceiver(),
        CreateContextParams());
  }

  void TearDown() override {
    network_context_.reset();
    service_.reset();
  }

  base::test::TaskEnvironment* task_environment() {
    return &task_environment_;
  }
  NetworkContext* network_context() const {
    return network_context_.get();
  }
  NetworkService* service() const {
    return service_.get();
  }

  mojom::NetworkContextParamsPtr CreateContextParams() {
    mojom::NetworkContextParamsPtr params =
        CreateNetworkContextParamsForTesting();
    // Use a dummy CertVerifier that always passes cert verification, since
    // these unittests don't need to test CertVerifier behavior.
    params->cert_verifier_params =
        FakeTestCertVerifierParamsFactory::GetCertVerifierParams();
    return params;
  }

  std::unique_ptr<MockHostResolverManager> CreateResolver() {
    net::HostResolver::ManagerOptions options;
    options.max_concurrent_resolves = kMaxJobs;
    options.max_system_retry_attempts = kMaxRetryAttempts;
    return std::make_unique<MockHostResolverManager>(
        options, nullptr /* notifier */, nullptr /* net_log */);
  }

 private:
  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<NetworkContext> network_context_;
  std::unique_ptr<NetworkService> service_;
  mojo::Remote<mojom::NetworkContext> network_context_remote_;
};

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
TEST_F(ArkWebNetworkServiceExtTest, SetURLRequestContext) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  testing::internal::CaptureStderr();
  service_ext->SetURLRequestContext(network_context());
  auto log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("Register network context and set network timeout"),
            std::string::npos);
  // url_request_context = null
  network_context()->url_request_context_ = nullptr;
  testing::internal::CaptureStderr();
  service_ext->SetURLRequestContext(network_context());
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("Register network context and set network timeout"),
            std::string::npos);
}

TEST_F(ArkWebNetworkServiceExtTest, SetConnectTimeout) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  service_ext->RegisterNetworkContext(network_context());
  EXPECT_NO_FATAL_FAILURE(service_ext->SetConnectTimeout(10));

  // url_request_context = null
  network_context()->url_request_context_ = nullptr;
  EXPECT_NO_FATAL_FAILURE(service_ext->SetConnectTimeout(10));
}

TEST_F(ArkWebNetworkServiceExtTest, BindDnsToNetwork) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  service_ext->RegisterNetworkContext(network_context());
  int network_id = 10;
  EXPECT_NO_FATAL_FAILURE(service_ext->BindDnsToNetwork(network_id));
  EXPECT_EQ(service_ext->network_for_dns_, network_id);
  // repeate set
  testing::internal::CaptureStderr();
  EXPECT_NO_FATAL_FAILURE(service_ext->BindDnsToNetwork(network_id));
  EXPECT_EQ(service_ext->network_for_dns_, network_id);
  auto log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("bind dns to network return for network is same with 10"),
      std::string::npos);

  network_id = -1;
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  testing::internal::CaptureStderr();
  service_ext->cfg_https_dns_fallback_enabled_ = true;
  EXPECT_NO_FATAL_FAILURE(service_ext->BindDnsToNetwork(network_id));
  EXPECT_EQ(service_ext->network_for_dns_, network_id);
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("SetHttpsDnsHostResolver, enabled 1"),
            std::string::npos);
#endif

  // url_request_context = null
  network_context()->url_request_context_ = nullptr;
  network_id = 1;
  EXPECT_NO_FATAL_FAILURE(service_ext->BindDnsToNetwork(network_id));
  EXPECT_EQ(service_ext->network_for_dns_, network_id);
}

TEST_F(ArkWebNetworkServiceExtTest, BindDnsToNetwork_HostResolverManager_Null) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  service_ext->RegisterNetworkContext(network_context());
  int network_id = 10;
  testing::internal::CaptureStderr();
  auto manager = service_ext->host_resolver_manager_.release();
  service_ext->host_resolver_manager_ = nullptr;
  EXPECT_NO_FATAL_FAILURE(service_ext->BindDnsToNetwork(network_id));
  EXPECT_EQ(service_ext->network_for_dns_, network_id);
  auto log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("bind dns to network 10 invalid dns cache."),
            std::string::npos);
  service_ext->host_resolver_manager_.reset(manager);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
TEST_F(ArkWebNetworkServiceExtTest, SetHttpsDnsFallbackData) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  service_ext->RegisterNetworkContext(network_context());
  EXPECT_NO_FATAL_FAILURE(service_ext->SetHttpsDnsFallbackData(nullptr));
  EXPECT_EQ(service_ext->cfg_https_dns_fallback_enabled_, false);

  int connect_job_with_dns_only_timeout = 10;
  {
    mojom::HttpsDnsFallbackConfigPtr config =
        mojom::HttpsDnsFallbackConfig::New();
    config->connect_job_with_dns_only_timeout =
        connect_job_with_dns_only_timeout;
    EXPECT_NO_FATAL_FAILURE(
        service_ext->SetHttpsDnsFallbackData(std::move(config)));
    EXPECT_EQ(service_ext->connect_job_with_secure_dns_only_timeout_,
              connect_job_with_dns_only_timeout);
  }

  // url_request_context = null
  {
    connect_job_with_dns_only_timeout = 11;
    network_context()->url_request_context_ = nullptr;
    mojom::HttpsDnsFallbackConfigPtr config =
        mojom::HttpsDnsFallbackConfig::New();
    config->connect_job_with_dns_only_timeout =
        connect_job_with_dns_only_timeout;
    EXPECT_NO_FATAL_FAILURE(
        service_ext->SetHttpsDnsFallbackData(std::move(config)));
    EXPECT_EQ(service_ext->connect_job_with_secure_dns_only_timeout_,
              connect_job_with_dns_only_timeout);
  }
}
#endif

#if BUILDFLAG(ARKWEB_HTTP_DNS)
TEST_F(ArkWebNetworkServiceExtTest, ConfigureStubHostResolverExt) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  service_ext->RegisterNetworkContext(network_context());

  net::DnsOverHttpsConfig dns_over_https_config{};
  // no kEnableNwebExHttpDnsFallback
  net::DnsConfigOverrides dns_config =
      service_ext->ConfigureStubHostResolverExt(net::SecureDnsMode::kSecure,
                                                dns_over_https_config);
  net::DnsConfig defaults;
  EXPECT_EQ(dns_config.nameservers, defaults.nameservers);
  // SecureDnsMode kSecure
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableNwebExHttpDnsFallback);
  dns_config = service_ext->ConfigureStubHostResolverExt(
      net::SecureDnsMode::kSecure, dns_over_https_config);
  EXPECT_EQ(dns_config.nameservers, defaults.nameservers);

  // SecureDnsMode kOff
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableNwebExHttpDnsFallback);
  dns_config = service_ext->ConfigureStubHostResolverExt(
      net::SecureDnsMode::kOff, dns_over_https_config);
#if !BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  EXPECT_EQ(dns_config.nameservers, defaults.nameservers);
#else
  EXPECT_NE(dns_config.nameservers, defaults.nameservers);
#endif
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
TEST_F(ArkWebNetworkServiceExtTest, NetworkContextSetHostIP) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  service_ext->RegisterNetworkContext(network_context());

  EXPECT_NO_FATAL_FAILURE(
      service_ext->NetworkContextSetHostIP(network_context()));
  // add host
  std::string host_name = "10.11.11.12";
  net_service::NetHelpers::SetHostIP(host_name, host_name, 3);
  auto mock_context =
      static_cast<MockArkWebNetworkContextExt*>(network_context());
  EXPECT_CALL(*mock_context, SetHostIP(host_name, ::testing::_, ::testing::_))
      .Times(1);
  EXPECT_NO_FATAL_FAILURE(
      service_ext->NetworkContextSetHostIP(network_context()));
  EXPECT_CALL(*mock_context, ClearHostIP(host_name)).Times(1);
  EXPECT_NO_FATAL_FAILURE(
      service_ext->DeregisterNetworkContextExt(network_context()));
}

TEST_F(ArkWebNetworkServiceExtTest, DeregisterNetworkContextExt) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  service_ext->RegisterNetworkContext(network_context());

  EXPECT_NO_FATAL_FAILURE(
      service_ext->DeregisterNetworkContextExt(network_context()));
  // add host
  std::string host_name = "10.11.11.12";
  net_service::NetHelpers::SetHostIP(host_name, host_name, 3);
  auto mock_context =
      static_cast<MockArkWebNetworkContextExt*>(network_context());
  EXPECT_CALL(*mock_context, ClearHostIP(host_name)).Times(1);
  EXPECT_NO_FATAL_FAILURE(
      service_ext->DeregisterNetworkContextExt(network_context()));
  auto hosts = net_service::NetHelpers::GetHostIP(host_name);
  EXPECT_TRUE(hosts.empty());
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
TEST_F(ArkWebNetworkServiceExtTest, SetSocketIdleTimeout) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  service_ext->RegisterNetworkContext(network_context());

  int32_t timeout = 10;
  EXPECT_NO_FATAL_FAILURE(service_ext->SetSocketIdleTimeout(timeout));
  EXPECT_EQ(net::ClientSocketPool::used_idle_socket_timeout(),
            base::Seconds(timeout));

  network_context()->url_request_context_ = nullptr;
  EXPECT_NO_FATAL_FAILURE(service_ext->SetSocketIdleTimeout(timeout));
}
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
TEST_F(ArkWebNetworkServiceExtTest, SetHttpsDnsHostResolver) {
  auto service_ext = service()->AsArkWebNetworkServiceExt();
  ASSERT_TRUE(service_ext);
  service_ext->RegisterNetworkContext(network_context());

  bool enabled = false;
  std::string server_template = "test template";
  EXPECT_NO_FATAL_FAILURE(
      service_ext->SetHttpsDnsHostResolver(enabled, server_template));
  EXPECT_EQ(service_ext->real_https_dns_fallback_enabled_, enabled);
  EXPECT_EQ(service_ext->real_http_dns_server_template_, server_template);

  // repeate set
  EXPECT_NO_FATAL_FAILURE(
      service_ext->SetHttpsDnsHostResolver(enabled, server_template));
  EXPECT_EQ(service_ext->real_https_dns_fallback_enabled_, enabled);
  EXPECT_EQ(service_ext->real_http_dns_server_template_, server_template);

  auto manager = service_ext->host_resolver_manager_.release();
  service_ext->host_resolver_manager_ = CreateResolver();
  auto resolver = static_cast<MockHostResolverManager*>(
      service_ext->host_resolver_manager_.get());
  EXPECT_NO_FATAL_FAILURE(
      service_ext->SetHttpsDnsHostResolver(enabled, server_template));
  EXPECT_EQ(service_ext->real_https_dns_fallback_enabled_, enabled);
  EXPECT_EQ(service_ext->real_http_dns_server_template_, server_template);
  EXPECT_FALSE(resolver->enabled());

  // valid template
  server_template = R"(
    {
      "servers": [{
        "template": "https://dnsserver.example.net/dns-query{?dns}",
        "endpoints": [{
          "ips": ["192.0.2.1", "2001:db8::1"]
        }, {
          "ips": ["192.0.2.2", "2001:db8::2"]
        }]
      }]
    }
  )";
  enabled = true;
  EXPECT_NO_FATAL_FAILURE(
      service_ext->SetHttpsDnsHostResolver(enabled, server_template));
  EXPECT_EQ(service_ext->real_https_dns_fallback_enabled_, enabled);
  EXPECT_EQ(service_ext->real_http_dns_server_template_, server_template);
  EXPECT_TRUE(resolver->enabled());
  service_ext->host_resolver_manager_.reset(manager);
}
#endif
}  // namespace network
