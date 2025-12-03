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

#include <vector>
#include <cstring>
#include <string>
#include <dlfcn.h>
#include <netdb.h>
#include <mutex>
#include <huks/native_huks_api.h>
#include <huks/native_huks_external_crypto_api.h>
#include <huks/native_huks_param.h>
 
#include <certificate_manager/cm_native_api.h>
 
#include "cert_mgr_adapter_impl.h"

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "base/files/file_path.h"
 
#include "third_party/bounds_checking_function/include/securec.h"
#include "third_party/boringssl/src/include/openssl/ssl.h"

#include <network/netstack/net_ssl/net_ssl_c.h>

using namespace OHOS::NWeb;

namespace OHOS::NWeb {

#if BUILDFLAG(ARKWEB_UNITTESTS)
namespace cert_mgr_adapter_impl_test {
#else
namespace {
#endif
 
struct HuksSignatureSpec {
    uint32_t algorithm;
    uint32_t padding;
    uint32_t digest;
};
 
using OHCertManagerGetUkeyCertificate = int32_t (*)(const OH_CM_Blob *ukeyCertIndex,
    const OH_CM_UkeyInfo *ukeyInfo, OH_CM_CredentialDetailList *certificateList);
using OHCertManagerFreeUkeyCertificate = void (*)(OH_CM_CredentialDetailList *certificateList);
 
using OHHuksInitExternalCryptoParamSet = struct OH_Huks_Result (*)(OH_Huks_ExternalCryptoParamSet **paramSet);
using OHHuksAddExternalCryptoParams = struct OH_Huks_Result (*)(OH_Huks_ExternalCryptoParamSet *paramSet,
    const OH_Huks_ExternalCryptoParam *params, uint32_t paramCnt);
using OHHuksBuildExternalCryptoParamSet = struct OH_Huks_Result (*)(OH_Huks_ExternalCryptoParamSet **paramSet);
using OHHuksFreeExternalCryptoParamSet = void (*)(OH_Huks_ExternalCryptoParamSet **paramSet);
 
using OHHuksOpenResource = struct OH_Huks_Result (*)(const struct OH_Huks_Blob *resourceId,
                                                         const OH_Huks_ExternalCryptoParamSet *paramSet);
using OHHuksCloseResource = struct OH_Huks_Result (*)(const struct OH_Huks_Blob *resourceId,
                                                          const OH_Huks_ExternalCryptoParamSet *paramSet);
using OHHuksGetUkeyPinAuthState = struct OH_Huks_Result (*)(const struct OH_Huks_Blob *resourceId,
                                                            const OH_Huks_ExternalCryptoParamSet *paramSet,
                                                            OH_Huks_ExternalPinAuthState *authState);
 
 
OHCertManagerGetUkeyCertificate certManagerGetUkeyCertificate = nullptr;
OHCertManagerFreeUkeyCertificate certManagerFreeUkeyCertificate = nullptr;
OHHuksInitExternalCryptoParamSet huksInitExternalCryptoParamSet = nullptr;
OHHuksAddExternalCryptoParams huksAddExternalCryptoParams = nullptr;
OHHuksBuildExternalCryptoParamSet huksBuildExternalCryptoParamSet = nullptr;
OHHuksFreeExternalCryptoParamSet huksFreeExternalCryptoParamSet = nullptr;
OHHuksOpenResource huksOpenResource = nullptr;
OHHuksCloseResource huksCloseResource = nullptr;
OHHuksGetUkeyPinAuthState huksGetUkeyPinAuthState = nullptr;

#if BUILDFLAG(ARKWEB_UNITTESTS)
OHCertManagerGetUkeyCertificate GetCertManagerGetUkeyCertificate() {
    return certManagerGetUkeyCertificate;
}

OHCertManagerFreeUkeyCertificate GetCertManagerFreeUkeyCertificate() {
    return certManagerFreeUkeyCertificate;
}

OHHuksInitExternalCryptoParamSet GetHuksInitExternalCryptoParamSet() {
    return huksInitExternalCryptoParamSet;
}

OHHuksAddExternalCryptoParams GetHuksAddExternalCryptoParams() {
    return huksAddExternalCryptoParams;
}

OHHuksBuildExternalCryptoParamSet GetHuksBuildExternalCryptoParamSet() {
    return huksBuildExternalCryptoParamSet;
}

OHHuksFreeExternalCryptoParamSet GetHuksFreeExternalCryptoParamSet() {
    return huksFreeExternalCryptoParamSet;
}

OHHuksOpenResource GetHuksOpenResource() {
    return huksOpenResource;
}

OHHuksCloseResource GetHuksCloseResource() {
    return huksCloseResource;
}

OHHuksGetUkeyPinAuthState GetHuksGetUkeyPinAuthState() {
    return huksGetUkeyPinAuthState;
}

void SetCertManagerGetUkeyCertificate(OHCertManagerGetUkeyCertificate func) {
    certManagerGetUkeyCertificate = func;
}

void SetCertManagerFreeUkeyCertificate(OHCertManagerFreeUkeyCertificate func) {
    certManagerFreeUkeyCertificate = func;
}

void SetHuksInitExternalCryptoParamSet(OHHuksInitExternalCryptoParamSet func) {
    huksInitExternalCryptoParamSet = func;
}

void SetHuksAddExternalCryptoParams(OHHuksAddExternalCryptoParams func) {
    huksAddExternalCryptoParams = func;
}

void SetHuksBuildExternalCryptoParamSet(OHHuksBuildExternalCryptoParamSet func) {
    huksBuildExternalCryptoParamSet = func;
}

void SetHuksFreeExternalCryptoParamSet(OHHuksFreeExternalCryptoParamSet func) {
    huksFreeExternalCryptoParamSet = func;
}

void SetHuksOpenResource(OHHuksOpenResource func) {
    huksOpenResource = func;
}

void SetHuksCloseResource(OHHuksCloseResource func) {
    huksCloseResource = func;
}

void SetHuksGetUkeyPinAuthState(OHHuksGetUkeyPinAuthState func) {
    huksGetUkeyPinAuthState = func;
}
#endif

void InitDlopenFun() {
  static std::once_flag initFlag;
std::call_once(initFlag, [](){
#if defined(ARCH_CPU_ARM64)
  base::FilePath cmFile("system/lib64/ndk/libohcert_manager.z.so");
#else
  base::FilePath cmFile("system/lib/ndk/libohcert_manager.z.so");
#endif
  void* dl = dlopen(cmFile.value().c_str(), RTLD_NOW);
  if (dl) {
    certManagerGetUkeyCertificate = reinterpret_cast<OHCertManagerGetUkeyCertificate>(dlsym(
                                                        dl, "OH_CertManager_GetUkeyCertificate"));
    certManagerFreeUkeyCertificate = reinterpret_cast<OHCertManagerFreeUkeyCertificate>(dlsym(
                                                        dl, "OH_CertManager_FreeUkeyCertificate"));
  }
 
#if defined(ARCH_CPU_ARM64)
  base::FilePath huksFile("system/lib64/ndk/libhuks_external_crypto.z.so");
#else
  base::FilePath huksFile("system/lib/ndk/libhuks_external_crypto.z.so");
#endif
  dl = dlopen(huksFile.value().c_str(), RTLD_NOW);
  if (dl) {
    huksInitExternalCryptoParamSet = reinterpret_cast<OHHuksInitExternalCryptoParamSet>(dlsym(
                                                        dl, "OH_Huks_InitExternalCryptoParamSet"));
    huksAddExternalCryptoParams = reinterpret_cast<OHHuksAddExternalCryptoParams>(dlsym(
                                                        dl, "OH_Huks_AddExternalCryptoParams"));
    huksBuildExternalCryptoParamSet = reinterpret_cast<OHHuksBuildExternalCryptoParamSet>(dlsym(
                                                        dl, "OH_Huks_BuildExternalCryptoParamSet"));
    huksFreeExternalCryptoParamSet = reinterpret_cast<OHHuksFreeExternalCryptoParamSet>(dlsym(
                                                        dl, "OH_Huks_FreeExternalCryptoParamSet"));
    huksOpenResource = reinterpret_cast<OHHuksOpenResource>(dlsym(
                                                        dl, "OH_Huks_OpenResource"));
    huksCloseResource = reinterpret_cast<OHHuksCloseResource>(dlsym(
                                                        dl, "OH_Huks_CloseResource"));
    huksGetUkeyPinAuthState = reinterpret_cast<OHHuksGetUkeyPinAuthState>(dlsym(
                                                        dl, "OH_Huks_GetUkeyPinAuthState"));
  }
});
}
 
OH_Huks_Result InitParamSet(
    struct OH_Huks_ParamSet **paramSet,
    const struct OH_Huks_Param *params,
    uint32_t paramCount)
{
    OH_Huks_Result ret = OH_Huks_InitParamSet(paramSet);
    if (ret.errorCode != OH_HUKS_SUCCESS) {
        return ret;
    }
    ret = OH_Huks_AddParams(*paramSet, params, paramCount);
    if (ret.errorCode != OH_HUKS_SUCCESS) {
        OH_Huks_FreeParamSet(paramSet);
        return ret;
    }
    ret = OH_Huks_BuildParamSet(paramSet);
    if (ret.errorCode != OH_HUKS_SUCCESS) {
        OH_Huks_FreeParamSet(paramSet);
        return ret;
    }
    return ret;
}
 
OH_Huks_Result InitExtParamSet(
    OH_Huks_ExternalCryptoParamSet **paramSet,
    const OH_Huks_ExternalCryptoParam *params,
    uint32_t paramCount)
{
    OH_Huks_Result ret;
    ret.errorCode = -1;
    if (!huksInitExternalCryptoParamSet) {
        WVLOG_E("OH_Huks_InitExternalCryptoParamSet does not exist");
        return ret;
    }
 
    if (!huksAddExternalCryptoParams) {
        WVLOG_E("OH_Huks_AddExternalCryptoParams does not exist");
        return ret;
    }
 
    if (!huksBuildExternalCryptoParamSet) {
        WVLOG_E("OH_Huks_BuildExternalCryptoParamSet does not exist");
        return ret;
    }
 
    if (!huksFreeExternalCryptoParamSet) {
        WVLOG_E("OH_Huks_BuildExternalCryptoParamSet does not exist");
        return ret;
    }
 
    ret = huksInitExternalCryptoParamSet(paramSet);
    if (ret.errorCode != OH_HUKS_SUCCESS) {
        return ret;
    }
    ret = huksAddExternalCryptoParams(*paramSet, params, paramCount);
    if (ret.errorCode != OH_HUKS_SUCCESS) {
        huksFreeExternalCryptoParamSet(paramSet);
        return ret;
    }
    ret = huksBuildExternalCryptoParamSet(paramSet);
    if (ret.errorCode != OH_HUKS_SUCCESS) {
        huksFreeExternalCryptoParamSet(paramSet);
        return ret;
    }
    return ret;
}
 
bool GetHuksSignatureSpec(uint16_t algorithm, HuksSignatureSpec& result)
{
    switch (algorithm) {
        case SSL_SIGN_RSA_PKCS1_SHA256:
            result = { OH_HUKS_ALG_RSA, OH_HUKS_PADDING_PKCS1_V1_5, OH_HUKS_DIGEST_SHA256 };
            return true;
        case SSL_SIGN_RSA_PKCS1_SHA384:
            result = { OH_HUKS_ALG_RSA, OH_HUKS_PADDING_PKCS1_V1_5, OH_HUKS_DIGEST_SHA384 };
            return true;
        case SSL_SIGN_RSA_PKCS1_SHA512:
            result = { OH_HUKS_ALG_RSA, OH_HUKS_PADDING_PKCS1_V1_5, OH_HUKS_DIGEST_SHA512 };
            return true;
        case SSL_SIGN_RSA_PSS_RSAE_SHA256:
            result = { OH_HUKS_ALG_RSA, OH_HUKS_PADDING_PSS, OH_HUKS_DIGEST_SHA256 };
            return true;
        case SSL_SIGN_RSA_PSS_RSAE_SHA384:
            result = { OH_HUKS_ALG_RSA, OH_HUKS_PADDING_PSS, OH_HUKS_DIGEST_SHA384 };
            return true;
        case SSL_SIGN_RSA_PSS_RSAE_SHA512:
            result = { OH_HUKS_ALG_RSA, OH_HUKS_PADDING_PSS, OH_HUKS_DIGEST_SHA512 };
            return true;
        case SSL_SIGN_ECDSA_SECP256R1_SHA256:
            result = { OH_HUKS_ALG_ECC, OH_HUKS_PADDING_NONE, OH_HUKS_DIGEST_SHA256 };
            return true;
        case SSL_SIGN_ECDSA_SECP384R1_SHA384:
            result = { OH_HUKS_ALG_ECC, OH_HUKS_PADDING_NONE, OH_HUKS_DIGEST_SHA384 };
            return true;
        case SSL_SIGN_ECDSA_SECP521R1_SHA512:
            result = { OH_HUKS_ALG_ECC, OH_HUKS_PADDING_NONE, OH_HUKS_DIGEST_SHA512 };
            return true;
        default:
            return false;
    }
}
}

#if BUILDFLAG(ARKWEB_UNITTESTS)
using cert_mgr_adapter_impl_test::InitDlopenFun;
using cert_mgr_adapter_impl_test::certManagerGetUkeyCertificate;
using cert_mgr_adapter_impl_test::certManagerFreeUkeyCertificate;
using cert_mgr_adapter_impl_test::huksOpenResource;
using cert_mgr_adapter_impl_test::huksFreeExternalCryptoParamSet;
using cert_mgr_adapter_impl_test::InitExtParamSet;
using cert_mgr_adapter_impl_test::huksCloseResource;
using cert_mgr_adapter_impl_test::huksGetUkeyPinAuthState;
using cert_mgr_adapter_impl_test::HuksSignatureSpec;
using cert_mgr_adapter_impl_test::InitParamSet;
#endif

CertManagerAdapterImpl::CertManagerAdapterImpl() {
    InitDlopenFun();
}

uint32_t CertManagerAdapterImpl::GetCertMaxSize()
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::GetCertMaxSize");
    return 0;
}

uint32_t CertManagerAdapterImpl::GetAppCertMaxSize()
{
    WVLOG_D("[adapter mock] CertManagerImpl::GetAppCertMaxSize");
    return OH_CM_MAX_LEN_CERTIFICATE_CHAIN;
}

uint32_t CertManagerAdapterImpl::GetSytemRootCertSum()
{
    WVLOG_D("[adapter mock] CertManagerImpl::GetSytemRootCertSum");
    return 0;
}

int32_t CertManagerAdapterImpl::GetCertDataBySubject(const char *subjectName, uint8_t* certData, int32_t certType)
{
    WVLOG_D("[adapter mock] CertManagerImpl::GetCertDataBySubject");
    return 0;
}

int32_t CertManagerAdapterImpl::GetSytemRootCertData(uint32_t certCount, uint8_t* certData)
{
    WVLOG_D("[adapter mock] CertManagerImpl::GetSytemRootCertData");
    return 0;
}

uint32_t CertManagerAdapterImpl::GetUserRootCertSum()
{
    WVLOG_D("[adapter mock] CertManagerImpl::GetUserRootCertSum");
    return 0;
}

int32_t CertManagerAdapterImpl::GetUserRootCertData(uint32_t certCount, uint8_t* certData)
{
    WVLOG_D("[adapter mock] CertManagerImpl::GetUserRootCertData");
    return 0;
}

int32_t CertManagerAdapterImpl::GetAppCert(uint8_t* uriData, uint8_t* certData, uint32_t* len)
{
    WVLOG_D("[adapter mock] CertManagerImpl::GetAppCert");
    return 0;
}

int32_t CertManagerAdapterImpl::Sign(const uint8_t* uri, const uint8_t* certData, uint32_t certDataLen,
    uint8_t* signData, uint32_t signDataLen)
{
    WVLOG_D("[adapter mock] CertManagerImpl::Sign");
    return 0;
}

bool CertManagerAdapterImpl::GetTrustAnchorsForHostName(
    const std::string& hostname, std::vector<std::string>& certs)
{
    NetStack_Certificates certificates;

    int32_t ret = OH_NetStack_GetCertificatesForHostName(hostname.c_str(), &certificates);
    if (ret != 0) {
        WVLOG_E("GetTrustAnchorsForHostName for hostname:%{public}s failed",
            hostname.c_str());
        OH_Netstack_DestroyCertificatesContent(&certificates);
        return false;
    }

    for (size_t i = 0; i < certificates.length; i++) {
        certs.push_back(certificates.content[i]);
    }
    OH_Netstack_DestroyCertificatesContent(&certificates);
    return true;
}

bool CertManagerAdapterImpl::GetPinSetForHostName(
    const std::string& hostname, std::vector<std::string>& pins)
{
    NetStack_CertificatePinning certificatePinning;
    int32_t ret = OH_NetStack_GetPinSetForHostName(hostname.c_str(), &certificatePinning);
    if (ret != 0) {
        WVLOG_E("GetPinSetForHostName for hostname:%{private}s failed, ret:%{private}d",
            hostname.c_str(), ret);
        return false;
    }

    std::string pinsString;
    if (certificatePinning.publicKeyHash == nullptr) {
        WVLOG_D("GetPinSetForHostName for hostname:%{public}s is empty", hostname.c_str());
        return true;
    } else {
        pinsString = certificatePinning.publicKeyHash;
        free(certificatePinning.publicKeyHash);
    }
    size_t start = 0;
    size_t pos = pinsString.find(";", start);
    while (pos != std::string::npos) {
        pins.emplace_back(pinsString.substr(start, pos - start));
        start = pos + 1;
        pos = pinsString.find(";", start);
    }
    pins.emplace_back(pinsString.substr(start));
    return true;
}

int32_t CertManagerAdapterImpl::GetUkeyCert(const std::string& identity, uint8_t* certData, uint32_t* certDataLen)
{
    WVLOG_I("CertManagerAdapterImpl::GetUkeyCert");
 
    OH_CM_Blob key = {identity.length(), (uint8_t*)identity.c_str()};
    OH_CM_UkeyInfo info = {OH_CM_CERT_PURPOSE_SIGN};
    OH_CM_CredentialDetailList credList = {0, nullptr};
 
    if (!certManagerGetUkeyCertificate) {
        WVLOG_E("OH_CertManager_GetUkeyCertificate does not exist");
        return -1;
    }
    if (!certManagerFreeUkeyCertificate) {
        WVLOG_E("OH_CertManager_FreeUkeyCertificate does not exist");
        return -1;
    }
 
    int32_t ret = certManagerGetUkeyCertificate(&key, &info, &credList);
    if (ret != OH_CM_SUCCESS || credList.credentialCount == 0) {
        WVLOG_E("GetUkeyCert, get ukey cert failed, ret = %{public}d ", ret);
        certManagerFreeUkeyCertificate(&credList);
        return ret;
    }
    
    if (!(credList.credential) ||
        credList.credential[0].credData.size == 0 ||
        credList.credential[0].credData.data == nullptr) {
        WVLOG_E("GetUkeyCert, Invalid credential data");
        certManagerFreeUkeyCertificate(&credList);
        return ret;
    }
 
    *certDataLen = credList.credential[0].credData.size;
    if (memcpy_s(certData, OH_CM_MAX_LEN_CERTIFICATE_CHAIN, credList.credential[0].credData.data,
            credList.credential[0].credData.size) != OH_CM_SUCCESS) {
        WVLOG_E("GetAppCert, memory copy failed");
        certManagerFreeUkeyCertificate(&credList);
        return -1;
    }
 
    certManagerFreeUkeyCertificate(&credList);
    return OH_CM_SUCCESS;
}
 
int32_t CertManagerAdapterImpl::OpenUKeyRemoteHandle(const std::string& identity)
{
    WVLOG_I("CertManagerAdapterImpl::OpenUKeyRemoteHandle");
 
    if (!huksOpenResource) {
        WVLOG_E("OH_Huks_OpenResource does not exist");
        return -1;
    }
 
    if (!huksFreeExternalCryptoParamSet) {
        WVLOG_E("OH_Huks_FreeExternalCryptoParamSet does not exist");
        return -1;
    }
 
    struct OH_Huks_Blob index = {identity.length(), (uint8_t*)identity.c_str()};
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam signParams[] = {
        {
            .tag = OH_HUKS_TAG_KEY_CLASS,
            .uint32Param = OH_HUKS_KEY_CLASS_EXTENSION
        }
    };
    struct OH_Huks_Result result = InitExtParamSet(&paramSet, signParams, sizeof(signParams) / sizeof(OH_Huks_Param));
    if (result.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("OpenUKeyRemoteHandle, init param set failed, errorCode = %{public}d ", result.errorCode);
        return result.errorCode;
    }
 
    result = huksOpenResource(&index, paramSet);
 
    if (result.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("OpenUKeyRemoteHandle, open remote handle failed, errorCode = %{public}d ", result.errorCode);
        huksFreeExternalCryptoParamSet(&paramSet);
        return result.errorCode;
    }
 
    huksFreeExternalCryptoParamSet(&paramSet);
    return OH_HUKS_SUCCESS;
}
 
int32_t CertManagerAdapterImpl::CloseUKeyRemoteHandle(const std::string& identity)
{
    WVLOG_I("CertManagerAdapterImpl::CloseUKeyRemoteHandle");
 
    if (!huksCloseResource) {
        WVLOG_E("OH_Huks_CloseRemoteHandle does not exist");
        return -1;
    }
 
    if (!huksFreeExternalCryptoParamSet) {
        WVLOG_E("OH_Huks_FreeExternalCryptoParamSet does not exist");
        return -1;
    }
 
    struct OH_Huks_Blob index = {identity.length(), (uint8_t*)identity.c_str()};
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam signParams[] = {
        {
            .tag = OH_HUKS_TAG_KEY_CLASS,
            .uint32Param = OH_HUKS_KEY_CLASS_EXTENSION
        }
    };
    struct OH_Huks_Result result = InitExtParamSet(&paramSet, signParams, sizeof(signParams) / sizeof(OH_Huks_Param));
    if (result.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("CloseUKeyRemoteHandle, init param set failed, errorCode = %{public}d ", result.errorCode);
        return result.errorCode;
    }
 
    result = huksCloseResource(&index, paramSet);
 
    if (result.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("CloseUKeyRemoteHandle, close remote handle failed, errorCode = %{public}d ", result.errorCode);
        huksFreeExternalCryptoParamSet(&paramSet);
        return result.errorCode;
    }
 
    huksFreeExternalCryptoParamSet(&paramSet);
    return OH_HUKS_SUCCESS;
}
 
int32_t CertManagerAdapterImpl::GetUkeyPinAuthState(const std::string& uri, bool* state)
{
    WVLOG_I("CertManagerAdapterImpl::GetUkeyPinAuthState");
 
    if (!huksGetUkeyPinAuthState) {
        WVLOG_E("OH_Huks_GetUkeyPinAuthState does not exist");
        return -1;
    }
 
    if (!huksFreeExternalCryptoParamSet) {
        WVLOG_E("OH_Huks_FreeExternalCryptoParamSet does not exist");
        return -1;
    }
 
    struct OH_Huks_Blob index = {uri.length(), (uint8_t*)uri.c_str()};
 
    OH_Huks_ExternalCryptoParamSet* paramSet = nullptr;
    OH_Huks_ExternalCryptoParam signParams[] = {
        {
            .tag = OH_HUKS_TAG_KEY_CLASS,
            .uint32Param = OH_HUKS_KEY_CLASS_EXTENSION
        }
    };
    struct OH_Huks_Result result = InitExtParamSet(&paramSet, signParams, sizeof(signParams) / sizeof(OH_Huks_Param));
    if (result.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("GetUkeyPinAuthState, init param set failed, errorCode = %{public}d ", result.errorCode);
        return result.errorCode;
    }

    OH_Huks_ExternalPinAuthState authState = OH_HUKS_EXT_CRYPTO_PIN_NO_AUTH;
    result = huksGetUkeyPinAuthState(&index, paramSet, &authState);
    if (result.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("GetUkeyPinAuthState, get ukey pin auth state failed, errorCode = %{public}d ", result.errorCode);
        huksFreeExternalCryptoParamSet(&paramSet);
        return result.errorCode;
    }
    *state = (authState == OH_HUKS_EXT_CRYPTO_PIN_AUTH_SUCCEEDED);
 
    huksFreeExternalCryptoParamSet(&paramSet);
    return OH_HUKS_SUCCESS;
}
 
int32_t CertManagerAdapterImpl::SignUsingHuks(const std::string& identity, const uint8_t* certData, uint32_t certDataLen,
        uint8_t* signData, uint32_t* signDataLen, uint16_t algorithm, uint32_t keySize)
{
    WVLOG_D("CertManagerAdapterImpl::SignHuks");
    struct OH_Huks_Blob keyAlias = {identity.length(), (uint8_t*)identity.c_str()};
    struct OH_Huks_ParamSet* paramSet = nullptr;
    struct HuksSignatureSpec spec;
    if (!GetHuksSignatureSpec(algorithm, spec)) {
        WVLOG_E("get huks signature spec fail, please check the signature algorithm");
        return -1;
    }
    struct OH_Huks_Param signParams[] = {
        {
            .tag = OH_HUKS_TAG_ALGORITHM,
            .uint32Param = spec.algorithm
        },
        {
            .tag = OH_HUKS_TAG_PURPOSE,
            .uint32Param = OH_HUKS_KEY_PURPOSE_SIGN
        },
        {
            .tag = OH_HUKS_TAG_KEY_SIZE,
            .uint32Param = keySize
        },
        {
            .tag = OH_HUKS_TAG_PADDING,
            .uint32Param = spec.padding
        },
        {
            .tag = OH_HUKS_TAG_DIGEST,
            .uint32Param = spec.digest
        },
        {
            .tag = OH_HUKS_TAG_KEY_CLASS,
            .uint32Param = OH_HUKS_KEY_CLASS_EXTENSION
        }
    };
    struct OH_Huks_Result result = InitParamSet(&paramSet, signParams, sizeof(signParams) / sizeof(OH_Huks_Param));
    if (result.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("SignHuks, init param set failed, errorCode = %{public}d ", result.errorCode);
        return result.errorCode;
    }
 
    uint64_t handleValue = 0;
    struct OH_Huks_Blob handleSign = {sizeof(uint64_t), (uint8_t *)&handleValue};
    result = OH_Huks_InitSession(&keyAlias, paramSet, &handleSign, nullptr);
    if (result.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("SignHuks, init session failed, errorCode = %{public}d ", result.errorCode);
        OH_Huks_FreeParamSet(&paramSet);
        return result.errorCode;
    }
 
    struct OH_Huks_Blob inData = {certDataLen, const_cast<uint8_t *>(certData)};
    struct OH_Huks_Blob outDataSign = { *signDataLen, signData };
    result = OH_Huks_FinishSession(&handleSign, paramSet, &inData, &outDataSign);
    if (result.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("SignHuks, finish session failed, errorCode = %{public}d ", result.errorCode);
        OH_Huks_FreeParamSet(&paramSet);
        return result.errorCode;
    }
    *signDataLen = outDataSign.size;
 
    OH_Huks_FreeParamSet(&paramSet);
    return OH_HUKS_SUCCESS;
}
}