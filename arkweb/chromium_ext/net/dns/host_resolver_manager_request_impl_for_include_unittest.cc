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

#include "base/test/task_environment.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "net/base/host_port_pair.h"
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

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/net/dns/public/resolve_info.h"
#include "arkweb/chromium_ext/net/dns/secure_dns_fallback_utils.h"

#define private public
#define protected public
#include "net/dns/host_resolver_manager.h"
#include "net/dns/host_resolver_manager_request_impl.h"
#undef protected
#undef private

namespace net {
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

namespace {

IPEndPoint MakeIPEndPoint(const std::string& ip_str, uint16_t port) {
  auto addr = IPAddress::FromIPLiteral(ip_str);
  CHECK(addr.has_value()) << "Invalid IP address: " << ip_str;
  return IPEndPoint(addr.value(), port);
}

std::vector<IPEndPoint> MakeIPEndPoints(
    const std::vector<std::string>& ip_strs, uint16_t port = 0) {
  std::vector<IPEndPoint> endpoints;
  for (const auto& ip_str : ip_strs) {
    endpoints.push_back(MakeIPEndPoint(ip_str, port));
  }
  return endpoints;
}

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

void SetSuspectData(const std::vector<std::string>& hosts,
                    const std::vector<std::string>& ips) {
  StoreSuspectIPListAndSourceHostList(ips, hosts);
}

void ClearSuspectData() { StoreSuspectIPListAndSourceHostList({}, {}); }

HostResolver::Host MakeHost(const std::string& host, uint16_t port) {
  return HostResolver::Host(HostPortPair(host, port));
}

}  // namespace

class HostResolverManagerRequestImplTest : public testing::Test {
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

    request_ = std::make_unique<HostResolverManager::RequestImpl>(
        NetLogWithSource(), MakeHost("example.com", 443),
        NetworkAnonymizationKey(), HostResolver::ResolveHostParameters(),
        resolve_context_->GetWeakPtr(),
        base::WeakPtr<HostResolverManager>(),
        base::DefaultTickClock::GetInstance());
  }

  void TearDown() override {
    request_.reset();
    resolve_context_.reset();
    url_request_context_.reset();
    ClearSuspectData();
  }

 protected:
  base::test::TaskEnvironment task_env_;
  std::unique_ptr<URLRequestContext> url_request_context_;
  std::unique_ptr<ResolveContext> resolve_context_;
  std::unique_ptr<HostResolverManager::RequestImpl> request_;
};

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_EmptyEndpoints_ReturnsEarly) {
  ClearSuspectData();
  HostCache::Entry entry = MakeHostCacheEntry(OK, {});
  std::vector<IPEndPoint> truncation_results;
  size_t initial_truncation_size = truncation_results.size();

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), initial_truncation_size);
  EXPECT_EQ(entry.error(), OK);
  EXPECT_TRUE(entry.ip_endpoints().empty());
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_HasError_ReturnsEarly) {
  ClearSuspectData();
  HostCache::Entry entry =
      MakeHostCacheEntry(ERR_NAME_NOT_RESOLVED, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;
  size_t initial_truncation_size = truncation_results.size();
  auto original_endpoints = entry.ip_endpoints();

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), initial_truncation_size);
  EXPECT_EQ(entry.error(), ERR_NAME_NOT_RESOLVED);
  EXPECT_EQ(entry.ip_endpoints().size(), original_endpoints.size());
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_HostNotInList_ReturnsEarly) {
  ClearSuspectData();

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;
  auto original_endpoints = entry.ip_endpoints();

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_TRUE(truncation_results.empty());
  EXPECT_EQ(entry.ip_endpoints().size(), original_endpoints.size());
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_NoSuspectIp_NoModification) {
  SetSuspectData({"example.com"}, {"1.2.3.4"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_TRUE(truncation_results.empty());
  EXPECT_EQ(entry.ip_endpoints().size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_HasSuspectIp_UpdatesResults) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
  EXPECT_EQ(entry.ip_endpoints()[0].ToStringWithoutPort(), "10.0.0.1");
  EXPECT_EQ(truncation_results[0].ToStringWithoutPort(), "192.168.1.1");
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_PartialSuspectIps_UpdatesResults) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "192.168.1.2"});

  HostCache::Entry entry = MakeHostCacheEntry(
      OK, {"192.168.1.1", "10.0.0.1", "192.168.1.2", "10.0.0.2"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 2u);
  EXPECT_EQ(entry.ip_endpoints().size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_AllSuspectIps_NoFallback) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "192.168.1.2"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"192.168.1.1", "192.168.1.2"});
  std::vector<IPEndPoint> truncation_results;
  auto original_endpoints = entry.ip_endpoints();

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(entry.error(), OK);
  EXPECT_EQ(entry.ip_endpoints().size(), original_endpoints.size());
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_AllSuspectIps_NeedModifyError) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(entry.error(), OK);
  EXPECT_EQ(truncation_results.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_IPv6Addresses) {
  SetSuspectData({"example.com"}, {"::1", "2001:db8::1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"::1", "2001:db8::1", "::2"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 2u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
  EXPECT_EQ(entry.ip_endpoints()[0].ToStringWithoutPort(), "::2");
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_MixedIPv4AndIPv6) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "::1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1", "::1", "::2"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 2u);
  EXPECT_EQ(entry.ip_endpoints().size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_PreservePortNumbers) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> legacy_addresses;
  legacy_addresses.push_back(MakeIPEndPoint("192.168.1.1", 80));
  legacy_addresses.push_back(MakeIPEndPoint("10.0.0.1", 443));

  constexpr base::TimeDelta kTtl = base::Seconds(3600);
  HostCache::Entry entry(OK, legacy_addresses, std::set<std::string>(),
                         HostCache::Entry::Source::SOURCE_DNS, kTtl);
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
  EXPECT_EQ(truncation_results[0].port(), 80);
  EXPECT_EQ(entry.ip_endpoints()[0].port(), 443);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_SingleAddress) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_EmptyHost) {
  SetSuspectData({""}, {"192.168.1.1"});

  auto request_empty = std::make_unique<HostResolverManager::RequestImpl>(
      NetLogWithSource(), MakeHost("", 443), NetworkAnonymizationKey(),
      HostResolver::ResolveHostParameters(), resolve_context_->GetWeakPtr(),
      base::WeakPtr<HostResolverManager>(),
      base::DefaultTickClock::GetInstance());

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_empty->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_SubdomainNotMatch) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  auto request_subdomain = std::make_unique<HostResolverManager::RequestImpl>(
      NetLogWithSource(), MakeHost("sub.example.com", 443),
      NetworkAnonymizationKey(), HostResolver::ResolveHostParameters(),
      resolve_context_->GetWeakPtr(), base::WeakPtr<HostResolverManager>(),
      base::DefaultTickClock::GetInstance());

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_subdomain->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_TRUE(truncation_results.empty());
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_CaseSensitive) {
  SetSuspectData({"Example.com"}, {"192.168.1.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_TRUE(truncation_results.empty());
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_MultipleHostsAndIps) {
  SetSuspectData({"example.com", "test.org"}, {"192.168.1.1", "10.0.0.1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1", "172.16.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 2u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_LocalhostAddresses) {
  SetSuspectData({"example.com"}, {"127.0.0.1", "::1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"127.0.0.1", "::1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 2u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_BroadcastAddress) {
  SetSuspectData({"example.com"}, {"255.255.255.255"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"255.255.255.255", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
  EXPECT_EQ(truncation_results[0].ToStringWithoutPort(), "255.255.255.255");
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_MulticastAddress) {
  SetSuspectData({"example.com"}, {"224.0.0.1", "ff02::1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"224.0.0.1", "ff02::1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_LinkLocalAddress) {
  SetSuspectData({"example.com"}, {"169.254.1.1", "fe80::1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"169.254.1.1", "fe80::1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 2u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_PublicDnsServers) {
  SetSuspectData({"example.com"}, {"8.8.8.8", "8.8.4.4", "1.1.1.1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"8.8.8.8", "8.8.4.4", "1.1.1.1", "192.168.1.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 3u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_LargeSuspectIPList) {
  std::vector<std::string> suspect_ips;
  for (int i = 1; i <= 100; ++i) {
    suspect_ips.push_back("192.168.1." + std::to_string(i));
  }
  SetSuspectData({"example.com"}, suspect_ips);

  std::vector<std::string> legacy_ip_strs;
  for (int i = 1; i <= 50; ++i) {
    legacy_ip_strs.push_back("192.168.1." + std::to_string(i));
  }
  legacy_ip_strs.push_back("10.0.0.1");
  legacy_ip_strs.push_back("10.0.0.2");

  HostCache::Entry entry = MakeHostCacheEntry(OK, legacy_ip_strs);
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 50u);
  EXPECT_EQ(entry.ip_endpoints().size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_LargeHostList) {
  std::vector<std::string> hosts;
  for (int i = 1; i <= 100; ++i) {
    hosts.push_back("host" + std::to_string(i) + ".example.com");
  }
  hosts.push_back("example.com");
  SetSuspectData(hosts, {"192.168.1.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_LargeLegacyAddressList) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<std::string> legacy_ip_strs;
  for (int i = 1; i <= 100; ++i) {
    legacy_ip_strs.push_back("10.0.0." + std::to_string(i));
  }
  legacy_ip_strs.push_back("192.168.1.1");

  HostCache::Entry entry = MakeHostCacheEntry(OK, legacy_ip_strs);
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
  EXPECT_EQ(entry.ip_endpoints().size(), 100u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_DuplicateSuspectIPs) {
  SetSuspectData({"example.com"},
                 {"192.168.1.1", "192.168.1.1", "10.0.0.1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1", "172.16.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_DuplicateHosts) {
  SetSuspectData({"example.com", "example.com", "test.org"}, {"192.168.1.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_DuplicateLegacyAddresses) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"192.168.1.1", "192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 2u);
  EXPECT_EQ(entry.ip_endpoints().size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_HostNameWithUnderscore) {
  SetSuspectData({"example_test.com"}, {"192.168.1.1"});

  auto request_underscore = std::make_unique<HostResolverManager::RequestImpl>(
      NetLogWithSource(), MakeHost("example_test.com", 443),
      NetworkAnonymizationKey(), HostResolver::ResolveHostParameters(),
      resolve_context_->GetWeakPtr(), base::WeakPtr<HostResolverManager>(),
      base::DefaultTickClock::GetInstance());

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_underscore->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_HostNameWithDash) {
  SetSuspectData({"example-test.com"}, {"192.168.1.1"});

  auto request_dash = std::make_unique<HostResolverManager::RequestImpl>(
      NetLogWithSource(), MakeHost("example-test.com", 443),
      NetworkAnonymizationKey(), HostResolver::ResolveHostParameters(),
      resolve_context_->GetWeakPtr(), base::WeakPtr<HostResolverManager>(),
      base::DefaultTickClock::GetInstance());

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_dash->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResults_LongHostName) {
  std::string long_host(200, 'a');
  SetSuspectData({long_host}, {"192.168.1.1"});

  auto request_long = std::make_unique<HostResolverManager::RequestImpl>(
      NetLogWithSource(), MakeHost(long_host, 443), NetworkAnonymizationKey(),
      HostResolver::ResolveHostParameters(), resolve_context_->GetWeakPtr(),
      base::WeakPtr<HostResolverManager>(),
      base::DefaultTickClock::GetInstance());

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  std::vector<IPEndPoint> truncation_results;

  request_long->MaybeModifyResolveLocallyResults(entry, truncation_results);

  EXPECT_EQ(truncation_results.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_UpdatesResolveInfo) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, OK);
  EXPECT_EQ(info.dns_status, kDnsResolvedByLocalDns);
  EXPECT_EQ(info.truncation_ips.size(), 1u);
  EXPECT_EQ(info.ip_endpoints.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
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

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_WithError) {
  ClearSuspectData();

  HostCache::Entry entry =
      MakeHostCacheEntry(ERR_NAME_NOT_RESOLVED, {"192.168.1.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedBySystemDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, ERR_NAME_NOT_RESOLVED);
  EXPECT_EQ(info.dns_status, kDnsResolvedBySystemDns);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_NoSuspectIps) {
  SetSuspectData({"example.com"}, {"1.2.3.4"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByHttpsDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, OK);
  EXPECT_EQ(info.dns_status, kDnsResolvedByHttpsDns);
  EXPECT_TRUE(info.truncation_ips.empty());
  EXPECT_EQ(info.ip_endpoints.size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_MultipleTruncationIps) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1", "172.16.0.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.truncation_ips.size(), 2u);
  EXPECT_EQ(info.ip_endpoints.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_AllTruncated) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.truncation_ips.size(), 2u);
  EXPECT_EQ(info.ip_endpoints.size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_VariousDnsStatus) {
  ClearSuspectData();

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedFromHosts);
  EXPECT_EQ(request_->resolve_info_.dns_status, kDnsResolvedFromHosts);

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedFromInsecureCache);
  EXPECT_EQ(request_->resolve_info_.dns_status, kDnsResolvedFromInsecureCache);

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedFromSecureCache);
  EXPECT_EQ(request_->resolve_info_.dns_status, kDnsResolvedFromSecureCache);

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedBySystemDns);
  EXPECT_EQ(request_->resolve_info_.dns_status, kDnsResolvedBySystemDns);

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByHttpsDns);
  EXPECT_EQ(request_->resolve_info_.dns_status, kDnsResolvedByHttpsDns);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_VariousErrorCodes) {
  ClearSuspectData();

  std::vector<int> error_codes = {OK,
                                  ERR_NAME_NOT_RESOLVED,
                                  ERR_DNS_CACHE_MISS,
                                  ERR_CONNECTION_RESET,
                                  ERR_TIMED_OUT,
                                  ERR_INTERNET_DISCONNECTED};

  for (int error : error_codes) {
    HostCache::Entry entry = MakeHostCacheEntry(error, {"192.168.1.1"});

    request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
        entry, kDnsResolvedByLocalDns);

    EXPECT_EQ(request_->resolve_info_.error_code, error);
  }
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_IPv6Endpoints) {
  SetSuspectData({"example.com"}, {"::1", "2001:db8::1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"::1", "2001:db8::1", "::2"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.truncation_ips.size(), 2u);
  EXPECT_EQ(info.ip_endpoints.size(), 1u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_MixedIPv4IPv6) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "::1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1", "::1", "::2"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.truncation_ips.size(), 2u);
  EXPECT_EQ(info.ip_endpoints.size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_PreservePortInEndpoints) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> legacy_addresses;
  legacy_addresses.push_back(MakeIPEndPoint("192.168.1.1", 80));
  legacy_addresses.push_back(MakeIPEndPoint("10.0.0.1", 443));

  constexpr base::TimeDelta kTtl = base::Seconds(3600);
  HostCache::Entry entry(OK, legacy_addresses, std::set<std::string>(),
                         HostCache::Entry::Source::SOURCE_DNS, kTtl);

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.ip_endpoints.size(), 1u);
  EXPECT_EQ(info.ip_endpoints[0], "10.0.0.1");
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_HostNotInList) {
  ClearSuspectData();

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_TRUE(info.truncation_ips.empty());
  EXPECT_EQ(info.ip_endpoints.size(), 2u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_LargeEndpointList) {
  ClearSuspectData();

  std::vector<std::string> ip_strs;
  for (int i = 1; i <= 100; ++i) {
    ip_strs.push_back("10.0.0." + std::to_string(i));
  }

  HostCache::Entry entry = MakeHostCacheEntry(OK, ip_strs);

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.ip_endpoints.size(), 100u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_EmptyEntryError) {
  ClearSuspectData();

  HostCache::Entry entry(ERR_NAME_NOT_RESOLVED, {}, std::set<std::string>(),
                         HostCache::Entry::Source::SOURCE_DNS,
                         base::Seconds(3600));

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedUndefined);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, ERR_NAME_NOT_RESOLVED);
  EXPECT_TRUE(info.ip_endpoints.empty());
  EXPECT_TRUE(info.truncation_ips.empty());
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_MultipleCalls) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  HostCache::Entry entry1 =
      MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});
  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry1, kDnsResolvedByLocalDns);
  EXPECT_EQ(request_->resolve_info_.dns_status, kDnsResolvedByLocalDns);
  EXPECT_EQ(request_->resolve_info_.ip_endpoints.size(), 1u);

  HostCache::Entry entry2 = MakeHostCacheEntry(OK, {"10.0.0.2", "10.0.0.3"});
  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry2, kDnsResolvedByHttpsDns);
  EXPECT_EQ(request_->resolve_info_.dns_status, kDnsResolvedByHttpsDns);
  // ip_endpoints is appended (emplace_back), so total is 1 + 2 = 3
  EXPECT_EQ(request_->resolve_info_.ip_endpoints.size(), 3u);
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_TruncationIpString) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1", "172.16.0.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.truncation_ips.size(), 2u);
  EXPECT_EQ(info.truncation_ips[0], "192.168.1.1");
  EXPECT_EQ(info.truncation_ips[1], "10.0.0.1");
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_IPv6TruncationString) {
  SetSuspectData({"example.com"}, {"::1", "2001:db8::1"});

  HostCache::Entry entry =
      MakeHostCacheEntry(OK, {"::1", "2001:db8::1", "::2"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.truncation_ips.size(), 2u);
  EXPECT_EQ(info.truncation_ips[0], "::1");
  EXPECT_EQ(info.truncation_ips[1], "2001:db8::1");
}

TEST_F(HostResolverManagerRequestImplTest,
       MaybeModifyResolveLocallyResultsAndUpdateResolveInfo_DnsCacheMissError) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  HostCache::Entry entry = MakeHostCacheEntry(OK, {"192.168.1.1", "10.0.0.1"});

  request_->MaybeModifyResolveLocallyResultsAndUpdateResolveInfo(
      entry, kDnsResolvedByLocalDns);

  const auto& info = request_->resolve_info_;
  EXPECT_EQ(info.error_code, OK);
}

#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

}  // namespace net
