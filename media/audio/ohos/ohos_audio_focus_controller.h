// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/audio_parameters.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/public/browser/web_contents.h"

namespace media {
class OHOSAudioFocusController {
public:
  // OHOSAudioOutputStream mediaSession State
  static void OnSuspend(const AudioParameters& parameters);

  static bool IsActive(const AudioParameters& parameters);

  static bool HasOnlyOneShotPlayersPublic(const AudioParameters& parameters);

  static bool GetPlayingState(const AudioParameters& parameters);

  static bool GetMuteState(const AudioParameters& parameters);

  static bool GetMediaSession(const AudioParameters& parameters);

  static void OneShotMediaPlayerStopped(const AudioParameters& parameters);

  static bool GetAudioExclusive(const AudioParameters& parameters);

  // AudioRendererCallback mediaSession State
  static int GetAudioResumeInterval(const AudioParameters& parameters);

  static void OnResume(const AudioParameters& parameters);

  static bool IsSuspended(const AudioParameters& parameters);

private:
  // OHOSAudioOutputStream mediaSession State
  static bool CheckActiveOnUIThread(const AudioParameters& params);

  static bool CheckOneShotPlayersOnUIThread(const AudioParameters& params);

  static bool CheckGetPlayingStateOnUIThread(const AudioParameters& params);

  static bool CheckGetMuteStateOnUIThread(const AudioParameters& params);

  static bool CheckGetMediaSessionUIThread(const AudioParameters& params);

  static bool CheckGetAudioExclusiveUIThread(const AudioParameters& params);

  // AudioRendererCallback mediaSession State
  static int CheckGetAudioResumeIntervalUIThread(const AudioParameters& params);

  static bool CheckIsSuspendedUIThread(const AudioParameters& params);

};

} // media