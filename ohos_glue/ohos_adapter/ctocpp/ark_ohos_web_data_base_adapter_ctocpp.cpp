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

#include "ohos_adapter/ctocpp/ark_ohos_web_data_base_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkOhosWebDataBaseAdapterCToCpp::ExistHttpAuthCredentials() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, exist_http_auth_credentials, false);

  // Execute
  return _struct->exist_http_auth_credentials(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkOhosWebDataBaseAdapterCToCpp::DeleteHttpAuthCredentials() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, delete_http_auth_credentials, );

  // Execute
  _struct->delete_http_auth_credentials(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkOhosWebDataBaseAdapterCToCpp::SaveHttpAuthCredentials(
    const ArkWebString &host, const ArkWebString &realm,
    const ArkWebString &username, const char *password) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, save_http_auth_credentials, );

  // Execute
  _struct->save_http_auth_credentials(_struct, &host, &realm, &username,
                                      password);
}

ARK_WEB_NO_SANITIZE
void ArkOhosWebDataBaseAdapterCToCpp::GetHttpAuthCredentials(
    const ArkWebString &host, const ArkWebString &realm, ArkWebString &username,
    char *password, uint32_t passwordSize) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_http_auth_credentials, );

  // Execute
  _struct->get_http_auth_credentials(_struct, &host, &realm, &username,
                                     password, passwordSize);
}

ArkOhosWebDataBaseAdapterCToCpp::ArkOhosWebDataBaseAdapterCToCpp() {
}

ArkOhosWebDataBaseAdapterCToCpp::~ArkOhosWebDataBaseAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkOhosWebDataBaseAdapterCToCpp, ArkOhosWebDataBaseAdapter,
    ark_ohos_web_data_base_adapter_t>::kBridgeType =
    ARK_OHOS_WEB_DATA_BASE_ADAPTER;

} // namespace OHOS::ArkWeb
