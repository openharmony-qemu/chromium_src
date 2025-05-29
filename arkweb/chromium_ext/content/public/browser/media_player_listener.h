// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_BROWSER_MEDIA_PLAYER_LISTENER_H_
#define CONTENT_PUBLIC_BROWSER_MEDIA_PLAYER_LISTENER_H_

#include <cstdint>
#include <string>

namespace content {

class MediaPlayerListener {
 public:
  virtual ~MediaPlayerListener() = default;
  virtual void OnStatusChanged(uint32_t status) {}
  virtual void OnMutedChanged(bool muted) {}
  virtual void OnPlaybackRateChanged(double playback_rate) {}
  virtual void OnDurationChanged(double duration) {}
  virtual void OnTimeUpdate(double current_time) {}
  virtual void OnBufferedEndTimeChanged(double buffered_end_time) {}
  virtual void OnEnded() {}
  virtual void OnFullscreenChanged(bool fullscreen) {}
  virtual void OnSeeking() {}
  virtual void OnSeekFinished() {}
  virtual void OnError(uint32_t error_code, const std::string& error_msg) {}
  virtual void OnVideoSizeChanged(int width, int height) {}
  virtual void OnFullscreenOverlayChanged(bool fullscreen_overlay) {}
};

} // namepsace content

#endif // CONTENT_PUBLIC_BROWSER_MEDIA_PLAYER_LISTENER_H_
