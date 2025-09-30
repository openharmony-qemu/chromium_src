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

#include <gtest/gtest.h>
#ifdef UNSAFE_BUFFERS_BUILD
#pragma allow_unsafe_buffers
#endif

#include "base/containers/linked_list.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/safe_ref.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#undef NET_EXPORT_PRIVATE
#define private public
#include "net/dns/host_resolver_manager_job.h"
#include "arkweb/chromium_ext/net/dns/arkweb_host_resolver_dns_task_ext.h"
#include "arkweb/chromium_ext/net/dns/arkweb_host_resolver_manager_job_ext.h"
#include "net/dns/host_resolver_dns_task.h"
#undef private
#include "net/dns/host_cache.h"
#include "net/dns/host_resolver_manager_unittest.h"
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#endif

namespace net {
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
class MockDnsTransaction : public DnsTransaction {
 public:
  MockDnsTransaction(const std::string& name, uint16_t type)
      : name_(name), type_(type) {}
  ~MockDnsTransaction() override = default;

  const std::string& GetHostname() const override { return name_; }

  // Returns the |qtype|.
  uint16_t GetType() const override { return type_; }

  // Starts the transaction.  Always completes asynchronously.
  void Start(ResponseCallback callback) {}

  void SetRequestPriority(RequestPriority priority) override{};

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  void SetNotNeedMoreAttemptIPQueryType(
      uint16_t not_need_more_another_ip_query_type) override {
    not_need_more_another_ip_query_type_ = not_need_more_another_ip_query_type;
  }
#endif
  uint16_t GetQueryType() {
    return not_need_more_another_ip_query_type_;
  }

 private:
  std::string name_;
  uint16_t type_;
  uint16_t not_need_more_another_ip_query_type_;
};

class ArkWebHostResolverDnsTest : public HostResolverManagerTest {
 public:
  explicit ArkWebHostResolverDnsTest(
      base::test::TaskEnvironment::TimeSource time_source =
          base::test::TaskEnvironment::TimeSource::MOCK_TIME)
      : HostResolverManagerTest(time_source) {}

  ~ArkWebHostResolverDnsTest() override {}

  void CreateTaskWithoutRequest(
      absl::variant<url::SchemeHostPort, HostPortPair> host) {
    HostResolverManager::JobKey job_key(HostResolver::Host(std::move(host)),
                                        resolve_context_.get());
    auto job = resolver_->AddJobWithoutRequest(
        job_key, HostResolver::ResolveHostParameters::CacheUsage::ALLOWED,
        &host_cache_, {HostResolverManager::TaskType::SECURE_DNS},
        RequestPriority::LOW, NetLogWithSource());

    job->dns_task_ = std::make_unique<HostResolverDnsTask>(
        resolver_->dns_client_.get(), job->key().host,
        job->key().network_anonymization_key, job->key().query_types,
        &*job->key().resolve_context, false, job->key().secure_dns_mode, job,
        job->net_log_, job->tick_clock_,
        !job->tasks_.empty() /* fallback_available */,
        job->https_svcb_options_);
    dns_task_ = job->dns_task_.get();
    dns_task_ext_ = std::make_unique<ArkWebHostResolverDnsTaskExt>(dns_task_);
  }

  void SetTaskSniffIpResult(bool result) {
    dns_task_->need_to_sniff_ip_result_ = result;
  }

  void SetTaskSavedResults(int net_error) {
    HostCache::Entry results(net_error, HostCache::Entry::SOURCE_UNKNOWN,
                             std::nullopt);
    dns_task_->saved_results_ = std::move(results);
  }
  void AddTransactionInfo(
      DnsQueryType type,
      HostResolverDnsTask::TransactionErrorBehavior error_behavior) {
    HostResolverDnsTask::TransactionInfo transaction_info(type, error_behavior);
    transaction_info.transaction = std::make_unique<MockDnsTransaction>(
        "test", DnsQueryTypeToQtype(transaction_info.type));
    dns_task_->transactions_in_progress_.insert(std::move(transaction_info));
  }

  void CheckTransactionInProcess() {
    for (auto& transaction_info : dns_task_->transactions_in_progress_) {
      if (transaction_info.type == DnsQueryType::A ||
          transaction_info.type == DnsQueryType::AAAA) {
        MockDnsTransaction* trans = static_cast<MockDnsTransaction*>(
            transaction_info.transaction.get());
        EXPECT_EQ(trans->GetQueryType(), trans->GetType());
      }
    }
  }

  void AddNeededTransactionInfo(
      DnsQueryType type,
      HostResolverDnsTask::TransactionErrorBehavior error_behavior) {
    dns_task_->transactions_needed_.emplace_back(type, error_behavior);
  }

 protected:
  void SetUp() override { HostResolverManagerTest::SetUp(); }
  void TearDown() override { HostResolverManagerTest::TearDown(); }

  ArkWebHostResolverDnsTaskExt* GetDnsTaskExt() { return dns_task_ext_.get(); }

 private:
  HostCache host_cache_{1};
  std::unique_ptr<ArkWebHostResolverDnsTaskExt> dns_task_ext_;
  HostResolverDnsTask* dns_task_;
};

TEST_F(ArkWebHostResolverDnsTest, ArkWebSetNotNeedQueryType) {
  CreateTaskWithoutRequest(HostPortPair("localhost", 80));
  auto dns_task = GetDnsTaskExt();
  ASSERT_TRUE(dns_task);
  int legacy_results_error = 1;
  // legacy_results_error ok, no process
  EXPECT_NO_FATAL_FAILURE(dns_task->ArkWebFailedTransaction(
      legacy_results_error, DnsQueryType::HTTPS));

  // no switches::kEnableNwebExHttpDnsFallback
  legacy_results_error = OK;
  EXPECT_NO_FATAL_FAILURE(dns_task->ArkWebSetNotNeedQueryType(
      legacy_results_error, DnsQueryType::HTTPS));

  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableNwebExHttpDnsFallback);
  // none AAA
  EXPECT_NO_FATAL_FAILURE(dns_task->ArkWebSetNotNeedQueryType(
      legacy_results_error, DnsQueryType::HTTPS));

  EXPECT_NO_FATAL_FAILURE(dns_task->ArkWebSetNotNeedQueryType(
      legacy_results_error, DnsQueryType::AAAA));
}

TEST_F(ArkWebHostResolverDnsTest, ArkWebFailedTransaction_Invalid) {
  CreateTaskWithoutRequest(HostPortPair("localhost", 80));
  auto dns_task = GetDnsTaskExt();
  ASSERT_TRUE(dns_task);
  int net_error = 0;
  // no value, no process
  EXPECT_NO_FATAL_FAILURE(
      dns_task->ArkWebFailedTransaction(net_error, std::nullopt));

  // has value, but none AddressType
  EXPECT_NO_FATAL_FAILURE(dns_task->ArkWebFailedTransaction(
      net_error, std::make_optional<DnsQueryType>(DnsQueryType::HTTPS)));
}

TEST_F(ArkWebHostResolverDnsTest,
       ArkWebFailedTransaction_AnyAOrAAAATransactionRemain) {
  CreateTaskWithoutRequest(HostPortPair("localhost", 80));
  auto dns_task = GetDnsTaskExt();
  ASSERT_TRUE(dns_task);
  int net_error = 0;
  AddTransactionInfo(
      DnsQueryType::A,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  AddTransactionInfo(
      DnsQueryType::AAAA,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  AddTransactionInfo(
      DnsQueryType::HTTPS,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  
  testing::internal::CaptureStderr();
  EXPECT_NO_FATAL_FAILURE(dns_task->ArkWebFailedTransaction(
      net_error, std::make_optional<DnsQueryType>(DnsQueryType::AAAA)));
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("The completed transaction [1] is failed"),
            std::string::npos);
}

TEST_F(ArkWebHostResolverDnsTest, ArkWebFailedTransaction_SavedResults) {
  CreateTaskWithoutRequest(HostPortPair("localhost", 80));
  auto dns_task = GetDnsTaskExt();
  ASSERT_TRUE(dns_task);
  int net_error = 0;
  SetTaskSavedResults(true);
  AddTransactionInfo(
      DnsQueryType::A,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  testing::internal::CaptureStderr();
  EXPECT_NO_FATAL_FAILURE(dns_task->ArkWebFailedTransaction(
      net_error, std::make_optional<DnsQueryType>(DnsQueryType::AAAA)));  
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("The completed transaction [2] is failed"),
            std::string::npos);
}

TEST_F(ArkWebHostResolverDnsTest, RecordFailedTransactionInfo) {
  CreateTaskWithoutRequest(HostPortPair("localhost", 80));
  auto dns_task = GetDnsTaskExt();
  ASSERT_TRUE(dns_task);
  // no process
  EXPECT_NO_FATAL_FAILURE(
      dns_task->RecordFailedTransactionInfo(0, 0, DnsQueryType::AAAA));
}

TEST_F(ArkWebHostResolverDnsTest, AnyAOrAAAATransactionRemain) {
  CreateTaskWithoutRequest(HostPortPair("localhost", 80));
  auto dns_task = GetDnsTaskExt();
  ASSERT_TRUE(dns_task);
  EXPECT_FALSE(dns_task->AnyAOrAAAATransactionRemain());
  AddTransactionInfo(
      DnsQueryType::A,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  AddTransactionInfo(
      DnsQueryType::AAAA,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  AddTransactionInfo(
      DnsQueryType::HTTPS,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  EXPECT_TRUE(dns_task->AnyAOrAAAATransactionRemain());
}

TEST_F(ArkWebHostResolverDnsTest, SetNotNeedMoreAttemptIPQueryType_Reurn) {
  CreateTaskWithoutRequest(HostPortPair("localhost", 80));
  auto dns_task = GetDnsTaskExt();
  ASSERT_TRUE(dns_task);
  // no process
  SetTaskSniffIpResult(false);
  EXPECT_NO_FATAL_FAILURE(
      dns_task->SetNotNeedMoreAttemptIPQueryType(DnsQueryType::AAAA));
  SetTaskSniffIpResult(true);
  EXPECT_NO_FATAL_FAILURE(
      dns_task->SetNotNeedMoreAttemptIPQueryType(DnsQueryType::HTTPS));
}

TEST_F(ArkWebHostResolverDnsTest, SetNotNeedMoreAttemptIPQueryType_WithAAAA) {
  CreateTaskWithoutRequest(HostPortPair("localhost", 80));
  auto dns_task = GetDnsTaskExt();
  ASSERT_TRUE(dns_task);
  SetTaskSniffIpResult(true);
  AddTransactionInfo(
      DnsQueryType::A,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  AddTransactionInfo(
      DnsQueryType::AAAA,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  AddTransactionInfo(
      DnsQueryType::HTTPS,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  EXPECT_NO_FATAL_FAILURE(
      dns_task->SetNotNeedMoreAttemptIPQueryType(DnsQueryType::AAAA));
  CheckTransactionInProcess();
}

TEST_F(ArkWebHostResolverDnsTest, SetNotNeedMoreAttemptIPQueryType_WithA) {
  CreateTaskWithoutRequest(HostPortPair("localhost", 80));
  auto dns_task = GetDnsTaskExt();
  ASSERT_TRUE(dns_task);
  SetTaskSniffIpResult(true);
  AddTransactionInfo(
      DnsQueryType::A,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  AddTransactionInfo(
      DnsQueryType::AAAA,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  AddTransactionInfo(
      DnsQueryType::HTTPS,
      HostResolverDnsTask::TransactionErrorBehavior::kFatalOrEmpty);
  EXPECT_NO_FATAL_FAILURE(
      dns_task->SetNotNeedMoreAttemptIPQueryType(DnsQueryType::A));
  CheckTransactionInProcess();

  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableNwebExHttpDnsFallback);
  EXPECT_NO_FATAL_FAILURE(
      dns_task->ArkWebSetNotNeedQueryType(OK, DnsQueryType::AAAA));
  CheckTransactionInProcess();
}
#endif
}  // namespace net