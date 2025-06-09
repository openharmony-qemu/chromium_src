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

#include "ohos_huks_crypto.h"

#include <algorithm>
#include <cstring>
#include <unordered_set>

#include "base/logging.h"
#include "crypto/random.h"

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
    memcpy((uint8_t*)iv, input.substr(0, kIvSize).c_str(), kIvSize);
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
    memcpy(output->data(), (uint8_t*)iv, kIvSize);
  }
  struct OH_Huks_Blob outData = {outputSize, (uint8_t*)output->data() + cipherIndex};
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
