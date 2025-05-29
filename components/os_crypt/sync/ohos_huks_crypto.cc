// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.WHITE_LIST
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_huks_crypto.h"

#include <algorithm>
#include <cstring>
#include <unordered_set>

#include "base/logging.h"
#include "crypto/random.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace {
static const uint32_t kIvSize = 16;
// alias whitelist
static const std::unordered_set<std::string> kWhiteList = {
    "chrome_huks_os_crypt_password_v1"};
}  // namespace

namespace crypto {

namespace ohos {
// static
OhosHuksCrypto& OhosHuksCrypto::GetInstance() {
  static OhosHuksCrypto instance;
  return instance;
}

bool OhosHuksCrypto::IsStringInWhitelist(const std::string& str) {
  return kWhiteList.find(str) != kWhiteList.end();
}

OH_Huks_Result OhosHuksCrypto::InitParamSet(struct OH_Huks_ParamSet** paramSet,
                                            const struct OH_Huks_Param* params,
                                            uint32_t paramCount) {
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

struct OH_Huks_Param g_genEncDecParams[] = {
    {.tag = OH_HUKS_TAG_ALGORITHM, .uint32Param = OH_HUKS_ALG_AES},
    {.tag = OH_HUKS_TAG_PURPOSE,
     .uint32Param = OH_HUKS_KEY_PURPOSE_ENCRYPT | OH_HUKS_KEY_PURPOSE_DECRYPT},
    {.tag = OH_HUKS_TAG_KEY_SIZE, .uint32Param = OH_HUKS_AES_KEY_SIZE_256},
    {.tag = OH_HUKS_TAG_PADDING, .uint32Param = OH_HUKS_PADDING_PKCS7},
    {.tag = OH_HUKS_TAG_BLOCK_MODE, .uint32Param = OH_HUKS_MODE_CBC}};

bool OhosHuksCrypto::EncryptKey(const std::string& alias,
                                const std::string& plaintext,
                                std::string* ciphertext) {
  return Crypt(true, alias, plaintext, ciphertext);
}

bool OhosHuksCrypto::DecryptKey(const std::string& alias,
                                const std::string& ciphertext,
                                std::string* plaintext) {
  return Crypt(false, alias, ciphertext, plaintext);
}

bool OhosHuksCrypto::Crypt(bool isEncrypt,
                           const std::string& alias,
                           const std::string& input,
                           std::string* output) {
  if (output == nullptr || !IsStringInWhitelist(alias)) {
    LOG(ERROR) << "Crypt parameter error";
    return false;
  }
  struct OH_Huks_Blob inData;
  std::string validData;
  uint8_t iv[kIvSize] = {0};
  std::size_t cipherIndex = 0;
  if (isEncrypt) {
    cipherIndex = kIvSize;
    base::span<uint8_t> ivSpan(iv, kIvSize);
    crypto::RandBytes(ivSpan);
    inData = {(unsigned int)input.length(), (uint8_t*)input.c_str()};
  } else {
    if (input.length() < kIvSize) {
      *output = std::string();
      return false;
    }
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
    if (memcpy_s((uint8_t*)iv, kIvSize, input.substr(0, kIvSize).c_str(), kIvSize) != EOK) {
      LOG(ERROR) << "Crypt memcpy_s failed.";
      *output = std::string();
      return false;
    }
#else
    memcpy((uint8_t*)iv, input.substr(0, kIvSize).c_str(), kIvSize);
#endif
    validData = input.substr(kIvSize);
    inData = {(unsigned int)validData.length(), (uint8_t*)validData.c_str()};
  }

  unsigned int purpose =
      isEncrypt ? OH_HUKS_KEY_PURPOSE_ENCRYPT : OH_HUKS_KEY_PURPOSE_DECRYPT;
  struct OH_Huks_Param g_cryptParams[] = {
      {.tag = OH_HUKS_TAG_ALGORITHM, .uint32Param = OH_HUKS_ALG_AES},
      {.tag = OH_HUKS_TAG_PURPOSE, .uint32Param = purpose},
      {.tag = OH_HUKS_TAG_KEY_SIZE, .uint32Param = OH_HUKS_AES_KEY_SIZE_256},
      {.tag = OH_HUKS_TAG_PADDING, .uint32Param = OH_HUKS_PADDING_PKCS7},
      {.tag = OH_HUKS_TAG_BLOCK_MODE, .uint32Param = OH_HUKS_MODE_CBC},
      {.tag = OH_HUKS_TAG_IV, .blob = {.size = kIvSize, .data = (uint8_t*)iv}}};

  struct OH_Huks_Blob keyAlias = {alias.length(), (uint8_t*)alias.c_str()};
  struct OH_Huks_ParamSet* genParamSet = nullptr;
  struct OH_Huks_ParamSet* cryptParamSet = nullptr;
  int outputSize = input.length() + kIvSize + kIvSize;
  output->clear();
  output->resize(outputSize, 0);
  if (isEncrypt) {
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
    if (memcpy_s(output->data(), kIvSize, (uint8_t*)iv, kIvSize) != EOK) {
      LOG(ERROR) << "Crypt memcpy_s failed.";
      *output = std::string();
      return false;
    }
#else
    memcpy(output->data(), (uint8_t*)iv, kIvSize);
#endif
  }
  struct OH_Huks_Blob outData = {outputSize,
                                 (uint8_t*)output->data() + cipherIndex};
  OH_Huks_Result ohResult =
      InitParamSet(&genParamSet, g_genEncDecParams,
                   sizeof(g_genEncDecParams) / sizeof(OH_Huks_Param));
  if (ohResult.errorCode != OH_HUKS_SUCCESS) {
    OH_Huks_FreeParamSet(&genParamSet);
    LOG(ERROR) << "init gen param set failed, error code: "
               << ohResult.errorCode;
    *output = std::string();
    return false;
  }

  ohResult = OH_Huks_IsKeyItemExist(&keyAlias, genParamSet);
  if (ohResult.errorCode != OH_HUKS_SUCCESS) {
    if (!isEncrypt) {
      OH_Huks_FreeParamSet(&genParamSet);
      LOG(ERROR) << "huks key is not exist, error code: " << ohResult.errorCode;
      *output = std::string();
      return false;
    }
    ohResult = OH_Huks_GenerateKeyItem(&keyAlias, genParamSet, nullptr);
    if (ohResult.errorCode != OH_HUKS_SUCCESS) {
      LOG(ERROR) << "generate key failed, error code: " << ohResult.errorCode;
      OH_Huks_FreeParamSet(&genParamSet);
      *output = std::string();
      return false;
    }
  }

  ohResult = InitParamSet(&cryptParamSet, g_cryptParams,
                          sizeof(g_cryptParams) / sizeof(OH_Huks_Param));
  if (ohResult.errorCode != OH_HUKS_SUCCESS) {
    LOG(ERROR) << "init encrypt param set failed, error code: "
               << ohResult.errorCode;
    OH_Huks_FreeParamSet(&genParamSet);
    OH_Huks_FreeParamSet(&cryptParamSet);
    *output = std::string();
    return false;
  }

  uint8_t handleC[sizeof(uint64_t)] = {0};
  struct OH_Huks_Blob handleCrypt = {sizeof(uint64_t), handleC};
  ohResult =
      OH_Huks_InitSession(&keyAlias, cryptParamSet, &handleCrypt, nullptr);
  if (ohResult.errorCode != OH_HUKS_SUCCESS) {
    LOG(ERROR) << "huks init invoke failed, error code: " << ohResult.errorCode;
    OH_Huks_FreeParamSet(&genParamSet);
    OH_Huks_FreeParamSet(&cryptParamSet);
    *output = std::string();
    return false;
  }
  ohResult =
      OH_Huks_FinishSession(&handleCrypt, cryptParamSet, &inData, &outData);
  if (ohResult.errorCode != OH_HUKS_SUCCESS) {
    LOG(ERROR) << "huks finish invoke failed, error code: "
               << ohResult.errorCode;
    OH_Huks_FreeParamSet(&genParamSet);
    OH_Huks_FreeParamSet(&cryptParamSet);
    *output = std::string();
    return false;
  }

  OH_Huks_FreeParamSet(&genParamSet);
  OH_Huks_FreeParamSet(&cryptParamSet);
  if (outData.data == nullptr) {
    LOG(ERROR) << "output data is nullptr";
    *output = std::string();
    return false;
  }

  output->resize(cipherIndex + outData.size);
  return true;
}
}  // namespace ohos

}  // namespace crypto
