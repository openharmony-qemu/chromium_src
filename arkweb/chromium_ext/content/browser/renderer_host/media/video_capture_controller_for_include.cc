// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_RENDERER_HOST_MEDIA_VIDEO_CAPTURE_CONTROLLER_H_
#error \
    "must be in include form CONTENT_BROWSER_RENDERER_HOST_MEDIA_VIDEO_CAPTURE_CONTROLLER_H_"
#endif

namespace content {
#if BUILDFLAG(ARKWEB_WEBRTC)
void VideoCaptureController::PauseClientBySessionId(
    const base::UnguessableToken& session_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);

  ControllerClient* client = FindClient(session_id, controller_clients_);
  if (!client) {
    DVLOG(1) << "Client not found";
    return;
  }

  if (client->paused) {
    DVLOG(1) << "Calling pause on paused client";
    return;
  }

  if (client->event_handler) {
    client->event_handler->OnCameraCaptureStateChanged(CameraCaptureState::PAUSED);
  }
  client->paused = true;
}

void VideoCaptureController::ResumeClientBySessionId(
    const base::UnguessableToken& session_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);

  ControllerClient* client = FindClient(session_id, controller_clients_);
  if (!client) {
    DVLOG(1) << "Client not found";
    return;
  }

  if (!client->paused) {
    DVLOG(1) << "Calling resume on unpaused client";
    return;
  }
  if (client->event_handler) {
    client->event_handler->OnCameraCaptureStateChanged(CameraCaptureState::ACTIVE);
  }
  client->paused = false;
}
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
void VideoCaptureController::SetScreenCaptureListener(
    VideoCaptureManager* video_capture_manager) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  video_capture_manager_ = video_capture_manager;
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)
}  // namespace content