// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_MEDIA_PLAYER_FOR_VAST_H_
#define OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_MEDIA_PLAYER_FOR_VAST_H_

#include <memory>
#include "cef/ohos_cef_ext/include/cef_media_player_listener_for_vast.h"

class NWebMediaPlayerListener;

namespace OHOS::NWeb {

class NWebMediaPlayerListenerForVAST : public CefMediaPlayerListenerForVAST {
 public:
  NWebMediaPlayerListenerForVAST(
      std::unique_ptr<NWebMediaPlayerListener> nweb_listener);
  ~NWebMediaPlayerListenerForVAST() override;

  void OnStatusChanged(uint32_t status) override;
  void OnMutedChanged(bool muted) override;
  void OnPlaybackRateChanged(double playback_rate) override;
  void OnDurationChanged(double duration) override;
  void OnTimeUpdate(double current_time) override;
  void OnBufferedEndTimeChanged(double buffered_time) override;
  void OnEnded() override;
  void OnFullscreenChanged(bool fullscreen) override;
  void OnSeeking() override;
  void OnSeekFinished() override;
  void OnError(uint32_t error_code, const std::string& error_msg) override;
  void OnVideoSizeChanged(int width, int height) override;
  void OnFullscreenOverlayChanged(bool fullscreen_overlay) override;
  void OnVolumeChanged(double volume) override;

 private:
  std::unique_ptr<NWebMediaPlayerListener> nweb_listener_;
};

} // namespace OHOS::NWeb

#endif // OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_MEDIA_PLAYER_FOR_VAST_H_
