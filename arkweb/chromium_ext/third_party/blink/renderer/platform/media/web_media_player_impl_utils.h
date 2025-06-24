// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_WEB_MEDIA_PLAYER_IMPL_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_WEB_MEDIA_PLAYER_IMPL_UTILS_H_

namespace blink {
class WebMediaPlayerImpl;

class WebMediaPlayerImplUtils {
public:
  WebMediaPlayerImpl* impl;
  WebMediaPlayerImplUtils(WebMediaPlayerImpl* WebMediaPlayerImpl);
  void ExitedFullscreenExt();
  bool DoLoadExt(WebMediaPlayer::CorsMode cors_mode, bool is_cache_disabled);
  void PlayExt();
  void PlayCapabilitiesExt();
  void PauseCapabilitiesExt();
  void OnMetadataExt();
  void OnPageHiddenExt(bool storing_in_bfcache);
  void CreateRendererExtSetRendererType();
  std::unique_ptr<media::Renderer> CreateRendererExtConfigRenderer(media::RequestOverlayInfoCB request_overlay_info_cb);
  void SetSuspendStateExt();
  void OnIdleTimeoutExt();
  void PauseExt();
  void DoSeekExt(base::TimeDelta time);
  void SetVolumeExt(double volume);
  void OnFrameShownExt();
  void OnFrameHiddenExt();
  void SetPosterExt(const WebURL& poster);
  bool UpdatePlayStateExt(bool can_auto_suspend);
  void SetDelegateStateExt();
  void UpdatePlayState_ComputePlayStateExt(bool& idle_suspended, bool& can_stay_suspended);
  bool ShouldPausePlaybackWhenHiddenExt();
  void DidEndAVSessionExt();
  void SuspendCdmSessionExt();
  void ResumeCdmSessionExt();
};

}

#endif //THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_WEB_MEDIA_PLAYER_IMPL_UTILS_H_
