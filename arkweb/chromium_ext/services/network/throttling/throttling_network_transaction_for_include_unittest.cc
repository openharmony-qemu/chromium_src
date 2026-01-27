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

#include "base/functional/bind.h"
#include "net/base/net_errors.h"
#include "net/http/http_transaction_test_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "services/network/throttling/throttling_network_transaction.h"
#undef private


namespace network {

class ThrottlingNetworkTransactionForIncludeTest : public ::testing::Test {
 public:
  std::unique_ptr<ThrottlingNetworkTransaction> CreateTransaction() {
    std::unique_ptr<net::HttpTransaction> trans;
    layer_.CreateTransaction(net::DEFAULT_PRIORITY, &trans);
    return std::make_unique<ThrottlingNetworkTransaction>(std::move(trans));
  }

  net::MockNetworkLayer layer_;
};

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
TEST_F(ThrottlingNetworkTransactionForIncludeTest,
       RestartWithSecureDnsOnly_CheckFailedTrue) {
  auto txn = CreateTransaction();
  txn->failed_ = true;

  int rv = txn->RestartWithSecureDnsOnly(base::DoNothing());
  EXPECT_EQ(rv, net::ERR_INTERNET_DISCONNECTED);
}

TEST_F(ThrottlingNetworkTransactionForIncludeTest,
       RestartWithSecureDnsOnly_NoInterceptor) {
  auto txn = CreateTransaction();

  int rv = txn->RestartWithSecureDnsOnly(base::DoNothing());
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(ThrottlingNetworkTransactionForIncludeTest,
       RestartWithSecureDnsOnly_WithInterceptor) {
  auto txn = CreateTransaction();

  auto interceptor = std::make_unique<ThrottlingNetworkInterceptor>();
  txn->interceptor_ = interceptor->GetWeakPtr();

  int rv = txn->RestartWithSecureDnsOnly(base::DoNothing());
  EXPECT_EQ(rv, net::ERR_FAILED);
}
#endif  // BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
TEST_F(ThrottlingNetworkTransactionForIncludeTest,
       RestartWithFallbackProxy_CheckFailedTrue) {
  auto txn = CreateTransaction();
  txn->failed_ = true;

  int rv = txn->RestartWithFallbackProxy(base::DoNothing());
  EXPECT_EQ(rv, net::ERR_INTERNET_DISCONNECTED);
}

TEST_F(ThrottlingNetworkTransactionForIncludeTest,
       RestartWithFallbackProxy_NoInterceptor) {
  auto txn = CreateTransaction();

  int rv = txn->RestartWithFallbackProxy(base::DoNothing());
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(ThrottlingNetworkTransactionForIncludeTest,
       RestartWithFallbackProxy_WithInterceptor) {
  auto txn = CreateTransaction();

  auto interceptor = std::make_unique<ThrottlingNetworkInterceptor>();
  txn->interceptor_ = interceptor->GetWeakPtr();

  int rv = txn->RestartWithFallbackProxy(base::DoNothing());
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(ThrottlingNetworkTransactionForIncludeTest,
       RestartWithDirect_CheckFailedTrue) {
  auto txn = CreateTransaction();
  txn->failed_ = true;

  int rv = txn->RestartWithDirect(base::DoNothing());
  EXPECT_EQ(rv, net::ERR_INTERNET_DISCONNECTED);
}

TEST_F(ThrottlingNetworkTransactionForIncludeTest,
       RestartWithDirect_NoInterceptor) {
  auto txn = CreateTransaction();

  int rv = txn->RestartWithDirect(base::DoNothing());
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(ThrottlingNetworkTransactionForIncludeTest,
       RestartWithDirect_WithInterceptor) {
  auto txn = CreateTransaction();

  auto interceptor = std::make_unique<ThrottlingNetworkInterceptor>();
  txn->interceptor_ = interceptor->GetWeakPtr();

  int rv = txn->RestartWithDirect(base::DoNothing());
  EXPECT_EQ(rv, net::ERR_FAILED);
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
}  // namespace network