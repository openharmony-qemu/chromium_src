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
#include <huks/native_huks_api.h>
#include <huks/native_huks_external_crypto_api.h>
#include <huks/native_huks_param.h>
#include <certificate_manager/cm_native_api.h>
#include "third_party/boringssl/src/include/openssl/ssl.h"

using namespace testing;
using namespace std;

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;

namespace OHOS::NWeb {
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

extern "C" {
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
} // extern "C"

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

class MockHuks {
public:
    MOCK_METHOD(OH_Huks_Result, OH_Huks_InitParamSet, (OH_Huks_ParamSet **paramSet));
    MOCK_METHOD(OH_Huks_Result, OH_Huks_AddParams, (OH_Huks_ParamSet *paramSet, 
                                                    const OH_Huks_Param *params, 
                                                    uint32_t paramCount));
    MOCK_METHOD(OH_Huks_Result, OH_Huks_BuildParamSet, (OH_Huks_ParamSet **paramSet));
    MOCK_METHOD(void, OH_Huks_FreeParamSet, (OH_Huks_ParamSet **paramSet));
    MOCK_METHOD(OH_Huks_Result, OH_Huks_InitSession, (const OH_Huks_Blob* keyAlias,
                                                      const OH_Huks_ParamSet* paramSet,
                                                      OH_Huks_Blob* handle,
                                                      OH_Huks_Blob* token));
    MOCK_METHOD(OH_Huks_Result, OH_Huks_FinishSession, (const OH_Huks_Blob* handle,
                                                        const OH_Huks_ParamSet* paramSet,
                                                        const OH_Huks_Blob* inData,
                                                        OH_Huks_Blob* outData));
};

MockHuks* g_mockHuks = nullptr;

extern "C" {
OH_Huks_Result OH_Huks_InitParamSet(OH_Huks_ParamSet **paramSet) {
    return g_mockHuks->OH_Huks_InitParamSet(paramSet);
}

OH_Huks_Result OH_Huks_AddParams(OH_Huks_ParamSet* paramSet,
                                 const OH_Huks_Param* params,
                                 uint32_t paramCount) {
    return g_mockHuks->OH_Huks_AddParams(paramSet, params, paramCount);
}

OH_Huks_Result OH_Huks_BuildParamSet(OH_Huks_ParamSet **paramSet) {
    return g_mockHuks->OH_Huks_BuildParamSet(paramSet);
}

void OH_Huks_FreeParamSet(OH_Huks_ParamSet **paramSet) {
    g_mockHuks->OH_Huks_FreeParamSet(paramSet);
}

OH_Huks_Result OH_Huks_InitSession(const OH_Huks_Blob* keyAlias,
                                   const OH_Huks_ParamSet* paramSet,
                                   OH_Huks_Blob* handle,
                                   OH_Huks_Blob* token) {
    return g_mockHuks->OH_Huks_InitSession(keyAlias, paramSet, handle, token);
}

OH_Huks_Result OH_Huks_FinishSession(const OH_Huks_Blob* handle,
                                     const OH_Huks_ParamSet* paramSet,
                                     const OH_Huks_Blob* inData,
                                     OH_Huks_Blob* outData) {
    return g_mockHuks->OH_Huks_FinishSession(handle, paramSet, inData, outData);
}
} // extern "C"

namespace cert_mgr_adapter_impl_test {
    struct HuksSignatureSpec {
        uint32_t algorithm;
        uint32_t padding;
        uint32_t digest;
    };

    using OHHuksInitExternalCryptoParamSet = struct OH_Huks_Result (*)(OH_Huks_ExternalCryptoParamSet **paramSet);
    using OHHuksAddExternalCryptoParams = struct OH_Huks_Result (*)(OH_Huks_ExternalCryptoParamSet *paramSet,
        const OH_Huks_ExternalCryptoParam *params, uint32_t paramCnt);
    using OHHuksBuildExternalCryptoParamSet = struct OH_Huks_Result (*)(OH_Huks_ExternalCryptoParamSet **paramSet);
    using OHHuksFreeExternalCryptoParamSet = void (*)(OH_Huks_ExternalCryptoParamSet **paramSet);
    using OHCertManagerGetUkeyCertificate = int32_t (*)(const OH_CM_Blob *ukeyCertIndex,
        const OH_CM_UkeyInfo *ukeyInfo, OH_CM_CredentialDetailList *certificateList);
    using OHCertManagerFreeUkeyCertificate = void (*)(OH_CM_CredentialDetailList *certificateList);
    using OHHuksOpenResource = struct OH_Huks_Result (*)(const struct OH_Huks_Blob *resourceId,
                                                         const OH_Huks_ExternalCryptoParamSet *paramSet);
    using OHHuksCloseResource = struct OH_Huks_Result (*)(const struct OH_Huks_Blob *resourceId,
                                                          const OH_Huks_ExternalCryptoParamSet *paramSet);
    using OHHuksGetUkeyPinAuthState = struct OH_Huks_Result (*)(const struct OH_Huks_Blob *resourceId,
                                                                const OH_Huks_ExternalCryptoParamSet *paramSet,
                                                                OH_Huks_ExternalPinAuthState *authState);

    OHCertManagerGetUkeyCertificate GetCertManagerGetUkeyCertificate();
    OHCertManagerFreeUkeyCertificate GetCertManagerFreeUkeyCertificate();
    OHHuksInitExternalCryptoParamSet GetHuksInitExternalCryptoParamSet();
    OHHuksAddExternalCryptoParams GetHuksAddExternalCryptoParams();
    OHHuksBuildExternalCryptoParamSet GetHuksBuildExternalCryptoParamSet();
    OHHuksFreeExternalCryptoParamSet GetHuksFreeExternalCryptoParamSet();
    OHHuksOpenResource GetHuksOpenResource();
    OHHuksCloseResource GetHuksCloseResource();
    OHHuksGetUkeyPinAuthState GetHuksGetUkeyPinAuthState();

    void SetCertManagerGetUkeyCertificate(OHCertManagerGetUkeyCertificate func);
    void SetCertManagerFreeUkeyCertificate(OHCertManagerFreeUkeyCertificate func);
    void SetHuksInitExternalCryptoParamSet(OHHuksInitExternalCryptoParamSet func);
    void SetHuksAddExternalCryptoParams(OHHuksAddExternalCryptoParams func);
    void SetHuksBuildExternalCryptoParamSet(OHHuksBuildExternalCryptoParamSet func);
    void SetHuksFreeExternalCryptoParamSet(OHHuksFreeExternalCryptoParamSet func);
    void SetHuksOpenResource(OHHuksOpenResource func);
    void SetHuksCloseResource(OHHuksCloseResource func);
    void SetHuksGetUkeyPinAuthState(OHHuksGetUkeyPinAuthState func);

    OH_Huks_Result InitParamSet(struct OH_Huks_ParamSet** paramSet,
                                const struct OH_Huks_Param* params,
                                uint32_t paramCount);
    OH_Huks_Result InitExtParamSet(OH_Huks_ExternalCryptoParamSet** paramSet,
                                const OH_Huks_ExternalCryptoParam* params,
                                uint32_t paramCount);
    bool GetHuksSignatureSpec(uint16_t algorithm, HuksSignatureSpec& result);
} // namespace cert_mgr_adapter_impl_test

namespace {
    int g_initCallCount = 0;
    int g_addCallCount = 0;
    int g_buildCallCount = 0;
    int g_freeCallCount = 0;
    OH_Huks_Result g_nextInitResult = {OH_HUKS_SUCCESS};
    OH_Huks_Result g_nextAddResult = {OH_HUKS_SUCCESS};
    OH_Huks_Result g_nextBuildResult = {OH_HUKS_SUCCESS};
    OH_Huks_Result g_nextHuksOpenResourceResult = {OH_HUKS_SUCCESS};
    OH_Huks_Result g_nextHuksCloseResourceResult = {OH_HUKS_SUCCESS};
    OH_Huks_Result g_nextHuksGetUkeyPinAuthStateResult = {OH_HUKS_SUCCESS};

    int g_getUkeyCertCallCount = 0;
    int g_freeUkeyCertCallCount = 0;
    int32_t g_nextGetUkeyCertResult = OH_CM_SUCCESS;
    bool g_shouldMockMemcpyFailure = false;
    bool g_mockEmptyCredList = false;
    bool g_mockNullCred = false;
    bool g_mockZeroSize = false;
    bool g_mockNullData = false;
    int g_huksOpenResourceCallCount = 0;
    int g_huksCloseResourceCallCount = 0;
    int g_huksGetUkeyPinAuthStateCallCount = 0;
    
    void ResetTestState() {
        g_initCallCount = 0;
        g_addCallCount = 0;
        g_buildCallCount = 0;
        g_freeCallCount = 0;
        g_nextInitResult.errorCode = OH_HUKS_SUCCESS;
        g_nextAddResult.errorCode = OH_HUKS_SUCCESS;
        g_nextBuildResult.errorCode = OH_HUKS_SUCCESS;
        g_nextHuksOpenResourceResult.errorCode = OH_HUKS_SUCCESS;
        g_nextHuksCloseResourceResult.errorCode = OH_HUKS_SUCCESS;
        g_nextHuksGetUkeyPinAuthStateResult.errorCode = OH_HUKS_SUCCESS;

        g_getUkeyCertCallCount = 0;
        g_freeUkeyCertCallCount = 0;
        g_nextGetUkeyCertResult = OH_CM_SUCCESS;
        g_shouldMockMemcpyFailure = false;
        g_mockEmptyCredList = false;
        g_mockNullCred = false;
        g_mockZeroSize = false;
        g_mockNullData = false;
        g_huksOpenResourceCallCount = 0;
        g_huksCloseResourceCallCount = 0;
        g_huksGetUkeyPinAuthStateCallCount = 0;
    }
}

OH_Huks_Result MockInitSuccess(OH_Huks_ExternalCryptoParamSet**) {
    g_initCallCount++;
    return g_nextInitResult;
}

OH_Huks_Result MockAddSuccess(OH_Huks_ExternalCryptoParamSet*, const OH_Huks_ExternalCryptoParam*, uint32_t) {
    g_addCallCount++;
    return g_nextAddResult;
}

OH_Huks_Result MockBuildSuccess(OH_Huks_ExternalCryptoParamSet**) {
    g_buildCallCount++;
    return g_nextBuildResult;
}

void MockFreeSuccess(OH_Huks_ExternalCryptoParamSet**) {
    g_freeCallCount++;
}

namespace TestData {
    static uint8_t dummyCertData[100];

    static OH_CM_Credential validCredential = {
        1, "test_type", "test_alias", "test_key_uri",
        1, 1, {100, dummyCertData}, OH_CM_CERT_PURPOSE_SIGN};

    static OH_CM_Credential zeroSizeCredential = {
        1, "test_type", "test_alias", "test_key_uri",
        1, 1, {0, dummyCertData}, OH_CM_CERT_PURPOSE_SIGN};

    static OH_CM_Credential nullDataCredential = {
        1, "test_type", "test_alias", "test_key_uri",
        1, 1, {100, nullptr}, OH_CM_CERT_PURPOSE_SIGN};

    static OH_CM_Credential validCredArray[1] = {validCredential};
    static OH_CM_Credential zeroSizeCredArray[1] = {zeroSizeCredential};
    static OH_CM_Credential nullDataCredArray[1] = {nullDataCredential};

    static OH_CM_CredentialDetailList validCredList = {1, validCredArray};
    static OH_CM_CredentialDetailList zeroSizeCredList = {1, zeroSizeCredArray};
    static OH_CM_CredentialDetailList nullDataCredList = {1, nullDataCredArray};
    static OH_CM_CredentialDetailList emptyCredList = {0, nullptr};
    static OH_CM_CredentialDetailList nullCredList = {1, nullptr};
}  // namespace TestData

int32_t MockGetUkeyCertificate(const OH_CM_Blob*,
                               const OH_CM_UkeyInfo*,
                               OH_CM_CredentialDetailList* certificateList) {
    g_getUkeyCertCallCount++;
    
    if (g_nextGetUkeyCertResult != OH_CM_SUCCESS) {
        return g_nextGetUkeyCertResult;
    }
    
    if (certificateList != nullptr) {
        if (g_mockEmptyCredList) {
            *certificateList = TestData::emptyCredList;
        } else if (g_mockNullCred) {
            *certificateList = TestData::nullCredList;
        } else if (g_mockZeroSize) {
            *certificateList = TestData::zeroSizeCredList;
        } else if (g_mockNullData) {
            *certificateList = TestData::nullDataCredList;
        } else {
            *certificateList = TestData::validCredList;
        }
    }
    
    return OH_CM_SUCCESS;
}

void MockFreeUkeyCertificate(OH_CM_CredentialDetailList *) {
    g_freeUkeyCertCallCount++;
}

OH_Huks_Result MockHuksOpenResource(const OH_Huks_Blob *, const OH_Huks_ExternalCryptoParamSet *) {
    g_huksOpenResourceCallCount++;
    return g_nextHuksOpenResourceResult;
}

OH_Huks_Result MockHuksCloseResource(const OH_Huks_Blob *, const OH_Huks_ExternalCryptoParamSet *) {
    g_huksCloseResourceCallCount++;
    return g_nextHuksCloseResourceResult;
}

OH_Huks_Result MockHuksGetUkeyPinAuthState(const OH_Huks_Blob*,
                                           const OH_Huks_ExternalCryptoParamSet*,
                                           OH_Huks_ExternalPinAuthState*) {
    g_huksGetUkeyPinAuthStateCallCount++;
    return g_nextHuksGetUkeyPinAuthStateResult;
}

class CertMgrAdapterImplTest : public ::testing::Test {
protected:
    void SetUp() override {
        g_mockHuks = &mockHuks_;

        ResetTestState();
        originalInit = cert_mgr_adapter_impl_test::GetHuksInitExternalCryptoParamSet();
        originalAdd = cert_mgr_adapter_impl_test::GetHuksAddExternalCryptoParams();
        originalBuild = cert_mgr_adapter_impl_test::GetHuksBuildExternalCryptoParamSet();
        originalFree = cert_mgr_adapter_impl_test::GetHuksFreeExternalCryptoParamSet();
        originalGetUkeyCert = cert_mgr_adapter_impl_test::GetCertManagerGetUkeyCertificate();
        originalFreeUkeyCert = cert_mgr_adapter_impl_test::GetCertManagerFreeUkeyCertificate();
        originalHuksOpenResource = cert_mgr_adapter_impl_test::GetHuksOpenResource();
        originalHuksCloseResource = cert_mgr_adapter_impl_test::GetHuksCloseResource();
        originalHuksGetUkeyPinAuthState = cert_mgr_adapter_impl_test::GetHuksGetUkeyPinAuthState();
    }

    void TearDown() override {
        g_mockHuks = nullptr;

        cert_mgr_adapter_impl_test::SetHuksInitExternalCryptoParamSet(originalInit);
        cert_mgr_adapter_impl_test::SetHuksAddExternalCryptoParams(originalAdd);
        cert_mgr_adapter_impl_test::SetHuksBuildExternalCryptoParamSet(originalBuild);
        cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(originalFree);
        cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(originalGetUkeyCert);
        cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(originalFreeUkeyCert);
        cert_mgr_adapter_impl_test::SetHuksOpenResource(originalHuksOpenResource);
        cert_mgr_adapter_impl_test::SetHuksCloseResource(originalHuksCloseResource);
        cert_mgr_adapter_impl_test::SetHuksGetUkeyPinAuthState(originalHuksGetUkeyPinAuthState);
    }

    void SetAllFunctionsToValid() {
        cert_mgr_adapter_impl_test::SetHuksInitExternalCryptoParamSet(MockInitSuccess);
        cert_mgr_adapter_impl_test::SetHuksAddExternalCryptoParams(MockAddSuccess);
        cert_mgr_adapter_impl_test::SetHuksBuildExternalCryptoParamSet(MockBuildSuccess);
        cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    }

    MockHuks mockHuks_;

private:
    cert_mgr_adapter_impl_test::OHHuksInitExternalCryptoParamSet originalInit = nullptr;
    cert_mgr_adapter_impl_test::OHHuksAddExternalCryptoParams originalAdd = nullptr;
    cert_mgr_adapter_impl_test::OHHuksBuildExternalCryptoParamSet originalBuild = nullptr;
    cert_mgr_adapter_impl_test::OHHuksFreeExternalCryptoParamSet originalFree = nullptr;
    cert_mgr_adapter_impl_test::OHCertManagerGetUkeyCertificate originalGetUkeyCert = nullptr;
    cert_mgr_adapter_impl_test::OHCertManagerFreeUkeyCertificate originalFreeUkeyCert = nullptr;
    cert_mgr_adapter_impl_test::OHHuksOpenResource originalHuksOpenResource = nullptr;
    cert_mgr_adapter_impl_test::OHHuksCloseResource originalHuksCloseResource = nullptr;
    cert_mgr_adapter_impl_test::OHHuksGetUkeyPinAuthState originalHuksGetUkeyPinAuthState =nullptr;
};

TEST_F(CertMgrAdapterImplTest, InitParamSet_001) {
    OH_Huks_ParamSet* paramSet = nullptr;
    OH_Huks_Param params[2] = {
        {.tag = OH_HUKS_TAG_ALGORITHM, .uint32Param = OH_HUKS_ALG_RSA},
        {.tag = OH_HUKS_TAG_KEY_SIZE, .uint32Param = 2048}
    };
    uint32_t paramCount = 2;

    EXPECT_CALL(mockHuks_, OH_Huks_InitParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_AddParams(::testing::_, ::testing::_, paramCount))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_BuildParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_FreeParamSet(::testing::_)).Times(0);

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, OH_HUKS_SUCCESS);
}

TEST_F(CertMgrAdapterImplTest, InitParamSet_002) {
    OH_Huks_ParamSet* paramSet = nullptr;
    OH_Huks_Param params[1] = {{.tag = OH_HUKS_TAG_ALGORITHM, .uint32Param = OH_HUKS_ALG_RSA}};
    uint32_t paramCount = 1;

    EXPECT_CALL(mockHuks_, OH_Huks_InitParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_AddParams(::testing::_, ::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(mockHuks_, OH_Huks_BuildParamSet(::testing::_)).Times(0);
    EXPECT_CALL(mockHuks_, OH_Huks_FreeParamSet(::testing::_)).Times(0);

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT);
}

TEST_F(CertMgrAdapterImplTest, InitParamSet_003) {
    OH_Huks_ParamSet* paramSet = nullptr;
    OH_Huks_Param params[1] = {{.tag = OH_HUKS_TAG_ALGORITHM, .uint32Param = OH_HUKS_ALG_RSA}};
    uint32_t paramCount = 1;

    EXPECT_CALL(mockHuks_, OH_Huks_InitParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_AddParams(::testing::_, ::testing::_, paramCount))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_FreeParamSet(::testing::_))
        .WillOnce(::testing::Return());
    EXPECT_CALL(mockHuks_, OH_Huks_BuildParamSet(::testing::_)).Times(0);

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT);
}

TEST_F(CertMgrAdapterImplTest, InitParamSet_004) {
    OH_Huks_ParamSet* paramSet = nullptr;
    OH_Huks_Param params[1] = {{.tag = OH_HUKS_TAG_ALGORITHM, .uint32Param = OH_HUKS_ALG_RSA}};
    uint32_t paramCount = 1;

    EXPECT_CALL(mockHuks_, OH_Huks_InitParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_AddParams(::testing::_, ::testing::_, paramCount))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_BuildParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_FreeParamSet(::testing::_))
        .WillOnce(::testing::Return());

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT);
}

TEST_F(CertMgrAdapterImplTest, InitExtParamSet_001) {
    cert_mgr_adapter_impl_test::SetHuksInitExternalCryptoParamSet(nullptr);
    cert_mgr_adapter_impl_test::SetHuksAddExternalCryptoParams(MockAddSuccess);
    cert_mgr_adapter_impl_test::SetHuksBuildExternalCryptoParamSet(MockBuildSuccess);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam params[1];
    uint32_t paramCount = 1;

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitExtParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, -1);
    EXPECT_EQ(g_initCallCount, 0);
    EXPECT_EQ(g_addCallCount, 0);
    EXPECT_EQ(g_buildCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, InitExtParamSet_002) {
    cert_mgr_adapter_impl_test::SetHuksInitExternalCryptoParamSet(MockInitSuccess);
    cert_mgr_adapter_impl_test::SetHuksAddExternalCryptoParams(nullptr);
    cert_mgr_adapter_impl_test::SetHuksBuildExternalCryptoParamSet(MockBuildSuccess);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam params[1];
    uint32_t paramCount = 1;

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitExtParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, -1);
    EXPECT_EQ(g_initCallCount, 0);
    EXPECT_EQ(g_addCallCount, 0);
    EXPECT_EQ(g_buildCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, InitExtParamSet_003) {
    cert_mgr_adapter_impl_test::SetHuksInitExternalCryptoParamSet(MockInitSuccess);
    cert_mgr_adapter_impl_test::SetHuksAddExternalCryptoParams(MockAddSuccess);
    cert_mgr_adapter_impl_test::SetHuksBuildExternalCryptoParamSet(nullptr);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam params[1];
    uint32_t paramCount = 1;

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitExtParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, -1);
    EXPECT_EQ(g_initCallCount, 0);
    EXPECT_EQ(g_addCallCount, 0);
    EXPECT_EQ(g_buildCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, InitExtParamSet_004) {
    cert_mgr_adapter_impl_test::SetHuksInitExternalCryptoParamSet(MockInitSuccess);
    cert_mgr_adapter_impl_test::SetHuksAddExternalCryptoParams(MockAddSuccess);
    cert_mgr_adapter_impl_test::SetHuksBuildExternalCryptoParamSet(MockBuildSuccess);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(nullptr);
    
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam params[1];
    uint32_t paramCount = 1;

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitExtParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, -1);
    EXPECT_EQ(g_initCallCount, 0);
    EXPECT_EQ(g_addCallCount, 0);
    EXPECT_EQ(g_buildCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, InitExtParamSet_005) {
    SetAllFunctionsToValid();
    g_nextInitResult.errorCode = -1;
    
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam params[1];
    uint32_t paramCount = 1;

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitExtParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, -1);
    EXPECT_EQ(g_initCallCount, 1);
    EXPECT_EQ(g_addCallCount, 0);
    EXPECT_EQ(g_buildCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, InitExtParamSet_006) {
    SetAllFunctionsToValid();
    g_nextAddResult.errorCode = -1;
    
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam params[1];
    uint32_t paramCount = 1;

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitExtParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, -1);
    EXPECT_EQ(g_initCallCount, 1);
    EXPECT_EQ(g_addCallCount, 1);
    EXPECT_EQ(g_buildCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, InitExtParamSet_007) {
    SetAllFunctionsToValid();
    g_nextBuildResult.errorCode = -1;
    
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam params[1];
    uint32_t paramCount = 1;

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitExtParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, -1);
    EXPECT_EQ(g_initCallCount, 1);
    EXPECT_EQ(g_addCallCount, 1);
    EXPECT_EQ(g_buildCallCount, 1);
    EXPECT_EQ(g_freeCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, InitExtParamSet_008) {
    SetAllFunctionsToValid();
    
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam params[1];
    uint32_t paramCount = 1;

    OH_Huks_Result result = cert_mgr_adapter_impl_test::InitExtParamSet(&paramSet, params, paramCount);
    EXPECT_EQ(result.errorCode, OH_HUKS_SUCCESS);
    EXPECT_EQ(g_initCallCount, 1);
    EXPECT_EQ(g_addCallCount, 1);
    EXPECT_EQ(g_buildCallCount, 1);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_001) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(SSL_SIGN_RSA_PKCS1_SHA256, result);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(result.algorithm, OH_HUKS_ALG_RSA);
    EXPECT_EQ(result.padding, OH_HUKS_PADDING_PKCS1_V1_5);
    EXPECT_EQ(result.digest, OH_HUKS_DIGEST_SHA256);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_002) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(SSL_SIGN_RSA_PKCS1_SHA384, result);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(result.algorithm, OH_HUKS_ALG_RSA);
    EXPECT_EQ(result.padding, OH_HUKS_PADDING_PKCS1_V1_5);
    EXPECT_EQ(result.digest, OH_HUKS_DIGEST_SHA384);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_003) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(SSL_SIGN_RSA_PKCS1_SHA512, result);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(result.algorithm, OH_HUKS_ALG_RSA);
    EXPECT_EQ(result.padding, OH_HUKS_PADDING_PKCS1_V1_5);
    EXPECT_EQ(result.digest, OH_HUKS_DIGEST_SHA512);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_004) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(SSL_SIGN_RSA_PSS_RSAE_SHA256, result);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(result.algorithm, OH_HUKS_ALG_RSA);
    EXPECT_EQ(result.padding, OH_HUKS_PADDING_PSS);
    EXPECT_EQ(result.digest, OH_HUKS_DIGEST_SHA256);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_005) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(SSL_SIGN_RSA_PSS_RSAE_SHA384, result);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(result.algorithm, OH_HUKS_ALG_RSA);
    EXPECT_EQ(result.padding, OH_HUKS_PADDING_PSS);
    EXPECT_EQ(result.digest, OH_HUKS_DIGEST_SHA384);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_006) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(SSL_SIGN_RSA_PSS_RSAE_SHA512, result);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(result.algorithm, OH_HUKS_ALG_RSA);
    EXPECT_EQ(result.padding, OH_HUKS_PADDING_PSS);
    EXPECT_EQ(result.digest, OH_HUKS_DIGEST_SHA512);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_007) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(SSL_SIGN_ECDSA_SECP256R1_SHA256, result);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(result.algorithm, OH_HUKS_ALG_ECC);
    EXPECT_EQ(result.padding, OH_HUKS_PADDING_NONE);
    EXPECT_EQ(result.digest, OH_HUKS_DIGEST_SHA256);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_008) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(SSL_SIGN_ECDSA_SECP384R1_SHA384, result);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(result.algorithm, OH_HUKS_ALG_ECC);
    EXPECT_EQ(result.padding, OH_HUKS_PADDING_NONE);
    EXPECT_EQ(result.digest, OH_HUKS_DIGEST_SHA384);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_009) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(SSL_SIGN_ECDSA_SECP521R1_SHA512, result);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(result.algorithm, OH_HUKS_ALG_ECC);
    EXPECT_EQ(result.padding, OH_HUKS_PADDING_NONE);
    EXPECT_EQ(result.digest, OH_HUKS_DIGEST_SHA512);
}

TEST_F(CertMgrAdapterImplTest, GetHuksSignatureSpec_010) {
    cert_mgr_adapter_impl_test::HuksSignatureSpec result;
    bool success = cert_mgr_adapter_impl_test::GetHuksSignatureSpec(0xFFFF, result);
    EXPECT_FALSE(success);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyCert_001) {
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[OH_CM_MAX_LEN_CERTIFICATE_CHAIN];
    uint32_t certDataLen = 0;

    cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(nullptr);
    cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(MockFreeUkeyCertificate);
    int32_t result = adapter.GetUkeyCert(identity, certData, &certDataLen);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_getUkeyCertCallCount, 0);
    EXPECT_EQ(g_freeUkeyCertCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyCert_002) {
    cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(MockGetUkeyCertificate);
    cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(nullptr);
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[OH_CM_MAX_LEN_CERTIFICATE_CHAIN];
    uint32_t certDataLen = 0;
    
    int32_t result = adapter.GetUkeyCert(identity, certData, &certDataLen);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_getUkeyCertCallCount, 0);
    EXPECT_EQ(g_freeUkeyCertCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyCert_003) {
    cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(MockGetUkeyCertificate);
    cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(MockFreeUkeyCertificate);
    g_nextGetUkeyCertResult = -1;
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[OH_CM_MAX_LEN_CERTIFICATE_CHAIN];
    uint32_t certDataLen = 0;
    
    int32_t result = adapter.GetUkeyCert(identity, certData, &certDataLen);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_getUkeyCertCallCount, 1);
    EXPECT_EQ(g_freeUkeyCertCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyCert_004) {
    cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(MockGetUkeyCertificate);
    cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(MockFreeUkeyCertificate);
    g_mockEmptyCredList = true;
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[OH_CM_MAX_LEN_CERTIFICATE_CHAIN];
    uint32_t certDataLen = 0;
    
    int32_t result = adapter.GetUkeyCert(identity, certData, &certDataLen);
    EXPECT_EQ(result, OH_CM_SUCCESS);
    EXPECT_EQ(g_getUkeyCertCallCount, 1);
    EXPECT_EQ(g_freeUkeyCertCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyCert_005) {
    cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(MockGetUkeyCertificate);
    cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(MockFreeUkeyCertificate);
    g_mockNullCred = true;
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[OH_CM_MAX_LEN_CERTIFICATE_CHAIN];
    uint32_t certDataLen = 0;
    
    int32_t result = adapter.GetUkeyCert(identity, certData, &certDataLen);
    EXPECT_EQ(result, OH_CM_SUCCESS);
    EXPECT_EQ(g_getUkeyCertCallCount, 1);
    EXPECT_EQ(g_freeUkeyCertCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyCert_006) {
    cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(MockGetUkeyCertificate);
    cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(MockFreeUkeyCertificate);
    g_mockZeroSize = true;
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[OH_CM_MAX_LEN_CERTIFICATE_CHAIN];
    uint32_t certDataLen = 0;
    
    int32_t result = adapter.GetUkeyCert(identity, certData, &certDataLen);
    EXPECT_EQ(result, OH_CM_SUCCESS);
    EXPECT_EQ(g_getUkeyCertCallCount, 1);
    EXPECT_EQ(g_freeUkeyCertCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyCert_007) {
    cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(MockGetUkeyCertificate);
    cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(MockFreeUkeyCertificate);
    g_mockNullData = true;
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[OH_CM_MAX_LEN_CERTIFICATE_CHAIN];
    uint32_t certDataLen = 0;
    
    int32_t result = adapter.GetUkeyCert(identity, certData, &certDataLen);
    EXPECT_EQ(result, OH_CM_SUCCESS);
    EXPECT_EQ(g_getUkeyCertCallCount, 1);
    EXPECT_EQ(g_freeUkeyCertCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyCert_008) {
    cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(MockGetUkeyCertificate);
    cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(MockFreeUkeyCertificate);
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[OH_CM_MAX_LEN_CERTIFICATE_CHAIN];
    uint32_t certDataLen = 0;
    
    int32_t result = adapter.GetUkeyCert(identity, certData, &certDataLen);
    EXPECT_EQ(result, OH_CM_SUCCESS);
    EXPECT_EQ(g_getUkeyCertCallCount, 1);
    EXPECT_EQ(g_freeUkeyCertCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, OpenUKeyRemoteHandle_001) {
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    cert_mgr_adapter_impl_test::SetHuksOpenResource(nullptr);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    int32_t result = adapter.OpenUKeyRemoteHandle(identity);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksOpenResourceCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, OpenUKeyRemoteHandle_002) {
    cert_mgr_adapter_impl_test::SetHuksOpenResource(MockHuksOpenResource);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(nullptr);
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    
    int32_t result = adapter.OpenUKeyRemoteHandle(identity);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksOpenResourceCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, OpenUKeyRemoteHandle_003) {
    cert_mgr_adapter_impl_test::SetHuksOpenResource(MockHuksOpenResource);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    
    int32_t result = adapter.OpenUKeyRemoteHandle(identity);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksOpenResourceCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, OpenUKeyRemoteHandle_004) {
    cert_mgr_adapter_impl_test::SetHuksOpenResource(MockHuksOpenResource);
    SetAllFunctionsToValid();
    g_nextHuksOpenResourceResult.errorCode = -1;
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    
    int32_t result = adapter.OpenUKeyRemoteHandle(identity);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksOpenResourceCallCount, 1);
    EXPECT_EQ(g_freeCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, OpenUKeyRemoteHandle_005) {
    cert_mgr_adapter_impl_test::SetHuksOpenResource(MockHuksOpenResource);
    SetAllFunctionsToValid();
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    
    int32_t result = adapter.OpenUKeyRemoteHandle(identity);
    EXPECT_EQ(result, OH_HUKS_SUCCESS);
    EXPECT_EQ(g_huksOpenResourceCallCount, 1);
    EXPECT_EQ(g_freeCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, CloseUKeyRemoteHandle_001) {
    cert_mgr_adapter_impl_test::SetHuksCloseResource(nullptr);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    
    int32_t result = adapter.CloseUKeyRemoteHandle(identity);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksCloseResourceCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, CloseUKeyRemoteHandle_002) {
    cert_mgr_adapter_impl_test::SetHuksCloseResource(MockHuksCloseResource);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(nullptr);
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    
    int32_t result = adapter.CloseUKeyRemoteHandle(identity);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksCloseResourceCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, CloseUKeyRemoteHandle_003) {
    cert_mgr_adapter_impl_test::SetHuksCloseResource(MockHuksCloseResource);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    
    int32_t result = adapter.CloseUKeyRemoteHandle(identity);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksCloseResourceCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, CloseUKeyRemoteHandle_004) {
    cert_mgr_adapter_impl_test::SetHuksCloseResource(MockHuksCloseResource);
    SetAllFunctionsToValid();
    g_nextHuksCloseResourceResult.errorCode = -1;
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    
    int32_t result = adapter.CloseUKeyRemoteHandle(identity);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksCloseResourceCallCount, 1);
    EXPECT_EQ(g_freeCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, CloseUKeyRemoteHandle_005) {
    cert_mgr_adapter_impl_test::SetHuksCloseResource(MockHuksCloseResource);
    SetAllFunctionsToValid();
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    
    int32_t result = adapter.CloseUKeyRemoteHandle(identity);
    EXPECT_EQ(result, OH_HUKS_SUCCESS);
    EXPECT_EQ(g_huksCloseResourceCallCount, 1);
    EXPECT_EQ(g_freeCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyPinAuthState_001) {
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    bool state = true;
    cert_mgr_adapter_impl_test::SetHuksGetUkeyPinAuthState(nullptr);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    int32_t result = adapter.GetUkeyPinAuthState(identity, &state);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksGetUkeyPinAuthStateCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyPinAuthState_002) {
    cert_mgr_adapter_impl_test::SetHuksGetUkeyPinAuthState(MockHuksGetUkeyPinAuthState);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(nullptr);
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    bool state = true;
    int32_t result = adapter.GetUkeyPinAuthState(identity, &state);
    
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksGetUkeyPinAuthStateCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyPinAuthState_003) {
    cert_mgr_adapter_impl_test::SetHuksGetUkeyPinAuthState(MockHuksGetUkeyPinAuthState);
    cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess);
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    bool state = true;
    int32_t result = adapter.GetUkeyPinAuthState(identity, &state);
    
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksGetUkeyPinAuthStateCallCount, 0);
    EXPECT_EQ(g_freeCallCount, 0);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyPinAuthState_004) {
    cert_mgr_adapter_impl_test::SetHuksGetUkeyPinAuthState(MockHuksGetUkeyPinAuthState);
    SetAllFunctionsToValid();
    g_nextHuksGetUkeyPinAuthStateResult.errorCode = -1;
    
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    bool state = true;
    int32_t result = adapter.GetUkeyPinAuthState(identity, &state);
    
    EXPECT_EQ(result, -1);
    EXPECT_EQ(g_huksGetUkeyPinAuthStateCallCount, 1);
    EXPECT_EQ(g_freeCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, GetUkeyPinAuthState_005) {
    cert_mgr_adapter_impl_test::SetHuksGetUkeyPinAuthState(MockHuksGetUkeyPinAuthState);
    SetAllFunctionsToValid();
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    bool state = true;
    int32_t result = adapter.GetUkeyPinAuthState(identity, &state);
    
    EXPECT_EQ(result, OH_HUKS_SUCCESS);
    EXPECT_EQ(g_huksGetUkeyPinAuthStateCallCount, 1);
    EXPECT_EQ(g_freeCallCount, 1);
}

TEST_F(CertMgrAdapterImplTest, SignUsingHuks_001) {
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[100] = {0};
    uint32_t certDataLen = 100;
    uint8_t signData[128] = {0};
    uint32_t signDataLen = 128;
    uint16_t algorithm = 0xFFFF;
    uint32_t keySize = 2048;

    int32_t result =
        adapter.SignUsingHuks(identity, certData, certDataLen, signData,
                              &signDataLen, algorithm, keySize);
    EXPECT_EQ(result, -1);
}

TEST_F(CertMgrAdapterImplTest, SignUsingHuks_002) {
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[100] = {0};
    uint32_t certDataLen = 100;
    uint8_t signData[128] = {0};
    uint32_t signDataLen = 128;
    uint16_t algorithm = SSL_SIGN_RSA_PKCS1_SHA256;
    uint32_t keySize = 2048;
    
    EXPECT_CALL(mockHuks_, OH_Huks_InitParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT, 0}));
    
    EXPECT_CALL(mockHuks_, OH_Huks_AddParams(::testing::_, ::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(mockHuks_, OH_Huks_BuildParamSet(::testing::_)).Times(0);
    EXPECT_CALL(mockHuks_, OH_Huks_FreeParamSet(::testing::_)).Times(0);
    int32_t result =
        adapter.SignUsingHuks(identity, certData, certDataLen, signData,
                              &signDataLen, algorithm, keySize);
    EXPECT_EQ(result, OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT);
}

TEST_F(CertMgrAdapterImplTest, SignUsingHuks_003) {
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[100] = {0};
    uint32_t certDataLen = 100;
    uint8_t signData[128] = {0};
    uint32_t signDataLen = 128;
    uint16_t algorithm = SSL_SIGN_RSA_PKCS1_SHA256;
    uint32_t keySize = 2048;
    
    EXPECT_CALL(mockHuks_, OH_Huks_InitParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_AddParams(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_BuildParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_FreeParamSet(::testing::_)).Times(1);
    EXPECT_CALL(mockHuks_, OH_Huks_InitSession(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT, 0}));
    int32_t result =
        adapter.SignUsingHuks(identity, certData, certDataLen, signData,
                              &signDataLen, algorithm, keySize);
    EXPECT_EQ(result, OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT);
}

TEST_F(CertMgrAdapterImplTest, SignUsingHuks_004) {
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[100] = {0};
    uint32_t certDataLen = 100;
    uint8_t signData[128] = {0};
    uint32_t signDataLen = 128;
    uint16_t algorithm = SSL_SIGN_RSA_PKCS1_SHA256;
    uint32_t keySize = 2048;
    
    EXPECT_CALL(mockHuks_, OH_Huks_InitParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_AddParams(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_BuildParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_FreeParamSet(::testing::_)).Times(1);
    EXPECT_CALL(mockHuks_, OH_Huks_InitSession(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_FinishSession(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT, 0}));
    int32_t result =
        adapter.SignUsingHuks(identity, certData, certDataLen, signData,
                              &signDataLen, algorithm, keySize);
    EXPECT_EQ(result, OH_HUKS_ERR_CODE_ILLEGAL_ARGUMENT);
}

TEST_F(CertMgrAdapterImplTest, SignUsingHuks_005) {
    CertManagerAdapterImpl adapter;
    std::string identity = "test_identity";
    uint8_t certData[100] = {0};
    uint32_t certDataLen = 100;
    uint8_t signData[128] = {0};
    uint32_t signDataLen = 128;
    uint16_t algorithm = SSL_SIGN_RSA_PKCS1_SHA256;
    uint32_t keySize = 2048;
    
    EXPECT_CALL(mockHuks_, OH_Huks_InitParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_AddParams(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_BuildParamSet(::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_FreeParamSet(::testing::_)).Times(1);
    EXPECT_CALL(mockHuks_, OH_Huks_InitSession(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    EXPECT_CALL(mockHuks_, OH_Huks_FinishSession(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(OH_Huks_Result{OH_HUKS_SUCCESS, 0}));
    int32_t result =
        adapter.SignUsingHuks(identity, certData, certDataLen, signData,
                              &signDataLen, algorithm, keySize);
    EXPECT_EQ(result, OH_HUKS_SUCCESS);
}
} // namespace OHOS::NWeb