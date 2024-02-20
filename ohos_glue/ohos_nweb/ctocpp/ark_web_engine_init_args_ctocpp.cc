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

#include "ohos_nweb/ctocpp/ark_web_engine_init_args_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkWebEngineInitArgsCToCpp::GetIsPopup() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_engine_init_args_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_is_popup, false);

  // Execute
  return _struct->get_is_popup(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkWebEngineInitArgsCToCpp::GetDumpPath() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_engine_init_args_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_dump_path,
                                   ark_web_string_default);

  // Execute
  return _struct->get_dump_path(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebStringList ArkWebEngineInitArgsCToCpp::GetArgsToAdd() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_engine_init_args_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_list_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_args_to_add,
                                   ark_web_string_list_default);

  // Execute
  return _struct->get_args_to_add(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebStringList ArkWebEngineInitArgsCToCpp::GetArgsToDelete() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_engine_init_args_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_list_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_args_to_delete,
                                   ark_web_string_list_default);

  // Execute
  return _struct->get_args_to_delete(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebEngineInitArgsCToCpp::GetIsFrameInfoDump() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_engine_init_args_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_is_frame_info_dump, false);

  // Execute
  return _struct->get_is_frame_info_dump(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebEngineInitArgsCToCpp::GetIsEnhanceSurface() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_engine_init_args_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_is_enhance_surface, false);

  // Execute
  return _struct->get_is_enhance_surface(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebEngineInitArgsCToCpp::GetIsMultiRendererProcess() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_engine_init_args_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_is_multi_renderer_process,
                                   false);

  // Execute
  return _struct->get_is_multi_renderer_process(_struct);
}

ArkWebEngineInitArgsCToCpp::ArkWebEngineInitArgsCToCpp() {
}

ArkWebEngineInitArgsCToCpp::~ArkWebEngineInitArgsCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkWebEngineInitArgsCToCpp, ArkWebEngineInitArgs,
                           ark_web_engine_init_args_t>::kBridgeType =
        ARK_WEB_ENGINE_INIT_ARGS;

} // namespace OHOS::ArkWeb
