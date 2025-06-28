// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_MEDIA_PLAYER_BRIDGE_H_
#define MEDIA_BASE_OHOS_MEDIA_PLAYER_BRIDGE_H_

#include <deque>
#include <map>

#include "base/memory/weak_ptr.h"
#include "base/containers/flat_map.h"
#include "base/task/single_thread_task_runner.h"
#include "base/timer/timer.h"
#include "media/base/media_export.h"
#include "media/base/ohos/ohos_media_resource_getter.h"
#include "net/cookies/site_for_cookies.h"
#include "net/storage_access_api/status.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/graphic_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/media_adapter.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace media {
class MEDIA_EXPORT OHOSMediaPlayerBridge {
 public:
  class Client {
   public:
    virtual void OnFrameAvailable(int fd,
                                  uint32_t size,
                                  int32_t coded_width,
                                  int32_t coded_height,
                                  int32_t visible_width,
                                  int32_t visible_height,
                                  int32_t format) = 0;

    // Called when media duration is first detected or changes.
    virtual void OnMediaDurationChanged(base::TimeDelta duration) = 0;

    // Called when playback completed.
    virtual void OnPlaybackComplete() = 0;

    // Called when error happens.
    virtual void OnError(int error) = 0;

    // Called when video size has changed.
    virtual void OnVideoSizeChanged(int width, int height) = 0;

    // Called when player InterruptEvent.
    virtual void OnPlayerInterruptEvent(int32_t value) = 0;

    virtual void OnAudioStateChanged(bool isAudible) = 0;

    virtual void OnPlayerSeekBack(base::TimeDelta back_time) = 0;

    virtual OHOSMediaResourceGetter* GetMediaResourceGetter() = 0;
  };

  enum MediaErrorType {
    MEDIA_ERROR_FORMAT,
    MEDIA_ERROR_DECODE,
    MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK,
    MEDIA_ERROR_INVALID_CODE,
    MEDIA_ERROR_SERVER_DIED,
  };

  OHOSMediaPlayerBridge(const GURL& url,
                        const net::SiteForCookies& site_for_cookies,
                        const url::Origin& top_frame_origin,
                        const std::string& user_agent,
                        net::StorageAccessApiStatus storage_access_api_status,
                        bool hide_url_log,
                        Client* client,
                        bool allow_credentials,
                        bool is_hls,
                        const base::flat_map<std::string, std::string> headers,
                        const std::vector<std::string>& grantMediaFileAccessDirs);
  virtual ~OHOSMediaPlayerBridge();

  OHOSMediaPlayerBridge(const OHOSMediaPlayerBridge&) = delete;
  OHOSMediaPlayerBridge& operator=(const OHOSMediaPlayerBridge&) = delete;

  int32_t Initialize();
  void Start();
  void Pause();
  void SeekTo(base::TimeDelta time);
  base::TimeDelta GetDuration();
  void SetVolume(float volume, bool ismuted);
  base::TimeDelta GetMediaTime();
  void FinishPaint(int fd);
  void SetPlaybackSpeed(OHOS::NWeb::PlaybackRateMode mode);
  void SetNativeWindowSurface(int native_window_id);

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetVideoSurface(int32_t surface_id);
#endif // ARKWEB_VIDEO_ASSISTANT

  void OnEnd();
  void OnError(int32_t errorCode);
  void OnBufferAvailable(
      std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> buffer);
  void OnPlayerStateUpdate(
      OHOS::NWeb::PlayerAdapter::PlayerStates player_state);
  void OnVideoSizeChanged(int32_t width, int32_t height);
  void OnPlayerInterruptEvent(int32_t value);
  void SeekDone();
  void OnSeekBack(base::TimeDelta extra_time);

#if BUILDFLAG(ARKWEB_PIP)
  void PipEnable(bool enable);
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
  void RecycleDmaBuffer();
  void ResumeDmaBuffer();
#endif  // ARKWEB_MEDIA_DMABUF
 private:
  int32_t SetFdSource(const std::string& path);
  void Prepare();
  void StartInternal();
  void SeekInternal(base::TimeDelta time);
  void PropagateDuration(base::TimeDelta duration);
  bool IsAudible(float volume);
  bool CheckIsPathValid(const std::string& path);
  void SetNativeWindowFromSurfaceId();

  // Callback function passed to `resource_getter_`. Called when the cookies
  // are retrieved.
  void OnCookiesRetrieved(const std::string& cookies);

  // Callback function passed to `resource_getter_`. Called when the auth
  // credentials are retrieved.
  void OnAuthCredentialsRetrieved(const std::u16string& username,
                                  const std::u16string& password);

  // Get media player header
  std::map<std::string, std::string> GetPlayerHeadersInternal();

  // Set media player surface and register listener
  void SetPlayerSurface();

#if BUILDFLAG(ARKWEB_MEDIA)
  uint64_t uv__get_addr_tag(void* addr);
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetVideoSurfaceNew(int32_t surface_id);
  void SetVideoSurfaceOld();
#endif // ARKWEB_VIDEO_ASSISTANT

  std::unique_ptr<OHOS::NWeb::PlayerAdapter> player_ = nullptr;
  void* native_window_origin_ = nullptr;

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  raw_ptr<Client> client_;
  GURL url_;
  bool prepared_;
  bool pending_play_;
  bool seek_complete_;
  bool should_seek_on_prepare_;
  float volume_;
  float current_volume_ = 0;
  bool is_muted_ = false;
  bool should_set_volume_on_prepare_;
  bool pause_when_prepared_ = false;
  base::TimeDelta duration_;
  base::TimeDelta pending_seek_;
  base::TimeDelta recording_seek_;
  OHOS::NWeb::PlayerAdapter::PlayerStates player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_IDLE;

  // MediaPlayer is unable to handle Seek request when playback end. We should
  // pending the SeekTo request until its playback state changed.
  bool seeking_on_playback_complete_;
  base::TimeDelta extra_time_;
  // It is a sign of rollback and SEEK_CLOSEST failure.
  bool seeking_back_complete_;
  // MediaPlayer File Access config
  std::vector<std::string> grantMediaFileAccessDirs_;
#if defined(RK3568)
  bool is_hls_;
#endif

  // HTTP Request Headers
  base::flat_map<std::string, std::string> headers_;

  // User agent string to be used for media player.
  const std::string user_agent_;

  // Used to determine if cookies are accessed in a third-party context.
  net::SiteForCookies site_for_cookies_;

  // Waiting to retrieve cookies for `url_`.
  bool pending_retrieve_cookies_;

  // Whether to prepare after cookies retrieved.
  bool should_prepare_on_retrieved_cookies_;

  // Used when determining if first-party cookies may be accessible in a third-party context.
  net::StorageAccessApiStatus storage_access_api_status_;

  // Cookies for `url_`.
  std::string cookies_;

  // Used to check for cookie content settings.
  url::Origin top_frame_origin_;

  // Whether user credentials are allowed to be passed.
  bool allow_credentials_;

  int32_t native_window_id_ = -1;

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  int32_t new_surface_id_ = -1;
  int32_t pending_new_surface_id_ = -1;
#endif // ARKWEB_VIDEO_ASSISTANT

  base::WeakPtrFactory<OHOSMediaPlayerBridge> weak_factory_{this};
};
}  // namespace media

#endif  // MEDIA_BASE_OHOS_MEDIA_PLAYER_BRIDGE_H_
