// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_player_bridge.h"

#include <fcntl.h>
#include <sys/stat.h>
#include "base/logging.h"
#include "net/http/http_request_headers.h"
#include "base/task/single_thread_task_runner.h"
#include "media/base/ohos/ohos_media_player_callback.h"
#include "ohos_adapter_helper.h"
#include "ohos_glue/base/include/ark_web_errno.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"

#include "content/browser/gpu/gpu_process_host.h"

extern "C" {
  typedef struct NativeWindow OHNativeWindow;
  int32_t OH_NativeWindow_CreateNativeWindowFromSurfaceId(uint64_t surfaceId, NativeWindow **window);
}

namespace media {

constexpr int MAX_TOLERABLE_SEEK_ERROR = 300;
static constexpr int PLAYER_INIT_OK = 0;
static constexpr int PLAYER_INIT_ERROR = -1;

OHOSMediaPlayerBridge::OHOSMediaPlayerBridge(
    const GURL& url,
    const net::SiteForCookies& site_for_cookies,
    const url::Origin& top_frame_origin,
    const std::string& user_agent,
    bool has_storage_access,
    bool hide_url_log,
    Client* client,
    bool allow_credentials,
    bool is_hls,
    const base::flat_map<std::string, std::string> headers)
    : client_(client),
      url_(url),
      prepared_(false),
      pending_play_(false),
      seek_complete_(true),
      should_seek_on_prepare_(false),
      should_set_volume_on_prepare_(false),
      seeking_on_playback_complete_(false),
      seeking_back_complete_(false),
      headers_(std::move(headers)),
      user_agent_(user_agent),
      site_for_cookies_(site_for_cookies),
      pending_retrieve_cookies_(false),
      should_prepare_on_retrieved_cookies_(false),
      has_storage_access_(has_storage_access),
      top_frame_origin_(top_frame_origin),
      allow_credentials_(allow_credentials) {
#if defined(RK3568)
  is_hls_ = is_hls;
#endif
}

OHOSMediaPlayerBridge::~OHOSMediaPlayerBridge() {
}

int32_t OHOSMediaPlayerBridge::Initialize() {
  if (url_.SchemeIsBlob()) {
    NOTREACHED();
    return PLAYER_INIT_ERROR;
  }
   if (allow_credentials_ && client_ != nullptr) {
    media::OHOSMediaResourceGetter* resource_getter_ = client_->GetMediaResourceGetter();
    if (resource_getter_) {
      pending_retrieve_cookies_ = true;
      resource_getter_->GetCookies(
          url_, site_for_cookies_, top_frame_origin_, has_storage_access_,
          base::BindOnce(&OHOSMediaPlayerBridge::OnCookiesRetrieved,
                        weak_factory_.GetWeakPtr()));
    }
  }
  return PLAYER_INIT_OK;
}

void OHOSMediaPlayerBridge::SetNativeWindowSurface(int native_window_id) {
  native_window_id_ = native_window_id;
}

void OHOSMediaPlayerBridge::OnCookiesRetrieved(const std::string& cookies) {
  cookies_ = cookies;
  pending_retrieve_cookies_ = false;
  if (client_ == nullptr || client_->GetMediaResourceGetter() == nullptr) {
    return;
  }
  client_->GetMediaResourceGetter()->GetAuthCredentials(
      url_, base::BindOnce(&OHOSMediaPlayerBridge::OnAuthCredentialsRetrieved,
                           weak_factory_.GetWeakPtr()));

  if (should_prepare_on_retrieved_cookies_) {
    should_prepare_on_retrieved_cookies_ = false;
    if (!player_) {
      LOG(ERROR) << "player_ is null";
      return;
    }
    auto player_headers = GetPlayerHeadersInternal();
    int32_t ret = player_->SetMediaSourceHeader(url_.spec(), player_headers);
    LOG(INFO) << "media player SetMediaSourceHeader url";
    if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
      ret = player_->SetSource(url_.spec());
      LOG(INFO) << "media player SetSource url";
    }
    if (ret != 0) {
      LOG(ERROR) << "SetPlayerSourceHeader error:ret= " << ret;
      return;
    }
    SetPlayerSurface();
  }
}

void OHOSMediaPlayerBridge::OnAuthCredentialsRetrieved(const std::u16string& username,
    const std::u16string& password) {
  GURL::ReplacementsW replacements;
  if (!username.empty()) {
    replacements.SetUsernameStr(username);
    if (!password.empty()) {
      replacements.SetPasswordStr(password);
    }
    url_ = url_.ReplaceComponents(replacements);
  }
}

std::map<std::string, std::string> OHOSMediaPlayerBridge::GetPlayerHeadersInternal() {
  std::map<std::string, std::string> player_headers;
  if (!cookies_.empty()) {
    player_headers.insert(std::pair<std::string, std::string>(net::HttpRequestHeaders::kCookie, cookies_));
  }
  if (!user_agent_.empty()) {
    player_headers.insert(std::pair<std::string, std::string>(net::HttpRequestHeaders::kUserAgent, user_agent_));
  }
  for (const auto& entry : headers_) {
    player_headers[entry.first] = entry.second;
  }
  return player_headers;
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
    if (pending_retrieve_cookies_) {
      should_prepare_on_retrieved_cookies_ = true;
      return;
    }
    auto player_headers = GetPlayerHeadersInternal();
    ret = player_->SetMediaSourceHeader(url_.spec(), player_headers);
    LOG(INFO) << "media player SetMediaSourceHeader url";
    if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
      ret = player_->SetSource(url_.spec());
      LOG(INFO) << "media player SetSource Url";
    }
  }
  if (ret != 0) {
    LOG(ERROR) << "SetSource error::ret=" << ret;
    return;
  }
  SetPlayerSurface();
}

void OHOSMediaPlayerBridge::SetPlayerSurface() {
  if (!player_) {
    LOG(ERROR) << "OHOSMediaPlayerBridge SetPlayerSurface player is null";
    return;
  }

  SetNativeWindowFromSurfaceId();
  if (native_window_origin_ == nullptr) {
    LOG(ERROR) << "OHOSMediaPlayerBridge GetNativeWindow is null";
    return;
  }

#ifdef OHOS_VIDEO_ASSISTANT
  int32_t ret = -1;
  if (pending_new_surface_id_ > 0) {
    LOG(INFO) << "SetPlayerSurface enter component pending_new_surface_id_:" << pending_new_surface_id_;
    pending_new_surface_id_ = -1;
    void* native_window = NWebNativeWindowTracker::Get()->GetNativeWindow(new_surface_id_);
    if (native_window) {
       ret = player_->SetVideoSurfaceNew(native_window);
      if (ret != 0) {
        LOG(ERROR) << "SetPlayerSurface SetVideoSurfaceNew error: new_surface_id_ = " << new_surface_id_
                  << ", native_window = " << native_window;
      }
    }
  } else {
    LOG(INFO) << "SetPlayerSurface enter web surface";
    ret = player_->SetVideoSurfaceNew(native_window_origin_);
    if (ret != 0) {
      LOG(ERROR) << "SetPlayerSurface enter web surface error::ret=" << ret;
    }
  }
#else
  int32_t ret = player_->SetVideoSurfaceNew(native_window_origin_);
  if (ret != 0) {
    LOG(ERROR) << "SetPlayerSurface enter web surface error::ret=" << ret;
  }
#endif // OHOS_VIDEO_ASSISTANT

  int32_t prepare_ret = player_->PrepareAsync();
  if (prepare_ret != 0) {
    LOG(ERROR) << "Prepare error::ret=" << prepare_ret << " url=" << url_.spec();
  }
}

void OHOSMediaPlayerBridge::SetNativeWindowFromSurfaceId() {
  if (native_window_id_ == -1) {
    LOG(ERROR) << "OHOSMediaPlayerBridge native_window id is invalid";
    return;
  }

  content::GpuProcessHost* gpu_process_host = content::GpuProcessHost::Get();
  if (!gpu_process_host || !gpu_process_host->gpu_host()) {
    LOG(ERROR) << "CreateMediaPlayer failed, no gpu host";
    return;
  }

  std::string surface_id_string = gpu_process_host->gpu_host()->GetSurfaceId(native_window_id_);
  LOG(INFO) << "CreateMediaPlayer native_window_id_: " << native_window_id_
            << ", surface_id_string: " << surface_id_string;

  uint64_t id_of_surface = std::stoull(surface_id_string);
  OHNativeWindow* oh_native_window = nullptr;
  int32_t oh_ret = OH_NativeWindow_CreateNativeWindowFromSurfaceId(id_of_surface, &oh_native_window);
  if (oh_ret != 0 || oh_native_window == nullptr) {
    LOG(ERROR) << "CreateNativeWindowFromSurfaceId failed.";
    return;
  }

  native_window_origin_ = oh_native_window;
  LOG(INFO) << "CreateNativeWindowFromSurfaceId successful.";
}

void OHOSMediaPlayerBridge::StartInternal() {
  if (player_ && prepared_) {
    if (!pause_when_prepared_) {
      LOG(INFO) << "OHOSMediaPlayerBridge, MediaPlayer Play()";
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
                  player_state_ != OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED &&
                  player_state_ != OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE) || pending_play_) {
    LOG(INFO) << "OHOSMediaPlayerBridge Pause when perpared, player_state_ is:" << static_cast<int32_t>(player_state_);
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
  seeking_back_complete_ = false;

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

void OHOSMediaPlayerBridge::OnSeekBack(base::TimeDelta extra_time) {
  extra_time_ = extra_time;
  if (!player_ || pending_seek_ == base::Milliseconds(0)) {
    return;
  }
  if (!seek_complete_) {
    int32_t time = -1;
    (void)player_->GetCurrentTime(time);
    recording_seek_ = base::Milliseconds(time);
    return;
  }
  if (seeking_back_complete_) {
    return;
  }

  //When processing seek requests, there may be a maximum error of 300ms between the nearest keyframe
  //found by mediaplayer and the time point of seekTo
  if ((recording_seek_ - extra_time_) > base::Milliseconds(MAX_TOLERABLE_SEEK_ERROR)) {
    if (client_) {
      client_->OnPlayerSeekBack(extra_time_);
      seeking_back_complete_ = true;
    }
    LOG(INFO) << "OHOSMediaPlayerBridge::OnSeekBack() recording_time= " << recording_seek_;
    LOG(INFO) << "OHOSMediaPlayerBridge::OnSeekBack() back_time= " << extra_time_;
  }
}

void OHOSMediaPlayerBridge::FinishPaint(int fd) {
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
      LOG(INFO) << "OnPlayerStateUpdate PLAYER_PREPARED, pending_play_";
      StartInternal();
      pending_play_ = false;
    } else if (pause_when_prepared_) {
      LOG(INFO) << "OnPlayerStateUpdate PLAYER_PREPARED, no pending_play then pause_when_prepared is false";
      pause_when_prepared_ = false;
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
}

void OHOSMediaPlayerBridge::OnVideoSizeChanged(int32_t width, int32_t height) {
  LOG(INFO) << "OHOSMediaPlayerBridge::OnVideoSizeChanged width:" << width << ", height:" << height;
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
#if defined(OHOS_MEDIA)
  fdsan_exchange_owner_tag(fd, 0, uv__get_addr_tag((void *)&fd));
#endif // defined(OHOS_MEDIA)
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
#if defined(OHOS_MEDIA)
  fdsan_close_with_tag(fd, uv__get_addr_tag((void *)&fd));
#else
  (void)close(fd);
#endif // defined(OHOS_MEDIA)
  return ret;
}

bool OHOSMediaPlayerBridge::IsAudible(float volume) {
  return volume > 0;
}

#if defined(OHOS_MEDIA)
uint64_t OHOSMediaPlayerBridge::uv__get_addr_tag(void* addr) {
  uint64_t tag = 0;

  if (addr != NULL) {
    tag = fdsan_create_owner_tag(FDSAN_OWNER_TYPE_FILE, (uint64_t)addr);
  }

  return tag;
}
#endif // defined(OHOS_MEDIA)

#ifdef OHOS_VIDEO_ASSISTANT
void OHOSMediaPlayerBridge::SetVideoSurface(int32_t surface_id) {
  LOG(INFO) << "OHOSMediaPlayerBridge::SetVideoSurface component, surface_id: " << surface_id;
  if (surface_id > 0) {
    SetVideoSurfaceNew(surface_id);
  } else {
    SetVideoSurfaceOld();
  }
}

void OHOSMediaPlayerBridge::SetVideoSurfaceNew(int32_t surface_id) {
  LOG(INFO) << "SetVideoSurfaceNew(" << surface_id << "), new_surface_id_["
            << new_surface_id_ << "], player_[" << player_.get() << "]";
  if (new_surface_id_ == surface_id) {
    return;
  }
  if (new_surface_id_ > 0) {
    NWebNativeWindowTracker::Get()->DestroyNativeWindow(new_surface_id_);
    new_surface_id_ = -1;
  }
  new_surface_id_ = surface_id;
  void* native_window = nullptr;
  if (player_) {
    native_window = NWebNativeWindowTracker::Get()->GetNativeWindow(surface_id);
  } else {
    LOG(INFO) << "SetVideoSurfaceNew, player is empty, has pending_new_surface_id_";
    pending_new_surface_id_ = new_surface_id_;
    if (!player_) {
      Prepare();
    }
  }
  if (native_window) {
    player_->SetVideoSurfaceNew(native_window);
  }
}

void OHOSMediaPlayerBridge::SetVideoSurfaceOld() {
  if (new_surface_id_ > 0) {
    NWebNativeWindowTracker::Get()->DestroyNativeWindow(new_surface_id_);
    new_surface_id_ = -1;
  }
  if (!player_) {
    LOG(INFO) << "web setVideoSurface  player is nullptr";
    return;
  }
  int32_t ret = player_->SetVideoSurfaceNew(native_window_origin_);
  if (ret != 0) {
    LOG(ERROR) << "web SetVideoSurface error" << ret;
    return;
  }
}
#endif // OHOS_VIDEO_ASSISTANT
}  // namespace media
