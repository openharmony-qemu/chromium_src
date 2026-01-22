/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_IMPL_H_
#error "must be in include form CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_IMPL_H_"
#endif

namespace content {

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
// LCOV_EXCL_START
bool MediaSessionImpl::HasOnlyOneShotPlayersPublic() const {
  return HasOnlyOneShotPlayers();
}

bool MediaSessionImpl::HasOneShotPlayersWhenSetMetadataPublic() const {
  return has_one_shot_players_;
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
MediaSessionImpl::NWebPlaybackState MediaSessionImpl::NWebGetState() {
  if (GetMediaAudioVideoStates().empty()) {
    return NWebPlaybackState::NONE;
  }
  if (IsSuspended()) {
    return NWebPlaybackState::PAUSED;
  }
  if (IsActive()) {
    return NWebPlaybackState::PLAYING;
  } else {
    return NWebPlaybackState::STOP;
  }
}

bool MediaSessionImpl::IsEndOfMedia() {
  bool ret = true;
  if (position_) {
    ret = position_->end_of_media();
    if (ret) {
      return ret;
    }
  }

  if (session_ohos_) {
    ret = session_ohos_->IsEndOfMedia();
    if (ret) {
      return ret;
    }
  }
  return ret;
}
// LCOV_EXCL_STOP

void MediaSessionImpl::SetEndOfMedia(bool end_of_media) {
  if (session_ohos_) {
    session_ohos_->SetEndOfMedia(end_of_media);
  }
}

bool MediaSessionImpl::GetPlayingState()
{
  return isPlayingState_;
}

void MediaSessionImpl::SetPlayingState(bool playingState)
{
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  RebuildAndNotifyMetadataChanged();
#endif // ARKWEB_MEDIA_AVSESSION
  isPlayingState_ = playingState;
}

bool MediaSessionImpl::GetMuteState()
{
  return is_muted_;
}

bool MediaSessionImpl::IsPlayingAudio() {
  bool ret = false;
  if (session_ohos_) {
    ret = session_ohos_->IsPlayingAudio();
  }
  return ret;
}

bool MediaSessionImpl::IsPauseByAvsession() {
  bool ret = false;
  if (session_ohos_) {
    ret = session_ohos_->IsPauseByAvsession();
  }
  return ret;
}

void MediaSessionImpl::SetPauseByAvsession(bool is_pause) {
  if (session_ohos_) {
    session_ohos_->SetPauseByAvsession(is_pause);
  }
}

void MediaSessionImpl::SetWebviewShow(bool show, bool is_special_for_audio) {
  if (session_ohos_) {
    session_ohos_->SetWebviewShow(show, is_special_for_audio);
  }
}

void MediaSessionImpl::SetSessionState(MediaSessionImpl::NWebMediaSessionState sessionState)
{
  sessionState_ = sessionState;
}

MediaSessionImpl::NWebMediaSessionState MediaSessionImpl::GetSessionState()
{
  return sessionState_;
}

#endif

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
void MediaSessionImpl::PutWebMediaAVSessionEnabled(bool enable) {
  LOG(INFO) << "media avsession MediaSessionImpl::PutWebMediaAVSessionEnabled enable is: " << enable;
  bool isWebMediaAVSessionSwitch = false;
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  if (currentProcess && !currentProcess->HasSwitch(switches::kEnableMediaAvsession)) {
    isWebMediaAVSessionSwitch = true;
  }
  if (enable) {
    if (isWebMediaAVSessionSwitch && !session_ohos_) {
      session_ohos_ = std::make_unique<MediaSessionOHOS>(this);
      if (web_contents() && web_contents()->GetPrimaryMainFrame() &&
        web_contents()->GetPrimaryMainFrame()->GetView()) {
        focused_ = web_contents()->GetPrimaryMainFrame()->GetView()->HasFocus();
      }
    }
  } else {
    if (session_ohos_) {
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
      ReportAvSessionStatus(std::to_string(enable));
#endif
      session_ohos_.reset();
    }
  }
  RebuildAndNotifyMetadataChanged();
}

void MediaSessionImpl::EndSessionWhenHide() {
  auto states = MediaSessionImpl::GetMediaAudioVideoStates();
  if (!states.empty() && states.back()!= MediaAudioVideoState::kAudioOnly && !focused_) {
    LOG(INFO) << "MediaSessionImpl: EndSessionWhenHide.";
    SetWebviewShow(false, false);
  }
}
#endif // ARKWEB_MEDIA_AVSESSION

void MediaSessionImpl::CreateSessionOhos() {
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  if (currentProcess &&
      !currentProcess->HasSwitch(switches::kEnableMediaAvsession)) {
    session_ohos_ = std::make_unique<MediaSessionOHOS>(this);
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
}

#if BUILDFLAG(ARKWEB_PIP)
void MediaSessionImpl::OnPictureInPictureStateChanged(
    const MediaPlayerId& id,
    uint32_t state, int32_t width, int32_t height) {
  if (auto* pip_window_controller_ =
          VideoPictureInPictureWindowControllerImpl::FromWebContents(
              web_contents())) {
    pip_window_controller_->OnPictureInPictureStateChanged(
        id, state, width, height);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_MEMORY_PRESSURE)
void MediaSessionImpl::OnMemoryPressure(
    base::MemoryPressureListener::MemoryPressureLevel memory_pressure_level) {
  memory_pressure_level_ = memory_pressure_level;
  switch (memory_pressure_level) {
    case base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_NONE:
      LOG(INFO) << "DMABUF::MediaSessionImpl, MEMORY_PRESSURE_LEVEL_NONE";
      break;
    case base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_MODERATE:
      LOG(INFO) << "DMABUF::MediaSessionImpl, MEMORY_PRESSURE_LEVEL_MODERATE";
      OnNotifyMemoryLevel(static_cast<int32_t>(memory_pressure_level));
      break;
    case base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_CRITICAL:
      LOG(INFO) << "DMABUF::MediaSessionImpl, MEMORY_PRESSURE_LEVEL_CRITICAL";
      OnNotifyMemoryLevel(static_cast<int32_t>(memory_pressure_level));
      break;
    default:
      LOG(WARNING) << "DMABUF::" << __func__ << ": Unknown memory pressure level";
      break;
  }
}

void MediaSessionImpl::OnNotifyMemoryLevel(int32_t level) {
  for (const auto& it : normal_players_) {
    it.first.observer->OnNotifyMemoryLevel(it.first.player_id, level);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
void MediaSessionImpl::UpdateMediaPlayersMuteState(int player_id, bool mute) {
  LOG(INFO) << __func__ << ", player_id:" << player_id << ", mute:" << mute
            << ", normal_players_ size:" << normal_players_.size()
            << ", players_mute_state_ size:" << players_mute_state_.size();
  if (normal_players_.empty()) {
    return;
  }
  for (const auto& iter : normal_players_) {
    if (iter.first.player_id == player_id) {
      players_mute_state_[player_id] = mute;
      break;
    }
  }
}

bool MediaSessionImpl::GetMediaPlayerMuteState() {
  for (const auto& iter : normal_players_) {
    int player_id = iter.first.player_id;
    auto mute_iter = players_mute_state_.find(player_id);
    if (mute_iter == players_mute_state_.end()) {
      continue;
    }
    if (!mute_iter->second) {
      LOG(INFO) << __func__ << ", player_id: " << player_id << " not muted";
      return false;
    }
  }
  return is_muted_;
}
#endif  // ARKWEB_MEDIA_POLICY

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
void MediaSessionImpl::OnNotifyMeidaCastUri(const std::string& media_uri) {
  if (session_ohos_) {
    LOG(INFO) << "MediaSessionImpl: OnNotifyMeidaCastUri";
    session_ohos_->OnNotifyMeidaCastUri(media_uri);
  }
}
 
void MediaSessionImpl::CreateAVCastAdapter() {
  LOG(INFO) << "MediaSessionImpl::CreateAVCastAdapter";
  if (session_ohos_) {
    LOG(INFO) << "MediaSessionImpl: CreateAVCastAdapter";
    session_ohos_->CreateAVCastAdapter();
  }
}
 
void MediaSessionImpl::HandleStopMediaCast() {
  LOG(INFO) << "MediaSessionImpl::HandleStopMediaCast";
  if (session_ohos_) {
    LOG(INFO) << "MediaSessionImpl: HandleStopMediaCast";
    session_ohos_->HandleStopMediaCast();
  }
}
 
void MediaSessionImpl::UpdateRemotePlayState(bool is_playing) {
  LOG(INFO) << "MediaSessionImpl::UpdateRemotePlayState";
  if (session_ohos_) {
    LOG(INFO) << "MediaSessionImpl: UpdateRemotePlayState";
    session_ohos_->UpdateRemotePlayState(is_playing);
  }
}
 
void MediaSessionImpl::UpdateRemotePlayPosition(int64_t position) {
  LOG(INFO) << "MediaSessionImpl::UpdateRemotePlayPosition";
  if (session_ohos_) {
    LOG(INFO) << "MediaSessionImpl: UpdateRemotePlayPosition";
    session_ohos_->UpdateRemotePlayPosition(position);
  }
}
 
 
int32_t MediaSessionImpl::GetMediaCastCurrentTime() {
  LOG(INFO) << "MediaSessionImpl::GetMediaCastCurrentTime, enter";
  for (const auto& it : normal_players_) {
    LOG(INFO) << "MediaSessionImpl::GetMediaCastCurrentTime";
    return it.first.observer->GetMediaCastCurrentTime(it.first.player_id);
  }
  
  return 0;
}
 
void MediaSessionImpl::PullUpCastBackGround(const std::string& device_name) {
  LOG(INFO) << "MediaSessionImpl::PullUpCastBackGround, enter";
  for (const auto& it : normal_players_) {
    LOG(INFO) << "MediaSessionImpl::PullUpCastBackGround";
    it.first.observer->PullUpCastBackGround(it.first.player_id, device_name);
  }
}
 
void MediaSessionImpl::MediaCastStopped() {
  LOG(INFO) << "MediaSessionImpl::MediaCastStopped, enter";
  for (const auto& it : normal_players_) {
    LOG(INFO) << "MediaSessionImpl::MediaCastStopped";
    it.first.observer->MediaCastStopped(it.first.player_id);
  }
}
 
void MediaSessionImpl::UpdateUiPlayState(bool is_playing) {
  LOG(INFO) << "MediaSessionImpl::UpdateUiPlayState, enter";
  for (const auto& it : normal_players_) {
    LOG(INFO) << "MediaSessionImpl::UpdateUiPlayState";
    it.first.observer->UpdateUiPlayState(it.first.player_id, is_playing);
  }
}
 
void MediaSessionImpl::UpdateUiPlayPosition(int64_t position) {
  LOG(DEBUG) << "MediaSessionImpl::UpdateUiPlayPosition, enter";
  for (const auto& it : normal_players_) {
    LOG(DEBUG) << "MediaSessionImpl::UpdateUiPlayPosition";
    it.first.observer->UpdateUiPlayPosition(it.first.player_id, position);
  }
}

bool MediaSessionImpl::IsPageBackground() {
  content::Visibility visibility = web_contents()->GetVisibility();
  bool is_background = (visibility != content::Visibility::VISIBLE);
  LOG(INFO) << "MediaSessionImpl::IsPageBackground " << is_background;
  return is_background;
}

void MediaSessionImpl::SetPauseByAvcast(bool pause_avcast) { 
  LOG(INFO) << "SetPauseByAvcast, pause_avcast: " << pause_avcast;
  pause_avcast_ = pause_avcast; 
}

void MediaSessionImpl::NotifyCastControlShow(bool is_show) {
  LOG(INFO) << "MediaSessionImpl::NotifyCastControlShow, enter";
  for (const auto& it : normal_players_) {
    it.first.observer->NotifyCastControlShow(it.first.player_id, is_show);
  }
}

#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)
}