// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_OHOS_AUDIO_FOCUS_CONTROLLER_H_
#define MEDIA_AUDIO_OHOS_AUDIO_FOCUS_CONTROLLER_H_

#include "media/base/audio_parameters.h"
#include "content/browser/media/session/media_session_impl.h"
#if BUILDFLAG(ARKWEB_TEST)
#undef private
#endif  // ARKWEB_TEST
#include "content/browser/web_contents/web_contents_impl.h"
#if BUILDFLAG(ARKWEB_TEST)
#define private public
#endif  // ARKWEB_TEST
#include "content/public/browser/web_contents.h"

namespace media {
class OHOSAudioFocusController {
public:
  // OHOSAudioOutputStream mediaSession State
  static void OnSuspend(const AudioParameters& parameters);
  
  static MediaContentType GetMediaContentType(const AudioParameters& parameters);

  static bool IsActive(const AudioParameters& parameters);

  static bool HasOnlyOneShotPlayersPublic(const AudioParameters& parameters);

  static content::MediaSessionImpl::NWebMediaSessionState GetSessionState(const AudioParameters& parameters);

  static bool GetPlayingState(const AudioParameters& parameters);

  static bool GetMuteState(const AudioParameters& parameters);

  static void OneShotMediaPlayerStopped(const AudioParameters& parameters);

  static bool GetAudioExclusive(const AudioParameters& parameters);

  // AudioRendererCallback mediaSession State
  static int GetAudioResumeInterval(const AudioParameters& parameters);

  static int GetAudioSessionType(const AudioParameters& parameters);

  static void OnResume(const AudioParameters& parameters);

  static bool IsSuspended(const AudioParameters& parameters);

  static bool GetMediaPlayerMuteState(const AudioParameters& parameters);

  // Suspend other Avplayer and OhosAudioOutputStream playback.
  static void SuspendOtherPlaybacks(const content::WebContentsImpl* webContentsImpl);

  static void SuspendOtherPlaybacks(const AudioParameters& params);
private:
  // OHOSAudioOutputStream mediaSession State
  static bool CheckActiveOnUIThread(const AudioParameters& params);

  static MediaContentType GetContentTypeOnUIThread(const AudioParameters& params);

  static bool CheckOneShotPlayersOnUIThread(const AudioParameters& params);

  static content::MediaSessionImpl::NWebMediaSessionState
      CheckGetSessionStateOnUIThread(const AudioParameters& params);

  static bool CheckGetPlayingStateOnUIThread(const AudioParameters& params);

  static bool CheckGetMuteStateOnUIThread(const AudioParameters& params);

  static bool CheckGetAudioExclusiveUIThread(const AudioParameters& params);

  // AudioRendererCallback mediaSession State
  static int CheckGetAudioResumeIntervalUIThread(const AudioParameters& params);

  static int CheckGetAudioSessionTypeUIThread(const AudioParameters& params);

  static bool CheckIsSuspendedUIThread(const AudioParameters& params);

  static bool CheckGetMediaPlayerMuteStateOnUIThread(const AudioParameters& params);

  static void CheckSuspendOtherPlaybacksUIThread(const content::WebContentsImpl* webContentsImpl);

  static void CheckSuspendOtherPlaybacksUIThread(const AudioParameters& params);
};

} // media

#endif // MEDIA_AUDIO_OHOS_AUDIO_FOCUS_CONTROLLER_H_
