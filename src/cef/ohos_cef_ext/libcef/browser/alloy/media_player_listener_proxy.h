// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_ALLOY_MEDIA_PLAYER_LISTENER_PROXY_H_
#define CEF_LIBCEF_BROWSER_ALLOY_MEDIA_PLAYER_LISTENER_PROXY_H_

#include <memory>
#include <utility>
#include "content/public/browser/media_player_listener.h"

class CefMediaPlayerListenerForVAST;

class MediaPlayerListenerProxy : public content::MediaPlayerListener {
 public:
  MediaPlayerListenerProxy(
      std::unique_ptr<CefMediaPlayerListenerForVAST> cef_listener);
  ~MediaPlayerListenerProxy() override;

  void OnStatusChanged(uint32_t status) override;
  void OnMutedChanged(bool muted) override;
  void OnPlaybackRateChanged(double playback_rate) override;
  void OnDurationChanged(double duration) override;
  void OnTimeUpdate(double current_time) override;
  void OnBufferedEndTimeChanged(double buffered_end_time) override;
  void OnEnded() override;
  void OnFullscreenChanged(bool fullscreen) override;
  void OnSeeking() override;
  void OnSeekFinished() override;
  void OnError(uint32_t error_code, const std::string& error_msg) override;
  void OnVideoSizeChanged(int width, int height) override;
  void OnFullscreenOverlayChanged(bool fullscreen_overlay) override;
  void OnVolumeChanged(double volume) override;

 private:
  std::unique_ptr<CefMediaPlayerListenerForVAST> cef_listener_;
};

#endif // CEF_LIBCEF_BROWSER_ALLOY_MEDIA_PLAYER_LISTENER_PROXY_H_
