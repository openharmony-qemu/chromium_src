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

#include "ohos_nweb/ctocpp/ark_web_native_media_player_bridge_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::UpdateRect(double x, double y,
                                                     double width,
                                                     double height) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, update_rect, );

  // Execute
  _struct->update_rect(_struct, x, y, width, height);
}

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::Play() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, play, );

  // Execute
  _struct->play(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::Pause() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, pause, );

  // Execute
  _struct->pause(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::Seek(double time) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, seek, );

  // Execute
  _struct->seek(_struct, time);
}

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::SetVolume(double volume) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_volume, );

  // Execute
  _struct->set_volume(_struct, volume);
}

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::SetMuted(bool isMuted) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_muted, );

  // Execute
  _struct->set_muted(_struct, isMuted);
}

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::SetPlaybackRate(double playbackRate) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_playback_rate, );

  // Execute
  _struct->set_playback_rate(_struct, playbackRate);
}

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::Release() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release, );

  // Execute
  _struct->release(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::EnterFullScreen() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, enter_full_screen, );

  // Execute
  _struct->enter_full_screen(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNativeMediaPlayerBridgeCToCpp::ExitFullScreen() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_native_media_player_bridge_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, exit_full_screen, );

  // Execute
  _struct->exit_full_screen(_struct);
}

ArkWebNativeMediaPlayerBridgeCToCpp::ArkWebNativeMediaPlayerBridgeCToCpp() {
}

ArkWebNativeMediaPlayerBridgeCToCpp::~ArkWebNativeMediaPlayerBridgeCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkWebNativeMediaPlayerBridgeCToCpp, ArkWebNativeMediaPlayerBridge,
    ark_web_native_media_player_bridge_t>::kBridgeType =
    ARK_WEB_NATIVE_MEDIA_PLAYER_BRIDGE;

} // namespace OHOS::ArkWeb
