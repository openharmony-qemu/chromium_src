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

#include "ohos_adapter/ctocpp/ark_media_avsession_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_media_avsession_callback_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_media_avsession_metadata_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_media_avsession_position_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkMediaAVSessionAdapterCToCpp::CreateAVSession(int32_t type) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_avsession_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_avsession, false);

  // Execute
  return _struct->create_avsession(_struct, type);
}

ARK_WEB_NO_SANITIZE
void ArkMediaAVSessionAdapterCToCpp::DestroyAVSession() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_avsession_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, destroy_avsession, );

  // Execute
  _struct->destroy_avsession(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkMediaAVSessionAdapterCToCpp::RegistCallback(
    ArkWebRefPtr<ArkMediaAVSessionCallbackAdapter> callbackAdapter) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_avsession_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, regist_callback, false);

  // Execute
  return _struct->regist_callback(
      _struct, ArkMediaAVSessionCallbackAdapterCppToC::Invert(callbackAdapter));
}

ARK_WEB_NO_SANITIZE
bool ArkMediaAVSessionAdapterCToCpp::IsActivated() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_avsession_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_activated, false);

  // Execute
  return _struct->is_activated(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkMediaAVSessionAdapterCToCpp::Activate() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_avsession_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, activate, false);

  // Execute
  return _struct->activate(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkMediaAVSessionAdapterCToCpp::DeActivate() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_avsession_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, de_activate, );

  // Execute
  _struct->de_activate(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkMediaAVSessionAdapterCToCpp::SetMetadata(
    const ArkWebRefPtr<ArkMediaAVSessionMetadataAdapter> metadata) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_avsession_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_metadata, );

  // Execute
  _struct->set_metadata(
      _struct, ArkMediaAVSessionMetadataAdapterCppToC::Invert(metadata));
}

ARK_WEB_NO_SANITIZE
void ArkMediaAVSessionAdapterCToCpp::SetPlaybackState(int32_t state) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_avsession_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_playback_state, );

  // Execute
  _struct->set_playback_state(_struct, state);
}

ARK_WEB_NO_SANITIZE
void ArkMediaAVSessionAdapterCToCpp::SetPlaybackPosition(
    const ArkWebRefPtr<ArkMediaAVSessionPositionAdapter> position) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_avsession_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_playback_position, );

  // Execute
  _struct->set_playback_position(
      _struct, ArkMediaAVSessionPositionAdapterCppToC::Invert(position));
}

ArkMediaAVSessionAdapterCToCpp::ArkMediaAVSessionAdapterCToCpp() {
}

ArkMediaAVSessionAdapterCToCpp::~ArkMediaAVSessionAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkMediaAVSessionAdapterCToCpp, ArkMediaAVSessionAdapter,
    ark_media_avsession_adapter_t>::kBridgeType = ARK_MEDIA_AVSESSION_ADAPTER;

} // namespace OHOS::ArkWeb
