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

#include "ohos_adapter/ctocpp/ark_ohos_web_permission_data_base_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkOhosWebPermissionDataBaseAdapterCToCpp::ExistPermissionByOrigin(
    const ArkWebString &origin, const int32_t &key) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_permission_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, exist_permission_by_origin, false);

  // Execute
  return _struct->exist_permission_by_origin(_struct, &origin, &key);
}

ARK_WEB_NO_SANITIZE
bool ArkOhosWebPermissionDataBaseAdapterCToCpp::GetPermissionResultByOrigin(
    const ArkWebString &origin, const int32_t &key, bool &result) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_permission_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_permission_result_by_origin,
                                   false);

  // Execute
  return _struct->get_permission_result_by_origin(_struct, &origin, &key,
                                                  &result);
}

ARK_WEB_NO_SANITIZE
void ArkOhosWebPermissionDataBaseAdapterCToCpp::SetPermissionByOrigin(
    const ArkWebString &origin, const int32_t &key, bool result) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_permission_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_permission_by_origin, );

  // Execute
  _struct->set_permission_by_origin(_struct, &origin, &key, result);
}

ARK_WEB_NO_SANITIZE
void ArkOhosWebPermissionDataBaseAdapterCToCpp::ClearPermissionByOrigin(
    const ArkWebString &origin, const int32_t &key) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_permission_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, clear_permission_by_origin, );

  // Execute
  _struct->clear_permission_by_origin(_struct, &origin, &key);
}

ARK_WEB_NO_SANITIZE
void ArkOhosWebPermissionDataBaseAdapterCToCpp::ClearAllPermission(
    const int32_t &key) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_permission_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, clear_all_permission, );

  // Execute
  _struct->clear_all_permission(_struct, &key);
}

ARK_WEB_NO_SANITIZE
void ArkOhosWebPermissionDataBaseAdapterCToCpp::GetOriginsByPermission(
    const int32_t &key, ArkWebStringVector &origins) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_web_permission_data_base_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_origins_by_permission, );

  // Execute
  _struct->get_origins_by_permission(_struct, &key, &origins);
}

ArkOhosWebPermissionDataBaseAdapterCToCpp::
    ArkOhosWebPermissionDataBaseAdapterCToCpp() {
}

ArkOhosWebPermissionDataBaseAdapterCToCpp::
    ~ArkOhosWebPermissionDataBaseAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkOhosWebPermissionDataBaseAdapterCToCpp,
    ArkOhosWebPermissionDataBaseAdapter,
    ark_ohos_web_permission_data_base_adapter_t>::kBridgeType =
    ARK_OHOS_WEB_PERMISSION_DATA_BASE_ADAPTER;

} // namespace OHOS::ArkWeb
