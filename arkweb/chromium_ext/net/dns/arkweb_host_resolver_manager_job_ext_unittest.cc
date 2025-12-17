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

#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "base/run_loop.h"
#include "base/test/task_environment.h"
#include "net/base/net_errors.h"
#include "net/proxy_resolution/proxy_config_service_fixed.h"
#include "net/proxy_resolution/proxy_config_with_annotation.h"
#include "net/traffic_annotation/network_traffic_annotation_test_helper.h"
#define private public
#define protected public
#include "net/dns/host_resolver_manager_job.h"
#include "net/dns/host_resolver_manager_request_impl.h"
#include "arkweb/chromium_ext/net/dns/arkweb_host_resolver_manager_job_ext.h"
#undef protected
#undef private
#include "net/dns/dns_test_util.h"
#include "net/dns/host_resolver_manager.h"
#include "net/dns/resolve_context.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_builder.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/scheme_host_port.h"

namespace net {

class ArkWebHostResolverManagerJobExtTest : public testing::Test {
 public:
  void SetUp() override {
    URLRequestContextBuilder builder;
    net::ProxyConfigWithAnnotation pcwa(net::ProxyConfig::CreateDirect(),
                                        TRAFFIC_ANNOTATION_FOR_TESTS);
    auto fixed = std::make_unique<net::ProxyConfigServiceFixed>(pcwa);
    builder.set_proxy_config_service(std::move(fixed));
    url_request_context_ = builder.Build();

    resolve_context_ =
        std::make_unique<ResolveContext>(url_request_context_.get(), true);

    base::WeakPtr<HostResolverManager> resolver;
    HostResolver::ResolveHostParameters::CacheUsage cache_usage =
        HostResolver::ResolveHostParameters::CacheUsage::ALLOWED;
    HostCache* host_cache = nullptr;
    std::deque<HostResolverManager::TaskType> tasks;
    RequestPriority pri = DEFAULT_PRIORITY;
    NetLogWithSource net_log;
    const base::TickClock* tick_clock = nullptr;
    HostResolver::HttpsSvcbOptions https_opt;

    job_ = std::make_unique<ArkWebHostResolverManagerJobExt>(
        resolver,
        HostResolverManager::JobKey(HostResolver::Host(url::SchemeHostPort(
                                        "https", "example.com", 443)),
                                    resolve_context_.get()),
        cache_usage, host_cache, std::move(tasks), pri, net_log, tick_clock,
        https_opt);
  }

  void TearDown() override {
    job_.reset();
    resolve_context_.reset();
    url_request_context_.reset();
  }

 protected:
  base::test::TaskEnvironment task_env_;
  std::unique_ptr<URLRequestContext> url_request_context_;
  std::unique_ptr<ResolveContext> resolve_context_;
  std::unique_ptr<ArkWebHostResolverManagerJobExt> job_;
};

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
TEST_F(ArkWebHostResolverManagerJobExtTest,
       CheckDnsFallBackTask_NotSecureFallback) {
  job_->dns_task_error_ = ERR_FAILED;
  job_->tasks_.clear();
  job_->tasks_.push_back(HostResolverManager::TaskType::DNS);

  auto tasks_before = job_->tasks_;
  int dns_err_before = job_->dns_task_error_;

  job_->CheckDnsFallBackTask(ERR_FAILED);
  // Not executed RunNextTask()  → tasks_ remains unchanged.
  EXPECT_EQ(job_->dns_task_error_, dns_err_before);
  EXPECT_EQ(job_->tasks_, tasks_before);
}

TEST_F(ArkWebHostResolverManagerJobExtTest, CheckDnsFallBackTask_EmptyTasks) {
  job_->dns_task_error_ = ERR_FAILED;
  job_->tasks_.clear();

  auto tasks_before = job_->tasks_;
  int dns_err_before = job_->dns_task_error_;

  job_->CheckDnsFallBackTask(ERR_NAME_NOT_RESOLVED);
  // Not executed RunNextTask() → tasks_ remains unchanged.
  EXPECT_EQ(job_->dns_task_error_, dns_err_before);
  EXPECT_EQ(job_->tasks_, tasks_before);
}

TEST_F(ArkWebHostResolverManagerJobExtTest, CheckDnsFallBackTask_NetOk) {
  job_->dns_task_error_ = ERR_FAILED;
  job_->tasks_.clear();

  auto tasks_before = job_->tasks_;
  int dns_err_before = job_->dns_task_error_;

  job_->CheckDnsFallBackTask(OK);
  // Not executed RunNextTask() → tasks_ remains unchanged.
  EXPECT_EQ(job_->dns_task_error_, dns_err_before);
  EXPECT_EQ(job_->tasks_, tasks_before);
}

TEST_F(ArkWebHostResolverManagerJobExtTest, CheckDnsFallBackTask_DnsOk) {
  job_->dns_task_error_ = OK;
  job_->tasks_.clear();

  auto tasks_before = job_->tasks_;
  int dns_err_before = job_->dns_task_error_;

  job_->CheckDnsFallBackTask(ERR_FAILED);
  // Not executed RunNextTask() → tasks_ remains unchanged.
  EXPECT_EQ(job_->dns_task_error_, dns_err_before);
  EXPECT_EQ(job_->tasks_, tasks_before);
}

TEST_F(ArkWebHostResolverManagerJobExtTest,
       ReportDnsFallBackTaskResult_SwitchOn) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableNwebExHttpDnsFallback);
  net::HostResolverManager resolver_manager(net::HostResolver::ManagerOptions(),
                                            nullptr, nullptr);
  resolver_manager.RegisterResolveContext(resolve_context_.get());

  net::DnsConfig cfg = net::CreateValidDnsConfig();
  net::MockDnsClientRuleList rules;
  rules.emplace_back(
      "example.com", net::dns_protocol::kTypeA, true,
      net::MockDnsClientRule::Result(net::BuildTestDnsAddressResponse(
          "example.com", net::IPAddress::IPv4Localhost(), "example.com")),
      false, url_request_context_.get());
  auto mock_dns = std::make_unique<net::MockDnsClient>(cfg, std::move(rules));
  mock_dns->SetForceDohServerAvailable(true);
  mock_dns->SetInsecureEnabled(false, false);
  resolver_manager.SetDnsClientForTesting(std::move(mock_dns));
  base::RunLoop().RunUntilIdle();

  auto req = resolver_manager.CreateRequest(
      net::HostResolver::Host(url::SchemeHostPort("https", "example.com", 443)),
      net::NetworkAnonymizationKey(), net::NetLogWithSource(),
      net::HostResolver::ResolveHostParameters(), resolve_context_.get());
  ASSERT_TRUE(req);
  int rv = req->Start(base::DoNothing());
  EXPECT_TRUE(rv == net::ERR_IO_PENDING);
  base::RunLoop().RunUntilIdle();

  auto* base_job = resolver_manager.jobs_.begin()->second.get();
  ASSERT_NE(base_job, nullptr);
  auto* ext_job = static_cast<net::ArkWebHostResolverManagerJobExt*>(base_job);
  ASSERT_NE(ext_job, nullptr);
  ASSERT_FALSE(ext_job->requests_.empty());

  ext_job->KillDnsTask();
  ext_job->dispatched_ = false;
  ext_job->StartDnsTask(true);
  ASSERT_TRUE(ext_job->dns_task_);
  ASSERT_TRUE(ext_job->dns_task_->secure());
  base::RunLoop().RunUntilIdle();
  ASSERT_FALSE(ext_job->dns_task_);
  req.reset();
  base::RunLoop().RunUntilIdle();
  resolver_manager.DeregisterResolveContext(resolve_context_.get());
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableNwebExHttpDnsFallback);
}

TEST_F(ArkWebHostResolverManagerJobExtTest,
       AddTransactionResultForReport_A_Set_To_Ipv4_Failed) {
  ArkWebHostResolverManagerJobExt obj(
      base::WeakPtr<net::HostResolverManager>(),
      net::HostResolverManager::JobKey(
          net::HostResolver::Host(
              url::SchemeHostPort("https", "example.com", 443)),
          resolve_context_.get()),
      net::HostResolver::ResolveHostParameters::CacheUsage::ALLOWED, nullptr,
      {}, net::DEFAULT_PRIORITY, net::NetLogWithSource(), nullptr,
      net::HostResolver::HttpsSvcbOptions());

  obj.failed_transactions_type_ =
      net::HostResolverManager::DnsTransactionAddressFailedType::BOTH_OK;
  obj.resolved_result_for_ipv4_ = 0;
  obj.resolved_result_for_ipv6_ = 0;

  obj.AddTransactionResultForReport(net::DnsQueryType::A,
                                    net::ERR_NAME_NOT_RESOLVED);

  EXPECT_EQ(obj.resolved_result_for_ipv4_, net::ERR_NAME_NOT_RESOLVED);
  EXPECT_EQ(obj.failed_transactions_type_,
            net::HostResolverManager::DnsTransactionAddressFailedType::
                IPV4_ADDRESS_FAILED);
}

TEST_F(ArkWebHostResolverManagerJobExtTest,
       AddTransactionResultForReport_A_Set_To_Both_Failed_When_Ipv6_Failed) {
  ArkWebHostResolverManagerJobExt obj(
      base::WeakPtr<net::HostResolverManager>(),
      net::HostResolverManager::JobKey(
          net::HostResolver::Host(
              url::SchemeHostPort("https", "example.com", 443)),
          resolve_context_.get()),
      net::HostResolver::ResolveHostParameters::CacheUsage::ALLOWED, nullptr,
      {}, net::DEFAULT_PRIORITY, net::NetLogWithSource(), nullptr,
      net::HostResolver::HttpsSvcbOptions());

  obj.failed_transactions_type_ = net::HostResolverManager::
      DnsTransactionAddressFailedType::IPV6_ADDRESS_FAILED;

  obj.AddTransactionResultForReport(net::DnsQueryType::A, net::OK);

  EXPECT_EQ(obj.resolved_result_for_ipv4_, net::OK);
  EXPECT_EQ(
      obj.failed_transactions_type_,
      net::HostResolverManager::DnsTransactionAddressFailedType::BOTH_FAILED);
}

TEST_F(ArkWebHostResolverManagerJobExtTest,
       AddTransactionResultForReport_AAAA_Set_To_Ipv6_Failed) {
  ArkWebHostResolverManagerJobExt obj(
      base::WeakPtr<net::HostResolverManager>(),
      net::HostResolverManager::JobKey(
          net::HostResolver::Host(
              url::SchemeHostPort("https", "example.com", 443)),
          resolve_context_.get()),
      net::HostResolver::ResolveHostParameters::CacheUsage::ALLOWED, nullptr,
      {}, net::DEFAULT_PRIORITY, net::NetLogWithSource(), nullptr,
      net::HostResolver::HttpsSvcbOptions());

  obj.failed_transactions_type_ =
      net::HostResolverManager::DnsTransactionAddressFailedType::BOTH_OK;

  obj.AddTransactionResultForReport(net::DnsQueryType::AAAA,
                                    net::ERR_TIMED_OUT);

  EXPECT_EQ(obj.resolved_result_for_ipv6_, net::ERR_TIMED_OUT);
  EXPECT_EQ(obj.failed_transactions_type_,
            net::HostResolverManager::DnsTransactionAddressFailedType::
                IPV6_ADDRESS_FAILED);
}

TEST_F(ArkWebHostResolverManagerJobExtTest,
       AddTransactionResultForReport_AAAA_Set_To_Both_Failed_When_Ipv4_Failed) {
  ArkWebHostResolverManagerJobExt obj(
      base::WeakPtr<net::HostResolverManager>(),
      net::HostResolverManager::JobKey(
          net::HostResolver::Host(
              url::SchemeHostPort("https", "example.com", 443)),
          resolve_context_.get()),
      net::HostResolver::ResolveHostParameters::CacheUsage::ALLOWED, nullptr,
      {}, net::DEFAULT_PRIORITY, net::NetLogWithSource(), nullptr,
      net::HostResolver::HttpsSvcbOptions());

  obj.failed_transactions_type_ = net::HostResolverManager::
      DnsTransactionAddressFailedType::IPV4_ADDRESS_FAILED;

  obj.AddTransactionResultForReport(net::DnsQueryType::AAAA, net::OK);

  EXPECT_EQ(obj.resolved_result_for_ipv6_, net::OK);
  EXPECT_EQ(
      obj.failed_transactions_type_,
      net::HostResolverManager::DnsTransactionAddressFailedType::BOTH_FAILED);
}

TEST_F(ArkWebHostResolverManagerJobExtTest,
       AddTransactionResultForReport_OtherType_NoChange) {
  ArkWebHostResolverManagerJobExt obj(
      base::WeakPtr<net::HostResolverManager>(),
      net::HostResolverManager::JobKey(
          net::HostResolver::Host(
              url::SchemeHostPort("https", "example.com", 443)),
          resolve_context_.get()),
      net::HostResolver::ResolveHostParameters::CacheUsage::ALLOWED, nullptr,
      {}, net::DEFAULT_PRIORITY, net::NetLogWithSource(), nullptr,
      net::HostResolver::HttpsSvcbOptions());

  obj.failed_transactions_type_ =
      net::HostResolverManager::DnsTransactionAddressFailedType::BOTH_OK;
  obj.resolved_result_for_ipv4_ = 123;
  obj.resolved_result_for_ipv6_ = 456;

  obj.AddTransactionResultForReport(net::DnsQueryType::HTTPS, net::ERR_FAILED);

  EXPECT_EQ(obj.failed_transactions_type_,
            net::HostResolverManager::DnsTransactionAddressFailedType::BOTH_OK);
  EXPECT_EQ(obj.resolved_result_for_ipv4_, 123);
  EXPECT_EQ(obj.resolved_result_for_ipv6_, 456);
}
#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

}  // namespace net