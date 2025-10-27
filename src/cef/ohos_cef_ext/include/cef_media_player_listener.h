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

#ifndef CEF_INCLUDE_CEF_MEDIA_PLAYER_LISTENER_H_
#define CEF_INCLUDE_CEF_MEDIA_PLAYER_LISTENER_H_

#include "build/build_config.h"
#include "cef/include/cef_base.h"

///
/// Class used to monitor custom media player status.
///
/*--cef(source=library)--*/
class CefMediaPlayerListener : public virtual CefBaseRefCounted {
 public:
  ///
  /// triggered on CefMediaPlayerListenerForVAST released
  ///
  /*--cef()--*/
  virtual ~CefMediaPlayerListener() = default;

  ///
  /// triggered on media playing state changed
  ///
  /*--cef()--*/
  virtual void OnStatusChanged(uint32_t status) = 0;

  ///
  /// triggered on media volume changed
  ///
  /*--cef()--*/
  virtual void OnVolumeChanged(double volume) = 0;

  ///
  /// triggered on media mute state changed
  ///
  /*--cef()--*/
  virtual void OnMutedChanged(bool muted) = 0;

  ///
  /// triggered on media playback sppeed changed
  ///
  /*--cef()--*/
  virtual void OnPlaybackRateChanged(double playback_rate) = 0;

  ///
  /// triggered on media duration changed
  ///
  /*--cef()--*/
  virtual void OnDurationChanged(double duration) = 0;

  ///
  /// triggered on media current playing time changed
  ///
  /*--cef()--*/
  virtual void OnTimeUpdate(double current_time) = 0;

  ///
  /// triggered on media buffered last length changed
  ///
  /*--cef()--*/
  virtual void OnBufferedEndTimeChanged(double buffered_time) = 0;

  ///
  /// triggered on media play at the end.
  ///
  /*--cef()--*/
  virtual void OnEnded() = 0;

  ///
  /// triggered on media network state changed
  ///
  /*--cef()--*/
  virtual void OnNetworkStateChanged(uint32_t state) = 0;

  ///
  /// triggered on media ready state changd
  ///
  /*--cef()--*/
  virtual void OnReadyStateChanged(uint32_t state) = 0;

  ///
  /// triggered on media fullscreen state changed.
  ///
  /*--cef()--*/
  virtual void OnFullscreenChanged(bool fullscreen) = 0;

  ///
  /// triggered on media is seeking
  ///
  /*--cef()--*/
  virtual void OnSeeking() = 0;

  ///
  /// triggered on media seek finished
  ///
  /*--cef()--*/
  virtual void OnSeekFinished() = 0;

  ///
  /// triggered on media playback error
  ///
  /*--cef()--*/
  virtual void OnError(uint32_t error_code, const CefString& error_msg) = 0;

  ///
  /// triggered on media size changed.
  ///
  /*--cef()--*/
  virtual void OnVideoSizeChanged(int width, int height) = 0;
};

#endif  // CEF_INCLUDE_CEF_MEDIA_PLAYER_LISTENER_H_
