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

#include "ohos_adapter/ctocpp/ark_display_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
uint64_t ArkDisplayAdapterCToCpp::GetId() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_id, 0);

  // Execute
  return _struct->get_id(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkDisplayAdapterCToCpp::GetWidth() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_width, 0);

  // Execute
  return _struct->get_width(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkDisplayAdapterCToCpp::GetHeight() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_height, 0);

  // Execute
  return _struct->get_height(_struct);
}

ARK_WEB_NO_SANITIZE
float ArkDisplayAdapterCToCpp::GetVirtualPixelRatio() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_virtual_pixel_ratio, 0);

  // Execute
  return _struct->get_virtual_pixel_ratio(_struct);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkDisplayAdapterCToCpp::GetRotation() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_rotation, 0);

  // Execute
  return _struct->get_rotation(_struct);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkDisplayAdapterCToCpp::GetOrientation() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_orientation, 0);

  // Execute
  return _struct->get_orientation(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkDisplayAdapterCToCpp::GetDpi() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_dpi, 0);

  // Execute
  return _struct->get_dpi(_struct);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkDisplayAdapterCToCpp::GetDisplayOrientation() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_adapter_t* _struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct,  0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_display_orientation,  0);

  // Execute
  return _struct->get_display_orientation(_struct);
}

ArkDisplayAdapterCToCpp::ArkDisplayAdapterCToCpp() {
}

ArkDisplayAdapterCToCpp::~ArkDisplayAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkDisplayAdapterCToCpp, ArkDisplayAdapter,
                           ark_display_adapter_t>::kBridgeType =
        ARK_DISPLAY_ADAPTER;

} // namespace OHOS::ArkWeb
