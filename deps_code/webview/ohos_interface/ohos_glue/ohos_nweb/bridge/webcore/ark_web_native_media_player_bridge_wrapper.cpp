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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_native_media_player_bridge_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeMediaPlayerBridgeWrapper::ArkWebNativeMediaPlayerBridgeWrapper(
    ArkWebRefPtr<ArkWebNativeMediaPlayerBridge> ark_web_native_vide_player_bridge)
    : ark_web_native_vide_player_bridge_(ark_web_native_vide_player_bridge)
{}

void ArkWebNativeMediaPlayerBridgeWrapper::UpdateRect(double x, double y, double width, double height)
{
    ark_web_native_vide_player_bridge_->UpdateRect(x, y, width, height);
}

void ArkWebNativeMediaPlayerBridgeWrapper::Play()
{
    ark_web_native_vide_player_bridge_->Play();
}

void ArkWebNativeMediaPlayerBridgeWrapper::Pause()
{
    ark_web_native_vide_player_bridge_->Pause();
}

void ArkWebNativeMediaPlayerBridgeWrapper::Seek(double time)
{
    ark_web_native_vide_player_bridge_->Seek(time);
}

void ArkWebNativeMediaPlayerBridgeWrapper::SetVolume(double volume)
{
    ark_web_native_vide_player_bridge_->SetVolume(volume);
}

void ArkWebNativeMediaPlayerBridgeWrapper::SetMuted(bool IsMuted)
{
    ark_web_native_vide_player_bridge_->SetMuted(IsMuted);
}

void ArkWebNativeMediaPlayerBridgeWrapper::SetPlaybackRate(double playbackRate)
{
    ark_web_native_vide_player_bridge_->SetPlaybackRate(playbackRate);
}

void ArkWebNativeMediaPlayerBridgeWrapper::Release()
{
    ark_web_native_vide_player_bridge_->Release();
}

void ArkWebNativeMediaPlayerBridgeWrapper::EnterFullScreen()
{
    ark_web_native_vide_player_bridge_->EnterFullScreen();
}

void ArkWebNativeMediaPlayerBridgeWrapper::ExitFullScreen()
{
    ark_web_native_vide_player_bridge_->ExitFullScreen();
}

void ArkWebNativeMediaPlayerBridgeWrapper::ResumeMediaPlayer()
{
    ark_web_native_vide_player_bridge_->ResumeMediaPlayer();
}

void ArkWebNativeMediaPlayerBridgeWrapper::SuspendMediaPlayer(ArkWebSuspendType type)
{
    ark_web_native_vide_player_bridge_->SuspendMediaPlayer(static_cast<int>(type));
}

} // namespace OHOS::ArkWeb
