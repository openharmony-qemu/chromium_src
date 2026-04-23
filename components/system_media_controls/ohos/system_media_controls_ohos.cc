/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "components/system_media_controls/ohos/system_media_controls_ohos.h"

#include <bundle/native_interface_bundle.h>

#include <memory>
#include <utility>

#include "base/files/file_util.h"
#include "base/files/scoped_temp_file.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/memory/ref_counted_memory.h"
#include "base/notimplemented.h"
#include "base/observer_list.h"
#include "base/process/process.h"
#include "base/strings/escape.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "components/system_media_controls/system_media_controls_observer.h"
#include "content/browser/browser_thread_impl.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"

namespace system_media_controls {

// static
std::unique_ptr<SystemMediaControls> SystemMediaControls::Create(
    const std::string& product_name,
    int window) {
  auto service = std::make_unique<internal::SystemMediaControlsOhos>();
  if (service->Initialize()) {
    return std::move(service);
  }
  return nullptr;
}

namespace internal {

namespace {

constexpr base::TimeDelta kUpdatePositionInterval = base::Milliseconds(500);

constexpr AVSession_Type kDefaultSessionType = SESSION_TYPE_AUDIO;

constexpr char kDefaultSessionTag[] = "chromiumTag";
constexpr char kDefaultAssetId[] = "000";

// Writes `bitmap` to a new temporary PNG file and returns a a pair of the
// file path and a managed base::ScopedTempFile bound to this sequence. This
// should be called on the file task runner so the file is cleaned up on the
// proper sequence.  The path will be empty if the image was empty or the
// image failed to write to the file.
std::pair<base::FilePath, base::SequenceBound<base::ScopedTempFile>>
WriteBitmapToTmpFile(const SkBitmap& bitmap) {
  if (bitmap.empty()) {
    return {};
  }

  gfx::Image image(gfx::ImageSkia::CreateFrom1xBitmap(bitmap));
  auto data = image.As1xPNGBytes();

  if (data->size() == 0) {
    return {};
  }

  base::ScopedTempFile scoped_file;
  if (!scoped_file.Create()) {
    return {};
  }

  if (!base::WriteFile(scoped_file.path(), *data)) {
    return {};
  }

  // Make a copy of the path before `scoped_file` is moved.
  base::FilePath path = scoped_file.path();
  return std::make_pair(std::move(path),
                        base::SequenceBound<base::ScopedTempFile>(
                            base::SequencedTaskRunner::GetCurrentDefault(),
                            std::move(scoped_file)));
}

AVSessionCallback_Result PlayCallback(OH_AVSession* session,
                                      AVSession_ControlCommand command,
                                      void* user_data) {
  auto* controls = static_cast<SystemMediaControlsOhos*>(user_data);
  if (!controls || !controls->GetWeakPtr()) {
    LOG(ERROR) << "[AVSession]" << __func__
               << " Invalid controls object in callback";
    return AVSESSION_CALLBACK_RESULT_FAILURE;
  }

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&SystemMediaControlsOhos::Play, controls->GetWeakPtr()));

  return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result PauseCallback(OH_AVSession* session,
                                       AVSession_ControlCommand command,
                                       void* user_data) {
  auto* controls = static_cast<SystemMediaControlsOhos*>(user_data);
  if (!controls || !controls->GetWeakPtr()) {
    LOG(ERROR) << "[AVSession]" << __func__
               << " Invalid controls object in callback";
    return AVSESSION_CALLBACK_RESULT_FAILURE;
  }

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&SystemMediaControlsOhos::Pause, controls->GetWeakPtr()));
  return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result StopCallback(OH_AVSession* session,
                                      AVSession_ControlCommand command,
                                      void* user_data) {
  auto* controls = static_cast<SystemMediaControlsOhos*>(user_data);
  if (!controls || !controls->GetWeakPtr()) {
    LOG(ERROR) << "[AVSession]" << __func__
               << " Invalid controls object in callback";
    return AVSESSION_CALLBACK_RESULT_FAILURE;
  }

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&SystemMediaControlsOhos::Stop, controls->GetWeakPtr()));
  return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result PlayNextCallback(OH_AVSession* session,
                                          AVSession_ControlCommand command,
                                          void* user_data) {
  auto* controls = static_cast<SystemMediaControlsOhos*>(user_data);
  if (!controls || !controls->GetWeakPtr()) {
    LOG(ERROR) << "[AVSession]" << __func__
               << " Invalid controls object in callback";
    return AVSESSION_CALLBACK_RESULT_FAILURE;
  }

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&SystemMediaControlsOhos::Next, controls->GetWeakPtr()));
  return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result PlayPreviousCallback(OH_AVSession* session,
                                              AVSession_ControlCommand command,
                                              void* user_data) {
  auto* controls = static_cast<SystemMediaControlsOhos*>(user_data);
  if (!controls || !controls->GetWeakPtr()) {
    LOG(ERROR) << "[AVSession]" << __func__
               << " Invalid controls object in callback";
    return AVSESSION_CALLBACK_RESULT_FAILURE;
  }

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&SystemMediaControlsOhos::Previous,
                                controls->GetWeakPtr()));
  return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result SeekCallback(OH_AVSession* session,
                                      uint64_t seek_time,
                                      void* user_data) {
  auto* controls = static_cast<SystemMediaControlsOhos*>(user_data);
  if (!controls || !controls->GetWeakPtr()) {
    LOG(ERROR) << "[AVSession]" << __func__
               << " Invalid controls object in callback";
    return AVSESSION_CALLBACK_RESULT_FAILURE;
  }

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&SystemMediaControlsOhos::OnSeekTo,
                                controls->GetWeakPtr(), seek_time));
  return AVSESSION_CALLBACK_RESULT_SUCCESS;
}
}  // namespace

SystemMediaControlsOhos::SystemMediaControlsOhos()
    : av_position_(std::make_unique<AVSession_PlaybackPosition>()),
      file_task_runner_(base::ThreadPool::CreateSequencedTaskRunner(
          {base::MayBlock(), base::TaskPriority::USER_VISIBLE})) {}

SystemMediaControlsOhos::~SystemMediaControlsOhos() {
  UnRegisterCallbacks();
  if (av_session_) {
    OH_AVSession_Deactivate(av_session_);
    OH_AVSession_Destroy(av_session_);
    av_session_ = nullptr;
  }

  if (av_metadata_) {
    OH_AVMetadata_Destroy(av_metadata_);
    av_metadata_ = nullptr;
  }
  if (av_builder_) {
    OH_AVMetadataBuilder_Destroy(av_builder_);
    av_builder_ = nullptr;
  }
}

void SystemMediaControlsOhos::AddObserver(
    SystemMediaControlsObserver* observer) {
  observers_.AddObserver(observer);
  if (is_initialized_) {
    observer->OnServiceReady();
  }
}

void SystemMediaControlsOhos::RemoveObserver(
    SystemMediaControlsObserver* observer) {
  observers_.RemoveObserver(observer);
}

void SystemMediaControlsOhos::SetIsPlayPauseEnabled(bool value) {
  if (value == play_pause_enable_) {
    return;
  }

  AVSession_ErrCode register_play_ret =
      HandleCommandCallback(CONTROL_CMD_PLAY, PlayCallback, value);

  AVSession_ErrCode register_pause_ret =
      HandleCommandCallback(CONTROL_CMD_PAUSE, PauseCallback, value);

  if (register_play_ret != AV_SESSION_ERR_SUCCESS ||
      register_pause_ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__
               << " failed,register play ret:" << register_play_ret
               << ",register pause ret:" << register_pause_ret
               << " ,value: " << value;
    return;
  }

  play_pause_enable_ = value;
}

void SystemMediaControlsOhos::SetIsStopEnabled(bool value) {
  if (value == stop_enable_) {
    return;
  }

  AVSession_ErrCode ret =
      HandleCommandCallback(CONTROL_CMD_STOP, StopCallback, value);
  if (ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__ << " failed,result:" << ret
               << " ,value: " << value;
    return;
  }
  stop_enable_ = value;
}

void SystemMediaControlsOhos::SetIsNextEnabled(bool value) {
  if (value == next_enable_) {
    return;
  }

  AVSession_ErrCode ret =
      HandleCommandCallback(CONTROL_CMD_PLAY_NEXT, PlayNextCallback, value);
  if (ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__ << " failed,result:" << ret
               << " ,value: " << value;
    return;
  }
  next_enable_ = value;
}

void SystemMediaControlsOhos::SetIsPreviousEnabled(bool value) {
  if (value == previous_enable_) {
    return;
  }

  AVSession_ErrCode ret = HandleCommandCallback(CONTROL_CMD_PLAY_PREVIOUS,
                                                PlayPreviousCallback, value);
  if (ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__ << " failed,result:" << ret
               << " ,value: " << value;
    return;
  }
  previous_enable_ = value;
}

void SystemMediaControlsOhos::SetIsSeekToEnabled(bool value) {
  if (value == seek_enable_) {
    return;
  }

  AVSession_ErrCode ret =
      value ? OH_AVSession_RegisterSeekCallback(av_session_, SeekCallback, this)
            : OH_AVSession_UnregisterSeekCallback(av_session_, SeekCallback);
  if (ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__ << " failed,result:" << ret
               << " ,value: " << value;
    return;
  }
  seek_enable_ = value;
}

void SystemMediaControlsOhos::SetPlaybackStatus(PlaybackStatus value) {
  AVSession_PlaybackState state;
  switch (value) {
    case PlaybackStatus::kPlaying:
      state = PLAYBACK_STATE_PLAYING;
      break;
    case PlaybackStatus::kPaused:
      state = PLAYBACK_STATE_PAUSED;
      break;
    case PlaybackStatus::kStopped:
      state = PLAYBACK_STATE_STOPPED;
      break;
    default:
      break;
  }
  AVSession_ErrCode ret = OH_AVSession_SetPlaybackState(av_session_, state);
  if (ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__ << " failed,result:" << ret
               << " ,state: " << state;
    return;
  }

  playing_ = (value == PlaybackStatus::kPlaying);
  if (playing_ && position_.has_value()) {
    StartPositionUpdateTimer();
  } else {
    StopPositionUpdateTimer();
  }
}

void SystemMediaControlsOhos::SetTitle(const std::u16string& value) {
  SetMetadataField(value, OH_AVMetadataBuilder_SetTitle, "title");
}

void SystemMediaControlsOhos::SetArtist(const std::u16string& value) {
  SetMetadataField(value, OH_AVMetadataBuilder_SetArtist, "artist");
}

void SystemMediaControlsOhos::SetAlbum(const std::u16string& value) {
  SetMetadataField(value, OH_AVMetadataBuilder_SetAlbum, "album");
}

void SystemMediaControlsOhos::SetThumbnail(const SkBitmap& bitmap) {
  file_task_runner_->PostTaskAndReplyWithResult(
      FROM_HERE, base::BindOnce(&WriteBitmapToTmpFile, bitmap),
      base::BindOnce(&SystemMediaControlsOhos::OnThumbnailFileWritten,
                     weak_factory_.GetWeakPtr()));
}

void SystemMediaControlsOhos::SetPosition(
    const media_session::MediaPosition& position) {
  position_ = position;
  UpdatePosition();

  if (playing_) {
    StartPositionUpdateTimer();
  }
}

void SystemMediaControlsOhos::ClearMetadata() {
  SetTitle(std::u16string());
  SetArtist(std::u16string());
  SetAlbum(std::u16string());
  SetThumbnail(SkBitmap());
  ClearPosition();
}

bool SystemMediaControlsOhos::GetVisibilityForTesting() const {
  NOTIMPLEMENTED();
  return false;
}

bool SystemMediaControlsOhos::Initialize() {
  OH_NativeBundle_ElementName main_element =
      OH_NativeBundle_GetMainElementName();

  AVSession_ErrCode ret = OH_AVSession_Create(
      kDefaultSessionType, kDefaultSessionTag, main_element.bundleName,
      main_element.abilityName, &av_session_);
  if (ret == AV_SESSION_ERR_SERVICE_EXCEPTION) {
    LOG(WARNING) << __func__
                 << " [AVSession] OH_AVSession_Create failed as "
                    "AV_SESSION_ERR_SERVICE_EXCEPTION for bundleName:"
                 << main_element.bundleName
                 << " abilityName: " << main_element.abilityName
                 << ". This might be because System Media Control is already "
                    "created by host Application.";
    return is_initialized_;
  }

  if (ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << __func__
               << " [AVSession] OH_AVSession_Create failed,result:" << ret;
    return is_initialized_;
  }
  ret = OH_AVSession_Activate(av_session_);
  if (ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << __func__
               << " [AVSession] OH_AVSession_Activate failed,result:" << ret;
    return is_initialized_;
  }
  InitAvMetaData();
  return is_initialized_;
}

void SystemMediaControlsOhos::InitAvMetaData() {
  AVMetadata_Result metadata_ret = OH_AVMetadataBuilder_Create(&av_builder_);
  if (metadata_ret != AVMETADATA_SUCCESS) {
    LOG(ERROR) << __func__
               << "[AVSession] OH_AVMetadataBuilder_Create failed, result:"
               << metadata_ret;
    return;
  }
  metadata_ret = OH_AVMetadataBuilder_SetAssetId(av_builder_, kDefaultAssetId);
  if (metadata_ret != AVMETADATA_SUCCESS) {
    LOG(ERROR) << __func__
               << "[AVSession] OH_AVMetadataBuilder_SetAssetId failed, result:"
               << metadata_ret;
    return;
  }
  is_initialized_ = true;
}

AVSession_ErrCode SystemMediaControlsOhos::HandleCommandCallback(
    AVSession_ControlCommand command,
    OH_AVSessionCallback_OnCommand callback,
    bool enable) {
  return enable ? OH_AVSession_RegisterCommandCallback(av_session_, command,
                                                       callback, this)
                : OH_AVSession_UnregisterCommandCallback(av_session_, command,
                                                         callback);
}

template <typename SetterFunc>
void SystemMediaControlsOhos::SetMetadataField(const std::u16string& value,
                                               SetterFunc setter,
                                               const std::string& field_name) {
  std::string utf8_value;
  if (!value.empty()) {
    utf8_value = base::UTF16ToUTF8(value);
  }

  AVMetadata_Result ret = setter(av_builder_, utf8_value.c_str());
  if (ret != AVMETADATA_SUCCESS) {
    LOG(ERROR) << "[AVSession] Set" << field_name << " failed, result: " << ret
               << " ,value: " << utf8_value;
    return;
  }
  UpdateAvMetaData();
}

void SystemMediaControlsOhos::UnRegisterCallbacks() {
  if (play_pause_enable_) {
    OH_AVSession_UnregisterCommandCallback(av_session_, CONTROL_CMD_PLAY,
                                           PlayCallback);
    OH_AVSession_UnregisterCommandCallback(av_session_, CONTROL_CMD_PAUSE,
                                           PauseCallback);
  }
  if (previous_enable_) {
    OH_AVSession_UnregisterCommandCallback(
        av_session_, CONTROL_CMD_PLAY_PREVIOUS, PlayPreviousCallback);
  }

  if (next_enable_) {
    OH_AVSession_UnregisterCommandCallback(av_session_, CONTROL_CMD_PLAY_NEXT,
                                           PlayNextCallback);
  }

  if (seek_enable_) {
    OH_AVSession_UnregisterSeekCallback(av_session_, SeekCallback);
  }
}

void SystemMediaControlsOhos::UpdateAvMetaData() {
  if (av_metadata_) {
    OH_AVMetadata_Destroy(av_metadata_);
    av_metadata_ = nullptr;
  }

  AVMetadata_Result metadata_ret =
      OH_AVMetadataBuilder_GenerateAVMetadata(av_builder_, &av_metadata_);
  if (metadata_ret != AVMETADATA_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__
               << " GenerateAVMetadata failed,result:" << metadata_ret;
    return;
  }

  AVSession_ErrCode av_session_ret =
      OH_AVSession_SetAVMetadata(av_session_, av_metadata_);
  if (av_session_ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__
               << " SetAVMetadata failed,result:" << av_session_ret;
  }
}

void SystemMediaControlsOhos::ClearPosition() {
  position_ = std::nullopt;
  StopPositionUpdateTimer();
  UpdatePosition();
}

void SystemMediaControlsOhos::UpdatePosition() {
  int64_t position_ms = 0;
  int64_t duration_ms = 0;

  if (position_.has_value()) {
    position_ms = position_->GetPosition().InMilliseconds();
    duration_ms = position_->duration().InMilliseconds();
  }

  if (duration_ms != duration_ms_) {
    AVMetadata_Result metadata_ret =
        OH_AVMetadataBuilder_SetDuration(av_builder_, duration_ms);
    if (metadata_ret != AVMETADATA_SUCCESS) {
      LOG(ERROR) << "[AVSession] " << __func__
                 << " SetDuration failed,result:" << metadata_ret;
    }
    UpdateAvMetaData();
    duration_ms_ = duration_ms;
  }

  av_position_->elapsedTime = position_ms;
  av_position_->updateTime =
      (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds();
  AVSession_ErrCode av_session_ret =
      OH_AVSession_SetPlaybackPosition(av_session_, av_position_.get());
  if (av_session_ret != AV_SESSION_ERR_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__
               << " SetPlaybackPosition failed,result:" << av_session_ret;
  }
}

void SystemMediaControlsOhos::StartPositionUpdateTimer() {
  // The timer should only run when the media is playing and has a position.
  DCHECK(playing_);
  DCHECK(position_.has_value());
  position_update_timer_.Start(
      FROM_HERE, kUpdatePositionInterval,
      base::BindRepeating(&SystemMediaControlsOhos::UpdatePosition,
                          GetWeakPtr()));
}

void SystemMediaControlsOhos::StopPositionUpdateTimer() {
  position_update_timer_.Stop();
}

base::WeakPtr<SystemMediaControlsOhos> SystemMediaControlsOhos::GetWeakPtr() {
  return weak_factory_.GetWeakPtr();
}

void SystemMediaControlsOhos::Play() {
  for (SystemMediaControlsObserver& obs : observers_) {
    obs.OnPlay(this);
  }
}

void SystemMediaControlsOhos::Pause() {
  for (SystemMediaControlsObserver& obs : observers_) {
    obs.OnPause(this);
  }
}

void SystemMediaControlsOhos::Stop() {
  for (SystemMediaControlsObserver& obs : observers_) {
    obs.OnStop(this);
  }
}

void SystemMediaControlsOhos::Next() {
  for (SystemMediaControlsObserver& obs : observers_) {
    obs.OnNext(this);
  }
}

void SystemMediaControlsOhos::Previous() {
  for (SystemMediaControlsObserver& obs : observers_) {
    obs.OnPrevious(this);
  }
}

void SystemMediaControlsOhos::OnSeekTo(uint64_t seek_time) {
  for (SystemMediaControlsObserver& obs : observers_) {
    obs.OnSeekTo(this, base::Milliseconds(seek_time));
  }
}

void SystemMediaControlsOhos::OnThumbnailFileWritten(
    std::pair<base::FilePath, base::SequenceBound<base::ScopedTempFile>>
        thumbnail) {
  const auto& path = thumbnail.first;
  auto url = path.empty() ? "" : "file://" + base::EscapePath(path.value());
  AVMetadata_Result ret =
      OH_AVMetadataBuilder_SetMediaImageUri(av_builder_, url.c_str());
  if (ret != AVMETADATA_SUCCESS) {
    LOG(ERROR) << "[AVSession] " << __func__ << " failed,result:" << ret;
  }
  UpdateAvMetaData();
  thumbnail_ = std::move(thumbnail.second);
}

}  // namespace internal

}  // namespace system_media_controls
