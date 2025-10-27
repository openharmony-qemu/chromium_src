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

#include "ohos_nweb/bridge/ark_web_native_media_player_bridge_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebSuspendType = OHOS::NWeb::SuspendType;

ArkWebNativeMediaPlayerBridgeImpl::ArkWebNativeMediaPlayerBridgeImpl(
    std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerBridge> nweb_native_vide_player_bridge)
    : nweb_native_vide_player_bridge_(nweb_native_vide_player_bridge)
{}

void ArkWebNativeMediaPlayerBridgeImpl::UpdateRect(double x, double y, double width, double height)
{
    nweb_native_vide_player_bridge_->UpdateRect(x, y, width, height);
}

void ArkWebNativeMediaPlayerBridgeImpl::Play()
{
    nweb_native_vide_player_bridge_->Play();
}

void ArkWebNativeMediaPlayerBridgeImpl::Pause()
{
    nweb_native_vide_player_bridge_->Pause();
}

void ArkWebNativeMediaPlayerBridgeImpl::Seek(double time)
{
    nweb_native_vide_player_bridge_->Seek(time);
}

void ArkWebNativeMediaPlayerBridgeImpl::SetVolume(double volume)
{
    nweb_native_vide_player_bridge_->SetVolume(volume);
}

void ArkWebNativeMediaPlayerBridgeImpl::SetMuted(bool IsMuted)
{
    nweb_native_vide_player_bridge_->SetMuted(IsMuted);
}

void ArkWebNativeMediaPlayerBridgeImpl::SetPlaybackRate(double playbackRate)
{
    nweb_native_vide_player_bridge_->SetPlaybackRate(playbackRate);
}

void ArkWebNativeMediaPlayerBridgeImpl::Release()
{
    nweb_native_vide_player_bridge_->Release();
}

void ArkWebNativeMediaPlayerBridgeImpl::EnterFullScreen()
{
    nweb_native_vide_player_bridge_->EnterFullScreen();
}

void ArkWebNativeMediaPlayerBridgeImpl::ExitFullScreen()
{
    nweb_native_vide_player_bridge_->ExitFullScreen();
}

void ArkWebNativeMediaPlayerBridgeImpl::ResumeMediaPlayer()
{
    nweb_native_vide_player_bridge_->ResumeMediaPlayer();
}

void ArkWebNativeMediaPlayerBridgeImpl::SuspendMediaPlayer(int type)
{
    nweb_native_vide_player_bridge_->SuspendMediaPlayer(static_cast<ArkWebSuspendType>(type));
}

} // namespace OHOS::ArkWeb
