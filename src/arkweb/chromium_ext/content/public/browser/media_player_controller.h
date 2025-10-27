// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_BROWSER_MEDIA_PLAYER_CONTROLLER_H_
#define CONTENT_PUBLIC_BROWSER_MEDIA_PLAYER_CONTROLLER_H_

#include <cstdint>

namespace content {

class MediaPlayerController {
 public:
  virtual ~MediaPlayerController() = default;
  virtual void Play() {}
  virtual void Pause() {}
  virtual void Seek(double time) {}
  virtual void SetMuted(bool muted) {}
  virtual void SetPlaybackRate(double playback_rate) {}
  virtual void ExitFullscreen() {}
  virtual bool SetVideoSurface(void* native_window) { return false; }
  virtual void Download() {}
  virtual void SetVolume(double volume) {}
  virtual double GetVolume() { return 1.0; }
};

} // namespace content

#endif // CONTENT_PUBLIC_BROWSER_MEDIA_PLAYER_CONTROLLER_H_
