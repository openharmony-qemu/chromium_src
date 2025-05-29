// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
