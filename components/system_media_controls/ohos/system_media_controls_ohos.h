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

#ifndef COMPONENTS_SYSTEM_MEDIA_CONTROLS_OHOS_SYSTEM_MEDIA_CONTROLS_OHOS_H_
#define COMPONENTS_SYSTEM_MEDIA_CONTROLS_OHOS_SYSTEM_MEDIA_CONTROLS_OHOS_H_

#include <multimedia/av_session/native_avmetadata.h>
#include <multimedia/av_session/native_avsession.h>

#include <optional>
#include <string>
#include <utility>

#include "base/component_export.h"
#include "base/files/file_path.h"
#include "base/files/scoped_temp_file.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/threading/sequence_bound.h"
#include "base/timer/timer.h"
#include "components/system_media_controls/system_media_controls.h"

namespace base {
class SequencedTaskRunner;
}

namespace system_media_controls {

class SystemMediaControlsObserver;

namespace internal {

class COMPONENT_EXPORT(SYSTEM_MEDIA_CONTROLS) SystemMediaControlsOhos
    : public SystemMediaControls {
 public:
  explicit SystemMediaControlsOhos();

  SystemMediaControlsOhos(const SystemMediaControlsOhos&) = delete;
  SystemMediaControlsOhos& operator=(const SystemMediaControlsOhos&) = delete;

  ~SystemMediaControlsOhos() override;

  // SystemMediaControls implementation.
  void AddObserver(SystemMediaControlsObserver* observer) override;
  void RemoveObserver(SystemMediaControlsObserver* observer) override;
  void SetEnabled(bool enabled) override {}
  void SetIsNextEnabled(bool value) override;
  void SetIsPreviousEnabled(bool value) override;
  void SetIsPlayPauseEnabled(bool value) override;
  void SetIsStopEnabled(bool value) override;
  void SetIsSeekToEnabled(bool value) override;
  void SetPlaybackStatus(PlaybackStatus value) override;
  void SetID(const std::string* value) override {}
  void SetTitle(const std::u16string& value) override;
  void SetArtist(const std::u16string& value) override;
  void SetAlbum(const std::u16string& value) override;
  void SetThumbnail(const SkBitmap& bitmap) override;
  void SetPosition(const media_session::MediaPosition& position) override;
  void ClearThumbnail() override {}
  void ClearMetadata() override;
  void UpdateDisplay() override {}
  bool GetVisibilityForTesting() const override;

  bool Initialize();
  base::WeakPtr<SystemMediaControlsOhos> GetWeakPtr();

  void Play();
  void Pause();
  void Stop();
  void Next();
  void Previous();
  void OnSeekTo(uint64_t seek_time);

 private:
  void InitAvMetaData();
  AVSession_ErrCode HandleCommandCallback(
      AVSession_ControlCommand command,
      OH_AVSessionCallback_OnCommand callback,
      bool enable);
  void UnRegisterCallbacks();

  template <typename SetterFunc>
  void SetMetadataField(const std::u16string& value,
                        SetterFunc setter,
                        const std::string& field_name);

  void UpdateAvMetaData();
  void ClearPosition();
  void UpdatePosition();

  void StartPositionUpdateTimer();
  void StopPositionUpdateTimer();

  void OnThumbnailFileWritten(
      std::pair<base::FilePath, base::SequenceBound<base::ScopedTempFile>>
          thumbnail);

  bool is_initialized_ = false;
  bool play_pause_enable_ = false;
  bool stop_enable_ = false;
  bool previous_enable_ = false;
  bool next_enable_ = false;
  bool seek_enable_ = false;

  std::optional<media_session::MediaPosition> position_;
  base::RepeatingTimer position_update_timer_;
  bool playing_ = false;

  int64_t duration_ms_;

  RAW_PTR_EXCLUSION OH_AVSession* av_session_ = nullptr;
  RAW_PTR_EXCLUSION OH_AVMetadataBuilder* av_builder_ = nullptr;
  RAW_PTR_EXCLUSION OH_AVMetadata* av_metadata_ = nullptr;

  std::unique_ptr<AVSession_PlaybackPosition> av_position_;

  scoped_refptr<base::SequencedTaskRunner> file_task_runner_;

  // A temporary file containing the thumbnail image.
  base::SequenceBound<base::ScopedTempFile> thumbnail_;

  base::ObserverList<SystemMediaControlsObserver> observers_;

  base::WeakPtrFactory<SystemMediaControlsOhos> weak_factory_{this};
};

}  // namespace internal

}  // namespace system_media_controls

#endif  // COMPONENTS_SYSTEM_MEDIA_CONTROLS_OHOS_SYSTEM_MEDIA_CONTROLS_OHOS_H_
