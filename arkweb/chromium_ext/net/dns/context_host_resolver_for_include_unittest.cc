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

#include <memory>
#include <string>

#include "base/functional/callback.h"
#include "base/test/task_environment.h"
#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"
#include "net/dns/host_resolver.h"
#include "net/dns/public/host_resolver_results.h"
#include "net/dns/resolve_context.h"
#include "net/log/net_log_with_source.h"
#include "net/proxy_resolution/proxy_config_service_fixed.h"
#include "net/proxy_resolution/proxy_config_with_annotation.h"
#include "net/traffic_annotation/network_traffic_annotation_test_helper.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_builder.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

#include "arkweb/build/features/features.h"

#define private public
#define protected public
#include "net/dns/context_host_resolver.h"
#include "net/dns/host_resolver_manager.h"
#undef protected
#undef private

namespace net {

namespace {

std::unique_ptr<URLRequestContext> CreateTestURLRequestContext() {
  URLRequestContextBuilder builder;
  net::ProxyConfigWithAnnotation pcwa(net::ProxyConfig::CreateDirect(),
                                      TRAFFIC_ANNOTATION_FOR_TESTS);
  auto fixed = std::make_unique<net::ProxyConfigServiceFixed>(pcwa);
  builder.set_proxy_config_service(std::move(fixed));
  return builder.Build();
}

}  // namespace

class ContextHostResolverForIncludeTest : public testing::Test {
 public:
  void SetUp() override {
    url_request_context_ = CreateTestURLRequestContext();
    resolve_context_ =
        std::make_unique<ResolveContext>(url_request_context_.get(), true);
  }

  void TearDown() override {
    resolve_context_.reset();
    url_request_context_.reset();
  }

 protected:
  base::test::TaskEnvironment task_env_;
  std::unique_ptr<URLRequestContext> url_request_context_;
  std::unique_ptr<ResolveContext> resolve_context_;
};

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)

TEST_F(ContextHostResolverForIncludeTest, CanUseSecureDnsFallback_WithManager) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  bool result = resolver->CanUseSecureDnsFallback();
  EXPECT_FALSE(result);

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest, GetLocalAddress_WithManager) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  IPEndPoint address;
  EXPECT_NO_FATAL_FAILURE(resolver->GetLocalAddress(&address));

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest,
       CanUseSecureDnsFallback_ManagerForwarding) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = owned_manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  EXPECT_EQ(resolver->manager_, manager_ptr);

  bool result1 = resolver->CanUseSecureDnsFallback();
  bool result2 = resolver->CanUseSecureDnsFallback();

  EXPECT_EQ(result1, result2);
}

TEST_F(ContextHostResolverForIncludeTest, GetLocalAddress_ManagerForwarding) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = owned_manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  EXPECT_EQ(resolver->manager_, manager_ptr);

  IPEndPoint address1;
  IPEndPoint address2;
  EXPECT_NO_FATAL_FAILURE(resolver->GetLocalAddress(&address1));
  EXPECT_NO_FATAL_FAILURE(resolver->GetLocalAddress(&address2));

  EXPECT_EQ(address1.ToString(), address2.ToString());
}

#endif  // BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)

TEST_F(ContextHostResolverForIncludeTest, GetDnsServersString_WithManager) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  std::string result = resolver->GetDnsServersString();
  EXPECT_NO_FATAL_FAILURE(resolver->GetDnsServersString());

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest,
       GetDnsServersString_ManagerForwarding) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  std::string result1 = resolver->GetDnsServersString();
  std::string result2 = resolver->GetDnsServersString();

  EXPECT_EQ(result1, result2);
}

#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)

TEST_F(ContextHostResolverForIncludeTest, NeedRetryDnsOnDnsHijack_EmptyUrl) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  GURL empty_url;
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(empty_url, error_code);
  EXPECT_FALSE(result);

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest, NeedRetryDnsOnDnsHijack_EmptyErrorCode) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  GURL url("http://example.com");
  std::string empty_error_code;

  bool result = resolver->NeedRetryDnsOnDnsHijack(url, empty_error_code);
  EXPECT_NO_FATAL_FAILURE(resolver->NeedRetryDnsOnDnsHijack(url, empty_error_code));

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest, NeedRetryDnsOnDnsHijack_WithManager) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  GURL url("http://example.com");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
  EXPECT_FALSE(result);

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_ManagerForwarding) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL url("http://example.com");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result1 = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
  bool result2 = resolver->NeedRetryDnsOnDnsHijack(url, error_code);

  EXPECT_EQ(result1, result2);
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_VariousErrorCodes) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  GURL url("http://example.com");

  std::vector<std::string> error_codes = {
      "ERR_NAME_NOT_RESOLVED",
      "ERR_CONNECTION_TIMED_OUT",
      "ERR_DNS_TIMED_OUT",
      "ERR_CONNECTION_RESET",
      "ERR_NETWORK_CHANGED",
  };

  for (const auto& error_code : error_codes) {
    bool result = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
    EXPECT_FALSE(result) << "Error code: " << error_code;
  }

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)

TEST_F(ContextHostResolverForIncludeTest, ManagerOwnership) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = owned_manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  EXPECT_EQ(resolver->manager_, manager_ptr);
  EXPECT_NE(resolver->owned_manager_, nullptr);
  EXPECT_NE(resolver->resolve_context_, nullptr);
}

TEST_F(ContextHostResolverForIncludeTest, NonOwnedManager) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  EXPECT_EQ(resolver->manager_, manager_ptr);
  EXPECT_EQ(resolver->owned_manager_, nullptr);
}

TEST_F(ContextHostResolverForIncludeTest, ShutdownBehavior) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  EXPECT_FALSE(resolver->shutting_down_);

  resolver->OnShutdown();

  EXPECT_TRUE(resolver->shutting_down_);
}

TEST_F(ContextHostResolverForIncludeTest, MultipleResolvers_SharedManager) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context1 = std::make_unique<ResolveContext>(url_request_context_.get(), true);
  auto context2 = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver1 = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context1));
  auto resolver2 = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context2));

  manager->RegisterResolveContext(resolver1->resolve_context_.get());
  manager->RegisterResolveContext(resolver2->resolve_context_.get());

  EXPECT_EQ(resolver1->manager_, manager.get());
  EXPECT_EQ(resolver2->manager_, manager.get());

  manager->DeregisterResolveContext(resolver1->resolve_context_.get());
  manager->DeregisterResolveContext(resolver2->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest, MultipleResolvers_OwnedManagers) {
  HostResolver::ManagerOptions options;

  auto owned_manager1 = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  auto owned_manager2 = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context1 = std::make_unique<ResolveContext>(url_request_context_.get(), true);
  auto context2 = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver1 = std::make_unique<ContextHostResolver>(
      std::move(owned_manager1), std::move(context1));
  auto resolver2 = std::make_unique<ContextHostResolver>(
      std::move(owned_manager2), std::move(context2));

  EXPECT_NE(resolver1->manager_, resolver2->manager_);
  EXPECT_NE(resolver1->owned_manager_, nullptr);
  EXPECT_NE(resolver2->owned_manager_, nullptr);
}

TEST_F(ContextHostResolverForIncludeTest, Lifecycle_DestroyResolverFirst) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);
  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
  resolver.reset();

  EXPECT_NE(manager.get(), nullptr);
}

TEST_F(ContextHostResolverForIncludeTest, Lifecycle_OwnedManagerDestruction) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = owned_manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  EXPECT_EQ(resolver->manager_, manager_ptr);

  resolver.reset();
}

TEST_F(ContextHostResolverForIncludeTest, ResolveContext_NotNull) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);
  ResolveContext* context_ptr = context.get();

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  EXPECT_NE(resolver->resolve_context_, nullptr);
  EXPECT_EQ(resolver->resolve_context_.get(), context_ptr);
}

TEST_F(ContextHostResolverForIncludeTest, ResolveContext_ForTestingAccessor) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  EXPECT_NE(resolver->resolve_context_for_testing(), nullptr);
}

TEST_F(ContextHostResolverForIncludeTest, ManagerOptions_DefaultOptions) {
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  EXPECT_EQ(resolver->manager_, manager.get());

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest, ManagerOptions_MaxConcurrentResolves) {
  HostResolver::ManagerOptions options;
  options.max_concurrent_resolves = 4;

  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  manager->RegisterResolveContext(resolver->resolve_context_.get());

  EXPECT_EQ(resolver->manager_, manager.get());

  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest, GetManagerForTesting_ReturnsCorrectManager) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = owned_manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  EXPECT_EQ(resolver->GetManagerForTesting(), manager_ptr);
}

TEST_F(ContextHostResolverForIncludeTest, GetContextForTesting_ReturnsValidContext) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  EXPECT_NE(resolver->GetContextForTesting(), nullptr);
}

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)

TEST_F(ContextHostResolverForIncludeTest,
       CanUseSecureDnsFallback_MultipleCallsConsistency) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  for (int i = 0; i < 10; ++i) {
    bool result = resolver->CanUseSecureDnsFallback();
    EXPECT_FALSE(result);
  }
}

TEST_F(ContextHostResolverForIncludeTest, GetLocalAddress_MultipleCallsConsistency) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  IPEndPoint first_address;
  resolver->GetLocalAddress(&first_address);

  for (int i = 0; i < 5; ++i) {
    IPEndPoint address;
    EXPECT_NO_FATAL_FAILURE(resolver->GetLocalAddress(&address));
    EXPECT_EQ(address.ToString(), first_address.ToString());
  }
}

#endif  // BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)

TEST_F(ContextHostResolverForIncludeTest, GetDnsServersString_MultipleCallsConsistency) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  std::string first_result = resolver->GetDnsServersString();

  for (int i = 0; i < 10; ++i) {
    std::string result = resolver->GetDnsServersString();
    EXPECT_EQ(result, first_result);
  }
}

TEST_F(ContextHostResolverForIncludeTest, GetDnsServersString_AfterShutdown) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  std::string result_before = resolver->GetDnsServersString();

  resolver->OnShutdown();

  std::string result_after = resolver->GetDnsServersString();

  EXPECT_EQ(result_before, result_after);
}

TEST_F(ContextHostResolverForIncludeTest, GetDnsServersString_EmptyResult) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  std::string result = resolver->GetDnsServersString();
  EXPECT_NO_FATAL_FAILURE(resolver->GetDnsServersString());
}

#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)

TEST_F(ContextHostResolverForIncludeTest, NeedRetryDnsOnDnsHijack_HttpsUrl) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL https_url("https://example.com/path?query=value");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(https_url, error_code);
  EXPECT_FALSE(result);
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_FtpUrl) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL ftp_url("ftp://ftp.example.com/file.txt");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(ftp_url, error_code);
  EXPECT_FALSE(result);
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_UrlWithPort) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL url_with_port("http://example.com:8080/path");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(url_with_port, error_code);
  EXPECT_FALSE(result);
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_UrlWithUserInfo) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL url_with_user("http://user:pass@example.com/path");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(url_with_user, error_code);
  EXPECT_FALSE(result);
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_IpAddressUrl) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL ip_url("http://192.168.1.1/path");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(ip_url, error_code);
  EXPECT_FALSE(result);
}

TEST_F(ContextHostResolverForIncludeTest, NeedRetryDnsOnDnsHijack_AllDnsErrorCodes) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL url("http://example.com");

  std::vector<std::string> dns_error_codes = {
      "ERR_NAME_NOT_RESOLVED",
      "ERR_DNS_TIMED_OUT",
      "ERR_DNS_SERVER_REQUIRES_TCP",
      "ERR_DNS_MALFORMED_RESPONSE",
      "ERR_DNS_INVALID_RESPONSE",
      "ERR_DNS_SERVER_FAILED",
      "ERR_DNS_SECURE_RESOLVER_HOSTNAME_RESOLUTION_FAILED",
  };

  for (const auto& error_code : dns_error_codes) {
    bool result = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
    EXPECT_FALSE(result) << "Error code: " << error_code;
  }
}

TEST_F(ContextHostResolverForIncludeTest, NeedRetryDnsOnDnsHijack_AllNetworkErrorCodes) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL url("http://example.com");

  std::vector<std::string> network_error_codes = {
      "ERR_CONNECTION_TIMED_OUT",
      "ERR_CONNECTION_RESET",
      "ERR_CONNECTION_REFUSED",
      "ERR_CONNECTION_ABORTED",
      "ERR_CONNECTION_FAILED",
      "ERR_NETWORK_CHANGED",
      "ERR_INTERNET_DISCONNECTED",
  };

  for (const auto& error_code : network_error_codes) {
    bool result = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
    EXPECT_FALSE(result) << "Error code: " << error_code;
  }
}

TEST_F(ContextHostResolverForIncludeTest, NeedRetryDnsOnDnsHijack_AfterShutdown) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL url("http://example.com");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result_before = resolver->NeedRetryDnsOnDnsHijack(url, error_code);

  resolver->OnShutdown();

  bool result_after = resolver->NeedRetryDnsOnDnsHijack(url, error_code);

  EXPECT_EQ(result_before, result_after);
}

TEST_F(ContextHostResolverForIncludeTest, NeedRetryDnsOnDnsHijack_MultipleCallsConsistency) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL url("http://example.com");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  for (int i = 0; i < 10; ++i) {
    bool result = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
    EXPECT_FALSE(result);
  }
}

TEST_F(ContextHostResolverForIncludeTest, NeedRetryDnsOnDnsHijack_VeryLongUrl) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  std::string long_path(1000, 'a');
  GURL long_url("http://example.com/" + long_path);
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(long_url, error_code);
  EXPECT_FALSE(result);
}

#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)

TEST_F(ContextHostResolverForIncludeTest, HostResolverInterface_Compliance) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  HostResolver* base_ptr = resolver.get();
  EXPECT_NE(base_ptr, nullptr);

  EXPECT_NO_FATAL_FAILURE(base_ptr->GetHostCache());
}

TEST_F(ContextHostResolverForIncludeTest, HostResolverInterface_GetHostCache) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  HostCache* cache = resolver->GetHostCache();
  EXPECT_NO_FATAL_FAILURE(resolver->GetHostCache());
}

TEST_F(ContextHostResolverForIncludeTest, HostResolverInterface_GetDnsConfigAsValue) {
  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  base::Value::Dict config = resolver->GetDnsConfigAsValue();
  EXPECT_NO_FATAL_FAILURE(resolver->GetDnsConfigAsValue());
}

}  // namespace net
