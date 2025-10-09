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

#include "arkweb/chromium_ext/net/url_request/url_request_context_ext.h"
#include "base/test/task_environment.h"
#include "net/http/http_network_session.h"
#include "net/http/http_transaction_factory.h"
#include "net/url_request/url_request_context_builder.h"
#include "net/url_request/url_request_test_util.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace net {

constexpr int connect_timeout = 30;
constexpr handles::NetworkHandle bind_dns_network = 1;

class NullSessionHttpTransactionFactory : public HttpTransactionFactory {
 public:
  NullSessionHttpTransactionFactory() = default;
  ~NullSessionHttpTransactionFactory() override = default;

  int CreateTransaction(RequestPriority priority,
                        std::unique_ptr<HttpTransaction>* trans) override {
    return ERR_NOT_IMPLEMENTED;
  }

  HttpCache* GetCache() override { return nullptr; }

  HttpNetworkSession* GetSession() override { return nullptr; }
};

class UrlRequesrtContextExtTest : public testing::Test {
 public:
  void SetUp() override {
    task_environment_.RunUntilIdle();
    task_runner_ = base::SequencedTaskRunner::GetCurrentDefault();
  }
  void TearDown() override {}

  scoped_refptr<base::SequencedTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
};

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
TEST_F(UrlRequesrtContextExtTest, SetConnectTimeout) {
  URLRequestContextBuilder builder;
  std::unique_ptr<URLRequestContext> context = builder.Build();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();
  ASSERT_NE(context_ext->http_transaction_factory(), nullptr);
  ASSERT_NE(context_ext->http_transaction_factory()->GetSession(), nullptr);
  ASSERT_NO_FATAL_FAILURE(context_ext->SetConnectTimeout(connect_timeout));
}

TEST_F(UrlRequesrtContextExtTest, SetConnectTimeout2) {
  URLRequestContextBuilder builder;
  std::unique_ptr<URLRequestContext> context = builder.Build();
  context->http_transaction_factory_.reset();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();
  ASSERT_EQ(context_ext->http_transaction_factory(), nullptr);
  ASSERT_NO_FATAL_FAILURE(context_ext->SetConnectTimeout(connect_timeout));
}

TEST_F(UrlRequesrtContextExtTest, SetConnectTimeout3) {
  URLRequestContextBuilder builder;
  builder.SetCreateHttpTransactionFactoryCallback(
      base::BindOnce([](HttpNetworkSession* session)
                         -> std::unique_ptr<net::HttpTransactionFactory> {
        return std::make_unique<NullSessionHttpTransactionFactory>();
      }));
  std::unique_ptr<URLRequestContext> context = builder.Build();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();
  ASSERT_NE(context_ext->http_transaction_factory(), nullptr);
  ASSERT_EQ(context_ext->http_transaction_factory()->GetSession(), nullptr);
  ASSERT_NO_FATAL_FAILURE(context_ext->SetConnectTimeout(connect_timeout));
}

TEST_F(UrlRequesrtContextExtTest, BindDnsToNetwork) {
  URLRequestContextBuilder builder;
  std::unique_ptr<URLRequestContext> context = builder.Build();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();

  ASSERT_NE(context_ext->http_transaction_factory(), nullptr);
  ASSERT_NE(context_ext->http_transaction_factory()->GetSession(), nullptr);
  context_ext->BindDnsToNetwork(bind_dns_network);
  EXPECT_EQ(context_ext->bound_network_for_dns(), bind_dns_network);
}

TEST_F(UrlRequesrtContextExtTest, BindDnsToNetwork2) {
  URLRequestContextBuilder builder;
  std::unique_ptr<URLRequestContext> context = builder.Build();
  context->http_transaction_factory_.reset();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();
  ASSERT_EQ(context_ext->http_transaction_factory(), nullptr);
  context_ext->BindDnsToNetwork(0);
  EXPECT_NE(context_ext->bound_network_for_dns(), 0);
}

TEST_F(UrlRequesrtContextExtTest, BindDnsToNetwork3) {
  URLRequestContextBuilder builder;
  builder.SetCreateHttpTransactionFactoryCallback(
      base::BindOnce([](HttpNetworkSession* session)
                         -> std::unique_ptr<net::HttpTransactionFactory> {
        return std::make_unique<NullSessionHttpTransactionFactory>();
      }));
  std::unique_ptr<URLRequestContext> context = builder.Build();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();
  ASSERT_NE(context_ext->http_transaction_factory(), nullptr);
  ASSERT_EQ(context_ext->http_transaction_factory()->GetSession(), nullptr);
  context_ext->BindDnsToNetwork(0);
  EXPECT_NE(context_ext->bound_network_for_dns(), 0);
}
#endif  // ARKWEB_EX_NETWORK_CONNECTION

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
TEST_F(UrlRequesrtContextExtTest, SetConnectJobWithSecureDnsOnlyTimeout) {
  URLRequestContextBuilder builder;
  std::unique_ptr<URLRequestContext> context = builder.Build();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();

  ASSERT_NE(context_ext->http_transaction_factory(), nullptr);
  ASSERT_NE(context_ext->http_transaction_factory()->GetSession(), nullptr);
  ASSERT_NO_FATAL_FAILURE(
      context_ext->SetConnectJobWithSecureDnsOnlyTimeout(connect_timeout));
}

TEST_F(UrlRequesrtContextExtTest, SetConnectJobWithSecureDnsOnlyTimeout2) {
  URLRequestContextBuilder builder;
  std::unique_ptr<URLRequestContext> context = builder.Build();
  context->http_transaction_factory_.reset();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();
  ASSERT_EQ(context_ext->http_transaction_factory(), nullptr);
  ASSERT_NO_FATAL_FAILURE(
      context_ext->SetConnectJobWithSecureDnsOnlyTimeout(connect_timeout));
}

TEST_F(UrlRequesrtContextExtTest, SetConnectJobWithSecureDnsOnlyTimeout3) {
  URLRequestContextBuilder builder;
  builder.SetCreateHttpTransactionFactoryCallback(
      base::BindOnce([](HttpNetworkSession* session)
                         -> std::unique_ptr<net::HttpTransactionFactory> {
        return std::make_unique<NullSessionHttpTransactionFactory>();
      }));
  std::unique_ptr<URLRequestContext> context = builder.Build();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();
  ASSERT_NE(context_ext->http_transaction_factory(), nullptr);
  ASSERT_EQ(context_ext->http_transaction_factory()->GetSession(), nullptr);
  ASSERT_NO_FATAL_FAILURE(
      context_ext->SetConnectJobWithSecureDnsOnlyTimeout(connect_timeout));
}

TEST_F(UrlRequesrtContextExtTest, CanUseSecureDnsFallback) {
  URLRequestContextBuilder builder;
  builder.set_host_resolver(nullptr);
  std::unique_ptr<URLRequestContext> context = builder.Build();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();

  EXPECT_FALSE(context_ext->CanUseSecureDnsFallback());
}

TEST_F(UrlRequesrtContextExtTest, CanUseSecureDnsFallback2) {
  URLRequestContextBuilder builder;
  std::unique_ptr<URLRequestContext> context = builder.Build();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();

  EXPECT_FALSE(context_ext->CanUseSecureDnsFallback());
}
#endif  // ARKWEB_EX_HTTP_DNS_FALLBACK

#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
TEST_F(UrlRequesrtContextExtTest, SetSocketIdleTimeout1) {
  URLRequestContextBuilder builder;
  std::unique_ptr<URLRequestContext> context = builder.Build();
  URLRequestContextExt* context_ext = context->AsURLRequestContextExt();
  ASSERT_NO_FATAL_FAILURE(context_ext->SetSocketIdleTimeout(1));
  context->http_network_session_.reset();
  URLRequestContextExt* context_ext1 = context->AsURLRequestContextExt();
  ASSERT_NO_FATAL_FAILURE(context_ext1->SetSocketIdleTimeout(1));
}
#endif  // ARKWEB_NETWORK_SERVICE
}  // namespace net
