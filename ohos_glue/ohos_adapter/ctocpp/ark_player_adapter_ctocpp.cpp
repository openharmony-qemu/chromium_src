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

#include "ohos_adapter/ctocpp/ark_player_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_player_callback_adapter_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_iconsumer_surface_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::SetPlayerCallback(
    ArkWebRefPtr<ArkPlayerCallbackAdapter> callbackAdapter) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_player_callback, 0);

  // Execute
  return _struct->set_player_callback(
      _struct, ArkPlayerCallbackAdapterCppToC::Invert(callbackAdapter));
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::SetSource(const ArkWebString &url) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_source1, 0);

  // Execute
  return _struct->set_source1(_struct, &url);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::SetSource(int32_t fd, int64_t offset,
                                          int64_t size) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_source2, 0);

  // Execute
  return _struct->set_source2(_struct, fd, offset, size);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::SetVideoSurface(
    ArkWebRefPtr<ArkIConsumerSurfaceAdapter> cSurfaceAdapter) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_video_surface, 0);

  // Execute
  return _struct->set_video_surface(
      _struct, ArkIConsumerSurfaceAdapterCToCpp::Revert(cSurfaceAdapter));
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::SetVolume(float leftVolume, float rightVolume) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_volume, 0);

  // Execute
  return _struct->set_volume(_struct, leftVolume, rightVolume);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::Seek(int32_t mSeconds, int32_t mode) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, seek, 0);

  // Execute
  return _struct->seek(_struct, mSeconds, mode);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::Play() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, play, 0);

  // Execute
  return _struct->play(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::Pause() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, pause, 0);

  // Execute
  return _struct->pause(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::PrepareAsync() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, prepare_async, 0);

  // Execute
  return _struct->prepare_async(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::GetCurrentTime(int32_t &currentTime) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_current_time, 0);

  // Execute
  return _struct->get_current_time(_struct, &currentTime);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::GetDuration(int32_t &duration) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_duration, 0);

  // Execute
  return _struct->get_duration(_struct, &duration);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPlayerAdapterCToCpp::SetPlaybackSpeed(int32_t mode) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_player_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_playback_speed, 0);

  // Execute
  return _struct->set_playback_speed(_struct, mode);
}

ArkPlayerAdapterCToCpp::ArkPlayerAdapterCToCpp() {
}

ArkPlayerAdapterCToCpp::~ArkPlayerAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkPlayerAdapterCToCpp, ArkPlayerAdapter,
                           ark_player_adapter_t>::kBridgeType =
        ARK_PLAYER_ADAPTER;

} // namespace OHOS::ArkWeb
