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
#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "testing/gtest/include/gtest/gtest.h"

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/net/dns/secure_dns_fallback_utils.h"

namespace net {
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

namespace {

IPEndPoint MakeIPEndPoint(const std::string& ip_str, uint16_t port) {
  auto addr = IPAddress::FromIPLiteral(ip_str);
  CHECK(addr.has_value()) << "Invalid IP address: " << ip_str;
  return IPEndPoint(addr.value(), port);
}

std::vector<IPEndPoint> MakeIPEndPoints(const std::vector<std::string>& ip_strs,
                                        uint16_t port = 0) {
  std::vector<IPEndPoint> endpoints;
  for (const auto& ip_str : ip_strs) {
    endpoints.push_back(MakeIPEndPoint(ip_str, port));
  }
  return endpoints;
}

void SetSuspectData(const std::vector<std::string>& hosts,
                    const std::vector<std::string>& ips) {
  StoreSuspectIPListAndSourceHostList(ips, hosts);
}

void ClearSuspectData() {
  StoreSuspectIPListAndSourceHostList({}, {});
}

}  // namespace

class SecureDnsFallbackUtilsTest : public testing::Test {
 public:
  void SetUp() override { ClearSuspectData(); }

  void TearDown() override { ClearSuspectData(); }

 protected:
  base::test::TaskEnvironment task_env_;
};

TEST_F(SecureDnsFallbackUtilsTest, StoreSuspectData_EmptyLists) {
  SetSuspectData({}, {});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, StoreSuspectData_SingleHostAndIp) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses.size(), 1u);
  EXPECT_EQ(out_addresses[0].ToStringWithoutPort(), "10.0.0.1");
  EXPECT_EQ(truncation_addresses[0].ToStringWithoutPort(), "192.168.1.1");
}

TEST_F(SecureDnsFallbackUtilsTest, StoreSuspectData_MultipleHostsAndIps) {
  SetSuspectData({"example.com", "test.org"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1", "172.16.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses.size(), 2u);
}

TEST_F(SecureDnsFallbackUtilsTest, StoreSuspectData_OverwritePreviousData) {
  // First set of data
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  // Overwrite with new data
  SetSuspectData({"test.org"}, {"10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);
  EXPECT_FALSE(result);

  out_addresses.clear();
  truncation_addresses.clear();
  need_to_modify = false;

  result = MaybeNeedToProcessAddressList(
      "test.org", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);
  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses[0].ToStringWithoutPort(), "10.0.0.1");
}

TEST_F(SecureDnsFallbackUtilsTest, StoreSuspectData_InvalidIpAddress) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "invalid-ip", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1", "172.16.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 2u);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_EmptyLegacyAddresses) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_FALSE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_TRUE(truncation_addresses.empty());
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_HostNotInList) {
  SetSuspectData({"other.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_FALSE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_TRUE(truncation_addresses.empty());
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_NoSuspectIps) {
  SetSuspectData({"example.com"}, {"1.2.3.4"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_FALSE(result);
  EXPECT_EQ(out_addresses.size(), 2u);
  EXPECT_TRUE(truncation_addresses.empty());
  EXPECT_FALSE(need_to_modify);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_OneSuspectIp) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1", "172.16.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 2u);
  EXPECT_EQ(truncation_addresses.size(), 1u);
  EXPECT_FALSE(need_to_modify);

  for (const auto& addr : out_addresses) {
    EXPECT_NE(addr.ToStringWithoutPort(), "192.168.1.1");
  }
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_MultipleSuspectIps) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1", "172.16.0.1", "8.8.8.8"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 2u);
  EXPECT_EQ(truncation_addresses.size(), 2u);
  EXPECT_FALSE(need_to_modify);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_AllSuspectIps) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_EQ(truncation_addresses.size(), 2u);
  EXPECT_TRUE(need_to_modify);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_AllSuspectIps_NoFallback) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, false,
      out_addresses, need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), legacy_addresses.size());
  EXPECT_FALSE(need_to_modify);
  EXPECT_EQ(truncation_addresses.size(), 2u);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_DohFallbackAvailable) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true,
      out_addresses, need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_TRUE(need_to_modify);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_DohFallbackNotAvailable) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, false,
      out_addresses, need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), legacy_addresses.size());
  EXPECT_FALSE(need_to_modify);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_IPv6Addresses) {
  SetSuspectData({"example.com"}, {"::1", "2001:db8::1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"::1", "2001:db8::1", "::2"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses.size(), 2u);
  EXPECT_EQ(out_addresses[0].ToStringWithoutPort(), "::2");
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_MixedIPv4AndIPv6) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "::1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1", "::1", "::2"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 2u);
  EXPECT_EQ(truncation_addresses.size(), 2u);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_PartialSuspectMatch) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"10.0.0.1", "192.168.1.1", "172.16.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 2u);
  EXPECT_EQ(truncation_addresses.size(), 1u);
  EXPECT_FALSE(need_to_modify);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_PreservePortNumbers) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses;
  legacy_addresses.push_back(MakeIPEndPoint("192.168.1.1", 80));
  legacy_addresses.push_back(MakeIPEndPoint("10.0.0.1", 443));

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses[0].port(), 80);
  EXPECT_EQ(out_addresses[0].port(), 443);
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_SingleAddress) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_EQ(truncation_addresses.size(), 1u);
  EXPECT_TRUE(need_to_modify);
}

TEST_F(SecureDnsFallbackUtilsTest, ClearSuspectData_ClearsAllData) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});
  ClearSuspectData();

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, HostName_EmptyHost) {
  SetSuspectData({""}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, HostName_SubdomainNotMatch) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "sub.example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, HostName_CaseSensitive) {
  SetSuspectData({"Example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, HostName_WithPort) {
  SetSuspectData({"example.com:443"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, HostName_LongHostName) {
  std::string long_host(200, 'a');
  SetSuspectData({long_host}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      long_host, legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, IPAddress_Localhost) {
  SetSuspectData({"example.com"}, {"127.0.0.1", "::1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"127.0.0.1", "::1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses.size(), 2u);
}

TEST_F(SecureDnsFallbackUtilsTest, IPAddress_BroadcastAddress) {
  SetSuspectData({"example.com"}, {"255.255.255.255"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"255.255.255.255", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses[0].ToStringWithoutPort(), "255.255.255.255");
}

TEST_F(SecureDnsFallbackUtilsTest, IPAddress_MulticastAddress) {
  SetSuspectData({"example.com"}, {"224.0.0.1", "ff02::1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"224.0.0.1", "ff02::1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 2u);
}

TEST_F(SecureDnsFallbackUtilsTest, IPAddress_LinkLocalAddress) {
  SetSuspectData({"example.com"}, {"169.254.1.1", "fe80::1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"169.254.1.1", "fe80::1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, IPAddress_PublicDnsServers) {
  SetSuspectData({"example.com"}, {"8.8.8.8", "8.8.4.4", "1.1.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"8.8.8.8", "8.8.4.4", "1.1.1.1", "192.168.1.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses.size(), 3u);
}

TEST_F(SecureDnsFallbackUtilsTest, IPAddress_IPv4MappedIPv6) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"::ffff:192.168.1.1", "192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
  EXPECT_EQ(truncation_addresses[0].ToStringWithoutPort(), "192.168.1.1");
}

TEST_F(SecureDnsFallbackUtilsTest, MultipleCalls_SameHost) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  std::vector<IPEndPoint> out_addresses1;
  std::vector<IPEndPoint> truncation_addresses1;
  bool need_to_modify1 = false;

  bool result1 = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses1,
      need_to_modify1, truncation_addresses1);

  std::vector<IPEndPoint> out_addresses2;
  std::vector<IPEndPoint> truncation_addresses2;
  bool need_to_modify2 = false;

  bool result2 = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses2,
      need_to_modify2, truncation_addresses2);

  EXPECT_EQ(result1, result2);
  EXPECT_EQ(out_addresses1.size(), out_addresses2.size());
  EXPECT_EQ(truncation_addresses1.size(), truncation_addresses2.size());
}

TEST_F(SecureDnsFallbackUtilsTest, MultipleCalls_DifferentHosts) {
  SetSuspectData({"example.com", "test.org"}, {"192.168.1.1"});

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  std::vector<IPEndPoint> out_addresses1;
  std::vector<IPEndPoint> truncation_addresses1;
  bool need_to_modify1 = false;

  bool result1 = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses1,
      need_to_modify1, truncation_addresses1);

  std::vector<IPEndPoint> out_addresses2;
  std::vector<IPEndPoint> truncation_addresses2;
  bool need_to_modify2 = false;

  bool result2 = MaybeNeedToProcessAddressList(
      "test.org", legacy_addresses, true, out_addresses2,
      need_to_modify2, truncation_addresses2);

  EXPECT_EQ(result1, result2);
  EXPECT_EQ(out_addresses1.size(), out_addresses2.size());
}

TEST_F(SecureDnsFallbackUtilsTest, DataPersistence_AcrossCalls) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  for (int i = 0; i < 5; ++i) {
    std::vector<IPEndPoint> out_addresses;
    std::vector<IPEndPoint> truncation_addresses;
    bool need_to_modify = false;

    bool result = MaybeNeedToProcessAddressList(
        "example.com", legacy_addresses, true, out_addresses,
        need_to_modify, truncation_addresses);

    EXPECT_TRUE(result);
    EXPECT_EQ(truncation_addresses.size(), 1u);
  }
}

TEST_F(SecureDnsFallbackUtilsTest, LargeSuspectIPList) {
  std::vector<std::string> suspect_ips;
  for (int i = 1; i <= 100; ++i) {
    suspect_ips.push_back("192.168.1." + std::to_string(i));
  }
  SetSuspectData({"example.com"}, suspect_ips);

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<std::string> legacy_ip_strs;
  for (int i = 1; i <= 50; ++i) {
    legacy_ip_strs.push_back("192.168.1." + std::to_string(i));
  }
  legacy_ip_strs.push_back("10.0.0.1");
  legacy_ip_strs.push_back("10.0.0.2");

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints(legacy_ip_strs, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 50u);
  EXPECT_EQ(out_addresses.size(), 2u);
}

TEST_F(SecureDnsFallbackUtilsTest, LargeHostList) {
  std::vector<std::string> hosts;
  for (int i = 1; i <= 100; ++i) {
    hosts.push_back("host" + std::to_string(i) + ".example.com");
  }
  hosts.push_back("target.example.com");
  SetSuspectData(hosts, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "target.example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, LargeLegacyAddressList) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<std::string> legacy_ip_strs;
  for (int i = 1; i <= 100; ++i) {
    legacy_ip_strs.push_back("10.0.0." + std::to_string(i));
  }
  legacy_ip_strs.push_back("192.168.1.1");

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints(legacy_ip_strs, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
  EXPECT_EQ(out_addresses.size(), 100u);
}

TEST_F(SecureDnsFallbackUtilsTest, OutAddresses_PrePopulated) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses =
      MakeIPEndPoints({"1.1.1.1", "2.2.2.2"}, 80);
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, NeedToModify_InitialStatePreserved) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = true;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_FALSE(need_to_modify);
}

TEST_F(SecureDnsFallbackUtilsTest, DoHFallback_PartialSuspectWithFallback) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true,
      out_addresses, need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_FALSE(need_to_modify);
  EXPECT_EQ(out_addresses.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, DoHFallback_AllSuspectNoFallback) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, false,
      out_addresses, need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 2u);
  EXPECT_FALSE(need_to_modify);
}

TEST_F(SecureDnsFallbackUtilsTest, HostName_WithUnderscore) {
  SetSuspectData({"example_test.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example_test.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, HostName_WithDash) {
  SetSuspectData({"example-test.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example-test.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, HostName_InternationalizedDomain) {
  SetSuspectData({"例子.测试"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "例子.测试", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, DuplicateSuspectIPs) {
  SetSuspectData({"example.com"},
                 {"192.168.1.1", "192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1", "172.16.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 2u);
}

TEST_F(SecureDnsFallbackUtilsTest, DuplicateHosts) {
  SetSuspectData({"example.com", "example.com", "test.org"},
                 {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, DuplicateLegacyAddresses) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 2u);
  EXPECT_EQ(out_addresses.size(), 1u);
}

#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

}  // namespace net