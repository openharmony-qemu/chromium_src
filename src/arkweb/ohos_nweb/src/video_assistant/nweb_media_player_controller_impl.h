// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_NWEB_SRC_VIDEO_ASSISTANT_NWEB_MEDIA_PLAYER_CONTROLLER_IMPL_H_
#define OHOS_NWEB_SRC_VIDEO_ASSISTANT_NWEB_MEDIA_PLAYER_CONTROLLER_IMPL_H_

#include <memory>
#include "ohos_nweb/src/capi/nweb_media_player_controller.h"

class CefMediaPlayerController;

namespace OHOS::NWeb {

class NWebMediaPlayerControllerImpl : public NWebMediaPlayerController {
 public:
  NWebMediaPlayerControllerImpl(std::unique_ptr<CefMediaPlayerController>);
  ~NWebMediaPlayerControllerImpl() override;

 protected:
  void Play() override;
  void Pause() override;
  void Seek(double time) override;
  void SetMuted(bool muted) override;
  void SetPlaybackRate(double playback_rate) override;
  void ExitFullscreen() override;
  void SetVideoSurface(void* native_window) override;
  void Download() override;
  void SetVolume(double volume) override;
  double GetVolume() override;
 private:
  std::unique_ptr<CefMediaPlayerController> cef_controller_;
};

} // namespace OHOS::NWeb

#endif // OHOS_NWEB_SRC_VIDEO_ASSISTANT_NWEB_MEDIA_PLAYER_CONTROLLER_IMPL_H_