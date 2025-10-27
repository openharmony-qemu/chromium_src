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

#include "ohos_nweb/bridge/ark_web_native_media_player_handler_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebMediaError = OHOS::NWeb::MediaError;
using ArkWebReadyState = OHOS::NWeb::ReadyState;
using ArkWebNetworkState = OHOS::NWeb::NetworkState;
using ArkWebPlaybackStatus = OHOS::NWeb::PlaybackStatus;

ArkWebNativeMediaPlayerHandlerImpl::ArkWebNativeMediaPlayerHandlerImpl(
    std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerHandler> nweb_native_media_player_handler)
    : nweb_native_media_player_handler_(nweb_native_media_player_handler)
{}

void ArkWebNativeMediaPlayerHandlerImpl::HandleStatusChanged(int status)
{
    nweb_native_media_player_handler_->HandleStatusChanged(static_cast<ArkWebPlaybackStatus>(status));
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleVolumeChanged(double volume)
{
    nweb_native_media_player_handler_->HandleVolumeChanged(volume);
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleMutedChanged(bool isMuted)
{
    nweb_native_media_player_handler_->HandleMutedChanged(isMuted);
}

void ArkWebNativeMediaPlayerHandlerImpl::HandlePlaybackRateChanged(double playbackRate)
{
    nweb_native_media_player_handler_->HandlePlaybackRateChanged(playbackRate);
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleDurationChanged(double duration)
{
    nweb_native_media_player_handler_->HandleDurationChanged(duration);
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleTimeUpdate(double playTime)
{
    nweb_native_media_player_handler_->HandleTimeUpdate(playTime);
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleBufferedEndTimeChanged(double bufferedEndTime)
{
    nweb_native_media_player_handler_->HandleBufferedEndTimeChanged(bufferedEndTime);
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleEnded()
{
    nweb_native_media_player_handler_->HandleEnded();
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleNetworkStateChanged(int state)
{
    nweb_native_media_player_handler_->HandleNetworkStateChanged(static_cast<ArkWebNetworkState>(state));
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleReadyStateChanged(int state)
{
    nweb_native_media_player_handler_->HandleReadyStateChanged(static_cast<ArkWebReadyState>(state));
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleFullScreenChanged(bool isFullScreen)
{
    nweb_native_media_player_handler_->HandleFullScreenChanged(isFullScreen);
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleSeeking()
{
    nweb_native_media_player_handler_->HandleSeeking();
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleSeekFinished()
{
    nweb_native_media_player_handler_->HandleSeekFinished();
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleError(int error, const ArkWebString& message)
{
    nweb_native_media_player_handler_->HandleError(
        static_cast<ArkWebMediaError>(error), ArkWebStringStructToClass(message));
}

void ArkWebNativeMediaPlayerHandlerImpl::HandleVideoSizeChanged(double width, double height)
{
    nweb_native_media_player_handler_->HandleVideoSizeChanged(width, height);
}

} // namespace OHOS::ArkWeb
