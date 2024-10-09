// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_player_bridge.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "media/base/ohos/ohos_media_player_callback.h"
#include "media/base/ohos/ohos_media_player_listener.h"
#include "ohos_adapter_helper.h"

namespace media {

constexpr int QUEUE_SIZE = 3;
static constexpr int PLAYER_INIT_OK = 0;
static constexpr int PLAYER_INIT_ERROR = -1;

OHOSMediaPlayerBridge::OHOSMediaPlayerBridge(
    const GURL& url,
    const net::SiteForCookies& site_for_cookies,
    const url::Origin& top_frame_origin,
    const std::string& user_agent,
    bool hide_url_log,
    Client* client,
    bool allow_credentials,
    bool is_hls)
    : client_(client),
      url_(url),
      prepared_(false),
      pending_play_(false),
      seek_complete_(true),
      should_seek_on_prepare_(false),
      should_set_volume_on_prepare_(false),
      seeking_on_playback_complete_(false) {
#if defined(RK3568)
  is_hls_ = is_hls;
#endif
}

OHOSMediaPlayerBridge::~OHOSMediaPlayerBridge() {
  cached_buffers_.clear();
}

int32_t OHOSMediaPlayerBridge::Initialize() {
  if (url_.SchemeIsBlob()) {
    NOTREACHED();
    return PLAYER_INIT_ERROR;
  }
  return PLAYER_INIT_OK;
}

void OHOSMediaPlayerBridge::Start() {
  if (!player_) {
    pending_play_ = true;
    Prepare();
  } else {
    if (prepared_) {
      StartInternal();
    } else {
      pending_play_ = true;
    }
  }
}

void OHOSMediaPlayerBridge::Prepare() {
  DCHECK(!player_);

  if (url_.SchemeIsBlob()) {
    NOTREACHED();
    return;
  }

  player_ = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreatePlayerAdapter();
  if (!player_) {
    LOG(ERROR) << "media create player_adapter failed";
    return;
  }

  task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  if (!task_runner_) {
    LOG(ERROR) << "media task runner failed";
    return;
  }

  std::unique_ptr<OHOSMediaPlayerCallback> media_player_callback =
      std::make_unique<OHOSMediaPlayerCallback>(task_runner_,
                                                weak_factory_.GetWeakPtr());
  int32_t ret = player_->SetPlayerCallback(std::move(media_player_callback));
  if (ret != 0) {
    LOG(ERROR) << "SetPlayerCallback error::ret=" << ret;
    return;
  }

  if (url_.SchemeIsFile()) {
    ret = SetFdSource(url_.GetContent());
  } else {
    ret = player_->SetSource(url_.spec());
  }
  if (ret != 0) {
    LOG(ERROR) << "SetSource error::ret=" << ret;
    return;
  }

  consumer_surface_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                          .CreateConsumerSurfaceAdapter();
  if (consumer_surface_ == nullptr) {
    LOG(ERROR) << "media create consumer surface adapter failed";
    return;
  }
  auto listener = std::make_unique<OHOSMediaPlayerListener>(
      task_runner_, weak_factory_.GetWeakPtr());
  consumer_surface_->RegisterConsumerListener(std::move(listener));
  consumer_surface_->SetUserData(
      surfaceFormat,
      std::to_string(OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_RGBA_8888));
  consumer_surface_->SetQueueSize(QUEUE_SIZE);
  ret = player_->SetVideoSurface(consumer_surface_);
  if (ret != 0) {
    LOG(ERROR) << "SetVideoSurface error::ret=" << ret;
    consumer_surface_ = nullptr;
    return;
  }

  ret = player_->PrepareAsync();
  if (ret != 0) {
    LOG(ERROR) << "Prepare error::ret=" << ret << " url=" << url_.spec();
  }
}

void OHOSMediaPlayerBridge::StartInternal() {
  if (player_ && prepared_) {
    if (!pause_when_prepared_) {
      player_->Play();
    } else {
      LOG(INFO) << "OHOSMediaPlayerBridge StartInternal start canceled, because of paused when perpared";
      pause_when_prepared_ = false;
    }
  }
}

void OHOSMediaPlayerBridge::Pause() {
  if ((player_ && player_state_ != OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED &&
                  player_state_ != OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PAUSED &&
                  player_state_ != OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STOPPED &&
                  player_state_ != OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE) || pending_play_) {
    LOG(INFO) << "OHOSMediaPlayerBridge Pause when perpared!!";
    pause_when_prepared_ = true;
  }
  if (player_ && player_state_ == OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED) {
    LOG(INFO) << "OHOSMediaPlayerBridge Pause successful!!";
    int32_t ret = player_->Pause();
    if (ret != 0) {
      LOG(ERROR) << "Pause error::ret=" << ret;
    } else {
      if (IsAudible(current_volume_)) {
        client_->OnAudioStateChanged(false);
      }
    }
  }
}

void OHOSMediaPlayerBridge::SeekTo(base::TimeDelta time) {
  pending_seek_ = time;

  LOG(INFO) << "OHOSMediaPlayerBridge::SeekTo time=" << time.InMilliseconds();
  if (!prepared_) {
    should_seek_on_prepare_ = true;
    return;
  }
  SeekInternal(time);
}

void OHOSMediaPlayerBridge::SeekInternal(base::TimeDelta time) {
  int32_t ret = player_->Seek(time.InMilliseconds(),
                              OHOS::NWeb::PlayerSeekMode::SEEK_CLOSEST);
  if (ret != 0) {
    LOG(ERROR) << "Seek error::ret=" << ret;
  } else {
    seek_complete_ = false;
  }
}

void OHOSMediaPlayerBridge::SetVolume(float volume, bool is_muted) {
  float setvolume = is_muted ? 0 : volume;
  is_muted_ = is_muted;
  if (!prepared_) {
    should_set_volume_on_prepare_ = true;
    volume_ = volume;
    return;
  }

  if (player_) {
    int32_t ret = player_->SetVolume(setvolume, setvolume);
    if (ret != 0) {
      LOG(ERROR) << "SetVolume error::ret=" << ret;
    } else {
      if (!IsAudible(current_volume_) && IsAudible(volume) &&
          player_state_ ==
              OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED) {
        client_->OnAudioStateChanged(true);
      } else if (IsAudible(current_volume_) && !IsAudible(volume) &&
                 player_state_ ==
                     OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED) {
        client_->OnAudioStateChanged(false);
      }
      current_volume_ = volume;
    }
  }
}

void OHOSMediaPlayerBridge::PropagateDuration(base::TimeDelta duration) {
  duration_ = duration;
  client_->OnMediaDurationChanged(duration_);
}

base::TimeDelta OHOSMediaPlayerBridge::GetDuration() {
  if (player_ && prepared_) {
    int32_t time = -1;
    (void)player_->GetDuration(time);
    return base::Milliseconds(time);
  }
  return base::TimeDelta();
}

base::TimeDelta OHOSMediaPlayerBridge::GetMediaTime() {
  if (!player_ || !prepared_ || seeking_on_playback_complete_ ||
      !seek_complete_) {
    return pending_seek_;
  }

  int32_t time = -1;
  int32_t ret = player_->GetCurrentTime(time);
  if (ret == 0 && time == -1) {
    //if is livestream, return system time
    auto system_time = base::Time::Now().ToInternalValue() / base::Time::kMicrosecondsPerMillisecond;
    return base::Milliseconds(system_time);
  }
  return base::Milliseconds(time);
}

void OHOSMediaPlayerBridge::SeekDone() {
  LOG(INFO) << "OHOSMediaPlayerBridge::SeekDone()";
  seek_complete_ = true;
}

void OHOSMediaPlayerBridge::FinishPaint(int fd) {
  if (!task_runner_->BelongsToCurrentThread()) {
    task_runner_->PostTask(FROM_HERE,
                           base::BindOnce(&OHOSMediaPlayerBridge::FinishPaint,
                                          weak_factory_.GetWeakPtr(), fd));
    return;
  }

  if (cached_buffers_.front()) {
    if (cached_buffers_.front()->GetFileDescriptor() != fd) {
      LOG(ERROR) << "match fd error render fd=" << fd << "  browser fd="
                 << cached_buffers_.front()->GetFileDescriptor();
    }
    int32_t ret = consumer_surface_->ReleaseBuffer(
        std::move(cached_buffers_.front()), -1);
    if (ret != OHOS::NWeb::GSErrorCode::GSERROR_OK) {
      LOG(ERROR) << "release buffer fail, ret=" << ret;
    }
  } else {
    LOG(ERROR) << "cached_buffers_.front() is nullptr";
  }
  if (!cached_buffers_.empty()) {
    cached_buffers_.pop_front();
  }
}

void OHOSMediaPlayerBridge::SetPlaybackSpeed(
    OHOS::NWeb::PlaybackRateMode mode) {
  if (player_) {
    int32_t ret = player_->SetPlaybackSpeed(mode);
    if (ret != 0) {
      LOG(ERROR) << "SetPlaybackSpeed error::ret=" << ret;
    }
  }
}

void OHOSMediaPlayerBridge::OnEnd() {
  client_->OnPlaybackComplete();
}

void OHOSMediaPlayerBridge::OnError(int32_t error_code) {
  client_->OnError(error_code);
}

void OHOSMediaPlayerBridge::OnPlayerStateUpdate(
    OHOS::NWeb::PlayerAdapter::PlayerStates player_state) {
  if (!player_) {
    return;
  }
  LOG(INFO) << "OHOSMediaPlayerBridge OnPlayerStateUpdate: from: " << static_cast<int32_t>(player_state_)
    << ", to: " << static_cast<int32_t>(player_state);
  if (player_state_ ==
          OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE &&
      player_state != player_state_) {
    seeking_on_playback_complete_ = false;
    pause_when_prepared_ = false;
  }

  if (player_state ==
          OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE &&
      player_state_ ==
          OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED) {
    if (IsAudible(current_volume_)) {
      client_->OnAudioStateChanged(false);
    }
  }

  if (player_state == OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED &&
      player_state != player_state_) {
    if (IsAudible(current_volume_)) {
      client_->OnAudioStateChanged(true);
    }
  }

  player_state_ = player_state;
  if (player_state ==
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED) {
    prepared_ = true;

    PropagateDuration(GetDuration());

    if (should_seek_on_prepare_) {
      SeekInternal(pending_seek_);
      pending_seek_ = base::Milliseconds(0);
      should_seek_on_prepare_ = false;
    }

    if (should_set_volume_on_prepare_) {
      SetVolume(volume_, is_muted_);
      should_set_volume_on_prepare_ = false;
    }

    if (pending_play_) {
      StartInternal();
      pending_play_ = false;
    }
  } else if (player_state ==
                 OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STATE_ERROR ||
             player_state ==
                 OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_IDLE ||
             player_state ==
                 OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_INITIALIZED ||
             player_state ==
                 OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARING) {
    prepared_ = false;
  }
}

void OHOSMediaPlayerBridge::OnBufferAvailable(
    std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> buffer) {
  if (!task_runner_->BelongsToCurrentThread()) {
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&OHOSMediaPlayerBridge::OnBufferAvailable,
                       weak_factory_.GetWeakPtr(), std::move(buffer)));
    return;
  }

  int fd = buffer->GetFileDescriptor();
  if (fd <= 0) {
    LOG(ERROR) << "surface buffer fd error fd:" << fd;
    int32_t ret = consumer_surface_->ReleaseBuffer(std::move(buffer), -1);
    if (ret != OHOS::NWeb::GSErrorCode::GSERROR_OK) {
      LOG(ERROR) << "release buffer fail, ret=" << ret;
    }
    return;
  }

  int32_t coded_height;
  int32_t coded_width;

  // video frame height must be 32*N
  const int step_height = 32;
  // argb format video frame should divided by 4
  const int argb_stride_step = 4;
  if (buffer->GetHeight() % step_height == 0) {
    coded_height = buffer->GetHeight();
  } else {
    coded_height = (buffer->GetHeight() / step_height + 1) * step_height;
  }
  if (buffer->GetFormat() ==
      OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_RGBA_8888) {
    coded_width = buffer->GetStride() / argb_stride_step;
    coded_height = buffer->GetHeight();
  } else {
    coded_width = buffer->GetStride();
  }

#if defined(RK3568)
  if (!is_hls_) {
    coded_width = buffer->GetWidth();
    coded_height = buffer->GetHeight();
  }
#endif
  client_->OnFrameAvailable(fd, buffer->GetSize(), coded_width, coded_height,
                            buffer->GetWidth(), buffer->GetHeight(),
                            buffer->GetFormat());
  cached_buffers_.push_back(std::move(buffer));
}

void OHOSMediaPlayerBridge::OnVideoSizeChanged(int32_t width, int32_t height) {
  if (client_) {
    client_->OnVideoSizeChanged(width, height);
  }
}

void OHOSMediaPlayerBridge::OnPlayerInterruptEvent(int32_t value) {
  if (client_) {
    client_->OnPlayerInterruptEvent(value);
  }
}

int32_t OHOSMediaPlayerBridge::SetFdSource(const std::string& path) {
  int32_t fd = open(path.c_str(), O_RDONLY);
  if (fd < 0) {
    LOG(ERROR) << "SetFdSource error:Open file failed";
    return -1;
  }
  struct stat64 buffer;
  if (fstat64(fd, &buffer) != 0) {
    LOG(ERROR) << "SetFdSource error:Get file state failed";
    (void)close(fd);
    return -1;
  }
  int64_t length = static_cast<int64_t>(buffer.st_size);
  int32_t ret = player_->SetSource(fd, 0, length);
  (void)close(fd);
  return ret;
}

bool OHOSMediaPlayerBridge::IsAudible(float volume) {
  return volume > 0;
}
}  // namespace media
