/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_NATIVE_MEDIA_PLAYER_HANDLER_IMPL_H_
#define OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_NATIVE_MEDIA_PLAYER_HANDLER_IMPL_H_

#include "cef/ohos_cef_ext/include/cef_media_player_listener.h"
#include "nweb_native_media_player.h"

namespace OHOS::NWeb {

class NWebNativeMediaPlayerHandlerImpl : public NWebNativeMediaPlayerHandler {
 public:
  explicit NWebNativeMediaPlayerHandlerImpl(
      CefOwnPtr<CefMediaPlayerListener> listener);
  ~NWebNativeMediaPlayerHandlerImpl() override;

  void HandleStatusChanged(PlaybackStatus status) override;
  void HandleVolumeChanged(double volume) override;
  void HandleMutedChanged(bool muted) override;
  void HandlePlaybackRateChanged(double playback_rate) override;
  void HandleDurationChanged(double duration) override;
  void HandleTimeUpdate(double current_time) override;
  void HandleBufferedEndTimeChanged(double buffered_time) override;
  void HandleEnded() override;
  void HandleNetworkStateChanged(NetworkState state) override;
  void HandleReadyStateChanged(ReadyState state) override;
  void HandleFullScreenChanged(bool isFullScreen) override;
  void HandleSeeking() override;
  void HandleSeekFinished() override;
  void HandleError(MediaError error_code,
                   const std::string& error_msg) override;
  void HandleVideoSizeChanged(double width, double height) override;

 private:
  CefOwnPtr<CefMediaPlayerListener> listener_;
};

}  // namespace OHOS::NWeb

#endif  // OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_NATIVE_MEDIA_PLAYER_HANDLER_IMPL_H_
