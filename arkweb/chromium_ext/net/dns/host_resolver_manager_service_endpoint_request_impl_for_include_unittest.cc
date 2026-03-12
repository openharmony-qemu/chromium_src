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
#include <set>
#include <string>
#include <vector>

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/net/dns/public/resolve_info.h"
#include "base/functional/callback.h"
#include "base/test/task_environment.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "net/base/connection_endpoint_metadata.h"
#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"
#include "net/base/network_anonymization_key.h"
#include "net/dns/host_cache.h"
#include "net/dns/host_resolver.h"
#include "net/dns/public/host_resolver_results.h"
#include "net/dns/public/resolve_error_info.h"
#include "net/dns/resolve_context.h"
#include "net/log/net_log_with_source.h"
#include "net/proxy_resolution/proxy_config_service_fixed.h"
#include "net/proxy_resolution/proxy_config_with_annotation.h"
#include "net/traffic_annotation/network_traffic_annotation_test_helper.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_builder.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/scheme_host_port.h"

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
#include "arkweb/chromium_ext/net/dns/secure_dns_fallback_utils.h"
#endif

#define private public
#define protected public
#include "net/dns/host_resolver_manager.h"
#include "net/dns/host_resolver_manager_service_endpoint_request_impl.h"
#undef protected
#undef private

namespace net {

namespace {

// Helper function to create a HostCache::Entry with the given error and IP
// endpoints.
HostCache::Entry MakeHostCacheEntry(int error,
                                    const std::vector<std::string>& ip_literals,
                                    uint16_t port = 0) {
  std::vector<IPEndPoint> endpoints;
  for (const auto& ip : ip_literals) {
    auto addr = IPAddress::FromIPLiteral(ip);
    if (addr.has_value()) {
      endpoints.emplace_back(*addr, port);
    }
  }
  constexpr base::TimeDelta kTtl = base::Seconds(3600);
  return HostCache::Entry(error, endpoints, std::set<std::string>(),
                          HostCache::Entry::Source::SOURCE_DNS, kTtl);
}

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
// Helper function to set suspect IP and source host lists for testing.
void SetSuspectData(const std::vector<std::string>& hosts,
                    const std::vector<std::string>& ips) {
  StoreSuspectIPListAndSourceHostList(ips, hosts);
}

// Helper function to clear all suspect data.
void ClearSuspectData() {
  StoreSuspectIPListAndSourceHostList({}, {});
}
#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

}  // namespace

class HostResolverManagerServiceEndpointRequestImplTest
    : public testing::Test {
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

    request_ = std::make_unique<HostResolverManager::ServiceEndpointRequestImpl>(
        url::SchemeHostPort("https", "example.com", 443),
        NetworkAnonymizationKey(),
        NetLogWithSource(),
        HostResolver::ResolveHostParameters(),
        resolve_context_->GetWeakPtr(),
        base::WeakPtr<HostResolverManager>(),
        base::DefaultTickClock::GetInstance());
  }

  void TearDown() override {
    request_.reset();
    resolve_context_.reset();
    url_request_context_.reset();
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
    ClearSuspectData();
#endif
  }

 protected:
  base::test::TaskEnvironment task_env_;
  std::unique_ptr<URLRequestContext> url_request_context_;
  std::unique_ptr<ResolveContext> resolve_context_;
  std::unique_ptr<HostResolverManager::ServiceEndpointRequestImpl> request_;
};

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

// Tests for MaybeModifyResolveLocallyResults

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResults_EmptyEndpoints_ReturnsEarly) {
  HostCache::Entry entry = MakeHostCacheEntry(OK, {});
  std::vector<IPEndPoint> truncation_results;
  size_t initial_truncation_size = truncation_results.size();

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  // Function should return early without modifying anything
  EXPECT_EQ(truncation_results.size(), initial_truncation_size);
  EXPECT_EQ(entry.error(), OK);
  EXPECT_TRUE(entry.ip_endpoints().empty());
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResults_HasError_ReturnsEarly) {
  HostCache::Entry entry = MakeHostCacheEntry(ERR_NAME_NOT_RESOLVED,
                                              {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;
  size_t initial_truncation_size = truncation_results.size();
  auto original_endpoints = entry.ip_endpoints();

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  // Function should return early due to error
  EXPECT_EQ(truncation_results.size(), initial_truncation_size);
  EXPECT_EQ(entry.error(), ERR_NAME_NOT_RESOLVED);
  EXPECT_EQ(entry.ip_endpoints().size(), original_endpoints.size());
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResults_HostNotInList_ReturnsEarly) {
  // No suspect data set, so host won't be in the list
  ClearSuspectData();

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;
  auto original_endpoints = entry.ip_endpoints();

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  // Host not in monitor list, should return early
  EXPECT_TRUE(truncation_results.empty());
  EXPECT_EQ(entry.ip_endpoints().size(), original_endpoints.size());
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResults_NoSuspectIp_NoModification) {
  // Set host list but not IP list (or set IP list with different IPs)
  SetSuspectData({"example.com"}, {"1.2.3.4"});  // Different IP

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  // None of the IPs are suspect, so no modification
  EXPECT_TRUE(truncation_results.empty());
  EXPECT_EQ(entry.ip_endpoints().size(), 2u);
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResults_HasSuspectIp_UpdatesResults) {
  // Set suspect data with matching host and one suspect IP
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  // Suspect IP should be moved to truncation_results
  EXPECT_EQ(truncation_results.size(), 1u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
  // Remaining IP should be 10.0.0.1
  EXPECT_EQ(entry.ip_endpoints()[0].ToStringWithoutPort(), "10.0.0.1");
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResults_PartialSuspectIps_UpdatesResults) {
  // Set multiple suspect IPs
  SetSuspectData({"example.com"}, {"192.168.1.1", "192.168.1.2"});

  HostCache::Entry entry = MakeHostCacheEntry(
      OK, {"192.168.1.1", "10.0.0.1", "192.168.1.2", "10.0.0.2"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  // Two suspect IPs should be moved to truncation_results
  EXPECT_EQ(truncation_results.size(), 2u);
  EXPECT_EQ(entry.ip_endpoints().size(), 2u);
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResults_AllSuspectIps_NoFallback) {
  // Set all IPs as suspect
  SetSuspectData({"example.com"}, {"192.168.1.1", "192.168.1.2"});

  HostCache::Entry entry = MakeHostCacheEntry(
      OK, {"192.168.1.1", "192.168.1.2"});
  std::vector<IPEndPoint> truncation_results;
  auto original_endpoints = entry.ip_endpoints();

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  // Since manager_ is null, secure_dns_fallback_available is false
  // All IPs are suspect but no fallback available, so original addresses are restored
  // need_to_modify_result = false, so error should remain OK
  EXPECT_EQ(entry.error(), OK);
  // Original addresses should be restored since no fallback available
  EXPECT_EQ(entry.ip_endpoints().size(), original_endpoints.size());
}

// Tests for MaybeModifyResolveLocallyResultsAndUpdateResolveInfo

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_UpdatesResolveInfo) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, OK);
  EXPECT_EQ(info.dns_status, kDnsResolvedByLocalDns);
  // Suspect IP should be in truncation_ips
  EXPECT_EQ(info.truncation_ips.size(), 1u);
  // Remaining IP should be in ip_endpoints
  EXPECT_EQ(info.ip_endpoints.size(), 1u);
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_EmptyResults) {
  ClearSuspectData();

  HostCache::Entry entry = MakeHostCacheEntry(OK, {});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedUndefined);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, OK);
  EXPECT_EQ(info.dns_status, kDnsResolvedUndefined);
  EXPECT_TRUE(info.truncation_ips.empty());
  EXPECT_TRUE(info.ip_endpoints.empty());
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_WithError) {
  ClearSuspectData();

  HostCache::Entry entry = MakeHostCacheEntry(ERR_NAME_NOT_RESOLVED,
                                              {"192.168.1.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedBySystemDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, ERR_NAME_NOT_RESOLVED);
  EXPECT_EQ(info.dns_status, kDnsResolvedBySystemDns);
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_NoSuspectIps) {
  // Host in list but IPs not suspect
  SetSuspectData({"example.com"}, {"1.2.3.4"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByHttpsDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, OK);
  EXPECT_EQ(info.dns_status, kDnsResolvedByHttpsDns);
  // No truncation since no suspect IPs
  EXPECT_TRUE(info.truncation_ips.empty());
  // All IPs should be in ip_endpoints
  EXPECT_EQ(info.ip_endpoints.size(), 2u);
}

#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)

// Tests for set_resolve_info

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       set_resolve_info_SetsBasicInfo) {
  std::vector<HostResolverManager::TaskType> finished_tasks;
  std::vector<IPEndPoint> truncation_results;

  request_->set_resolve_info(ERR_CONNECTION_RESET, kDnsResolvedByLocalDns,
                             finished_tasks, truncation_results);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, ERR_CONNECTION_RESET);
  EXPECT_EQ(info.dns_status, kDnsResolvedByLocalDns);
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       set_resolve_info_HandlesEmptyFinalizedResult) {
  // finalized_result_ is not set, so no IPs should be extracted
  std::vector<HostResolverManager::TaskType> finished_tasks;
  std::vector<IPEndPoint> truncation_results;

  request_->set_resolve_info(OK, kDnsResolvedUndefined, finished_tasks,
                             truncation_results);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, OK);
  EXPECT_TRUE(info.ip_endpoints.empty());
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       set_resolve_info_AddsTruncationIps) {
  std::vector<HostResolverManager::TaskType> finished_tasks;

  // Create truncation results
  auto ip1 = IPAddress::FromIPLiteral("192.168.1.1");
  auto ip2 = IPAddress::FromIPLiteral("10.0.0.1");
  ASSERT_TRUE(ip1.has_value());
  ASSERT_TRUE(ip2.has_value());

  std::vector<IPEndPoint> truncation_results;
  truncation_results.emplace_back(*ip1, 0);
  truncation_results.emplace_back(*ip2, 0);

  request_->set_resolve_info(OK, kDnsResolvedByLocalDns, finished_tasks,
                             truncation_results);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.truncation_ips.size(), 2u);
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       set_resolve_info_AddsTransitionTypes) {
  // Create finished tasks
  std::vector<HostResolverManager::TaskType> finished_tasks;
  finished_tasks.push_back(HostResolverManager::TaskType::DNS);
  finished_tasks.push_back(HostResolverManager::TaskType::MDNS);
  finished_tasks.push_back(HostResolverManager::TaskType::SECURE_DNS);

  std::vector<IPEndPoint> truncation_results;

  request_->set_resolve_info(OK, kDnsResolvedByLocalDns, finished_tasks,
                             truncation_results);

  const auto& info = request_->resolve_info_;
  // Check that transition types were added (dns_transition should be non-zero)
  EXPECT_NE(info.dns_transition, 0u);
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       set_resolve_info_EmptyTasks) {
  std::vector<HostResolverManager::TaskType> finished_tasks;  // Empty
  std::vector<IPEndPoint> truncation_results;

  request_->set_resolve_info(OK, kDnsResolvedUndefined, finished_tasks,
                             truncation_results);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.dns_transition, 0u);
}

TEST_F(HostResolverManagerServiceEndpointRequestImplTest,
       set_resolve_info_WithFinalizedResult) {
  // Set up finalized_result_ with endpoints
  std::vector<ServiceEndpoint> endpoints;

  // Create IPv4 endpoints
  auto ipv4_1 = IPAddress::FromIPLiteral("192.168.1.1");
  auto ipv4_2 = IPAddress::FromIPLiteral("10.0.0.1");
  auto ipv6_1 = IPAddress::FromIPLiteral("::1");

  ASSERT_TRUE(ipv4_1.has_value());
  ASSERT_TRUE(ipv4_2.has_value());
  ASSERT_TRUE(ipv6_1.has_value());

  std::vector<IPEndPoint> ipv4_endpoints;
  ipv4_endpoints.emplace_back(*ipv4_1, 443);
  ipv4_endpoints.emplace_back(*ipv4_2, 443);

  std::vector<IPEndPoint> ipv6_endpoints;
  ipv6_endpoints.emplace_back(*ipv6_1, 443);

  ServiceEndpoint endpoint(ipv4_endpoints, ipv6_endpoints,
                           ConnectionEndpointMetadata());
  endpoints.push_back(endpoint);

  request_->finalized_result_.emplace(std::move(endpoints),
                                      std::set<std::string>());

  std::vector<HostResolverManager::TaskType> finished_tasks;
  std::vector<IPEndPoint> truncation_results;

  request_->set_resolve_info(OK, kDnsResolvedByLocalDns, finished_tasks,
                             truncation_results);

  const auto& info = request_->resolve_info_;
  // Should have 3 IPs (2 IPv4 + 1 IPv6)
  EXPECT_EQ(info.ip_endpoints.size(), 3u);
}

#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

}  // namespace net
