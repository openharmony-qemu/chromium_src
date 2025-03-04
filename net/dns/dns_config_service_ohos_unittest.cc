// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/dns/dns_config_service_ohos.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#include "base/test/task_environment.h"
#include "base/test/test_timeouts.h"
#include "build/build_config.h"
#include "net/base/ip_address.h"
#include "net/dns/public/dns_protocol.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace net {

namespace {

const char* const kNameserversIPv4[] = {
    "8.8.8.8",
    "192.168.1.1",
    "63.1.2.4",
    "1.0.0.1",
};

const char* const kNameserversIPv6[] = {
    "::1",
    "2001:DB8:0::42",
    "2001:0:4137:9e76:2087:4a7d:8c27:1c2",
    "::FFFF:129.144.52.38",
};

TEST(DnsConfigServiceOhosTest, CreateAndDestroy) {
  base::test::TaskEnvironment task_environment(
      base::test::TaskEnvironment::MainThreadType::IO);

  auto service = std::make_unique<internal::DnsConfigServiceOhos>();
  service.reset();
  task_environment.RunUntilIdle();
}

TEST(DnsConfigServiceOhosTest, GetIpv4EndPoint) {
  for (unsigned i = 0; i < std::size(kNameserversIPv4); i++) {
    IPAddress ip;
    EXPECT_TRUE(ip.AssignFromIPLiteral(kNameserversIPv4[i]));

    NetConn_NetAddr netAddr{1, 0, NS_DEFAULTPORT + i, ""};
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
    strcpy_s(netAddr.address, sizeof(netAddr.address), kNameserversIPv4[i]);
#else
    strcpy(netAddr.address, kNameserversIPv4[i]);
#endif
    IPEndPoint ipe = std::move(*internal::GetIpv4EndPoint(netAddr));
    ASSERT_TRUE(ipe.address() == ip);
    ASSERT_TRUE(ipe.port() == netAddr.port);
  }
}

TEST(DnsConfigServiceOhosTest, GetIpv6EndPoint) {
  for (unsigned i = 0; i < std::size(kNameserversIPv6); i++) {
    IPAddress ip;
    EXPECT_TRUE(ip.AssignFromIPLiteral(kNameserversIPv6[i]));

    NetConn_NetAddr netAddr{1, 0, NS_DEFAULTPORT + i, ""};
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
    strcpy_s(netAddr.address, sizeof(netAddr.address), kNameserversIPv4[i]);
#else
    strcpy(netAddr.address, kNameserversIPv6[i]);
#endif
    IPEndPoint ipe = std::move(*internal::GetIpv6EndPoint(netAddr));
    ASSERT_TRUE(ipe.address() == ip);
    ASSERT_TRUE(ipe.port() == netAddr.port);
  }
}

}  // namespace

}  // namespace net
