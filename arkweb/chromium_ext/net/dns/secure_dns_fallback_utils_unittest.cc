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

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

#include "base/test/task_environment.h"
#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "testing/gtest/include/gtest/gtest.h"

#include "arkweb/chromium_ext/net/dns/secure_dns_fallback_utils.h"

namespace net {

namespace {

// Helper function to create IPEndPoint from IP string and port
IPEndPoint MakeIPEndPoint(const std::string& ip_str, uint16_t port) {
  auto addr = IPAddress::FromIPLiteral(ip_str);
  EXPECT_TRUE(addr.has_value()) << "Invalid IP address: " << ip_str;
  return IPEndPoint(addr.value(), port);
}

// Helper function to create a vector of IPEndPoints from IP strings
std::vector<IPEndPoint> MakeIPEndPoints(
    const std::vector<std::string>& ip_strs,
    uint16_t port = 0) {
  std::vector<IPEndPoint> endpoints;
  for (const auto& ip_str : ip_strs) {
    endpoints.push_back(MakeIPEndPoint(ip_str, port));
  }
  return endpoints;
}

// Helper function to set suspect IP and source host lists for testing
void SetSuspectData(const std::vector<std::string>& hosts,
                    const std::vector<std::string>& ips) {
  StoreSuspectIPListAndSourceHostList(ips, hosts);
}

// Helper function to clear all suspect data
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

// ============================================================================
// Tests for StoreSuspectIPListAndSourceHostList
// ============================================================================

TEST_F(SecureDnsFallbackUtilsTest, StoreSuspectData_EmptyLists) {
  // Store empty lists
  SetSuspectData({}, {});

  // Test with empty host list - should not match any host
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  // Host not in list, should return false
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

  // Old host should not be in the list anymore
  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);
  EXPECT_FALSE(result);

  // New host should be in the list
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
  // Include invalid IP address in the list
  SetSuspectData({"example.com"}, {"192.168.1.1", "invalid-ip", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1", "172.16.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  // Invalid IP should be skipped, but valid IPs should still be processed
  EXPECT_TRUE(result);
  EXPECT_EQ(truncation_addresses.size(), 2u);
}

// ============================================================================
// Tests for MaybeNeedToProcessAddressList - Basic Cases
// ============================================================================

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_EmptyLegacyAddresses) {
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses;  // Empty

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  // Empty legacy addresses should return false early
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

  // Host not in monitor list, should return false
  EXPECT_FALSE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_TRUE(truncation_addresses.empty());
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_NoSuspectIps) {
  SetSuspectData({"example.com"}, {"1.2.3.4"});  // Different IP

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  // No suspect IPs matched, should return false
  EXPECT_FALSE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_TRUE(truncation_addresses.empty());
}

// ============================================================================
// Tests for MaybeNeedToProcessAddressList - Suspect IP Handling
// ============================================================================

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
  EXPECT_FALSE(need_to_modify);  // Still have non-suspect IPs

  // Verify remaining addresses don't contain suspect IP
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
  EXPECT_TRUE(need_to_modify);  // All IPs are suspect, fallback available
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_AllSuspectIps_NoFallback) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  // 参数顺序: host, legacy_addresses, doh_fallback_available,
  //          out_addresses, need_to_modify_result, truncation_address
  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses,
      false,  // doh_fallback_available = false
      out_addresses, need_to_modify, truncation_addresses);

  // 源代码逻辑 (secure_dns_fallback_utils.cc:81-87):
  // 当 out_addresses.empty() 且 doh_fallback_available=false 时,
  // 执行 out_addresses = legacy_addresses 恢复原始地址
  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), legacy_addresses.size());
  EXPECT_FALSE(need_to_modify);
  // 验证 truncation_addresses 包含所有 suspect IPs
  EXPECT_EQ(truncation_addresses.size(), 2u);
}

// ============================================================================
// Tests for MaybeNeedToProcessAddressList - DoH Fallback Behavior
// ============================================================================

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_DohFallbackAvailable) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true,  // doh_fallback_available = true
      out_addresses, need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  EXPECT_TRUE(out_addresses.empty());  // All suspect IPs removed
  EXPECT_TRUE(need_to_modify);  // Should signal need to modify result
}

TEST_F(SecureDnsFallbackUtilsTest, MaybeNeedToProcess_DohFallbackNotAvailable) {
  SetSuspectData({"example.com"}, {"192.168.1.1", "10.0.0.1"});

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, false,  // doh_fallback_available = false
      out_addresses, need_to_modify, truncation_addresses);

  EXPECT_TRUE(result);
  // Original addresses restored when no fallback available
  EXPECT_EQ(out_addresses.size(), legacy_addresses.size());
  EXPECT_FALSE(need_to_modify);
}

// ============================================================================
// Tests for MaybeNeedToProcessAddressList - Edge Cases
// ============================================================================

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
  // Only some IPs are suspect
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
  EXPECT_FALSE(need_to_modify);  // Still have valid IPs, no need to modify
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

  // Verify port numbers are preserved
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
  EXPECT_TRUE(need_to_modify);  // Single suspect IP, need fallback
}

// ============================================================================
// Tests for ClearSuspectData
// ============================================================================

TEST_F(SecureDnsFallbackUtilsTest, ClearSuspectData_ClearsAllData) {
  // First set some data
  SetSuspectData({"example.com"}, {"192.168.1.1"});

  // Clear all data
  ClearSuspectData();

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_addresses;
  bool need_to_modify = false;

  std::vector<IPEndPoint> legacy_addresses =
      MakeIPEndPoints({"192.168.1.1", "10.0.0.1"}, 443);

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify, truncation_addresses);

  // After clearing, host should not be in list
  EXPECT_FALSE(result);
}

}  // namespace net

#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
