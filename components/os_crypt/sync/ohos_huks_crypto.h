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

#ifndef COMPONENTS_OS_CRYPT_SYNC_OHOS_HUKS_CRYPTO_H_
#define COMPONENTS_OS_CRYPT_SYNC_OHOS_HUKS_CRYPTO_H_

#include <huks/native_huks_api.h>
#include <huks/native_huks_param.h>
#include <huks/native_huks_type.h>
#include <cstdint>
#include <string>

#include "base/component_export.h"

namespace crypto {

namespace ohos {

class COMPONENT_EXPORT(OS_CRYPT) OhosHuksCrypto {
 public:
  static OhosHuksCrypto& GetInstance();

  ~OhosHuksCrypto() = default;

  bool EncryptKey(const std::string& alias,
                  const std::string& plaintext,
                  std::string* ciphertext);

  bool DecryptKey(const std::string& alias,
                  const std::string& ciphertext,
                  std::string* plaintext);

 private:
  OhosHuksCrypto() = default;

  OhosHuksCrypto(const OhosHuksCrypto& other) = delete;

  OhosHuksCrypto& operator=(const OhosHuksCrypto&) = delete;

  OH_Huks_Result InitParamSet(struct OH_Huks_ParamSet** paramSet,
                              const struct OH_Huks_Param* params,
                              uint32_t paramCount);

  bool Crypt(bool isEncrypt,
             const std::string& alias,
             const std::string& input,
             std::string* output);

  bool IsStringInWhitelist(const std::string& str);
};
}  // namespace ohos

}  // namespace crypto

#endif  // COMPONENTS_OS_CRYPT_SYNC_OHOS_HUKS_CRYPTO_H_
