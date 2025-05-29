// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_RENDERER_HOST_MEDIA_VIDEO_CAPTURE_MANAGER_H_EXT_
#define CONTENT_BROWSER_RENDERER_HOST_MEDIA_VIDEO_CAPTURE_MANAGER_H_EXT_

#include "arkweb/build/features/features.h"
#include "content/browser/renderer_host/media/video_capture_manager.h"
#include "content/browser/renderer_host/media/video_capture_provider.h"

namespace content {

class VideoCaptureManager;

class VideoCaptureManagerExt : public VideoCaptureManager {
public:
  VideoCaptureManagerExt(
      std::unique_ptr<content::VideoCaptureProvider> video_capture_provider,
      base::RepeatingCallback<void(const std::string&)> emit_log_message_cb);

  VideoCaptureManagerExt* AsVideoCaptureManagerExt() {
    return this;
  }

#if BUILDFLAG(ARKWEB_WEBRTC)
  void StartCamera(int nWebId) const;
  void StopCamera(int nWebId) const;
  void CloseCamera(int nWebId) const;
  void BindSessionIdToNWebId(media::VideoCaptureSessionId sessionId, int nWebId);
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  void StopScreenCapture(const std::string& session_id);
  void SetScreenCapturePickerShow();
  void DisableSessionReuse();
  void ScreenCaptureOpened(const std::string& session_id);
  void OnScreenCaptureOpened(const std::string& session_id);
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)
};
}

#endif  // CONTENT_BROWSER_RENDERER_HOST_MEDIA_VIDEO_CAPTURE_MANAGER_H_EXT_