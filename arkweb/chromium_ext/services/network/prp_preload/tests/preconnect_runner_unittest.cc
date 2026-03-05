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

#include "testing/gtest/include/gtest/gtest.h"

#include "arkweb/chromium_ext/services/network/prp_preload/src/preload_runner/preconnect_runner.h"
#include "base/test/task_environment.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_builder.h"
#include "net/url_request/url_request_test_util.h"
#include "net/http/transport_security_state.h"

namespace ohos_prp_preload {

class PreconnectRunnerTest : public testing::Test {
public:
  static void SetUpTestCase();
  static void TearDownTestCase();
  void SetUp() override;
  void TearDown() override;
protected:
  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
};

void PreconnectRunnerTest::SetUpTestCase() {}

void PreconnectRunnerTest::TearDownTestCase() {}

void PreconnectRunnerTest::SetUp() {
  task_environment_ = std::make_unique<base::test::TaskEnvironment>(
    base::test::TaskEnvironment::MainThreadType::DEFAULT);
}

void PreconnectRunnerTest::TearDown() {
  task_environment_.reset();
}

TEST_F(PreconnectRunnerTest, PreconnectSocket_AllowCredentialsTest) {
  GURL url("https://example.com");
  bool allow_credentials = true;
  net::NetworkAnonymizationKey network_anonymization_key = 
    net::NetworkAnonymizationKey::CreateSameSite(net::SchemefulSite(url));
  std::unique_ptr<net::URLRequestContext> url_request_context = 
    net::CreateTestURLRequestContextBuilder()->Build();
  
  EXPECT_NE(url_request_context, nullptr);
  PreconnectRunner::PreconnectSocket(url, allow_credentials, network_anonymization_key, 
    url_request_context->GetWeakPtr());
}

TEST_F(PreconnectRunnerTest, PreconnectSocket_NotAllowCredentialsTest) {
  GURL url("https://example.com");
  bool allow_credentials = false;
  net::NetworkAnonymizationKey network_anonymization_key = 
    net::NetworkAnonymizationKey::CreateSameSite(net::SchemefulSite(url));
  std::unique_ptr<net::URLRequestContext> url_request_context = 
    net::CreateTestURLRequestContextBuilder()->Build();
  
  EXPECT_NE(url_request_context, nullptr);
  PreconnectRunner::PreconnectSocket(url, allow_credentials, network_anonymization_key, 
    url_request_context->GetWeakPtr());
}

TEST_F(PreconnectRunnerTest, PreconnectSocket_NullContextTest) {
  GURL url("https://example.com");
  bool allow_credentials = true;
  net::NetworkAnonymizationKey network_anonymization_key = 
    net::NetworkAnonymizationKey::CreateSameSite(net::SchemefulSite(url));
  base::WeakPtr<net::URLRequestContext> url_request_context;
  
  PreconnectRunner::PreconnectSocket(url, allow_credentials, network_anonymization_key, 
    url_request_context);
}

TEST_F(PreconnectRunnerTest, PreconnectSocket_HTTPUrlTest) {
  GURL url("http://example.com");
  bool allow_credentials = true;
  net::NetworkAnonymizationKey network_anonymization_key = 
    net::NetworkAnonymizationKey::CreateSameSite(net::SchemefulSite(url));
  std::unique_ptr<net::URLRequestContext> url_request_context = 
    net::CreateTestURLRequestContextBuilder()->Build();
  
  EXPECT_NE(url_request_context, nullptr);
  PreconnectRunner::PreconnectSocket(url, allow_credentials, network_anonymization_key, 
    url_request_context->GetWeakPtr());
}

TEST_F(PreconnectRunnerTest, GetHSTSRedirect_HTTPUrlTest) {
  GURL url("http://example.com");
  std::unique_ptr<net::URLRequestContext> url_request_context = 
    net::CreateTestURLRequestContextBuilder()->Build();
  
  EXPECT_NE(url_request_context, nullptr);
  GURL result = PreconnectRunner::GetHSTSRedirect(url, url_request_context->GetWeakPtr());
  EXPECT_EQ(result, url);
}

TEST_F(PreconnectRunnerTest, GetHSTSRedirect_HTTPSUrlTest) {
  GURL url("https://example.com");
  std::unique_ptr<net::URLRequestContext> url_request_context = 
    net::CreateTestURLRequestContextBuilder()->Build();
  
  EXPECT_NE(url_request_context, nullptr);
  GURL result = PreconnectRunner::GetHSTSRedirect(url, url_request_context->GetWeakPtr());
  EXPECT_EQ(result, url);
}

TEST_F(PreconnectRunnerTest, GetHSTSRedirect_NoTransportSecurityStateTest) {
  GURL url("http://example.com");
  auto builder = net::CreateTestURLRequestContextBuilder();
  builder->DisableHttpCache();
  std::unique_ptr<net::URLRequestContext> url_request_context = builder->Build();
  
  EXPECT_NE(url_request_context, nullptr);
  GURL result = PreconnectRunner::GetHSTSRedirect(url, url_request_context->GetWeakPtr());
  EXPECT_EQ(result, url);
}

} // namespace ohos_prp_preload