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

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "base/test/task_environment.h"
#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "net/test/test_with_task_environment.h"
#include "testing/gtest/include/gtest/gtest.h"

#include "arkweb/chromium_ext/net/dns/secure_dns_fallback_utils.h"

namespace net {

namespace {

IPEndPoint MakeEndPoint(const std::string& ip, uint16_t port = 0) {
  IPAddress address;
  CHECK(address.AssignFromIPLiteral(ip));
  return IPEndPoint(address, port);
}

}  // namespace

class SecureDnsFallbackUtilsTest : public TestWithTaskEnvironment {
 protected:
  SecureDnsFallbackUtilsTest() = default;
  ~SecureDnsFallbackUtilsTest() override = default;

  void SetUp() override {
    // Initialize with empty lists
    StoreSuspectIPListAndSourceHostList({}, {});
  }

  void TearDown() override {
    // Clean up after each test
    StoreSuspectIPListAndSourceHostList({}, {});
  }
};

// ==================== StoreSuspectIPListAndSourceHostList Tests ====================

TEST_F(SecureDnsFallbackUtilsTest, StoreEmptyLists) {
  StoreSuspectIPListAndSourceHostList({}, {});

  std::vector<IPEndPoint> legacy_addresses = {MakeEndPoint("192.168.1.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, StoreSuspectIPListOnly) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1", "10.0.0.2"}, {});

  std::vector<IPEndPoint> legacy_addresses = {MakeEndPoint("10.0.0.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  // Should return false because host is not in source host list
  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, StoreSourceHostListOnly) {
  StoreSuspectIPListAndSourceHostList({}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {MakeEndPoint("192.168.1.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  // Should return false because no suspect IP matched
  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, StoreBothLists) {
  StoreSuspectIPListAndSourceHostList(
      {"10.0.0.1", "10.0.0.2"}, {"example.com", "test.com"});

  std::vector<IPEndPoint> legacy_addresses = {
      MakeEndPoint("10.0.0.1"), MakeEndPoint("192.168.1.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_address.size(), 1u);
}

// ==================== MaybeNeedToProcessAddressList Tests ====================

TEST_F(SecureDnsFallbackUtilsTest, EmptyLegacyAddresses) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses;
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, HostNotInSourceList) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {MakeEndPoint("10.0.0.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "not-in-list.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, NoSuspectIPMatched) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {
      MakeEndPoint("192.168.1.1"), MakeEndPoint("192.168.1.2")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  // No suspect IP matched, should return false
  EXPECT_FALSE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, PartialSuspectIPMatch) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {
      MakeEndPoint("10.0.0.1"), MakeEndPoint("192.168.1.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_address.size(), 1u);
  EXPECT_FALSE(need_to_modify_result);
}

TEST_F(SecureDnsFallbackUtilsTest, AllIPsAreSuspectWithDohFallback) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1", "10.0.0.2"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {
      MakeEndPoint("10.0.0.1"), MakeEndPoint("10.0.0.2")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_EQ(truncation_address.size(), 2u);
  EXPECT_TRUE(need_to_modify_result);
}

TEST_F(SecureDnsFallbackUtilsTest, AllIPsAreSuspectWithoutDohFallback) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1", "10.0.0.2"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {
      MakeEndPoint("10.0.0.1"), MakeEndPoint("10.0.0.2")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, false, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  // When no DoH fallback available, should restore original addresses
  EXPECT_EQ(out_addresses.size(), 2u);
  EXPECT_EQ(truncation_address.size(), 2u);
  EXPECT_FALSE(need_to_modify_result);
}

TEST_F(SecureDnsFallbackUtilsTest, MultipleSuspectIPsAndMixedAddresses) {
  StoreSuspectIPListAndSourceHostList(
      {"10.0.0.1", "10.0.0.2", "10.0.0.3"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {
      MakeEndPoint("192.168.1.1"), MakeEndPoint("10.0.0.1"),
      MakeEndPoint("192.168.1.2"), MakeEndPoint("10.0.0.2"),
      MakeEndPoint("192.168.1.3")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 3u);
  EXPECT_EQ(truncation_address.size(), 2u);
  EXPECT_FALSE(need_to_modify_result);
}

TEST_F(SecureDnsFallbackUtilsTest, IPv6Addresses) {
  StoreSuspectIPListAndSourceHostList({"::1", "2001:db8::1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {
      MakeEndPoint("::1"), MakeEndPoint("2001:db8::2")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_address.size(), 1u);
}

TEST_F(SecureDnsFallbackUtilsTest, MixedIPv4AndIPv6) {
  StoreSuspectIPListAndSourceHostList(
      {"10.0.0.1", "2001:db8::1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {
      MakeEndPoint("10.0.0.1"), MakeEndPoint("2001:db8::1"),
      MakeEndPoint("192.168.1.1"), MakeEndPoint("2001:db8::2")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 2u);
  EXPECT_EQ(truncation_address.size(), 2u);
}

TEST_F(SecureDnsFallbackUtilsTest, InvalidIPInSuspectList) {
  // Store with one invalid IP (should be ignored)
  StoreSuspectIPListAndSourceHostList({"10.0.0.1", "invalid-ip"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {MakeEndPoint("10.0.0.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_EQ(truncation_address.size(), 1u);
  EXPECT_TRUE(need_to_modify_result);
}

TEST_F(SecureDnsFallbackUtilsTest, MultipleHostsInSourceList) {
  StoreSuspectIPListAndSourceHostList(
      {"10.0.0.1"}, {"example.com", "test.com", "demo.com"});

  std::vector<IPEndPoint> legacy_addresses = {MakeEndPoint("10.0.0.1")};

  // Test each host
  for (const auto& host : {"example.com", "test.com", "demo.com"}) {
    std::vector<IPEndPoint> out_addresses;
    std::vector<IPEndPoint> truncation_address;
    bool need_to_modify_result = false;

    bool result = MaybeNeedToProcessAddressList(
        host, legacy_addresses, true, out_addresses,
        need_to_modify_result, truncation_address);

    EXPECT_TRUE(result) << "Host: " << host;
  }
}

TEST_F(SecureDnsFallbackUtilsTest, SingleSuspectIPMatch) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {MakeEndPoint("10.0.0.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_EQ(truncation_address.size(), 1u);
  EXPECT_TRUE(need_to_modify_result);
}

TEST_F(SecureDnsFallbackUtilsTest, RepeatedCallsWithDifferentData) {
  // First call with initial data
  StoreSuspectIPListAndSourceHostList({"10.0.0.1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses1 = {MakeEndPoint("10.0.0.1")};
  std::vector<IPEndPoint> out_addresses1;
  std::vector<IPEndPoint> truncation_address1;
  bool need_to_modify_result1 = false;

  bool result1 = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses1, true, out_addresses1,
      need_to_modify_result1, truncation_address1);

  EXPECT_TRUE(result1);

  // Update with different data
  StoreSuspectIPListAndSourceHostList({"192.168.1.1"}, {"test.com"});

  std::vector<IPEndPoint> legacy_addresses2 = {MakeEndPoint("192.168.1.1")};
  std::vector<IPEndPoint> out_addresses2;
  std::vector<IPEndPoint> truncation_address2;
  bool need_to_modify_result2 = false;

  bool result2 = MaybeNeedToProcessAddressList(
      "test.com", legacy_addresses2, true, out_addresses2,
      need_to_modify_result2, truncation_address2);

  EXPECT_TRUE(result2);

  // Old host should no longer work
  std::vector<IPEndPoint> legacy_addresses3 = {MakeEndPoint("10.0.0.1")};
  std::vector<IPEndPoint> out_addresses3;
  std::vector<IPEndPoint> truncation_address3;
  bool need_to_modify_result3 = false;

  bool result3 = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses3, true, out_addresses3,
      need_to_modify_result3, truncation_address3);

  EXPECT_FALSE(result3);  // example.com is no longer in the list
}

TEST_F(SecureDnsFallbackUtilsTest, LargeSuspectList) {
  std::vector<std::string> suspect_ips;
  for (int i = 0; i < 100; ++i) {
    suspect_ips.push_back("10.0." + std::to_string(i / 256) + "." +
                          std::to_string(i % 256));
  }

  StoreSuspectIPListAndSourceHostList(suspect_ips, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses;
  legacy_addresses.push_back(MakeEndPoint("10.0.0.50"));
  legacy_addresses.push_back(MakeEndPoint("192.168.1.1"));

  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_EQ(out_addresses.size(), 1u);
  EXPECT_EQ(truncation_address.size(), 1u);
}

// ==================== Edge Cases Tests ====================

TEST_F(SecureDnsFallbackUtilsTest, EmptyHostInSourceList) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1"}, {""});

  std::vector<IPEndPoint> legacy_addresses = {MakeEndPoint("10.0.0.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
}

TEST_F(SecureDnsFallbackUtilsTest, SameIPMultipleTimesInLegacyAddresses) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {
      MakeEndPoint("10.0.0.1"), MakeEndPoint("10.0.0.1")};
  std::vector<IPEndPoint> out_addresses;
  std::vector<IPEndPoint> truncation_address;
  bool need_to_modify_result = false;

  bool result = MaybeNeedToProcessAddressList(
      "example.com", legacy_addresses, true, out_addresses,
      need_to_modify_result, truncation_address);

  EXPECT_TRUE(result);
  EXPECT_TRUE(out_addresses.empty());
  EXPECT_EQ(truncation_address.size(), 2u);
  EXPECT_TRUE(need_to_modify_result);
}

TEST_F(SecureDnsFallbackUtilsTest, DohFallbackAvailableBoundary) {
  StoreSuspectIPListAndSourceHostList({"10.0.0.1"}, {"example.com"});

  std::vector<IPEndPoint> legacy_addresses = {MakeEndPoint("10.0.0.1")};

  // Test with doh_fallback_available = true
  {
    std::vector<IPEndPoint> out_addresses;
    std::vector<IPEndPoint> truncation_address;
    bool need_to_modify_result = false;

    bool result = MaybeNeedToProcessAddressList(
        "example.com", legacy_addresses, true, out_addresses,
        need_to_modify_result, truncation_address);

    EXPECT_TRUE(result);
    EXPECT_TRUE(need_to_modify_result);
  }

  // Test with doh_fallback_available = false
  {
    std::vector<IPEndPoint> out_addresses;
    std::vector<IPEndPoint> truncation_address;
    bool need_to_modify_result = false;

    bool result = MaybeNeedToProcessAddressList(
        "example.com", legacy_addresses, false, out_addresses,
        need_to_modify_result, truncation_address);

    EXPECT_TRUE(result);
    EXPECT_FALSE(need_to_modify_result);
    EXPECT_EQ(out_addresses.size(), 1u);
  }
}

}  // namespace net

#endif  // BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
