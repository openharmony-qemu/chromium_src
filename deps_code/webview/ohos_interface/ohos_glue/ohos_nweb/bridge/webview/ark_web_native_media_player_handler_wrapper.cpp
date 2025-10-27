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

#include "ohos_nweb/bridge/ark_web_native_media_player_handler_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeMediaPlayerHandlerWrapper::ArkWebNativeMediaPlayerHandlerWrapper(
    ArkWebRefPtr<ArkWebNativeMediaPlayerHandler> ark_web_native_media_player_handler)
    : ark_web_native_media_player_handler_(ark_web_native_media_player_handler)
{}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleStatusChanged(ArkWebPlaybackStatus status)
{
    ark_web_native_media_player_handler_->HandleStatusChanged(static_cast<int>(status));
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleVolumeChanged(double volume)
{
    ark_web_native_media_player_handler_->HandleVolumeChanged(volume);
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleMutedChanged(bool isMuted)
{
    ark_web_native_media_player_handler_->HandleMutedChanged(isMuted);
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandlePlaybackRateChanged(double playbackRate)
{
    ark_web_native_media_player_handler_->HandlePlaybackRateChanged(playbackRate);
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleDurationChanged(double duration)
{
    ark_web_native_media_player_handler_->HandleDurationChanged(duration);
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleTimeUpdate(double playTime)
{
    ark_web_native_media_player_handler_->HandleTimeUpdate(playTime);
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleBufferedEndTimeChanged(double bufferedEndTime)
{
    ark_web_native_media_player_handler_->HandleBufferedEndTimeChanged(bufferedEndTime);
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleEnded()
{
    ark_web_native_media_player_handler_->HandleEnded();
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleNetworkStateChanged(ArkWebNetworkState state)
{
    ark_web_native_media_player_handler_->HandleNetworkStateChanged(static_cast<int>(state));
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleReadyStateChanged(ArkWebReadyState state)
{
    ark_web_native_media_player_handler_->HandleReadyStateChanged(static_cast<int>(state));
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleFullScreenChanged(bool isFullScreen)
{
    ark_web_native_media_player_handler_->HandleFullScreenChanged(isFullScreen);
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleSeeking()
{
    ark_web_native_media_player_handler_->HandleSeeking();
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleSeekFinished()
{
    ark_web_native_media_player_handler_->HandleSeekFinished();
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleError(ArkWebMediaError error, const std::string& message)
{
    ArkWebString stMessage = ArkWebStringClassToStruct(message);

    ark_web_native_media_player_handler_->HandleError(static_cast<int>(error), stMessage);

    ArkWebStringStructRelease(stMessage);
}

void ArkWebNativeMediaPlayerHandlerWrapper::HandleVideoSizeChanged(double width, double height)
{
    ark_web_native_media_player_handler_->HandleVideoSizeChanged(width, height);
}

} // namespace OHOS::ArkWeb
