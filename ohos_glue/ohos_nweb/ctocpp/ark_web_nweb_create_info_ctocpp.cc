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

#include "ohos_nweb/ctocpp/ark_web_nweb_create_info_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_nweb/ctocpp/ark_web_engine_init_args_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_output_frame_callback_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
uint32_t ArkWebNWebCreateInfoCToCpp::GetWidth() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_nweb_create_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_width, 0);

  // Execute
  return _struct->get_width(_struct);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkWebNWebCreateInfoCToCpp::GetHeight() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_nweb_create_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_height, 0);

  // Execute
  return _struct->get_height(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCreateInfoCToCpp::GetIsIncognitoMode() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_nweb_create_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_is_incognito_mode, false);

  // Execute
  return _struct->get_is_incognito_mode(_struct);
}

ARK_WEB_NO_SANITIZE
void *ArkWebNWebCreateInfoCToCpp::GetProducerSurface() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_nweb_create_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_producer_surface, nullptr);

  // Execute
  return _struct->get_producer_surface(_struct);
}

ARK_WEB_NO_SANITIZE
void *ArkWebNWebCreateInfoCToCpp::GetEnhanceSurfaceInfo() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_nweb_create_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_enhance_surface_info, nullptr);

  // Execute
  return _struct->get_enhance_surface_info(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebEngineInitArgs>
ArkWebNWebCreateInfoCToCpp::GetEngineInitArgs() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_nweb_create_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_engine_init_args, nullptr);

  // Execute
  ark_web_engine_init_args_t *_retval = _struct->get_engine_init_args(_struct);

  // Return type: refptr_same
  return ArkWebEngineInitArgsCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebOutputFrameCallback>
ArkWebNWebCreateInfoCToCpp::GetOutputFrameCallback() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_nweb_create_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_output_frame_callback, nullptr);

  // Execute
  ark_web_output_frame_callback_t *_retval =
      _struct->get_output_frame_callback(_struct);

  // Return type: refptr_same
  return ArkWebOutputFrameCallbackCToCpp::Invert(_retval);
}

ArkWebNWebCreateInfoCToCpp::ArkWebNWebCreateInfoCToCpp() {
}

ArkWebNWebCreateInfoCToCpp::~ArkWebNWebCreateInfoCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkWebNWebCreateInfoCToCpp, ArkWebNWebCreateInfo,
                           ark_web_nweb_create_info_t>::kBridgeType =
        ARK_WEB_NWEB_CREATE_INFO;

} // namespace OHOS::ArkWeb
