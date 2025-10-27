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

#ifndef ARK_WEB_NATIVE_MEDIA_PLAYER_HANDLER_WRAPPER_H_
#define ARK_WEB_NATIVE_MEDIA_PLAYER_HANDLER_WRAPPER_H_
#pragma once

#include "include/nweb_native_media_player.h"
#include "ohos_nweb/include/ark_web_native_media_player_handler.h"

namespace OHOS::ArkWeb {

using ArkWebMediaError = OHOS::NWeb::MediaError;
using ArkWebReadyState = OHOS::NWeb::ReadyState;
using ArkWebNetworkState = OHOS::NWeb::NetworkState;
using ArkWebPlaybackStatus = OHOS::NWeb::PlaybackStatus;

class ArkWebNativeMediaPlayerHandlerWrapper : public OHOS::NWeb::NWebNativeMediaPlayerHandler {
public:
    ArkWebNativeMediaPlayerHandlerWrapper(
        ArkWebRefPtr<ArkWebNativeMediaPlayerHandler> ark_web_native_media_player_handler);
    ~ArkWebNativeMediaPlayerHandlerWrapper() = default;

    void HandleStatusChanged(ArkWebPlaybackStatus status) override;

    void HandleVolumeChanged(double volume) override;

    void HandleMutedChanged(bool isMuted) override;

    void HandlePlaybackRateChanged(double playbackRate) override;

    void HandleDurationChanged(double duration) override;

    void HandleTimeUpdate(double playTime) override;

    void HandleBufferedEndTimeChanged(double bufferedEndTime) override;

    void HandleEnded() override;

    void HandleNetworkStateChanged(ArkWebNetworkState state) override;

    void HandleReadyStateChanged(ArkWebReadyState state) override;

    void HandleFullScreenChanged(bool isFullScreen) override;

    void HandleSeeking() override;

    void HandleSeekFinished() override;

    void HandleError(ArkWebMediaError error, const std::string& message) override;

    void HandleVideoSizeChanged(double width, double height) override;

private:
    ArkWebRefPtr<ArkWebNativeMediaPlayerHandler> ark_web_native_media_player_handler_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_MEDIA_PLAYER_HANDLER_WRAPPER_H_
