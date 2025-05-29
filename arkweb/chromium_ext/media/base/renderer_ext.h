// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_RENDERER_EXT_H_
#define MEDIA_BASE_RENDERER_EXT_H_

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "arkweb/chromium_ext/media/base/action_reason.h"
#include "base/containers/flat_map.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

namespace media {

class RendererExt {
public:
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual void Initialize(CreateTextureCB create_texture_cb,
                          DestroyTextureCB destroy_texture_cb) {}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  virtual void SetSurfaceId(int surface_id, const gfx::Rect& rect) {}
  virtual void SetMediaPlayerState(bool is_suspend, int suspend_type) {}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA)
  virtual void SetNativeWindowSurface(int native_window_id) {}
#endif // ARKWEB_MEDIA

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  struct MediaSourceInfo {
    std::string media_source;
    std::string media_format;
  };

  virtual void SetMediaSourceList(
      const std::vector<MediaSourceInfo>& source_infos) {}
  virtual void SetMediaControls(bool show_media_controls,
                                const std::vector<std::string>& controls_list) {
  }
  virtual void SetMuted(bool muted) {}
  virtual void SetPoster(const std::string& poster_url) {}
  virtual void SetAttributes(
      base::flat_map<std::string, std::string> attributes) {}
  virtual void SetReferrer(const std::string& referrer) {}
  using OnGetRectCallback = base::RepeatingCallback<void(const gfx::Rect&)>;
  using SurfaceCreatedCallback =
      base::OnceCallback<void(int, OnGetRectCallback)>;
  virtual void SetSurfaceCreatedCallback(SurfaceCreatedCallback cb) {}
  using UpdatePlaybackStatusCallback = base::RepeatingCallback<void(uint32_t)>;
  virtual void SetUpdatePlaybackStatusCallback(
      UpdatePlaybackStatusCallback cb) {}
  using UpdateVolumeCallback = base::RepeatingCallback<void(double)>;
  virtual void SetUpdateVolumeCallback(UpdateVolumeCallback cb) {}
  using UpdateMutedCallback = base::RepeatingCallback<void(bool)>;
  virtual void SetUpdateMutedCallback(UpdateMutedCallback cb) {}
  using UpdatePlaybackRateCallback = base::RepeatingCallback<void(double)>;
  virtual void SetUpdatePlaybackRateCallback(UpdatePlaybackRateCallback cb) {}

  virtual void SetIsAudio(bool is_audio) {}

  virtual void SetPlaybackRateWithReason(double playback_rate,
                                         ActionReason reason) {}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  virtual void SetVideoSurface(int32_t surface_id) {}
#endif  // ARKWEB_VIDEO_ASSISTANT
};

}
#endif // MEDIA_BASE_RENDERER_EXT_H_