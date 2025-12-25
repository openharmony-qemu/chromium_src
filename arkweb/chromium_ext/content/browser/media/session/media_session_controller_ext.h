// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_CONTROLLER_H_EXT_
#define CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_CONTROLLER_H_EXT_

#include "content/browser/media/session/media_session_controller.h"

namespace content {

class MediaSessionController;

class MediaSessionControllerExt : public MediaSessionController {
public:
  MediaSessionControllerExt(const MediaPlayerId& id,
                            WebContentsImpl* web_contents);

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  void OnSetHtmlPlayEnabled(int player_id, bool enabled) override;

  // Set whether to the HTML play can be used to control media
  void SetHtmlPlayEnabled(bool enabled);

  // Set whether to the HTML play can be used to  control media
  void SetSessionStateIfNeed(bool isNeedMediaSession);

  void HasOneShotPlayersWhenSetMetadata(media::MediaContentType media_content_type);
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  void OnEndAVSession(const MediaPlayerId& id, bool is_hidden);
#endif // ARKWEB_MEDIA_AVSESSION

  MediaSessionControllerExt* AsMediaSessionControllerExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_PIP)
  void OnPictureInPictureStateChanged(
      const MediaPlayerId& id, uint32_t state, int32_t width, int32_t height);
#endif

#if BUILDFLAG(ARKWEB_MEDIA_MEMORY_PRESSURE)
  void OnNotifyMemoryLevel(int player_id, int32_t level) override;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  void OnNotifyMeidaCastUri(const std::string& media_uri);
  void CreateAVCastAdapter();
  void HandleStopMediaCast();
  void UpdateRemotePlayState(bool is_playing);
  void UpdateRemotePlayPosition(int64_t position);
  int32_t GetMediaCastCurrentTime(int player_id) override;
  void PullUpCastBackGround(int player_id, const std::string& device_name) override;
  void UpdateUiPlayState(int player_id, bool is_playing) override;
  void UpdateUiPlayPosition(int player_id, int64_t position) override;
  void MediaCastStopped(int player_id) override;
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)
};

}

#endif  // CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_CONTROLLER_H_EXT_