/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_NATIVE_MEDIA_PLAYER_HANDLER_H_
#define ARK_WEB_NATIVE_MEDIA_PLAYER_HANDLER_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=web core)--*/
class ArkWebNativeMediaPlayerHandler : public virtual ArkWebBaseRefCounted {
public:
  /*--ark web()--*/
  virtual void HandleStatusChanged(int status) = 0;

  /*--ark web()--*/
  virtual void HandleVolumeChanged(double volume) = 0;

  /*--ark web()--*/
  virtual void HandleMutedChanged(bool isMuted) = 0;

  /*--ark web()--*/
  virtual void HandlePlaybackRateChanged(double playbackRate) = 0;

  /*--ark web()--*/
  virtual void HandleDurationChanged(double duration) = 0;

  /*--ark web()--*/
  virtual void HandleTimeUpdate(double playTime) = 0;

  /*--ark web()--*/
  virtual void HandleBufferedEndTimeChanged(double bufferedEndTime) = 0;

  /*--ark web()--*/
  virtual void HandleEnded() = 0;

  /*--ark web()--*/
  virtual void HandleNetworkStateChanged(int state) = 0;

  /*--ark web()--*/
  virtual void HandleReadyStateChanged(int state) = 0;

  /*--ark web()--*/
  virtual void HandleFullScreenChanged(bool isFullScreen) = 0;

  /*--ark web()--*/
  virtual void HandleSeeking() = 0;

  /*--ark web()--*/
  virtual void HandleSeekFinished() = 0;

  /*--ark web()--*/
  virtual void HandleError(int error, const ArkWebString &message) = 0;

  /*--ark web()--*/
  virtual void HandleVideoSizeChanged(double width, double height) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_MEDIA_PLAYER_HANDLER_H_
