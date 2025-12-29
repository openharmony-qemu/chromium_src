// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_MEDIA_WEB_CONTENTS_OBSERVER_H_
#error "must be in include form CONTENT_BROWSER_MEDIA_MEDIA_WEB_CONTENTS_OBSERVER_H_"
#endif
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "content/public/browser/media_player_listener.h"
#endif // ARKWEB_VIDEO_ASSISTANT

namespace content {

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
void MediaWebContentsObserver::MediaPlayerObserverHostImpl::OnMediaPlayerGone() {
  PlayerInfo* player_info = GetPlayerInfo();
  if (!player_info)
    return;

  player_info->SetIsPlayerGone();
}
#endif
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
void MediaWebContentsObserver::SetHtmlPlayEnabled(bool enabled) {
  if (session_controllers_manager_) {
    session_controllers_manager_->SetHtmlPlayEnabled(enabled);
  }
}
#endif // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
bool MediaWebContentsObserver::IsPlayerIdInMediaPlayerRemotesMap(const MediaPlayerId& player_id) {
  auto it = media_player_remotes_.find(player_id);
  if (it != media_player_remotes_.end())
    return true;

  return false;
}
#endif // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER) || BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
bool MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    IsWebContentsAvailable() {
  if (!media_web_contents_observer_) {
    return false;
  }

  auto* web_contents_impl = media_web_contents_observer_->web_contents_impl();
  if (!web_contents_impl) {
    return false;
  }

  return web_contents_impl->AsWebContentsImplExt() != nullptr;
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    UpdateLayerRect(const gfx::Rect& rect) {
  if (IsWebContentsAvailable()) {
    media_web_contents_observer_->web_contents_impl()->AsWebContentsImplExt()->UpdateLayerRect(
        media_player_id_, rect);
  }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    FullscreenChanged(bool is_fullscreen) {
  if (IsWebContentsAvailable()) {
    media_web_contents_observer_->web_contents_impl()->AsWebContentsImplExt()->FullScreenChanged(
        media_player_id_, is_fullscreen);
  }
}

void MediaWebContentsObserver::RequestEnterFullscreen(const MediaPlayerId& player_id) {
  const auto iter = media_player_remotes_.find(player_id);
  if (iter == media_player_remotes_.end()) {
    LOG(WARNING) << "RequestEnterFullscreen failed";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(WARNING) << "RequestEnterFullscreen failed";
#endif // ARKWEB_LOGGER_REPORT
    return;
  }

  if (iter->second) {
    iter->second->RequestEnterFullscreen();
  }
}

void MediaWebContentsObserver::RequestExitFullscreen(const MediaPlayerId& player_id) {
  const auto iter = media_player_remotes_.find(player_id);
  if (iter == media_player_remotes_.end()) {
    LOG(WARNING) << "RequestExitFullscreen failed";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(WARNING) << "RequestEnterFullscreen failed";
#endif // ARKWEB_LOGGER_REPORT
    return;
  }

  if (iter->second) {
    iter->second->RequestExitFullscreen();
  }
}
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
void MediaWebContentsObserver::MediaPlayerObserverHostImpl::OnGetMediaTitle(
    const std::string& data) {
  if (media_web_contents_observer_ &&
      media_web_contents_observer_->web_contents_impl()) {
    media_web_contents_observer_->web_contents_impl()->SetMediaTitle(data);
  }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::OnGetVideoPoster(
    const std::string& data) {
  if (media_web_contents_observer_ &&
      media_web_contents_observer_->web_contents_impl()) {
    media_web_contents_observer_->web_contents_impl()->SetVideoPoster(data);
  }
}

// LCOV_EXCL_START
void MediaWebContentsObserver::MediaPlayerObserverHostImpl::OnInitMediaTitle() {
  if (media_web_contents_observer_ &&
      media_web_contents_observer_->web_contents_impl()) {
    media_web_contents_observer_->web_contents_impl()->SetMediaTitle("");
  }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::OnInitVideoPoster() {
  if (media_web_contents_observer_ &&
      media_web_contents_observer_->web_contents_impl()) {
    media_web_contents_observer_->web_contents_impl()->SetVideoPoster("");
  }
}
// LCOV_EXCL_STOP

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::OnEndAVSession(
    bool is_hidden) {
  if (is_hidden && media_web_contents_observer_ &&
      media_web_contents_observer_->session_controllers_manager()) {
    media_web_contents_observer_->session_controllers_manager()->OnEndAVSession(
        media_player_id_, is_hidden);
  }
}
#endif // ARKWEB_MEDIA_AVSESSION

bool MediaWebContentsObserver::IsMediaPlaying(const MediaPlayerId& player_id) {
  auto player_info = GetPlayerInfo(player_id);
  return player_info && player_info->is_playing();
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MediaWebContentsObserver::SetPlaybackRate(double playback_rate,
                                               const MediaPlayerId& player_id) {
  const auto iter = media_player_remotes_.find(player_id);
  if (iter == media_player_remotes_.end()) {
    return;
  }

  if (iter->second) {
    iter->second->SetPlaybackRate(playback_rate);
  }
}

void MediaWebContentsObserver::SetVolume(double volume,
                                         const MediaPlayerId& player_id)
{
  const auto iter = media_player_remotes_.find(player_id);
  if (iter == media_player_remotes_.end()) {
    return;
  }

  if (iter->second) {
    iter->second->SetVolume(volume);
  }
}

double MediaWebContentsObserver::GetVolume(const MediaPlayerId& player_id)
{
  double volume = -1.0;
  const auto iter = media_player_remotes_.find(player_id);
  if (iter == media_player_remotes_.end()) {
    return volume;
  }

  if (iter->second) {
    iter->second->GetVolume(&volume);
  }

  return volume;
}
void MediaWebContentsObserver::RequestFullScreen(
    bool enable,
    const MediaPlayerId& player_id) {
  const auto iter = media_player_remotes_.find(player_id);
  if (iter == media_player_remotes_.end()) {
    return;
  }

  if (!iter->second) {
    return;
  }

  if (enable) {
    iter->second->RequestEnterFullscreen();
  } else {
    iter->second->RequestExitFullscreen();
  }
}

void MediaWebContentsObserver::RequestDownloadUrl(
    const MediaPlayerId& player_id) {
  const auto iter = media_player_remotes_.find(player_id);
  if (iter == media_player_remotes_.end()) {
    return;
  }

  if (iter->second) {
    iter->second->RequestDownloadUrl();
  }
}

void MediaWebContentsObserver::HidePlaybackSpeedList(
    const MediaPlayerId& player_id) {
  const auto iter = media_player_remotes_.find(player_id);
  if (iter == media_player_remotes_.end()) {
    return;
  }

  if (iter->second) {
    iter->second->HidePlaybackSpeedList();
  }
}

void MediaWebContentsObserver::MediaPlayerHostImpl::RequestVideoAssistantConfig(
    RequestVideoAssistantConfigCallback callback) {
  LOG(INFO) << "RequestVideoAssistantConfig";
  auto config = media::mojom::VideoAssistantConfig::New(true, true,
      media::mojom::VideoAssistantDownloadButton::kDownloadPerPage);
  if (media_web_contents_observer_) {
    auto* web_contents_impl = media_web_contents_observer_->web_contents_impl();
    if (web_contents_impl && web_contents_impl->AsWebContentsImplExt()) {
      web_contents_impl->AsWebContentsImplExt()->PopluateVideoAssistantConfig(config);
      std::move(callback).Run(std::move(config));
    }
  }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    OnVideoPlaying(
        media::mojom::VideoAttributesForVASTPtr video_attributes) {
  if (IsWebContentsAvailable()) {
    LOG(INFO) << "OnVideoPlaying";
    media_web_contents_observer_->web_contents_impl()->AsWebContentsImplExt()->OnVideoPlaying(
        std::move(video_attributes), media_player_id_);
  }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    OnUpdateVideoAttributes(
        media::mojom::VideoAttributesForVASTPtr video_attributes) {
  if (IsWebContentsAvailable()) {
    LOG(INFO) << "OnUpdateVideoAttributes";
    media_web_contents_observer_->web_contents_impl()->AsWebContentsImplExt()->OnUpdateVideoAttributes(
        std::move(video_attributes), media_player_id_);
  }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    OnVideoDestroyed() {
  if (IsWebContentsAvailable()) {
    LOG(INFO) << "OnVideoDestroyed";
    media_web_contents_observer_->web_contents_impl()->AsWebContentsImplExt()->OnVideoDestroyed(
        media_player_id_);
  }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    OnFullScreenOverlayEnter(
        media::mojom::MediaInfoForVASTPtr media_info_ptr) {
  if (!media_info_ptr) {
    LOG(ERROR) << "OnFullScreenOverlayEnter MediaInfo is empty";
    return;
  }

  if (IsWebContentsAvailable()) {
    LOG(INFO) << "OnFullScreenOverlayEnter";
    media_player_listener_ = media_web_contents_observer_
        ->web_contents_impl()->AsWebContentsImplExt()->OnFullScreenOverlayEnter(
            std::move(media_info_ptr), media_player_id_);
  }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::UpdatePlayStateOverlay(
    uint32_t playState) {
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::UpdatePlayStateOverlay enter. playState is " << playState;
  media_player_listener_->OnStatusChanged(playState);
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::MutedChangedOverlay(bool muted) {
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::MutedChangedOverlay enter. muted is " << muted;
  media_player_listener_->OnMutedChanged(muted);
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::PlaybackRateChangedOverlay(double playback_rate) {
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::PlaybackRateChangedOverlay enter. playback_rate is " << playback_rate;
  media_player_listener_->OnPlaybackRateChanged(playback_rate);
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::DurationChangedOverlay(double duration) {
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::DurationChangedOverlay enter. duration is " << duration;
  media_player_listener_->OnDurationChanged(duration);
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::TimeUpdateOverlay(double current_time) {
  if (!media_player_listener_) {
    return;
  }
  media_player_listener_->OnTimeUpdate(current_time);
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::BufferedEndTimeChangedOverlay(double buffered_end_time) {
  if (!media_player_listener_) {
    return;
  }
  media_player_listener_->OnBufferedEndTimeChanged(buffered_end_time);
}

// LCOV_EXCL_START
void MediaWebContentsObserver::MediaPlayerObserverHostImpl::EndedOverlay() {
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::EndedOverlay enter.";
  media_player_listener_->OnEnded();
}
// LCOV_EXCL_STOP

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::FullscreenChangedOverlay(bool fullscreen) {
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::FullscreenChangedOverlay enter. fullscreen is " << fullscreen;
  media_player_listener_->OnFullscreenChanged(fullscreen);
  if (!fullscreen) {
    media_player_listener_ = nullptr;
  }
}

// LCOV_EXCL_START
void MediaWebContentsObserver::MediaPlayerObserverHostImpl::SeekingOverlay() {
  if (!media_player_listener_) {
    return;
  }
  media_player_listener_->OnSeeking();
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::SeekingFinishedOverlay() {
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::SeekingFinishedOverlay enter.";
  media_player_listener_->OnSeekFinished();
}
// LCOV_EXCL_STOP

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::ErrorOverlay(int32_t error_code,
  const std::string& error_msg) {
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::ErrorOverlay enter.";
  media_player_listener_->OnError(error_code, error_msg);
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::VideoSizeChangedOverlay(int32_t width, int32_t height) {
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::VideoSizeChangedOverlay enter. width = " << width << ", height = " << height;
  media_player_listener_->OnVideoSizeChanged(width, height);
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::OnVolumeChanged(double volume)
{
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::OnVolumeChanged enter. volume = " << volume;
  media_player_listener_->OnVolumeChanged(volume);
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    FullscreenOverlayChanged(
        bool fullscreen_overlay, const std::string& decoder_name) {
  if (IsWebContentsAvailable()) {
    media_web_contents_observer_->web_contents_impl()->AsWebContentsImplExt()->ReportVideoDecoderName(
        decoder_name);
  }
  if (!media_player_listener_) {
    return;
  }
  LOG(INFO) << "MediaWebContentsObserver::FullscreenOverlayChanged("
            << fullscreen_overlay << ", " << decoder_name << ")";
  media_player_listener_->OnFullscreenOverlayChanged(fullscreen_overlay);
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_PIP)
MediaPlayerId MediaWebContentsObserver::GetMediaPlayerId(
  int delegate_id, int child_id, int frame_routing_id, bool& status) {
  MediaPlayerId id(
    GlobalRenderFrameHostId(child_id, frame_routing_id),
                            delegate_id);
  for (auto& media_player_remote : media_player_remotes_) {
    if (media_player_remote.first == id) {
      status = true;
      return media_player_remote.first;
    }
  }
  status = false;
  LOG(INFO) << "Pip can't find playid:" << delegate_id
            << " " << child_id << " " << frame_routing_id;
  return id;
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    OnPictureInPictureStateChanged(
        uint32_t state, int32_t width, int32_t height) {
  if (media_web_contents_observer_ &&
      media_web_contents_observer_->session_controllers_manager()) {
    media_web_contents_observer_->session_controllers_manager()
        ->OnPictureInPictureStateChanged(media_player_id_, state, width, height);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
void MediaWebContentsObserver::MediaPlayerObserverHostImpl::OnMediaCastEnter() {
  LOG(INFO) << "MediaPlayerObserverHostImpl::OnMediaCastEnter";
 if (!media_web_contents_observer_) {
   LOG(ERROR) << "MediaPlayerObserverHostImpl::OnMediaCastEnter, media_web_contents_observer_ is nullptr";
   return;
 }
 if (media_web_contents_observer_->web_contents_impl() && media_web_contents_observer_
         ->web_contents_impl()->AsWebContentsImplExt()) {
   media_web_contents_observer_
        ->web_contents_impl()->AsWebContentsImplExt()->OnMediaCastEnter();
  }
  if (media_web_contents_observer_->session_controllers_manager()) {
    media_web_contents_observer_->session_controllers_manager()->CreateAVCastAdapter(media_player_id_);
  }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::
    OnNotifyMeidaCastUri(const std::string& media_uri) {
    LOG(INFO) << "MediaPlayerObserverHostImpl::OnNotifyMeidaCastUri, mediaUri: " << media_uri;
    if (media_web_contents_observer_->session_controllers_manager()) {
      media_web_contents_observer_->session_controllers_manager()->OnNotifyMeidaCastUri(media_player_id_, media_uri);
    }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::HandleStopMediaCast() {
    LOG(INFO) << "MediaPlayerObserverHostImpl::HandleStopMediaCast";
    if (media_web_contents_observer_->session_controllers_manager()) {
      media_web_contents_observer_->session_controllers_manager()->HandleStopMediaCast(media_player_id_);
    }
}

void MediaWebContentsObserver::MediaPlayerObserverHostImpl::UpdateRemotePlayState(bool is_playing) {
  LOG(INFO) << "MediaPlayerObserverHostImpl::UpdateRemotePlayState";
  if (media_web_contents_observer_->session_controllers_manager()) {
    media_web_contents_observer_->session_controllers_manager()->UpdateRemotePlayState(media_player_id_, is_playing);
  }
}
void MediaWebContentsObserver::MediaPlayerObserverHostImpl::UpdateRemotePlayPosition(int64_t position) {
  LOG(INFO) << "MediaPlayerObserverHostImpl::UpdateRemotePlayPosition";
  if (media_web_contents_observer_->session_controllers_manager()) {
    media_web_contents_observer_->session_controllers_manager()->UpdateRemotePlayPosition(media_player_id_, position);
  }
}
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)
}