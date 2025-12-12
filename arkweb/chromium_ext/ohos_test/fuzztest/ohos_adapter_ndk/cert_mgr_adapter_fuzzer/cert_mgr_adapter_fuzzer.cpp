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

#include <fuzzer/FuzzedDataProvider.h>
#include <certificate_manager/cm_native_api.h>
#include <huks/native_huks_api.h>
#include <huks/native_huks_external_crypto_api.h>
#include <huks/native_huks_param.h>

#include "cert_mgr_adapter/cert_mgr_adapter_impl.h"

namespace OHOS::NWeb {
namespace cert_mgr_adapter_impl_test {
using OHHuksInitExternalCryptoParamSet =
    struct OH_Huks_Result (*)(OH_Huks_ExternalCryptoParamSet** paramSet);
using OHHuksAddExternalCryptoParams =
    struct OH_Huks_Result (*)(OH_Huks_ExternalCryptoParamSet* paramSet,
                              const OH_Huks_ExternalCryptoParam* params,
                              uint32_t paramCnt);
using OHHuksBuildExternalCryptoParamSet =
    struct OH_Huks_Result (*)(OH_Huks_ExternalCryptoParamSet** paramSet);
using OHHuksFreeExternalCryptoParamSet =
    void (*)(OH_Huks_ExternalCryptoParamSet** paramSet);
using OHCertManagerGetUkeyCertificate =
    int32_t (*)(const OH_CM_Blob* ukeyCertIndex,
                const OH_CM_UkeyInfo* ukeyInfo,
                OH_CM_CredentialDetailList* certificateList);
using OHCertManagerFreeUkeyCertificate =
    void (*)(OH_CM_CredentialDetailList* certificateList);
using OHHuksOpenResource =
    struct OH_Huks_Result (*)(const struct OH_Huks_Blob* resourceId,
                              const OH_Huks_ExternalCryptoParamSet* paramSet);
using OHHuksCloseResource =
    struct OH_Huks_Result (*)(const struct OH_Huks_Blob* resourceId,
                              const OH_Huks_ExternalCryptoParamSet* paramSet);
using OHHuksGetUkeyPinAuthState =
    struct OH_Huks_Result (*)(const struct OH_Huks_Blob* resourceId,
                              const OH_Huks_ExternalCryptoParamSet* paramSet,
                              OH_Huks_ExternalPinAuthState* authState);

  void SetCertManagerGetUkeyCertificate(OHCertManagerGetUkeyCertificate func);
  void SetCertManagerFreeUkeyCertificate(OHCertManagerFreeUkeyCertificate func);
  void SetHuksInitExternalCryptoParamSet(OHHuksInitExternalCryptoParamSet func);
  void SetHuksAddExternalCryptoParams(OHHuksAddExternalCryptoParams func);
  void SetHuksBuildExternalCryptoParamSet(OHHuksBuildExternalCryptoParamSet func);
  void SetHuksFreeExternalCryptoParamSet(OHHuksFreeExternalCryptoParamSet func);
  void SetHuksOpenResource(OHHuksOpenResource func);
  void SetHuksCloseResource(OHHuksCloseResource func);
  void SetHuksGetUkeyPinAuthState(OHHuksGetUkeyPinAuthState func);
}  // namespace cert_mgr_adapter_impl_test

namespace {
OH_Huks_Result g_nextInitResult = {OH_HUKS_SUCCESS};
OH_Huks_Result g_nextAddResult = {OH_HUKS_SUCCESS};
OH_Huks_Result g_nextBuildResult = {OH_HUKS_SUCCESS};
OH_Huks_Result g_nextHuksOpenResourceResult = {OH_HUKS_SUCCESS};
OH_Huks_Result g_nextHuksCloseResourceResult = {OH_HUKS_SUCCESS};
OH_Huks_Result g_nextHuksGetUkeyPinAuthStateResult = {OH_HUKS_SUCCESS};
int32_t g_freeCallCount = 0;
int32_t g_freeUkeyCertCallCount = 0;
}  // namespace

OH_Huks_Result MockInitSuccess(OH_Huks_ExternalCryptoParamSet**) {
  return g_nextInitResult;
}

OH_Huks_Result MockAddSuccess(OH_Huks_ExternalCryptoParamSet*,
                              const OH_Huks_ExternalCryptoParam*,
                              uint32_t) {
  return g_nextAddResult;
}

OH_Huks_Result MockBuildSuccess(OH_Huks_ExternalCryptoParamSet**) {
  return g_nextBuildResult;
}

void MockFreeSuccess(OH_Huks_ExternalCryptoParamSet**) {
  g_freeCallCount++;
}

int32_t MockGetUkeyCertificate(const OH_CM_Blob*,
                               const OH_CM_UkeyInfo*,
                               OH_CM_CredentialDetailList* certificateList) {
  return OH_CM_SUCCESS;
}

void MockFreeUkeyCertificate(OH_CM_CredentialDetailList*) {
  g_freeUkeyCertCallCount++;
}

OH_Huks_Result MockHuksOpenResource(const OH_Huks_Blob*,
                                    const OH_Huks_ExternalCryptoParamSet*) {
  return g_nextHuksOpenResourceResult;
}

OH_Huks_Result MockHuksCloseResource(const OH_Huks_Blob*,
                                     const OH_Huks_ExternalCryptoParamSet*) {
  return g_nextHuksCloseResourceResult;
}

OH_Huks_Result MockHuksGetUkeyPinAuthState(
    const OH_Huks_Blob*,
    const OH_Huks_ExternalCryptoParamSet*,
    OH_Huks_ExternalPinAuthState*) {
  return g_nextHuksGetUkeyPinAuthStateResult;
}

void SetAllFunctionsToValid(FuzzedDataProvider& fdp) {
  std::vector<bool> shouldSetFunctions;
  for (int i = 0; i < 9; i++) {
    shouldSetFunctions.push_back(fdp.ConsumeBool());
  }
  shouldSetFunctions[0]
      ? cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(MockGetUkeyCertificate)
      : cert_mgr_adapter_impl_test::SetCertManagerGetUkeyCertificate(nullptr);
  shouldSetFunctions[1]
      ? cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(MockFreeUkeyCertificate)
      : cert_mgr_adapter_impl_test::SetCertManagerFreeUkeyCertificate(nullptr);
  shouldSetFunctions[2]
      ? cert_mgr_adapter_impl_test::SetHuksInitExternalCryptoParamSet(MockInitSuccess)
      : cert_mgr_adapter_impl_test::SetHuksInitExternalCryptoParamSet(nullptr);
  shouldSetFunctions[3]
      ? cert_mgr_adapter_impl_test::SetHuksAddExternalCryptoParams(MockAddSuccess)
      : cert_mgr_adapter_impl_test::SetHuksAddExternalCryptoParams(nullptr);
  shouldSetFunctions[4]
      ? cert_mgr_adapter_impl_test::SetHuksBuildExternalCryptoParamSet(MockBuildSuccess)
      : cert_mgr_adapter_impl_test::SetHuksBuildExternalCryptoParamSet(nullptr);
  shouldSetFunctions[5]
      ? cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(MockFreeSuccess)
      : cert_mgr_adapter_impl_test::SetHuksFreeExternalCryptoParamSet(nullptr);
  shouldSetFunctions[6]
      ? cert_mgr_adapter_impl_test::SetHuksOpenResource(MockHuksOpenResource)
      : cert_mgr_adapter_impl_test::SetHuksOpenResource(nullptr);
  shouldSetFunctions[7]
      ? cert_mgr_adapter_impl_test::SetHuksCloseResource(MockHuksCloseResource)
      : cert_mgr_adapter_impl_test::SetHuksCloseResource(nullptr);
  shouldSetFunctions[8]
      ? cert_mgr_adapter_impl_test::SetHuksGetUkeyPinAuthState(MockHuksGetUkeyPinAuthState)
      : cert_mgr_adapter_impl_test::SetHuksGetUkeyPinAuthState(nullptr);
}

void SetAllResult(FuzzedDataProvider& fdp) {
  std::vector<bool> shouldSetResult;
  for (int i = 0; i < 6; i++) {
    shouldSetResult.push_back(fdp.ConsumeBool());
  }
  shouldSetResult[0]
      ? g_nextInitResult = {OH_HUKS_SUCCESS}
      : g_nextInitResult = {OH_HUKS_ERR_CODE_PERMISSION_FAIL};
  shouldSetResult[1]
      ? g_nextAddResult = {OH_HUKS_SUCCESS}
      : g_nextAddResult = {OH_HUKS_ERR_CODE_PERMISSION_FAIL};
  shouldSetResult[2]
      ? g_nextBuildResult = {OH_HUKS_SUCCESS}
      : g_nextBuildResult = {OH_HUKS_ERR_CODE_PERMISSION_FAIL};
  shouldSetResult[3]
      ? g_nextHuksOpenResourceResult = {OH_HUKS_SUCCESS}
      : g_nextHuksOpenResourceResult = {OH_HUKS_ERR_CODE_PERMISSION_FAIL};
  shouldSetResult[4]
      ? g_nextHuksCloseResourceResult = {OH_HUKS_SUCCESS}
      : g_nextHuksCloseResourceResult = {OH_HUKS_ERR_CODE_PERMISSION_FAIL};
  shouldSetResult[5]
      ? g_nextHuksGetUkeyPinAuthStateResult = {OH_HUKS_SUCCESS}
      : g_nextHuksGetUkeyPinAuthStateResult = {OH_HUKS_ERR_CODE_PERMISSION_FAIL};
}

bool LLVMFuzzerTestSetup() {
  return true;
}

void FuzzApi(const uint8_t* data, size_t size) {
  FuzzedDataProvider fdp(data, size);
  CertManagerAdapterImpl certMgr;
  certMgr.GetCertMaxSize();
  certMgr.GetAppCertMaxSize();
  certMgr.GetSytemRootCertSum();
  certMgr.GetCertDataBySubject(nullptr, nullptr, 0);
  certMgr.GetSytemRootCertData(0, nullptr);
  certMgr.GetUserRootCertSum();
  certMgr.GetUserRootCertData(0, nullptr);
  certMgr.GetAppCert(nullptr, nullptr, nullptr);
  certMgr.Sign(nullptr, nullptr, 0, nullptr, 0);
  std::vector<std::string> certs;
  std::string hostname = fdp.ConsumeRandomLengthString();
  certMgr.GetTrustAnchorsForHostName(hostname, certs);
  std::vector<std::string> pins;
  certMgr.CertManagerAdapterImpl::GetPinSetForHostName(hostname, pins);
  SetAllFunctionsToValid(fdp);
  SetAllResult(fdp);
  std::vector<uint16_t> algorithms = {0x0401, 0x0501, 0x0601, 0x0804, 0x0805,
                                      0x0806, 0x0403, 0x0503, 0x0603, 0xFFFF};
  uint32_t tmp = fdp.ConsumeIntegralInRange<uint32_t>(0, (algorithms.size() - 1));
  uint16_t algorithm = algorithms[tmp];
  std::string identity = fdp.ConsumeRandomLengthString();
  uint32_t certDataLen =
      fdp.ConsumeIntegralInRange<uint32_t>(0, OH_CM_MAX_LEN_CERTIFICATE_CHAIN);
  std::vector<uint8_t> certData(certDataLen);
  certMgr.GetUkeyCert(identity, certData.data(), &certDataLen);
  certMgr.OpenUKeyRemoteHandle(identity);
  certMgr.CloseUKeyRemoteHandle(identity);
  std::string uri = fdp.ConsumeRandomLengthString();
  bool state = true;
  certMgr.GetUkeyPinAuthState(uri, &state);
  uint32_t signDataLen = fdp.ConsumeIntegralInRange<uint32_t>(0, 1024);
  std::vector<uint8_t> signData(signDataLen);
  uint32_t keySize = fdp.ConsumeIntegral<uint32_t>();
  certMgr.SignUsingHuks(identity, certData.data(), certDataLen, signData.data(),
                        &signDataLen, algorithm, keySize);
}
}  // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size < sizeof(int32_t))) {
    return 0;
  }
  static bool init = OHOS::NWeb::LLVMFuzzerTestSetup();
  if (init) {
    OHOS::NWeb::FuzzApi(data, size);
  }
  return 0;
}
