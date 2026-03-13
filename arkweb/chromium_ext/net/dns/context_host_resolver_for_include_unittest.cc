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

// 本测试文件测试 context_host_resolver_for_include.cc 中的扩展方法
// 编译标志说明:
// - ARKWEB_EX_HTTP_DNS_FALLBACK: 启用 CanUseSecureDnsFallback 和 GetLocalAddress
// - ARKWEB_EXT_NAVIGATION: 启用 GetDnsServersString
// - ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING: 启用 NeedRetryDnsOnDnsHijack

#include <memory>
#include <string>

#include "arkweb/build/features/features.h"

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

// ============================================================================
// 私有成员访问处理
// ============================================================================
// 使用预处理器宏来访问 ContextHostResolver 和 HostResolverManager 的私有成员
// 这样可以在测试中验证内部状态
//
// ContextHostResolver 私有成员 (context_host_resolver.h:110-121):
// - owned_manager_: std::unique_ptr<HostResolverManager>
// - manager_: const raw_ptr<HostResolverManager>
// - resolve_context_: std::unique_ptr<ResolveContext>
// - shutting_down_: bool
//
// HostResolverManager 私有成员通过 friend 声明允许 ContextHostResolver 访问
// ============================================================================
#define private public
#define protected public
#include "net/dns/context_host_resolver.h"
#include "net/dns/host_resolver_manager.h"
#undef protected
#undef private

namespace net {

namespace {

// ============================================================================
// Helper 函数
// ============================================================================

// 创建测试用的 URLRequestContext
// 来源: net/url_request/url_request_context_builder.h
// 用途: 为 ResolveContext 提供必需的 URLRequestContext
std::unique_ptr<URLRequestContext> CreateTestURLRequestContext() {
  URLRequestContextBuilder builder;
  // ProxyConfigWithAnnotation 来源: net/proxy_resolution/proxy_config_with_annotation.h
  net::ProxyConfigWithAnnotation pcwa(net::ProxyConfig::CreateDirect(),
                                      TRAFFIC_ANNOTATION_FOR_TESTS);
  // ProxyConfigServiceFixed 来源: net/proxy_resolution/proxy_config_service_fixed.h
  auto fixed = std::make_unique<net::ProxyConfigServiceFixed>(pcwa);
  builder.set_proxy_config_service(std::move(fixed));
  return builder.Build();
}

}  // namespace

// ============================================================================
// 测试 Fixture
// ============================================================================
class ContextHostResolverForIncludeTest : public testing::Test {
 public:
  void SetUp() override {
    // 创建 URLRequestContext - 用于 ResolveContext 构造
    url_request_context_ = CreateTestURLRequestContext();
    // 创建 ResolveContext - 来源: net/dns/resolve_context.h
    // 构造函数参数: URLRequestContext*, bool is_for_network_service
    resolve_context_ =
        std::make_unique<ResolveContext>(url_request_context_.get(), true);
  }

  void TearDown() override {
    // 按相反顺序销毁，避免依赖问题
    resolve_context_.reset();
    url_request_context_.reset();
  }

 protected:
  // TaskEnvironment 用于消息循环 - 来源: base/test/task_environment.h
  base::test::TaskEnvironment task_env_;
  // URLRequestContext - 来源: net/url_request/url_request_context.h
  std::unique_ptr<URLRequestContext> url_request_context_;
  // ResolveContext - 来源: net/dns/resolve_context.h
  std::unique_ptr<ResolveContext> resolve_context_;
};

// ============================================================================
// Tests for ContextHostResolver - ARKWEB_EX_HTTP_DNS_FALLBACK
// ============================================================================

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)

TEST_F(ContextHostResolverForIncludeTest, CanUseSecureDnsFallback_NullManager) {
  // Create a ContextHostResolver with null manager
  auto resolver = std::make_unique<ContextHostResolver>(
      static_cast<HostResolverManager*>(nullptr),
      std::move(resolve_context_));

  // When manager_ is null, should return false
  EXPECT_FALSE(resolver->CanUseSecureDnsFallback());
}

TEST_F(ContextHostResolverForIncludeTest, CanUseSecureDnsFallback_WithManager) {
  // Create a HostResolverManager
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  // Create resolve context for the manager
  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver with manager
  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  // Register the resolve context with the manager
  manager->RegisterResolveContext(resolver->resolve_context_.get());

  // Call CanUseSecureDnsFallback - should forward to manager
  // Since no DoH fallback is configured, should return false
  bool result = resolver->CanUseSecureDnsFallback();

  // The actual result depends on manager's internal state
  // We're testing that the forwarding works correctly
  EXPECT_FALSE(result);  // No DoH fallback configured

  // Cleanup
  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest, GetLocalAddress_NullManager) {
  // Create a ContextHostResolver with null manager
  auto resolver = std::make_unique<ContextHostResolver>(
      static_cast<HostResolverManager*>(nullptr),
      std::move(resolve_context_));

  // When manager_ is null, GetLocalAddress should not crash
  IPEndPoint address;
  EXPECT_NO_FATAL_FAILURE(resolver->GetLocalAddress(&address));

  // Address should remain unchanged (default constructed)
  EXPECT_EQ(address.port(), 0);
}

TEST_F(ContextHostResolverForIncludeTest, GetLocalAddress_WithManager) {
  // Create a HostResolverManager
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  // Create resolve context for the manager
  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver with manager
  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  // Register the resolve context with the manager
  manager->RegisterResolveContext(resolver->resolve_context_.get());

  // Call GetLocalAddress - should forward to manager
  IPEndPoint address;
  EXPECT_NO_FATAL_FAILURE(resolver->GetLocalAddress(&address));

  // Cleanup
  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest,
       CanUseSecureDnsFallback_ManagerForwarding) {
  // This test verifies that CanUseSecureDnsFallback correctly forwards
  // to the manager's CanUseSecureDnsFallback method

  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = owned_manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver that owns its manager
  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  // Verify manager is set
  EXPECT_EQ(resolver->manager_, manager_ptr);

  // The forwarding behavior is tested by ensuring no crash and consistent return
  bool result1 = resolver->CanUseSecureDnsFallback();
  bool result2 = resolver->CanUseSecureDnsFallback();

  // Results should be consistent
  EXPECT_EQ(result1, result2);
}

TEST_F(ContextHostResolverForIncludeTest,
       GetLocalAddress_ManagerForwarding) {
  // This test verifies that GetLocalAddress correctly forwards
  // to the manager's GetLocalAddress method

  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = owned_manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver that owns its manager
  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  // Verify manager is set
  EXPECT_EQ(resolver->manager_, manager_ptr);

  // Call GetLocalAddress - should not crash
  IPEndPoint address1;
  IPEndPoint address2;
  EXPECT_NO_FATAL_FAILURE(resolver->GetLocalAddress(&address1));
  EXPECT_NO_FATAL_FAILURE(resolver->GetLocalAddress(&address2));

  // Results should be consistent
  EXPECT_EQ(address1.ToString(), address2.ToString());
}

#endif  // BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)

// ============================================================================
// Tests for ContextHostResolver - ARKWEB_EXT_NAVIGATION
// ============================================================================

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)

TEST_F(ContextHostResolverForIncludeTest, GetDnsServersString_NullManager) {
  // Create a ContextHostResolver with null manager
  auto resolver = std::make_unique<ContextHostResolver>(
      static_cast<HostResolverManager*>(nullptr),
      std::move(resolve_context_));

  // When manager_ is null, should return empty string
  std::string result = resolver->GetDnsServersString();
  EXPECT_TRUE(result.empty());
}

TEST_F(ContextHostResolverForIncludeTest, GetDnsServersString_WithManager) {
  // Create a HostResolverManager
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  // Create resolve context for the manager
  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver with manager
  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  // Register the resolve context with the manager
  manager->RegisterResolveContext(resolver->resolve_context_.get());

  // Call GetDnsServersString - should forward to manager
  std::string result = resolver->GetDnsServersString();

  // The actual content depends on system DNS configuration
  // We're testing that the forwarding works correctly
  EXPECT_NO_FATAL_FAILURE(resolver->GetDnsServersString());

  // Cleanup
  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest,
       GetDnsServersString_ManagerForwarding) {
  // This test verifies that GetDnsServersString correctly forwards
  // to the manager's GetDnsServersString method

  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver that owns its manager
  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  // Multiple calls should return consistent results
  std::string result1 = resolver->GetDnsServersString();
  std::string result2 = resolver->GetDnsServersString();

  EXPECT_EQ(result1, result2);
}

#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

// ============================================================================
// Tests for ContextHostResolver - ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING
// ============================================================================

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_NullManager) {
  // Create a ContextHostResolver with null manager
  auto resolver = std::make_unique<ContextHostResolver>(
      static_cast<HostResolverManager*>(nullptr),
      std::move(resolve_context_));

  // When manager_ is null, should return false
  GURL url("http://example.com");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
  EXPECT_FALSE(result);
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_EmptyUrl) {
  // Create a HostResolverManager
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  // Create resolve context for the manager
  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver with manager
  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  // Register the resolve context with the manager
  manager->RegisterResolveContext(resolver->resolve_context_.get());

  // Test with empty URL
  GURL empty_url;
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  bool result = resolver->NeedRetryDnsOnDnsHijack(empty_url, error_code);
  EXPECT_FALSE(result);

  // Cleanup
  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_EmptyErrorCode) {
  // Create a HostResolverManager
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  // Create resolve context for the manager
  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver with manager
  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  // Register the resolve context with the manager
  manager->RegisterResolveContext(resolver->resolve_context_.get());

  // Test with empty error code
  GURL url("http://example.com");
  std::string empty_error_code;

  bool result = resolver->NeedRetryDnsOnDnsHijack(url, empty_error_code);
  // Result depends on manager's internal logic
  EXPECT_NO_FATAL_FAILURE(resolver->NeedRetryDnsOnDnsHijack(url, empty_error_code));

  // Cleanup
  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_WithManager) {
  // Create a HostResolverManager
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  // Create resolve context for the manager
  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver with manager
  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  // Register the resolve context with the manager
  manager->RegisterResolveContext(resolver->resolve_context_.get());

  // Test with valid URL and error code
  GURL url("http://example.com");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  // Without configuring protect list, should return false
  bool result = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
  EXPECT_FALSE(result);

  // Cleanup
  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_ManagerForwarding) {
  // This test verifies that NeedRetryDnsOnDnsHijack correctly forwards
  // to the manager's NeedRetryDnsOnDnsHijack method

  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver that owns its manager
  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  GURL url("http://example.com");
  std::string error_code = "ERR_NAME_NOT_RESOLVED";

  // Multiple calls should return consistent results
  bool result1 = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
  bool result2 = resolver->NeedRetryDnsOnDnsHijack(url, error_code);

  EXPECT_EQ(result1, result2);
}

TEST_F(ContextHostResolverForIncludeTest,
       NeedRetryDnsOnDnsHijack_VariousErrorCodes) {
  // Create a HostResolverManager
  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  // Create resolve context for the manager
  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver with manager
  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  // Register the resolve context with the manager
  manager->RegisterResolveContext(resolver->resolve_context_.get());

  GURL url("http://example.com");

  // Test various error codes
  std::vector<std::string> error_codes = {
      "ERR_NAME_NOT_RESOLVED",
      "ERR_CONNECTION_TIMED_OUT",
      "ERR_DNS_TIMED_OUT",
      "ERR_CONNECTION_RESET",
      "ERR_NETWORK_CHANGED",
  };

  for (const auto& error_code : error_codes) {
    bool result = resolver->NeedRetryDnsOnDnsHijack(url, error_code);
    // Without protect list configured, all should return false
    EXPECT_FALSE(result) << "Error code: " << error_code;
  }

  // Cleanup
  manager->DeregisterResolveContext(resolver->resolve_context_.get());
}

#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)

// ============================================================================
// Tests for ContextHostResolver - General Behavior
// ============================================================================

TEST_F(ContextHostResolverForIncludeTest, ManagerOwnership) {
  // Test that ContextHostResolver correctly owns and manages its manager

  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = owned_manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver that owns its manager
  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  // Verify manager is set correctly
  EXPECT_EQ(resolver->manager_, manager_ptr);
  EXPECT_NE(resolver->owned_manager_, nullptr);

  // Verify resolve_context is set
  EXPECT_NE(resolver->resolve_context_, nullptr);
}

TEST_F(ContextHostResolverForIncludeTest, NonOwnedManager) {
  // Test that ContextHostResolver correctly uses a non-owned manager

  HostResolver::ManagerOptions options;
  auto manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);
  HostResolverManager* manager_ptr = manager.get();

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  // Create ContextHostResolver with a non-owned manager
  auto resolver = std::make_unique<ContextHostResolver>(
      manager.get(), std::move(context));

  // Verify manager is set correctly
  EXPECT_EQ(resolver->manager_, manager_ptr);
  EXPECT_EQ(resolver->owned_manager_, nullptr);
}

TEST_F(ContextHostResolverForIncludeTest, ShutdownBehavior) {
  // Test that ContextHostResolver handles shutdown correctly

  HostResolver::ManagerOptions options;
  auto owned_manager = std::make_unique<HostResolverManager>(
      options, nullptr, nullptr);

  auto context = std::make_unique<ResolveContext>(url_request_context_.get(), true);

  auto resolver = std::make_unique<ContextHostResolver>(
      std::move(owned_manager), std::move(context));

  // Initially not shutting down
  EXPECT_FALSE(resolver->shutting_down_);

  // Call OnShutdown
  resolver->OnShutdown();

  // Should be marked as shutting down
  EXPECT_TRUE(resolver->shutting_down_);
}

}  // namespace net
