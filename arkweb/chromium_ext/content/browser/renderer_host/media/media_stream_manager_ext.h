// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_RENDERER_HOST_MEDIA_MEDIA_STREAM_MANAGER_H_EXT_
#define CONTENT_BROWSER_RENDERER_HOST_MEDIA_MEDIA_STREAM_MANAGER_H_EXT_

#include "content/browser/renderer_host/media/media_stream_manager.h"

namespace content {

class MediaStreamManage;

class MediaStreamManagerExt : public MediaStreamManager {
public:
  explicit MediaStreamManagerExt(media::AudioSystem* audio_system);

  MediaStreamManagerExt(
      media::AudioSystem* audio_system,
      std::unique_ptr<VideoCaptureProvider> video_capture_provider);

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  using ScreenCaptureCallback = base::RepeatingCallback<
      void(int32_t nweb_id, const char* session_id, int32_t code)>;

  static void SetScreenCaptureDelegateCallback(ScreenCaptureCallback callback);
  static void SendScreenCaptureStateToNative(int32_t nweb_id,
                                             const std::string& session_id,
                                             int32_t state);
  static ScreenCaptureCallback screen_capture_callback_;
  void StopScreenCapture(int32_t nweb_id, const std::string& session_id);
  void SetScreenCapturePickerShow();
  void DisableSessionReuse();
  void SendScreenCaptureState(const std::string& session_id, int32_t state);
  void OnScreenCaptureOpened(const std::string& session_id) override;
  void RemoveNWebIdBySession(const base::UnguessableToken& capture_session_id);
  void AddNWebIdBySession(int32_t nweb_id, const base::UnguessableToken& session_id);
#endif  // BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_WEBRTC)
  int GetNWebIdMatchStreamType(GlobalRenderFrameHostId host_id);
  void CloseAudioCapture(int32_t nweb_id); 
#endif

  MediaStreamManagerExt* AsMediaStreamManagerExt() override {
    return this;
  }
#if BUILDFLAG(ARKWEB_TEST)
public:
#else
private:
#endif // ARKWEB_TEST
#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  std::map<std::string, int> nweb_id_maps_;
  mutable std::mutex nweb_id_mutex_;
  std::list<SessionIdState> session_id_state_;
#endif  // BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  base::WeakPtrFactory<MediaStreamManagerExt> weak_factory_{this};
  
};
} // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_MEDIA_MEDIA_STREAM_MANAGER_H_EXT_