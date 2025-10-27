// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_INCLUDE_CEF_MEDIA_PLAYER_CONTROLLER_H_
#define CEF_INCLUDE_CEF_MEDIA_PLAYER_CONTROLLER_H_

#include <cstdint>

class CefMediaPlayerController {
 public:
  virtual ~CefMediaPlayerController() = default;
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

#endif // CEF_INCLUDE_CEF_MEDIA_PLAYER_CONTROLLER_H_