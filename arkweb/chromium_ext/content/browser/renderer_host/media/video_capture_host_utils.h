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

#ifndef VIDEO_CAPTURE_HOST_UTILS_H_
#define VIDEO_CAPTURE_HOST_UTILS_H_

#include "arkweb/build/features/features.h"
#include "content/public/browser/global_routing_id.h"
#if BUILDFLAG(ARKWEB_WEBRTC)
#include "content/browser/renderer_host/media/video_capture_controller_event_handler.h"
#include "content/browser/renderer_host/media/video_capture_host.h"
#endif

namespace content {
class VideoCaptureHost;

class VideoCaptureHostUtils {
public:
  raw_ptr<VideoCaptureHost> videoCaptureHost = nullptr;
  VideoCaptureHostUtils(VideoCaptureHost* impl);

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  void SetRenderFrameHostId(GlobalRenderFrameHostId render_frame_host_id);
  static void ReportStartScreenCaptureBind(int child_id);
  static void ReportStopScreenCaptureBind(int child_id);
  void ReportStartScreenCapture();
  void ReportStopScreenCapture();
#endif
#if BUILDFLAG(ARKWEB_WEBRTC)
  void OnCameraCaptureStateChanged(CameraCaptureState new_state);
  void OnCameraCaptureStarted(const VideoCaptureControllerID& controller_id);
#endif

private:
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  GlobalRenderFrameHostId render_frame_host_id_;
#endif
#if BUILDFLAG(ARKWEB_WEBRTC)
  CameraCaptureState camera_state_ = CameraCaptureState::NONE;
#endif
};
}
#endif