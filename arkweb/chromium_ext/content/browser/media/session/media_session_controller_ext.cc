// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/session/media_session_controller.h"
#include "content/browser/media/media_web_contents_observer.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"

namespace content {
  constexpr int kMilliseconds = 1000;

  MediaSessionControllerExt::MediaSessionControllerExt(const MediaPlayerId& id,
                                                       WebContentsImpl* web_contents)
    : MediaSessionController(id, std::move(web_contents)) {
  }

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
void MediaSessionControllerExt::OnSetHtmlPlayEnabled(int player_id, bool enabled) {
  DCHECK_EQ(player_id_, player_id);
  if (!web_contents_)
    return;
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer)
    return;
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_))
    web_contents_observer->GetMediaPlayerRemote(id_)->SetHtmlPlayEnabled(enabled);
}

void MediaSessionControllerExt::SetHtmlPlayEnabled(bool enabled) {
  OnSetHtmlPlayEnabled(player_id_, enabled);
}

void MediaSessionControllerExt::SetSessionStateIfNeed(bool isNeedMediaSession)
{
  if (!media_session_)
    return;
  if (media_content_type_ == media::MediaContentType::kOneShot) {
    LOG(INFO) << "MediaSessionController contentType is oneShot, don't control mediaSession";
    return;
  }
  if (isNeedMediaSession) {
    LOG(INFO) << "MediaSessionController media has mediaSession";
    media_session_->SetSessionState(MediaSessionImpl::NWebMediaSessionState::NEED);
  } else {
    LOG(INFO) << "MediaSessionController media is preloading, has no mediaSession";
    media_session_->SetSessionState(MediaSessionImpl::NWebMediaSessionState::NONEED);
  }
}

void MediaSessionControllerExt::HasOneShotPlayersWhenSetMetadata(
    media::MediaContentType media_content_type) {
  if (!media_session_) {
    return;
  }
  if (media_content_type == media::MediaContentType::kOneShot) {
    LOG(INFO) << "MediaSessionController contentType is oneShot, has webrtc oneShot players";
    media_session_->has_one_shot_players_ = true;
  }
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_PIP)
void MediaSessionControllerExt::OnPictureInPictureStateChanged(
    const MediaPlayerId& id,
    uint32_t state, int32_t width, int32_t height) {
  media_session_->OnPictureInPictureStateChanged(id, state, width, height);
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
void MediaSessionControllerExt::OnEndAVSession(const MediaPlayerId& id, bool is_hidden) {
  if (is_hidden && media_session_) {
    LOG(INFO) << "MediaSessionControllerExt: OnEndAVSession";
    media_session_->EndSessionWhenHide();
  }
}
#endif // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_MEDIA_MEMORY_PRESSURE)
void MediaSessionControllerExt::OnNotifyMemoryLevel(int player_id, int32_t level) {
  DCHECK_EQ(player_id_, player_id);
  if (!web_contents_) {
    LOG(ERROR) << "DMABUF::OnNotifyMemoryLevel, web_contents is null";
    return;
  }
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer) {
    LOG(ERROR) << "DMABUF::OnNotifyMemoryLevel, web_contents_observer is null";
    return;
  }
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_)) {
    LOG(INFO) << "DMABUF::MediaSessionControllerExt:OnNotifyMemoryLevel, level = " << level;
    web_contents_observer->GetMediaPlayerRemote(id_)->NotifyMemoryLevel(level);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
void MediaSessionControllerExt::OnNotifyMeidaCastUri(const std::string& media_uri) {
  LOG(INFO) << "MediaSessionController::OnNotifyMeidaCastUri,  mediaUri: " << media_uri;
  if (media_session_) {
    media_session_->OnNotifyMeidaCastUri(media_uri);
  }
}

void MediaSessionControllerExt::CreateAVCastAdapter() {
  LOG(INFO) << "MediaSessionController::CreateAVCastAdapter";
  if (media_session_) {
    media_session_->CreateAVCastAdapter();
  }
}

void MediaSessionControllerExt::HandleStopMediaCast() {
  LOG(INFO) << "MediaSessionController::HandleStopMediaCast";
  if (media_session_) {
    media_session_->HandleStopMediaCast();
  }
}

void MediaSessionControllerExt::UpdateRemotePlayState(bool is_playing) {
  LOG(INFO) << "MediaSessionController::UpdateRemotePlayState";
  if (media_session_) {
    media_session_->UpdateRemotePlayState(is_playing);
  }
}

void MediaSessionControllerExt::UpdateRemotePlayPosition(int64_t position) {
  LOG(INFO) << "MediaSessionController::UpdateRemotePlayPosition";
  if (media_session_) {
    media_session_->UpdateRemotePlayPosition(position);
  }
}

int32_t MediaSessionControllerExt::GetMediaCastCurrentTime(int player_id) {
  DCHECK_EQ(player_id_, player_id);
  if (!web_contents_) {
    LOG(ERROR) << "GetMediaCastCurrentTime, web_contents is null";
    return 0;
  }
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer) {
    LOG(ERROR) << "GetMediaCastCurrentTime, web_contents_observer is null";
    return 0;
  }
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_)) {
    LOG(INFO) << "MediaSessionControllerExt:GetMediaCastCurrentTime";
    double time = 0.0;
    web_contents_observer->GetMediaPlayerRemote(id_)->GetMediaCastCurrentTime(&time);
    LOG(INFO) << "Current media cast time: " << time;
    return static_cast<int32_t>(time * kMilliseconds);
  }
  return 0;
}

void MediaSessionControllerExt::PullUpCastBackGround(int player_id, const std::string& device_name) {
  LOG(INFO) << "MediaSessionControllerExt::PullUpCastBackGround enter";
  if (!web_contents_) {
    LOG(ERROR) << "GetMediaCastCurrentTime, web_contents is null";
    return;
  }
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer) {
    LOG(ERROR) << "PullUpCastBackGround, web_contents_observer is null";
    return;
  }
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_)) {
    LOG(INFO) << "MediaSessionControllerExt:PullUpCastBackGround.";
    web_contents_observer->GetMediaPlayerRemote(id_)->PullUpCastBackGround(device_name);
  }
}

void MediaSessionControllerExt::MediaCastStopped(int player_id) {
  LOG(INFO) << "MediaSessionControllerExt::MediaCastStopped enter";
  if (!web_contents_) {
    LOG(ERROR) << "MediaCastStopped, web_contents is null";
    return;
  }
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer) {
    LOG(ERROR) << "MediaCastStopped, web_contents_observer is null";
    return;
  }
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_)) {
    LOG(INFO) << "MediaSessionControllerExt:MediaCastStopped";
    web_contents_observer->GetMediaPlayerRemote(id_)->MediaCastStopped();
  }
}

void MediaSessionControllerExt::UpdateUiPlayState(int player_id, bool is_playing) {
  LOG(INFO) << "MediaSessionControllerExt::UpdateUiPlayState enter";
  if (!web_contents_) {
    LOG(ERROR) << "UpdateUiPlayState, web_contents is null";
    return;
  }
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer) {
    LOG(ERROR) << "UpdateUiPlayState, web_contents_observer is null";
    return;
  }
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_)) {
    LOG(INFO) << "MediaSessionControllerExt:UpdateUiPlayState";
    web_contents_observer->GetMediaPlayerRemote(id_)->UpdateUiPlayState(is_playing);
  }
}

void MediaSessionControllerExt::UpdateUiPlayPosition(int player_id, int64_t position) {
  if (!web_contents_) {
    LOG(ERROR) << "UpdateUiPlayPosition, web_contents is null";
    return;
  }
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer) {
    LOG(ERROR) << "UpdateUiPlayPosition, web_contents_observer is null";
    return;
  }
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_)) {
    LOG(DEBUG) << "MediaSessionControllerExt:UpdateUiPlayPosition ";
    web_contents_observer->GetMediaPlayerRemote(id_)->UpdateUiPlayPosition(position);
  }
}

void MediaSessionControllerExt::MediaCastStopByNavigation() {
  if (!web_contents_)
    return;
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer)
    return;
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_)) {
    web_contents_observer->GetMediaPlayerRemote(id_)->MediaCastStopByNavigation();
  }
}

void MediaSessionControllerExt::SetPauseByAvcast(bool pause_avcast) {
  LOG(INFO) << "MediaSessionController::SetPauseByAvcast";
  if (media_session_) {
    media_session_->SetPauseByAvcast(pause_avcast);
  }
}

#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)
}