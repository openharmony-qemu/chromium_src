/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
    strcpy(netAddr.address, kNameserversIPv4[i]);
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
    strcpy(netAddr.address, kNameserversIPv6[i]);
    IPEndPoint ipe = std::move(*internal::GetIpv6EndPoint(netAddr));
    ASSERT_TRUE(ipe.address() == ip);
    ASSERT_TRUE(ipe.port() == netAddr.port);
  }
}

}  // namespace

}  // namespace net
