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

#include "cert_mgr_adapter_impl.h"
#include "third_party/bounds_checking_function/include/securec.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <network/netstack/net_ssl/net_ssl_c.h>

#include <cstdint>
#include <cstring>
#include <thread>

using namespace OHOS::NWeb;
using namespace testing;
using namespace std;

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;

class MockNetStack {
 public:
  MOCK_METHOD(int32_t,
              Mock_OH_NetStack_GetCertificatesForHostName,
              (const char*, NetStack_Certificates*));
  MOCK_METHOD(void,
              Mock_OH_Netstack_DestroyCertificatesContent,
              (NetStack_Certificates*));
  MOCK_METHOD(int32_t,
              Mock_OH_NetStack_GetPinSetForHostName,
              (const char*, NetStack_CertificatePinning*));
};

MockNetStack* g_mockNetStack = nullptr;

int32_t OH_NetStack_GetCertificatesForHostName(
    const char* hostname,
    NetStack_Certificates* certificates) {
  if (hostname == nullptr || certificates == nullptr) {
    return -1;
  }
  return g_mockNetStack->Mock_OH_NetStack_GetCertificatesForHostName(
      hostname, certificates);
}

void OH_Netstack_DestroyCertificatesContent(
    NetStack_Certificates* certificates) {
  if (certificates == nullptr) {
    return;
  }
  g_mockNetStack->Mock_OH_Netstack_DestroyCertificatesContent(certificates);
}
int32_t OH_NetStack_GetPinSetForHostName(
    const char* hostname,
    NetStack_CertificatePinning* certificates) {
  if (hostname == nullptr || certificates == nullptr) {
    return -1;
  }
  return g_mockNetStack->Mock_OH_NetStack_GetPinSetForHostName(hostname,
                                                               certificates);
}

class CertManagerAdapterImplTest : public testing::Test {
 public:
  void SetUp() override { g_mockNetStack = new MockNetStack(); }
  void TearDown() override {
    delete g_mockNetStack;
    g_mockNetStack = nullptr;
  }
  CertManagerAdapterImpl adapter;
};

// Test case 1: Success case with certificates returned
TEST_F(CertManagerAdapterImplTest, GetTrustAnchorsForHostName001) {
  const string hostname = "example.com";
  vector<string> expected_certs = {"cert1", "cert2"};

  NetStack_Certificates mock_certs;
  mock_certs.length = expected_certs.size();
  mock_certs.content = new char *[mock_certs.length];
  for (size_t i = 0; i < mock_certs.length; i++) {
    mock_certs.content[i] = new char[expected_certs[i].size() + 1];
    errno_t res = strcpy_s(mock_certs.content[i], sizeof(mock_certs.content[i]), expected_certs[i].c_str());
    if (res != EOK) {
      return;
    }
  }

  EXPECT_CALL(*g_mockNetStack, Mock_OH_NetStack_GetCertificatesForHostName(
                                   StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_certs), Return(0)));

  EXPECT_CALL(*g_mockNetStack, Mock_OH_Netstack_DestroyCertificatesContent(_))
      .Times(1);

  vector<string> actual_certs;
  bool result = adapter.GetTrustAnchorsForHostName(hostname, actual_certs);
  EXPECT_TRUE(result);
  EXPECT_THAT(actual_certs, ContainerEq(expected_certs));

  for (size_t i = 0; i < mock_certs.length; i++) {
    delete[] mock_certs.content[i];
  }
  delete[] mock_certs.content;
}

// Test case 2: API return failure
TEST_F(CertManagerAdapterImplTest, GetTrustAnchorsForHostName002) {
  const string hostname = "invalid.com";

  EXPECT_CALL(*g_mockNetStack, Mock_OH_NetStack_GetCertificatesForHostName(
                                   StrEq(hostname.c_str()), _))
      .WillOnce(Return(-1));

  EXPECT_CALL(*g_mockNetStack, Mock_OH_Netstack_DestroyCertificatesContent(_))
      .Times(1);

  vector<string> actual_certs;
  bool result = adapter.GetTrustAnchorsForHostName(hostname, actual_certs);
  EXPECT_FALSE(result);
  EXPECT_TRUE(actual_certs.empty());
}

// Test case 3: Empty certificates returned
TEST_F(CertManagerAdapterImplTest, GetTrustAnchorsForHostName003) {
  const string hostname = "empty.example.com";

  NetStack_Certificates mock_certs;
  mock_certs.length = 0;
  mock_certs.content = nullptr;

  EXPECT_CALL(*g_mockNetStack, Mock_OH_NetStack_GetCertificatesForHostName(
                                   StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_certs), Return(0)));

  EXPECT_CALL(*g_mockNetStack, Mock_OH_Netstack_DestroyCertificatesContent(_))
      .Times(1);

  vector<string> actual_certs;
  bool result = adapter.GetTrustAnchorsForHostName(hostname, actual_certs);

  EXPECT_TRUE(result);
  EXPECT_TRUE(actual_certs.empty());
}

// Test case 4: Memory management test - ensure destruction is called even on
// failure
TEST_F(CertManagerAdapterImplTest, GetTrustAnchorsForHostName004) {
  const string hostname = "test.com";

  NetStack_Certificates mock_certs;
  mock_certs.length = 2;
  mock_certs.content = new char *[mock_certs.length];
  mock_certs.content[0] = new char[10];
  errno_t res = strcpy_s(mock_certs.content[0], sizeof(mock_certs.content[0]), "cert1");
  if (res != EOK) {
    return;
  }

  mock_certs.content[1] = new char[10];
  res = strcpy_s(mock_certs.content[1], sizeof(mock_certs.content[1]), "cert2");
  if (res != EOK) {
    return;
  }

  EXPECT_CALL(*g_mockNetStack, Mock_OH_NetStack_GetCertificatesForHostName(
                                   StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_certs), Return(1)));

  EXPECT_CALL(*g_mockNetStack, Mock_OH_Netstack_DestroyCertificatesContent(_))
      .Times(1);

  vector<string> actual_certs;
  bool result = adapter.GetTrustAnchorsForHostName(hostname, actual_certs);
  EXPECT_FALSE(result);

  delete[] mock_certs.content[0];
  delete[] mock_certs.content[1];
  delete[] mock_certs.content;
}

// Test case 1: Success case with multiple pins
TEST_F(CertManagerAdapterImplTest, GetPinSetForHostName001) {
  const string hostname = "example.com";
  const string pinsString = "pin1;pin2;pin3";
  vector<string> expected_pins = {"pin1", "pin2", "pin3"};

  NetStack_CertificatePinning mock_pining;
  mock_pining.publicKeyHash = strdup(pinsString.c_str());
  if (mock_pining.publicKeyHash == nullptr) {
    return;
  }

  EXPECT_CALL(*g_mockNetStack,
              Mock_OH_NetStack_GetPinSetForHostName(StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_pining), Return(0)));

  vector<string> actual_pins;
  bool result = adapter.GetPinSetForHostName(hostname, actual_pins);

  EXPECT_TRUE(result);
  EXPECT_THAT(actual_pins, ContainerEq(expected_pins));
}

// Test case 2: Success case with single pins
TEST_F(CertManagerAdapterImplTest, GetPinSetForHostName002) {
  const string hostname = "single.example.com";
  const string pinsString = "single_pin";
  vector<string> expected_pins = {"single_pin"};

  NetStack_CertificatePinning mock_pining;
  mock_pining.publicKeyHash = strdup(pinsString.c_str());
  if (mock_pining.publicKeyHash == nullptr) {
    return;
  }

  EXPECT_CALL(*g_mockNetStack,
              Mock_OH_NetStack_GetPinSetForHostName(StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_pining), Return(0)));

  vector<string> actual_pins;
  bool result = adapter.GetPinSetForHostName(hostname, actual_pins);
  EXPECT_TRUE(result);
  EXPECT_THAT(actual_pins, ContainerEq(expected_pins));
}

// Test case 3: Empty publicKeyHash (nullptr)
TEST_F(CertManagerAdapterImplTest, GetPinSetForHostName003) {
  const string hostname = "empty.example.com";
  NetStack_CertificatePinning mock_pining;
  mock_pining.publicKeyHash = nullptr;

  EXPECT_CALL(*g_mockNetStack,
              Mock_OH_NetStack_GetPinSetForHostName(StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_pining), Return(0)));

  vector<string> actual_pins;
  bool result = adapter.GetPinSetForHostName(hostname, actual_pins);
  EXPECT_TRUE(result);
  EXPECT_TRUE(actual_pins.empty());
}

// Test case 4: Empty publicKeyHash (empty string)
TEST_F(CertManagerAdapterImplTest, GetPinSetForHostName004) {
  const string hostname = "empty-string.example.com";
  vector<string> expected_pins = {""};

  NetStack_CertificatePinning mock_pining;
  mock_pining.publicKeyHash = strdup("");
  if (mock_pining.publicKeyHash == nullptr) {
    return;
  }

  EXPECT_CALL(*g_mockNetStack,
              Mock_OH_NetStack_GetPinSetForHostName(StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_pining), Return(0)));

  vector<string> actual_pins;
  bool result = adapter.GetPinSetForHostName(hostname, actual_pins);

  EXPECT_TRUE(result);
  EXPECT_THAT(actual_pins, ContainerEq(expected_pins));
}

// Test case 5: API return failure
TEST_F(CertManagerAdapterImplTest, GetPinSetForHostName005) {
  const string hostname = "failure.example.com";

  EXPECT_CALL(*g_mockNetStack,
              Mock_OH_NetStack_GetPinSetForHostName(StrEq(hostname.c_str()), _))
      .WillOnce(Return(-1));

  vector<string> actual_pins;
  bool result = adapter.GetPinSetForHostName(hostname, actual_pins);

  EXPECT_FALSE(result);
  EXPECT_TRUE(actual_pins.empty());
}

// Test case 6: Pins with various separators
TEST_F(CertManagerAdapterImplTest, GetPinSetForHostName006) {
  const string hostname = "complex.example.com";
  const string pinsString = "pin1;pin2;pin3;pin4";
  vector<string> expected_pins = {"pin1", "pin2", "pin3", "pin4"};

  NetStack_CertificatePinning mock_pining;
  mock_pining.publicKeyHash = strdup(pinsString.c_str());
  if (mock_pining.publicKeyHash == nullptr) {
    return;
  }

  EXPECT_CALL(*g_mockNetStack,
              Mock_OH_NetStack_GetPinSetForHostName(StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_pining), Return(0)));

  vector<string> actual_pins;
  bool result = adapter.GetPinSetForHostName(hostname, actual_pins);

  EXPECT_TRUE(result);
  EXPECT_THAT(actual_pins, ContainerEq(expected_pins));
}

// Test case 7: No semicolon in pins string
TEST_F(CertManagerAdapterImplTest, GetPinSetForHostName007) {
  const string hostname = "nosemicolon.example.com";
  const string pinsString = "justonepin";
  vector<string> expected_pins = {"justonepin"};

  NetStack_CertificatePinning mock_pining;
  mock_pining.publicKeyHash = strdup(pinsString.c_str());
  if (mock_pining.publicKeyHash == nullptr) {
    return;
  }

  EXPECT_CALL(*g_mockNetStack,
              Mock_OH_NetStack_GetPinSetForHostName(StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_pining), Return(0)));

  vector<string> actual_pins;
  bool result = adapter.GetPinSetForHostName(hostname, actual_pins);

  EXPECT_TRUE(result);
  EXPECT_THAT(actual_pins, ContainerEq(expected_pins));
}

// Test case 8: Memory leak test - ensure free called (indirectly)
TEST_F(CertManagerAdapterImplTest, GetPinSetForHostName008) {
  const string hostname = "memory.example.com";
  const string pinsString = "pin1;pin2";

  NetStack_CertificatePinning mock_pining;
  mock_pining.publicKeyHash = strdup(pinsString.c_str());
  if (mock_pining.publicKeyHash == nullptr) {
    return;
  }

  EXPECT_CALL(*g_mockNetStack,
              Mock_OH_NetStack_GetPinSetForHostName(StrEq(hostname.c_str()), _))
      .WillOnce(DoAll(SetArgPointee<1>(mock_pining), Return(0)));

  vector<string> actual_pins;
  bool result = adapter.GetPinSetForHostName(hostname, actual_pins);

  EXPECT_TRUE(result);
  EXPECT_THAT(actual_pins, ElementsAre("pin1", "pin2"));
}