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

#include "net/dns/dns_config_service_ohos.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>

#include "base/test/task_environment.h"
#include "base/test/test_timeouts.h"
#include "build/build_config.h"
#include "net/base/ip_address.h"
#include "net/dns/public/dns_protocol.h"
#include "securec.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

int32_t (*g_OH_NetConn_GetConnectionProperties)(
    NetConn_NetHandle* netHandle,
    NetConn_ConnectionProperties* prop) = nullptr;

int32_t (*g_OH_NetConn_GetDefaultNet)(NetConn_NetHandle* netHandle) = nullptr;

#ifdef __cplusplus
extern "C" {

extern int32_t __real_OH_NetConn_GetDefaultNet(NetConn_NetHandle* netHandle);
int32_t __wrap_OH_NetConn_GetDefaultNet(NetConn_NetHandle* netHandle) {
  if (g_OH_NetConn_GetDefaultNet) {
    return g_OH_NetConn_GetDefaultNet(netHandle);
  }
  return __real_OH_NetConn_GetDefaultNet(netHandle);
}

extern int32_t __real_OH_NetConn_GetConnectionProperties(
    NetConn_NetHandle* netHandle,
    NetConn_ConnectionProperties* prop);
int32_t __wrap_OH_NetConn_GetConnectionProperties(
    NetConn_NetHandle* netHandle,
    NetConn_ConnectionProperties* prop) {
  if (g_OH_NetConn_GetConnectionProperties) {
    return g_OH_NetConn_GetConnectionProperties(netHandle, prop);
  }
  return __real_OH_NetConn_GetConnectionProperties(netHandle, prop);
}
}
#endif

namespace net {
namespace {
enum AddressFamily { FAMILY_IPV4 = 1, FAMILY_IPV6 = 2 };
static constexpr NetConn_NetAddr kTestAddresses[] = {
    {FAMILY_IPV4, 0, NS_DEFAULTPORT, "92.168.2.2"},
    {FAMILY_IPV4, 0, 0, "192.168.2.2"},
    {FAMILY_IPV6, 0, NS_DEFAULTPORT, "2001:0:4137:9e76:2087:4a7d:8c27:1c2"},
};
static constexpr NetConn_NetAddr kTestInvalidAddresses[] = {
    {FAMILY_IPV4, 0, NS_DEFAULTPORT, "92.168.2.256"},  // invalid ipv4
    {0, 0, NS_DEFAULTPORT, "92.168.2.256"},        // invalid ipv4
    {FAMILY_IPV6, 0, NS_DEFAULTPORT, "2001:0:4137:2087:4a7d:8c27:rrrr"}};
}  // namespace
namespace internal {

class DnsConfigServiceOhosTest : public ::testing::Test {
 public:
  DnsConfigServiceOhosTest() = default;

  void OnConfigChanged(const DnsConfig& config) {
    callback_called_ = true;
    last_config_ = config;
  }

 protected:
  void SetUp() override {
    g_OH_NetConn_GetDefaultNet = nullptr;
    g_OH_NetConn_GetConnectionProperties = nullptr;
    service_ = std::make_unique<DnsConfigServiceOhos>();
  }

  void TearDown() override {
    g_OH_NetConn_GetDefaultNet = nullptr;
    g_OH_NetConn_GetConnectionProperties = nullptr;
    service_.reset();
  }

  DnsConfigServiceOhos* GetService() { return service_.get(); }
  bool WasCallbackCalled() const { return callback_called_; }
  const DnsConfig& last_config() const { return last_config_; }
  void RunUntilIdle() { task_environment_.RunUntilIdle(); }

  void SetDefaultNetStub() {
    g_OH_NetConn_GetDefaultNet = [](NetConn_NetHandle* netHandle) -> int32_t {
      return 0;
    };

    g_OH_NetConn_GetConnectionProperties =
        [](NetConn_NetHandle* netHandle,
           NetConn_ConnectionProperties* prop) -> int32_t {
      prop->dnsListSize = sizeof(kTestAddresses) / sizeof(kTestAddresses[0]);
      EXPECT_TRUE(prop->dnsListSize < NETCONN_MAX_ADDR_SIZE);
      memcpy_s(prop->dnsList, NETCONN_MAX_ADDR_SIZE, kTestAddresses, sizeof(kTestAddresses));
      return 0;
    };
  }

 private:
  base::test::TaskEnvironment task_environment_{
      base::test::TaskEnvironment::MainThreadType::IO};
  std::unique_ptr<DnsConfigServiceOhos> service_;
  bool callback_called_ = false;
  DnsConfig last_config_;
};

TEST_F(DnsConfigServiceOhosTest, ReadConfig) {
  EXPECT_TRUE(GetService() != nullptr);
  SetDefaultNetStub();
  GetService()->ReadConfig(base::BindRepeating(
      &DnsConfigServiceOhosTest::OnConfigChanged, base::Unretained(this)));
  RunUntilIdle();
#if !defined(COMPONENT_BUILD) && defined(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  EXPECT_TRUE(WasCallbackCalled());
  const DnsConfig& config = last_config();
  EXPECT_TRUE(config.IsValid());
  EXPECT_FALSE(config.nameservers.empty());
#endif
}

TEST_F(DnsConfigServiceOhosTest, WatchConfig) {
  EXPECT_TRUE(GetService() != nullptr);
  SetDefaultNetStub();
  GetService()->WatchConfig(base::BindRepeating(
      &DnsConfigServiceOhosTest::OnConfigChanged, base::Unretained(this)));
  RunUntilIdle();
#if !defined(COMPONENT_BUILD) && defined(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  EXPECT_TRUE(WasCallbackCalled());
  const DnsConfig& config = last_config();
  EXPECT_TRUE(config.IsValid());
  EXPECT_FALSE(config.nameservers.empty());
#endif
}

TEST_F(DnsConfigServiceOhosTest, ToEndPoint) {
  for (auto net_addr : kTestAddresses) {
    absl::optional<IPEndPoint> ipe;
    if (net_addr.family == FAMILY_IPV4) {
      ipe = GetIpv4EndPoint(net_addr);
    } else {
      ipe = GetIpv6EndPoint(net_addr);
    }
    ASSERT_TRUE(ipe.has_value());
    EXPECT_EQ(ipe->address().ToString(), net_addr.address);
    EXPECT_EQ(ipe->port(), net_addr.port == 0 ? NS_DEFAULTPORT : net_addr.port);
  }
}

TEST_F(DnsConfigServiceOhosTest, ToEndPoint_Invalid) {
  for (auto net_addr : kTestInvalidAddresses) {
    absl::optional<IPEndPoint> ipe;
    if (net_addr.family == FAMILY_IPV6) {
      ipe = GetIpv6EndPoint(net_addr);
    } else {
      ipe = GetIpv4EndPoint(net_addr);
    }
    EXPECT_FALSE(ipe.has_value());
  }
}

TEST_F(DnsConfigServiceOhosTest, GetNameserversFromConnectProperties) {
  g_OH_NetConn_GetDefaultNet = [](NetConn_NetHandle* netHandle) -> int32_t {
    return -1;
  };
  // fail OH_NetConn_GetDefaultNet
  auto nameservers = GetNameserversFromConnectProperties();
  EXPECT_FALSE(nameservers.has_value());

  g_OH_NetConn_GetDefaultNet = [](NetConn_NetHandle* netHandle) -> int32_t {
    return 0;
  };
  g_OH_NetConn_GetConnectionProperties =
      [](NetConn_NetHandle* netHandle,
         NetConn_ConnectionProperties* prop) -> int32_t { return -1; };
  // fail OH_NetConn_GetConnectionProperties
  nameservers = GetNameserversFromConnectProperties();
  EXPECT_FALSE(nameservers.has_value());

  SetDefaultNetStub();
  nameservers = GetNameserversFromConnectProperties();
  ASSERT_TRUE(nameservers.has_value());
  EXPECT_EQ(nameservers.value().size(),
            sizeof(kTestAddresses) / sizeof(kTestAddresses[0]));
}

TEST_F(DnsConfigServiceOhosTest,
       GetNameserversFromConnectProperties_InvalidAddress) {
  g_OH_NetConn_GetDefaultNet = [](NetConn_NetHandle* netHandle) -> int32_t {
    return 0;
  };
  g_OH_NetConn_GetConnectionProperties =
      [](NetConn_NetHandle* netHandle,
         NetConn_ConnectionProperties* prop) -> int32_t {
    prop->dnsListSize = sizeof(kTestInvalidAddresses) / sizeof(kTestInvalidAddresses[0]);
    EXPECT_TRUE(prop->dnsListSize < NETCONN_MAX_ADDR_SIZE);
    memcpy_s(prop->dnsList, NETCONN_MAX_ADDR_SIZE, kTestInvalidAddresses, sizeof(kTestInvalidAddresses));
    return 0;
  };
  auto nameservers = GetNameserversFromConnectProperties();
  ASSERT_FALSE(nameservers.has_value());
}
}  // namespace internal
}  // namespace net
