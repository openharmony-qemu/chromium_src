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

#ifndef OHOS_NWEB_SRC_CEF_DELEGATE_CUSTOM_MEDIA_PLAYER_IMPL_H_
#define OHOS_NWEB_SRC_CEF_DELEGATE_CUSTOM_MEDIA_PLAYER_IMPL_H_

#include "cef/include/internal/cef_ptr.h"
#include "cef/ohos_cef_ext/include/cef_custom_media_player_delegate.h"
#include "ohos_nweb/include/nweb_native_media_player.h"

namespace OHOS::NWeb {

class CustomMediaPlayerImpl : public CefCustomMediaPlayerDelegate {
 public:
  explicit CustomMediaPlayerImpl(
      std::shared_ptr<NWebNativeMediaPlayerBridge> bridge);
  ~CustomMediaPlayerImpl() override;

  void UpdateLayerRect(int x, int y, int width, int height) override;
  void Play() override;
  void Pause() override;
  void Seek(double target_time) override;
  void SetVolume(double volume) override;
  void SetMuted(bool muted) override;
  void SetPlaybackRate(double rate) override;
  void Release() override;
  void EnterFullscreen() override;
  void ExitFullscreen() override;
  void ResumeMediaPlayer() override;
  void SuspendMediaPlayer(int suspend_type) override;

 private:
  std::shared_ptr<NWebNativeMediaPlayerBridge> bridge_;
};

}  // namespace OHOS::NWeb
#endif  // OHOS_NWEB_SRC_CEF_DELEGATE_CUSTOM_MEDIA_PLAYER_IMPL_H_
