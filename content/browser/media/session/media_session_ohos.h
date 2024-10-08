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

#ifndef CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_OHOS_H_
#define CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_OHOS_H_

#include <memory>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/timer/timer.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "services/media_session/public/mojom/media_session.mojom.h"

#include "media_avsession_adapter.h"

namespace content {

class MediaSessionImpl;

class MediaSessionOHOS final
    : public media_session::mojom::MediaSessionObserver {
 public:
  explicit MediaSessionOHOS(MediaSessionImpl* session);
  ~MediaSessionOHOS() override;
  MediaSessionOHOS& operator=(const MediaSessionOHOS&) = delete;
  MediaSessionOHOS(const MediaSessionOHOS&) = delete;

  void MediaSessionInfoChanged(
      media_session::mojom::MediaSessionInfoPtr session_info) override;
  void MediaSessionMetadataChanged(
      const absl::optional<media_session::MediaMetadata>& metadata) override;
  void MediaSessionActionsChanged(
      const std::vector<media_session::mojom::MediaSessionAction>& action) override;
  void MediaSessionImagesChanged(
      const base::flat_map<media_session::mojom::MediaSessionImageType,
                           std::vector<media_session::MediaImage>>& images) override;
  void MediaSessionPositionChanged(
      const absl::optional<media_session::MediaPosition>& position) override;

  bool SetWebviewShow(bool show);
  bool SetWebviewShowForAudio(bool show);
  bool SetWebviewShowForVideo(bool show);
  void Resume();
  void Suspend();
  void Stop();
  void SeekTo(const int64_t millis);
  void UpdateTimer();

  bool IsEndOfMedia();
  void SetEndOfMedia(bool end_of_media);
 private:
  void CheckMediaInfo();
  void Prepare(OHOS::NWeb::MediaAVSessionType type);
  OHOS::NWeb::MediaAVSessionType GetMediaType(
    const std::vector<media_session::mojom::MediaAudioVideoState>& states);

  bool is_webview_show_;
  bool is_playing_;
  bool is_initialized_;
  bool is_end_of_media_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  std::shared_ptr<OHOS::NWeb::MediaAVSessionMetadataAdapter> av_metadata_;
  std::shared_ptr<OHOS::NWeb::MediaAVSessionPositionAdapter> av_position_;
  std::unique_ptr<OHOS::NWeb::MediaAVSessionAdapter> avsession_adapter_;
  OHOS::NWeb::MediaAVSessionType media_type_;
  base::WeakPtrFactory<MediaSessionOHOS> weak_factory_{this};
  const raw_ptr<MediaSessionImpl, DanglingUntriaged> media_session_;
  mojo::Receiver<media_session::mojom::MediaSessionObserver> observer_receiver_{this};

  static constexpr base::TimeDelta kReportMediaInfoInterval = base::Seconds(1);
};

class OHOSMediaAVSessionCallback : public OHOS::NWeb::MediaAVSessionCallbackAdapter {
 public:
  OHOSMediaAVSessionCallback(
      const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
      base::WeakPtr<MediaSessionOHOS> media_session_ohos);
  virtual ~OHOSMediaAVSessionCallback();
  OHOSMediaAVSessionCallback(const OHOSMediaAVSessionCallback&) = delete;
  OHOSMediaAVSessionCallback& operator=(const OHOSMediaAVSessionCallback&) = delete;

  void Play() override;
  void Pause() override;
  void Stop() override;
  void SeekTo(int64_t millisTime) override;

 private:
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::WeakPtr<MediaSessionOHOS> media_session_ohos_;
};

class OHOSMediaAVSessionMetadata : public OHOS::NWeb::MediaAVSessionMetadataAdapter {
 public:
  void SetTitle(const std::string& title) override;
  std::string GetTitle() override;
  void SetArtist(const std::string& artist) override;
  std::string GetArtist() override;
  void SetAlbum(const std::string& album) override;
  std::string GetAlbum() override;

 private:
  std::string title_;
  std::string artist_;
  std::string album_;
};

class OHOSMediaAVSessionPosition : public OHOS::NWeb::MediaAVSessionPositionAdapter {
 public:
  void SetDuration(int64_t duration) override;
  int64_t GetDuration() override;
  void SetElapsedTime(int64_t elapsedTime) override;
  int64_t GetElapsedTime() override;
  void SetUpdateTime(int64_t updateTime) override;
  int64_t GetUpdateTime() override;

 private:
  int64_t duration_;
  int64_t elapsed_time_;
  int64_t update_time_;
};
}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_OHOS_H_
