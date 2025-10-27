// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_ALLOY_CEF_MEDIA_PLAYER_CONTROLLER_IMPL_H_
#define CEF_LIBCEF_BROWSER_ALLOY_CEF_MEDIA_PLAYER_CONTROLLER_IMPL_H_

#include <memory>
#include <utility>
#include "include/cef_media_player_controller.h"

namespace content {
class MediaPlayerController;
} // namepsace content

class CefMediaPlayerControllerImpl : public CefMediaPlayerController {
 public:
  CefMediaPlayerControllerImpl(
      std::unique_ptr<content::MediaPlayerController> media_player_controller);
  ~CefMediaPlayerControllerImpl() override;
  void Play() override;
  void Pause() override;
  void Seek(double time) override;
  void SetMuted(bool muted) override;
  void SetPlaybackRate(double playback_rate) override;
  void ExitFullscreen() override;
  bool SetVideoSurface(void* native_window) override;
  void Download() override;
  void SetVolume(double volume) override;
  double GetVolume() override;
 private:
  std::unique_ptr<content::MediaPlayerController> media_player_controller_;
};

#endif // CEF_LIBCEF_BROWSER_ALLOY_CEF_MEDIA_PLAYER_CONTROLLER_IMPL_H_
