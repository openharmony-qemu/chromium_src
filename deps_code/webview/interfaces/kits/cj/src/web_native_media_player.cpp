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

#include <cstdint>
#include <string>
#include "webview_javascript_execute_callback.h"
#include "webview_javascript_result_callback.h"
#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"
#include "cj_common_ffi.h"
#include "ffi_remote_data.h"
#include "application_context.h"
#include "webview_log.h"
#include "webview_utils.h"
#include "nweb_store_web_archive_callback.h"
#include "web_native_media_player.h"
#include "web_runtime_delegate.h"

namespace OHOS::Webview {

NativeMediaPlayerHandlerImpl::NativeMediaPlayerHandlerImpl(
    int32_t nwebId, std::shared_ptr<NWeb::NWebNativeMediaPlayerHandler> handler)
    : nwebId_(nwebId), handler_(handler) {}

void NativeMediaPlayerHandlerImpl::HandleVideoSizeChanged(double width, double height)
{
    if (handler_) {
        handler_->HandleVideoSizeChanged(width, height);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleVideoSizeChanged is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleError(NWeb::MediaError error, const char* errorMessage)
{
    if (handler_) {
        handler_->HandleError(error, errorMessage);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleError is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleSeekFinished()
{
    if (handler_) {
        handler_->HandleSeekFinished();
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleSeekFinished is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleSeeking()
{
    if (handler_) {
        handler_->HandleSeeking();
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleSeeking is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleFullScreenChanged(bool fullscreen)
{
    if (handler_) {
        handler_->HandleFullScreenChanged(fullscreen);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleFullScreenChanged is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleReadyStateChanged(NWeb::ReadyState state)
{
    if (handler_) {
        handler_->HandleReadyStateChanged(state);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleReadyStateChanged is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleNetworkStateChanged(NWeb::NetworkState state)
{
    if (handler_) {
        handler_->HandleNetworkStateChanged(state);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleNetworkStateChanged is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleEnded()
{
    if (handler_) {
        handler_->HandleEnded();
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleEnded is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleBufferedEndTimeChanged(double bufferedEndTime)
{
    if (handler_) {
        handler_->HandleBufferedEndTimeChanged(bufferedEndTime);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleBufferedEndTimeChanged is null");
    }
}

void NativeMediaPlayerHandlerImpl::NativeMediaPlayerHandlerImpl::HandleTimeUpdate(double playTime)
{
    if (handler_) {
        handler_->HandleTimeUpdate(playTime);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleTimeUpdate is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleDurationChanged(double duration)
{
    if (handler_) {
        handler_->HandleDurationChanged(duration);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleDurationChanged is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandlePlaybackRateChanged(double playbackRate)
{
    if (handler_) {
        handler_->HandlePlaybackRateChanged(playbackRate);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandlePlaybackRateChanged is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleMutedChanged(bool muted)
{
    if (handler_) {
        handler_->HandleMutedChanged(muted);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleMutedChanged is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleVolumeChanged(double volume)
{
    if (handler_) {
        handler_->HandleVolumeChanged(volume);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleVolumeChanged is null");
    }
}

void NativeMediaPlayerHandlerImpl::HandleStatusChanged(NWeb::PlaybackStatus status)
{
    if (handler_) {
        handler_->HandleStatusChanged(status);
    } else {
        WEBVIEWLOGE("NativeMediaPlayerHandlerImpl::HandleStatusChanged is null");
    }
}

// RemoteMediaPlayer
void RemoteMediaPlayer::ExitFullScreen(int64_t id)
{
    auto exitFullscreen = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeExitFullscreen;
    if (!exitFullscreen) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::ExitFullScreen is empty.");
        return;
    }
    exitFullscreen(id);
}

void RemoteMediaPlayer::EnterFullScreen(int64_t id)
{
    auto enterFullscreen = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeEnterFullscreen;
    if (!enterFullscreen) {
        WEBVIEWLOGE("RemoteMediaPlayer::EnterFullScreen is empty.");
        return;
    }
    enterFullscreen(id);
}

void RemoteMediaPlayer::Release(int64_t id)
{
    auto release = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeRelease;
    if (!release) {
        WEBVIEWLOGE("RemoteMediaPlayer::Release is empty.");
        return;
    }
    release(id);
}

void RemoteMediaPlayer::SetPlaybackRate(int64_t id, double playbackRate)
{
    auto setPlaybackRate = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeSetPlaybackRate;
    if (!setPlaybackRate) {
        WEBVIEWLOGE("RemoteMediaPlayer::SetPlaybackRate is empty.");
        return;
    }
    setPlaybackRate(id, playbackRate);
}

void RemoteMediaPlayer::SetMuted(int64_t id, bool isMuted)
{
    auto setMuted = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeSetMuted;
    if (!setMuted) {
        WEBVIEWLOGE("RemoteMediaPlayer::setMuted is empty.");
        return;
    }
    setMuted(id, isMuted);
}

void RemoteMediaPlayer::SetVolume(int64_t id, double volume)
{
    auto setVolume = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeSetVolume;
    if (!setVolume) {
        WEBVIEWLOGE("RemoteMediaPlayer::SetVolume is empty.");
        return;
    }
    setVolume(id, volume);
}

void RemoteMediaPlayer::Seek(int64_t id, double time)
{
    auto seek = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeSeek;
    if (!seek) {
        WEBVIEWLOGE("RemoteMediaPlayer::Seek is empty.");
        return;
    }
    seek(id, time);
}

void RemoteMediaPlayer::Pause(int64_t id)
{
    auto pause = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgePause;
    if (!pause) {
        WEBVIEWLOGE("RemoteMediaPlayer::Pause is empty.");
        return;
    }
    pause(id);
}

void RemoteMediaPlayer::Play(int64_t id)
{
    auto play = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgePlay;
    if (!play) {
        WEBVIEWLOGE("RemoteMediaPlayer::Play is empty.");
        return;
    }
    play(id);
}

void RemoteMediaPlayer::UpdateRect(int64_t id, double x, double y, double width, double height)
{
    auto updateRect = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeUpdateRect;
    if (!updateRect) {
        WEBVIEWLOGE("RemoteMediaPlayer::UpdateRect is empty.");
        return;
    }
    CRectEvent rectEvent = { x, y, width, height };
    updateRect(id, rectEvent);
}

void RemoteMediaPlayer::ResumeMediaPlayer(int64_t id)
{
    auto resumePlayer = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeResumePlayer;
    if (!resumePlayer) {
        WEBVIEWLOGE("RemoteMediaPlayer::ResumeMediaPlayer is empty.");
        return;
    }
    resumePlayer(id);
}

void RemoteMediaPlayer::SuspendMediaPlayer(int64_t id, NWeb::SuspendType type)
{
    auto suspendPlayer = WebRuntimeDelegate::GetInstance().GetCJFuncs().atCOHOSNativeMediaPlayerBridgeSuspendPlayer;
    if (!suspendPlayer) {
        WEBVIEWLOGE("RemoteMediaPlayer::SuspendMediaPlayer is empty.");
        return;
    }
    suspendPlayer(id, static_cast<int32_t>(type));
}

//NWebNativeMediaPlayerBridgeImpl
NWebNativeMediaPlayerBridgeImpl::NWebNativeMediaPlayerBridgeImpl(int64_t nwebId,
    sptr<RemoteMediaPlayer> remoteMediaPlayer) : nwebId_(nwebId), remoteMediaPlayer_(remoteMediaPlayer) {}

void NWebNativeMediaPlayerBridgeImpl::ExitFullScreen()
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::ExitFullScreen is nullptr.");
        return;
    }
    remoteMediaPlayer_->ExitFullScreen(nwebId_);
}

void NWebNativeMediaPlayerBridgeImpl::EnterFullScreen()
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::EnterFullScreen is nullptr.");
        return;
    }
    remoteMediaPlayer_->EnterFullScreen(nwebId_);
}

void NWebNativeMediaPlayerBridgeImpl::Release()
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::Release is nullptr.");
        return;
    }
    remoteMediaPlayer_->Release(nwebId_);
}

void NWebNativeMediaPlayerBridgeImpl::SetPlaybackRate(double playbackRate)
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::SetPlaybackRate is nullptr.");
        return;
    }
    remoteMediaPlayer_->SetPlaybackRate(nwebId_, playbackRate);
}

void NWebNativeMediaPlayerBridgeImpl::SetMuted(bool isMuted)
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::SetMuted is nullptr.");
        return;
    }
    remoteMediaPlayer_->SetMuted(nwebId_, isMuted);
}

void NWebNativeMediaPlayerBridgeImpl::SetVolume(double volume)
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::SetVolume is nullptr.");
        return;
    }
    remoteMediaPlayer_->SetVolume(nwebId_, volume);
}

void NWebNativeMediaPlayerBridgeImpl::Seek(double time)
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::Seek is nullptr.");
        return;
    }
    remoteMediaPlayer_->Seek(nwebId_, time);
}

void NWebNativeMediaPlayerBridgeImpl::Pause()
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::Pause is nullptr.");
        return;
    }
    remoteMediaPlayer_->Pause(nwebId_);
}

void NWebNativeMediaPlayerBridgeImpl::Play()
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::Play is nullptr.");
        return;
    }
    remoteMediaPlayer_->Play(nwebId_);
}

void NWebNativeMediaPlayerBridgeImpl::UpdateRect(double x, double y, double width, double height)
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::UpdateRect is nullptr.");
        return;
    }
    remoteMediaPlayer_->UpdateRect(nwebId_, x, y, width, height);
}

void NWebNativeMediaPlayerBridgeImpl::ResumeMediaPlayer()
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::ResumeMediaPlayer is nullptr.");
        return;
    }
    remoteMediaPlayer_->ResumeMediaPlayer(nwebId_);
}

void NWebNativeMediaPlayerBridgeImpl::SuspendMediaPlayer(NWeb::SuspendType type)
{
    if (remoteMediaPlayer_ == nullptr) {
        WEBVIEWLOGE("NWebNativeMediaPlayerBridgeImpl::SuspendMediaPlayer is nullptr.");
        return;
    }
    remoteMediaPlayer_->SuspendMediaPlayer(nwebId_, type);
}

NWebCreateNativeMediaPlayerCallbackImpl::NWebCreateNativeMediaPlayerCallbackImpl(
    int32_t nwebId, std::function<int64_t(int64_t, CMediaInfo)> callback)
    : nwebId_(nwebId), callback_(callback)
{}

CArrMediaSourceInfo NWebCreateNativeMediaPlayerCallbackImpl::ConstructMediaSrcList(
    std::vector<std::shared_ptr<NWeb::NWebMediaSourceInfo>> mediaSrcList_)
{
    CMediaSourceInfo* result = static_cast<CMediaSourceInfo*>(malloc(sizeof(CMediaSourceInfo) * mediaSrcList_.size()));
    if (result == nullptr) {
        return {};
    }
    for (size_t i = 0; i < mediaSrcList_.size(); i++) {
        const char* format = MallocCString(mediaSrcList_[i]->GetFormat());
        const char* source = MallocCString(mediaSrcList_[i]->GetSource());
        int32_t type = static_cast<int32_t>(mediaSrcList_[i]->GetType());
        CMediaSourceInfo cMediaSourceInfo = {format, source, type};
        result[i] = cMediaSourceInfo;
    }
    CArrMediaSourceInfo mediaSrcList = {result, mediaSrcList_.size()};
    return mediaSrcList;
}

CNativeMediaPlayerSurfaceInfo NWebCreateNativeMediaPlayerCallbackImpl::ConstructSurfaceInfo(
    std::shared_ptr<NWeb::NWebNativeMediaPlayerSurfaceInfo> surfaceInfo_)
{
    CNativeMediaPlayerSurfaceInfo surfaceInfo;
    CRectEvent event;
    surfaceInfo.id = MallocCString(surfaceInfo_->GetId());
    event.x = surfaceInfo_->GetX();
    event.y = surfaceInfo_->GetY();
    event.width = surfaceInfo_->GetWidth();
    event.height = surfaceInfo_->GetHeight();
    surfaceInfo.rect = event;
    return surfaceInfo;
}

CArrString NWebCreateNativeMediaPlayerCallbackImpl::ConstructControlList(
    std::vector<std::string> controlList_)
{
    char** result = static_cast<char**>(malloc(sizeof(char *) * controlList_.size()));
    if (result == nullptr) {
        return {};
    }
    for (size_t i = 0; i < controlList_.size(); i++) {
        result[i] = MallocCString(controlList_[i]);
    }
    CArrString controlList = {result, controlList_.size()};
    return controlList;
}

ArrMapItem NWebCreateNativeMediaPlayerCallbackImpl::ConstructMap(
    std::map<std::string, std::string> headers_)
{
    MapItem* result3 = static_cast<MapItem*>(malloc(sizeof(MapItem) * headers_.size()));
    if (result3 == nullptr) {
        return {};
    }
    size_t i = 0;
    for (const auto& pair : headers_) {
        MapItem mapItem = {MallocCString(pair.first), MallocCString(pair.second)};
        result3[i] = mapItem;
        i++;
    }
    ArrMapItem headers = {result3, headers_.size()};
    return headers;
}

std::shared_ptr<NWeb::NWebNativeMediaPlayerBridge> NWebCreateNativeMediaPlayerCallbackImpl::OnCreate(
    std::shared_ptr<NWeb::NWebNativeMediaPlayerHandler> handler, std::shared_ptr<NWeb::NWebMediaInfo> mediaInfo)
{
    WEBVIEWLOGD("begin to create native media player,nweb id is %{public}d", nwebId_);
    if (!callback_) {
        WEBVIEWLOGE("callback is null,nweb id is %{public}d", nwebId_);
        return nullptr;
    }
    if (!handler || !mediaInfo) {
        WEBVIEWLOGE("param is null,nweb id is %{public}d", nwebId_);
        return nullptr;
    }
    auto handlerImpl = FFIData::Create<NativeMediaPlayerHandlerImpl>(nwebId_, handler);
    if (!handlerImpl) {
        WEBVIEWLOGE("Create handlerImpl failed, nweb id is %{public}d", nwebId_);
        return nullptr;
    }
    int64_t handlerId = handlerImpl->GetID();
    std::string embedID_ = mediaInfo->GetEmbedId();
    const char* embedID = MallocCString(embedID_);
    int32_t mediaType = static_cast<int32_t>(mediaInfo->GetMediaType());
    CArrMediaSourceInfo mediaSrcList = ConstructMediaSrcList(mediaInfo->GetSourceInfos());
    CNativeMediaPlayerSurfaceInfo surfaceInfo = ConstructSurfaceInfo(mediaInfo->GetSurfaceInfo());
    bool controlsShown = mediaInfo->GetIsControlsShown();
    CArrString controlList = ConstructControlList(mediaInfo->GetControls());
    bool muted = mediaInfo->GetIsMuted();
    std::string posterUrl_ = mediaInfo->GetPosterUrl();
    const char* posterUrl = MallocCString(posterUrl_);
    int32_t preload = static_cast<int32_t>(mediaInfo->GetPreload());
    ArrMapItem headers = ConstructMap(mediaInfo->GetHeaders());
    ArrMapItem attributes = ConstructMap(mediaInfo->GetAttributes());
    CMediaInfo cMediaInfo = {embedID, mediaType, mediaSrcList, surfaceInfo,
        controlsShown, controlList, muted, posterUrl, preload, headers, attributes};
    int64_t nativeMediaPlayerBridgeId = callback_(handlerId, cMediaInfo);
    auto remoteMediaPlayer = OHOS::FFI::RemoteData::Create<OHOS::Webview::RemoteMediaPlayer>(nativeMediaPlayerBridgeId);
    return std::make_shared<NWebNativeMediaPlayerBridgeImpl>(nativeMediaPlayerBridgeId, remoteMediaPlayer);
}
} // namespace OHOS::Webview