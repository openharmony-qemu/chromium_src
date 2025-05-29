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

#include "content/browser/media/session/media_session_ohos.h"

#include <chrono>

#include "base/ohos/sys_info_utils_ext.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/media_session.h"
#include "services/media_session/public/cpp/media_image.h"
#include "services/media_session/public/cpp/media_position.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace content {

MediaSessionOHOS::MediaSessionOHOS(MediaSessionImpl* session)
    : media_session_(session) {
  DCHECK(session);

  is_playing_ = false;
  is_end_of_media_ = false;
  is_initialized_ = false;
  media_type_ = OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID;
  avsession_adapter_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                           .CreateMediaAVSessionAdapter();

  if (avsession_adapter_) {
    av_metadata_ = std::make_shared<OHOSMediaAVSessionMetadata>();
    av_position_ = std::make_shared<OHOSMediaAVSessionPosition>();
    session->AddObserver(observer_receiver_.BindNewPipeAndPassRemote());
  } else {
    LOG(ERROR) << __FUNCTION__ << " media avsession adapter create failed";
  }
}

MediaSessionOHOS::~MediaSessionOHOS() {}

void MediaSessionOHOS::Prepare(OHOS::NWeb::MediaAVSessionType type) {
  if (type == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID) {
    return;
  }
  is_initialized_ = true;
  LOG(INFO) << __FUNCTION__ << " media avsession will create avsession";
  if (avsession_adapter_ && avsession_adapter_->CreateAVSession(type)) {
    media_type_ = type;
    if (av_metadata_ && !av_metadata_->GetTitle().empty()) {
      avsession_adapter_->SetMetadata(av_metadata_);
    }

    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    if (task_runner_) {
      auto media_avsession_callback =
          std::make_unique<OHOSMediaAVSessionCallback>(
              task_runner_, weak_factory_.GetWeakPtr());
      if (avsession_adapter_->RegistCallback(
              std::move(media_avsession_callback))) {
        LOG(INFO) << __FUNCTION__
                  << " media avsession register callback success";
      } else {
        LOG(ERROR) << __FUNCTION__
                   << " media avsession register callback failed";
      }
    } else {
      LOG(ERROR) << __FUNCTION__ << " media avsession get task runner failed";
    }

    media_session_->RebuildAndNotifyMetadataChanged();
    media_session_->RebuildAndNotifyMediaPositionChanged();
  }
}

OHOS::NWeb::MediaAVSessionType MediaSessionOHOS::GetMediaType(
    const std::vector<media_session::mojom::MediaAudioVideoState>& states) {
  OHOS::NWeb::MediaAVSessionType type =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID;
  for (auto state : states) {
    switch (state) {
      case media_session::mojom::MediaAudioVideoState::kAudioOnly:
        type = OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;
        break;
      case media_session::mojom::MediaAudioVideoState::kVideoOnly:
      case media_session::mojom::MediaAudioVideoState::kAudioVideo:
        type = OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_VIDEO;
        return type;
      case media_session::mojom::MediaAudioVideoState::kDeprecatedUnknown:
      default:
        break;
    }
  }
  return type;
}

void MediaSessionOHOS::MediaSessionInfoChanged(
    media_session::mojom::MediaSessionInfoPtr session_info) {
  if (!avsession_adapter_ || !session_info) {
    LOG(ERROR)
        << __FUNCTION__
        << " media avsession avsession_adapter_ or session_info is null return";
    return;
  }
  if (!is_initialized_) {
    if (session_info->audio_video_states) {
      auto session_type =
          GetMediaType(session_info->audio_video_states.value());
      if (session_type == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID) {
        LOG(WARNING)
            << __FUNCTION__
            << " media avsession before Prepare return for type invalid";
        return;
      }
      Prepare(session_type);
    }
  }
  is_playing_ =
      session_info->state !=
          media_session::mojom::MediaSessionInfo::SessionState::kInactive &&
      session_info->playback_state ==
          media_session::mojom::MediaPlaybackState::kPlaying;
  OHOS::NWeb::MediaAVSessionPlayState playback_state;
  if (is_playing_) {
    playback_state = OHOS::NWeb::MediaAVSessionPlayState::STATE_PLAY;
    if (session_info->audio_video_states) {
      auto session_type =
          GetMediaType(session_info->audio_video_states.value());
      if (session_type == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID) {
        LOG(ERROR) << __FUNCTION__
                   << " media avsession before Prepare return for type invalid";
        return;
      }
      Prepare(session_type);
    }
  } else {
    playback_state = OHOS::NWeb::MediaAVSessionPlayState::STATE_PAUSE;
  }
  avsession_adapter_->SetPlaybackState(playback_state);
}

void MediaSessionOHOS::MediaSessionMetadataChanged(
    const absl::optional<media_session::MediaMetadata>& metadata) {
  if (!avsession_adapter_ || !metadata) {
    LOG(ERROR)
        << __FUNCTION__
        << " media avsession avsession_adapter_ or metadata is null return";
    return;
  }
  std::string title, artist, album;
  base::UTF16ToUTF8(metadata.value().title.c_str(),
                    metadata.value().title.length(), &title);
  av_metadata_->SetTitle(title);
  base::UTF16ToUTF8(metadata.value().artist.c_str(),
                    metadata.value().artist.length(), &artist);
  av_metadata_->SetArtist(artist);
  base::UTF16ToUTF8(metadata.value().album.c_str(),
                    metadata.value().album.length(), &album);
  av_metadata_->SetAlbum(album);
  if (av_metadata_ && avsession_adapter_ && !av_metadata_->GetTitle().empty()) {
    avsession_adapter_->SetMetadata(av_metadata_);
  }
}

void MediaSessionOHOS::MediaSessionActionsChanged(
    const std::vector<media_session::mojom::MediaSessionAction>& actions) {}

void MediaSessionOHOS::MediaSessionImagesChanged(
    const base::flat_map<media_session::mojom::MediaSessionImageType,
                         std::vector<media_session::MediaImage>>& images) {
  if (!avsession_adapter_ || images.empty()) {
    LOG(ERROR)
        << __FUNCTION__
        << "media avsession avsession_adapter_ or metadata is null return";
    return;
  }
  auto it = images.find(media_session::mojom::MediaSessionImageType::kArtwork);
  if (it != images.end()) {
    for (auto& it_img : it->second) {
      av_metadata_->SetImageUrl(it_img.src.spec());
    }
  }
  if (av_metadata_ && avsession_adapter_ && !av_metadata_->GetTitle().empty()) {
    avsession_adapter_->SetMetadata(av_metadata_);
  }
}

void MediaSessionOHOS::MediaSessionPositionChanged(
    const std::optional<media_session::MediaPosition>& position) {
  if (!avsession_adapter_ || !position || !av_position_) {
    LOG(WARNING) << __FUNCTION__
                 << " media avsession avsession_adapter_ or position or av_position_ null";
    return;
  }
  if (media_type_ == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID) {
    LOG(WARNING) << __FUNCTION__ << " media avsession return for invalid type";
    return;
  }
  auto real_duration = position.value().duration().InMilliseconds();
  av_position_->SetDuration(real_duration);
  auto real_position = position.value().GetPosition().InMilliseconds();
  av_position_->SetElapsedTime(real_position);
  auto now = std::chrono::system_clock::now();
  auto millis = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  av_position_->SetUpdateTime(millis.time_since_epoch().count());
  avsession_adapter_->SetPlaybackPosition(av_position_);
}

void MediaSessionOHOS::Resume() {
  DCHECK(media_session_);
  if (media_type_ == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID) {
    LOG(ERROR) << __FUNCTION__ << " media avsession return for invalid type";
    return;
  }
  media_session_->Resume(MediaSession::SuspendType::kUI);
}

void MediaSessionOHOS::Suspend() {
  DCHECK(media_session_);
  if (media_type_ == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID) {
    LOG(ERROR) << __FUNCTION__ << " media avsession type invalid";
    return;
  }
  media_session_->Suspend(MediaSession::SuspendType::kUI);
}

void MediaSessionOHOS::Stop() {
  DCHECK(media_session_);
  if (media_type_ == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID) {
    LOG(ERROR) << __FUNCTION__ << " media avsession type invalid";
    return;
  }
  media_session_->Stop(MediaSession::SuspendType::kUI);
}

void MediaSessionOHOS::SeekTo(const int64_t millis) {
  DCHECK(media_session_);
  if (media_type_ == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID) {
    LOG(ERROR) << __FUNCTION__ << " media avsession type invalid";
    return;
  }
  if (millis >= 0) {
    media_session_->SeekTo(base::Milliseconds(millis));
    if (is_playing_) {
      media_session_->Resume(MediaSession::SuspendType::kUI);
    } else {
      media_session_->Suspend(MediaSession::SuspendType::kUI);
    }
    media_session_->RebuildAndNotifyMediaPositionChanged();
  } else {
    LOG(ERROR) << __FUNCTION__ << "receive an illegal millis of " << millis;
  }
}

bool MediaSessionOHOS::IsEndOfMedia() {
  return is_end_of_media_;
}

void MediaSessionOHOS::SetEndOfMedia(bool end_of_media) {
  is_end_of_media_ = end_of_media;
}

bool MediaSessionOHOS::IsPauseByAvsession() {
  return is_avsession_pause_cmd_;
}

void MediaSessionOHOS::SetPauseByAvsession(bool is_pause) {
  is_avsession_pause_cmd_ = is_pause;
}

void MediaSessionOHOS::SetWebviewShow(bool show, bool is_special_for_audio) {
  if (media_type_ == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID) {
    LOG(WARNING) << __FUNCTION__
                 << " media avsession media_type invalid return";
    return;
  }
  if (is_special_for_audio) {
    if (media_type_ != OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO) {
      LOG(ERROR) << __FUNCTION__
                 << " media avsession media_type is not audio return";
      return;
    }
  }
  if (show) {
    if (media_session_) {
      media_session_->RebuildAndNotifyMediaSessionInfoChanged();
    }
  } else {
    if (avsession_adapter_) {
      avsession_adapter_->DestroyAVSession();
      media_type_ = OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID;
    }
  }
}

bool MediaSessionOHOS::IsPlayingAudio() {
  bool ret = false;
  if (!media_session_ || !avsession_adapter_) {
    return ret;
  }

  if (media_type_ == OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO) {
    ret = true;
  }
  return ret;
}

OHOSMediaAVSessionCallback::OHOSMediaAVSessionCallback(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<MediaSessionOHOS> media_session_ohos)
    : task_runner_(task_runner), media_session_ohos_(media_session_ohos) {
  DCHECK(task_runner_.get());
  DCHECK(media_session_ohos_);
}

OHOSMediaAVSessionCallback::~OHOSMediaAVSessionCallback() {}

void OHOSMediaAVSessionCallback::Play() {
  if (!media_session_ohos_) {
    return;
  }
  task_runner_->PostTask(FROM_HERE, base::BindOnce(&MediaSessionOHOS::Resume,
                                                   media_session_ohos_));
  task_runner_->PostTask(FROM_HERE,
                         base::BindOnce(&MediaSessionOHOS::SetPauseByAvsession,
                                        media_session_ohos_, false));
}

void OHOSMediaAVSessionCallback::Pause() {
  if (!media_session_ohos_) {
    return;
  }
  task_runner_->PostTask(FROM_HERE, base::BindOnce(&MediaSessionOHOS::Suspend,
                                                   media_session_ohos_));
  task_runner_->PostTask(FROM_HERE,
                         base::BindOnce(&MediaSessionOHOS::SetPauseByAvsession,
                                        media_session_ohos_, true));
}

void OHOSMediaAVSessionCallback::Stop() {
  if (!media_session_ohos_) {
    return;
  }
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&MediaSessionOHOS::Stop, media_session_ohos_));
}

void OHOSMediaAVSessionCallback::SeekTo(int64_t millisTime) {
  if (!media_session_ohos_) {
    return;
  }
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&MediaSessionOHOS::SeekTo, media_session_ohos_,
                                millisTime));
}

void OHOSMediaAVSessionMetadata::SetTitle(const std::string& title) {
  title_ = title;
}

std::string OHOSMediaAVSessionMetadata::GetTitle() {
  return title_;
}

void OHOSMediaAVSessionMetadata::SetArtist(const std::string& artist) {
  artist_ = artist;
}

std::string OHOSMediaAVSessionMetadata::GetArtist() {
  return artist_;
}

void OHOSMediaAVSessionMetadata::SetAlbum(const std::string& album) {
  album_ = album;
}

std::string OHOSMediaAVSessionMetadata::GetAlbum() {
  return album_;
}

void OHOSMediaAVSessionPosition::SetDuration(int64_t duration) {
  duration_ = duration;
}

int64_t OHOSMediaAVSessionPosition::GetDuration() {
  return duration_;
}

void OHOSMediaAVSessionPosition::SetElapsedTime(int64_t elapsedTime) {
  elapsed_time_ = elapsedTime;
}

int64_t OHOSMediaAVSessionPosition::GetElapsedTime() {
  return elapsed_time_;
}

void OHOSMediaAVSessionPosition::SetUpdateTime(int64_t updateTime) {
  update_time_ = updateTime;
}

int64_t OHOSMediaAVSessionPosition::GetUpdateTime() {
  return update_time_;
}

void OHOSMediaAVSessionMetadata::SetImageUrl(const std::string& image_url) {
  image_url_ = image_url;
}

std::string OHOSMediaAVSessionMetadata::GetImageUrl() {
  return image_url_;
}

}  // namespace content
                       