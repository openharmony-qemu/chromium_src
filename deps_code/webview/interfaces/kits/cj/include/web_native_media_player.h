/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NATIVE_MEDIA_PLAYER_IMPL_FFI_H
#define NATIVE_MEDIA_PLAYER_IMPL_FFI_H

#include <cstdint>
#include <map>
#include "ffi_remote_data.h"
#include "web_errors.h"
#include "webview_javascript_result_callback.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_native_media_player.h"

namespace OHOS::Webview {
    class NativeMediaPlayerHandlerImpl : public OHOS::FFI::FFIData {
        DECL_TYPE(NativeMediaPlayerHandlerImpl, OHOS::FFI::FFIData)
    public:
        NativeMediaPlayerHandlerImpl()=default;
        NativeMediaPlayerHandlerImpl(int32_t nwebId, std::shared_ptr<NWeb::NWebNativeMediaPlayerHandler> handler);
        ~NativeMediaPlayerHandlerImpl() = default;
        void HandleStatusChanged(NWeb::PlaybackStatus status);
        void HandleVideoSizeChanged(double width, double height);
        void HandleError(NWeb::MediaError error, const char* errorMessage);
        void HandleSeekFinished();
        void HandleSeeking();
        void HandleFullScreenChanged(bool fullscreen);
        void HandleReadyStateChanged(NWeb::ReadyState state);
        void HandleNetworkStateChanged(NWeb::NetworkState state);
        void HandleEnded();
        void HandleBufferedEndTimeChanged(double bufferedEndTime);
        void HandleTimeUpdate(double playTime);
        void HandleDurationChanged(double duration);
        void HandlePlaybackRateChanged(double playbackRate);
        void HandleMutedChanged(bool muted);
        void HandleVolumeChanged(double volume);
        int32_t nwebId_ = -1;
        std::shared_ptr<NWeb::NWebNativeMediaPlayerHandler> handler_ = nullptr;
    };

    class RemoteMediaPlayer : public OHOS::FFI::RemoteData {
        CJ_REMOTE_CLASS(RemoteMediaPlayer)
    public:
        void UpdateRect(int64_t id, double x, double y, double width, double height);
        void Play(int64_t id);
        void Pause(int64_t id);
        void Seek(int64_t id, double time);
        void SetVolume(int64_t id, double volume);
        void SetMuted(int64_t id, bool isMuted);
        void SetPlaybackRate(int64_t id, double playbackRate);
        void Release(int64_t id);
        void EnterFullScreen(int64_t id);
        void ExitFullScreen(int64_t id);
        void ResumeMediaPlayer(int64_t id);
        void SuspendMediaPlayer(int64_t id, NWeb::SuspendType type);
    };

    class NWebNativeMediaPlayerBridgeImpl : public NWeb::NWebNativeMediaPlayerBridge, public OHOS::FFI::FFIData {
        DECL_TYPE(NWebNativeMediaPlayerBridgeImpl, OHOS::FFI::FFIData)
    public:
        NWebNativeMediaPlayerBridgeImpl(int64_t nwebId, sptr<RemoteMediaPlayer> remoteMediaPlayer);
        ~NWebNativeMediaPlayerBridgeImpl() override = default;
        void UpdateRect(double x, double y, double width, double height) override;
        void Play() override;
        void Pause() override;
        void Seek(double time) override;
        void SetVolume(double volume) override;
        void SetMuted(bool isMuted) override;
        void SetPlaybackRate(double playbackRate) override;
        void Release() override;
        void EnterFullScreen() override;
        void ExitFullScreen() override;
        void ResumeMediaPlayer() override;
        void SuspendMediaPlayer(NWeb::SuspendType type) override;
    private:
        int64_t nwebId_;
        sptr<RemoteMediaPlayer> remoteMediaPlayer_;
    };

    class NWebCreateNativeMediaPlayerCallbackImpl :
    public NWeb::NWebCreateNativeMediaPlayerCallback, public OHOS::FFI::FFIData {
        DECL_TYPE(NWebCreateNativeMediaPlayerCallbackImpl, OHOS::FFI::FFIData)
        public:
            explicit NWebCreateNativeMediaPlayerCallbackImpl(int32_t nwebId,
                std::function<int64_t(int64_t, CMediaInfo)> callback);
            ~NWebCreateNativeMediaPlayerCallbackImpl() = default;

            std::shared_ptr<NWeb::NWebNativeMediaPlayerBridge> OnCreate(
                std::shared_ptr<NWeb::NWebNativeMediaPlayerHandler> handler,
                std::shared_ptr<NWeb::NWebMediaInfo> mediaInfo) override;
            CNativeMediaPlayerSurfaceInfo ConstructSurfaceInfo(
                std::shared_ptr<NWeb::NWebNativeMediaPlayerSurfaceInfo> surfaceInfo_);
            CArrMediaSourceInfo ConstructMediaSrcList(
                std::vector<std::shared_ptr<NWeb::NWebMediaSourceInfo>> mediaSrcList_);
            CArrString ConstructControlList(std::vector<std::string> controlList_);
            ArrMapItem ConstructMap(std::map<std::string, std::string> headers_);

        private:
            int32_t nwebId_ = -1;
            std::function<int64_t(int64_t, CMediaInfo)> callback_ = nullptr;
    };
}
#endif // NATIVE_MEDIA_PLAYER_IMPL_FFI_H