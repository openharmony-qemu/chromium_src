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

#include "arkweb/build/features/features.h"
#include "base/compiler_specific.h"
#include "base/test/task_environment.h"
#include "build/buildflag.h"

#include <dlfcn.h>
#include <netdb.h>

#include "net/dns/arkweb_address_info_ext.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "gtest/gtest.h"

namespace net {

class MockAddrInfoForNetwork {
 public:
  static MockAddrInfoForNetwork& getInstance() {
    static MockAddrInfoForNetwork instance;
    return instance;
  }
  MOCK_METHOD(OHGetAddrInfoForNetwork, GetOHGetAddrInfoForNetwork, (), ());
  MOCK_METHOD(OHFreeDnsResult, GetOHFreeDnsResult, (), ());
};

#ifdef __cplusplus
extern "C" {
#endif
static int g_mock_flag = 0;
OHGetAddrInfoForNetwork __real_GetOHGetAddrInfoForNetwork();
OHGetAddrInfoForNetwork __wrap_GetOHGetAddrInfoForNetwork() {
  if (g_mock_flag) {
    return __real_GetOHGetAddrInfoForNetwork();
  } else {
    return MockAddrInfoForNetwork::getInstance().GetOHGetAddrInfoForNetwork();
  }
}

OHFreeDnsResult __real_GetOHFreeDnsResult();
OHFreeDnsResult __wrap_GetOHFreeDnsResult() {
  if (g_mock_flag) {
    return __real_GetOHFreeDnsResult();
  } else {
    return MockAddrInfoForNetwork::getInstance().GetOHFreeDnsResult();
  }
}
#ifdef __cplusplus
}
#endif

namespace {

class ArkWebAddressInfoExtTest : public ::testing::Test {
 protected:
  MockAddrInfoForNetwork mock_addr_info;
};

TEST_F(ArkWebAddressInfoExtTest, TestGetOHGetAddrInfoForNetwork_Success) {
  OHGetAddrInfoForNetwork mock_result =
      [](char* host, char* serv, struct addrinfo* hints, struct addrinfo** res,
         int32_t net_id) -> int32_t {};

  EXPECT_CALL(mock_addr_info, GetOHGetAddrInfoForNetwork())
      .WillOnce(testing::Return(mock_result));

  OHGetAddrInfoForNetwork result = mock_addr_info.GetOHGetAddrInfoForNetwork();
  EXPECT_EQ(result, mock_result);
}

TEST_F(ArkWebAddressInfoExtTest, TestGetOHGetAddrInfoForNetwork_Fail) {
  EXPECT_CALL(mock_addr_info, GetOHGetAddrInfoForNetwork())
      .WillOnce(testing::Return(nullptr));

  OHGetAddrInfoForNetwork result = mock_addr_info.GetOHGetAddrInfoForNetwork();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ArkWebAddressInfoExtTest, TestGetOHFreeDnsResult_Success) {
  OHFreeDnsResult mock_result = [](struct addrinfo* res) -> int32_t {};

  EXPECT_CALL(mock_addr_info, GetOHFreeDnsResult())
      .WillOnce(testing::Return(mock_result));

  OHFreeDnsResult result = mock_addr_info.GetOHFreeDnsResult();
  EXPECT_EQ(result, mock_result);
}

TEST_F(ArkWebAddressInfoExtTest, TestGetOHFreeDnsResult_Fail) {
  EXPECT_CALL(mock_addr_info, GetOHFreeDnsResult())
      .WillOnce(testing::Return(nullptr));

  OHFreeDnsResult result = mock_addr_info.GetOHFreeDnsResult();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ArkWebAddressInfoExtTest, TestInvalidNetworkHandle) {
  struct addrinfo* result = nullptr;
  char host[] = "example.com";
  char serv[] = "http";
  struct addrinfo hints = {};

  int ret = ohos::GetAddrInfoForNetwork(host, serv, &hints, &result,
                                        handles::kInvalidNetworkHandle);
  EXPECT_EQ(ret, EAI_SYSTEM);
  EXPECT_EQ(errno, EINVAL);
}

TEST_F(ArkWebAddressInfoExtTest, TestGetAddrInfoForNetworkIsNull) {
  EXPECT_CALL(MockAddrInfoForNetwork::getInstance(),
              GetOHGetAddrInfoForNetwork())
      .WillOnce(testing::Return(nullptr));
  struct addrinfo* result = nullptr;
  char host[] = "example.com";
  char serv[] = "http";
  struct addrinfo hints = {};

  int ret = ohos::GetAddrInfoForNetwork(host, serv, &hints, &result, 1);
  EXPECT_EQ(ret, EAI_SYSTEM);
  EXPECT_EQ(errno, ENOSYS);
}

TEST_F(ArkWebAddressInfoExtTest, TestFreeDnsResultNull) {
  EXPECT_CALL(MockAddrInfoForNetwork::getInstance(), GetOHFreeDnsResult())
      .WillOnce(testing::Return(nullptr));
  std::shared_ptr<struct addrinfo> res = std::make_shared<struct addrinfo>();
  int ret = ohos::FreeDnsResult(res.get());
  EXPECT_EQ(ret, EAI_SYSTEM);
  EXPECT_EQ(errno, ENOSYS);
}

}  // namespace

}  // namespace net