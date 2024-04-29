/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_adapter/ctocpp/ark_keystore_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
ArkWebString
ArkKeystoreAdapterCToCpp::EncryptKey(const ArkWebString &alias,
                                     const ArkWebString &plainData) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_keystore_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, encrypt_key,
                                   ark_web_string_default);

  // Execute
  return _struct->encrypt_key(_struct, &alias, &plainData);
}

ARK_WEB_NO_SANITIZE
ArkWebString
ArkKeystoreAdapterCToCpp::DecryptKey(const ArkWebString &alis,
                                     const ArkWebString &encryptedData) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_keystore_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, decrypt_key,
                                   ark_web_string_default);

  // Execute
  return _struct->decrypt_key(_struct, &alis, &encryptedData);
}

ArkKeystoreAdapterCToCpp::ArkKeystoreAdapterCToCpp() {
}

ArkKeystoreAdapterCToCpp::~ArkKeystoreAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkKeystoreAdapterCToCpp, ArkKeystoreAdapter,
                           ark_keystore_adapter_t>::kBridgeType =
        ARK_KEYSTORE_ADAPTER;

} // namespace OHOS::ArkWeb
