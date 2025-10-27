// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/permission/alloy_access_request.h"
#include "libcef/browser/permission/alloy_permission_request_handler.h"

#if BUILDFLAG(ARKWEB_WEBRTC)
#include "base/logging.h"
#include "chrome/browser/media/webrtc/media_capture_devices_dispatcher.h"
#include "content/public/browser/media_capture_devices.h"
#include "third_party/blink/public/mojom/mediastream/media_stream.mojom.h"
#include "media/audio/audio_device_description.h"
#include "third_party/blink/public/common/permissions/permission_utils.h"
#include "components/permissions/permission_util.h"
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

namespace {
  constexpr int32_t kSystemAudioSourceId = -2;
}

AlloyAccessRequest::AlloyAccessRequest(const CefString& origin,
                                       int resources,
                                       cef_permission_callback_t callback)
    : origin_(origin), resources_(resources), callback_(std::move(callback)) {}

AlloyAccessRequest::~AlloyAccessRequest() {
#if BUILDFLAG(IS_OHOS)
  if (!callback_.is_null()) {
    std::move(callback_).Run(false);
  }
#else
  std::move(callback_).Run(false);
#endif
}

CefString AlloyAccessRequest::Origin() {
  return origin_;
}

int AlloyAccessRequest::ResourceAcessId() {
  return resources_;
}

void AlloyAccessRequest::ReportRequestResult(bool allowed) {
#if BUILDFLAG(IS_OHOS)
  if (!callback_.is_null()) {
    std::move(callback_).Run(allowed);
  }
#else
  std::move(callback_).Run(allowed);
#endif
}

#if BUILDFLAG(ARKWEB_SENSOR)
AlloySensorAccessRequest::AlloySensorAccessRequest(
    CefBrowserHostBase* const browser,
    const CefString& origin,
    cef_permission_callback_t callback)
    : browser_(browser), origin_(origin), callback_(std::move(callback)) {}

AlloySensorAccessRequest::~AlloySensorAccessRequest() {
  if (!callback_.is_null()) {
    std::move(callback_).Run(false);
  }
}

CefString AlloySensorAccessRequest::Origin() {
  return origin_;
}

int AlloySensorAccessRequest::ResourceAcessId() {
  return AlloyAccessRequest::Resources::SENSORS;
}

void AlloySensorAccessRequest::ReportRequestResult(bool allowed) {
  if (callback_.is_null()) {
    LOG(ERROR) << "ReportRequestResult callback is null.";
    return;
  }
  LOG(INFO) << "ReportRequestResult, permission status: " << allowed;
  if (browser_) {
    AlloyPermissionRequestHandler* permission_handler =
        browser_->GetPermissionRequestHandler();
    if (permission_handler) {
      permission_handler->SetSensorPermission(origin_, allowed);
    }
  }
  std::move(callback_).Run(allowed);
}
#endif  // BUILDFLAG(ARKWEB_SENSOR)

#if BUILDFLAG(ARKWEB_WEBRTC)
std::map<GURL, int32_t> AlloyMediaAccessRequest::camera_permission_ = {};
std::map<GURL, int32_t> AlloyMediaAccessRequest::microphone_permission_ = {};

AlloyMediaAccessRequest::AlloyMediaAccessRequest(
    CefBrowserHostBase* const browser,
    const content::MediaStreamRequest& request,
    content::MediaResponseCallback callback)
    : browser_(browser), request_(request), callback_(std::move(callback)) {
  requesting_origin_ = GetMediaAccessRequestOriginAsURL();
}

AlloyMediaAccessRequest::~AlloyMediaAccessRequest() {
  LOG(INFO) << "AlloyMediaAccessRequest::~AlloyMediaAccessRequest()";
  if (!callback_.is_null()) {
    std::move(callback_).Run(
        blink::mojom::StreamDevicesSet(),
        blink::mojom::MediaStreamRequestResult::PERMISSION_DENIED,
        std::unique_ptr<content::MediaStreamUI>());
  }

  if (AlloyMediaAccessRequest::microphone_permission_.count(requesting_origin_)) {
    AlloyMediaAccessRequest::microphone_permission_.erase(requesting_origin_);
  }
  if (AlloyMediaAccessRequest::camera_permission_.count(requesting_origin_)) {
    AlloyMediaAccessRequest::camera_permission_.erase(requesting_origin_);
  }
}

CefString AlloyMediaAccessRequest::Origin() {
  return request_.security_origin.spec();
}

int AlloyMediaAccessRequest::ResourceAcessId() {
  return (request_.audio_type ==
                  blink::mojom::MediaStreamType::DEVICE_AUDIO_CAPTURE
              ? AlloyAccessRequest::Resources::AUDIO_CAPTURE
              : 0) |
         (request_.video_type ==
                  blink::mojom::MediaStreamType::DEVICE_VIDEO_CAPTURE
              ? AlloyAccessRequest::Resources::VIDEO_CAPTURE
              : 0);
}

GURL AlloyMediaAccessRequest::GetMediaAccessRequestOriginAsURL() {
  content::RenderFrameHost* renderFrameHost = content::RenderFrameHost::FromID(
      request_.render_process_id, request_.render_frame_id);
  
  if (!renderFrameHost) {
    LOG(ERROR) << "AlloyMediaAccessRequest::GetMediaAccessRequestOriginAsURL, renderFrameHost is null";
    return GURL();
  }
 
  return permissions::PermissionUtil::GetLastCommittedOriginAsURL(renderFrameHost);
}

void AlloyMediaAccessRequest::ReportRequestResult(bool allowed) {
  LOG(WARNING) << "AlloyMediaAccessRequest::ReportRequestResult"
               << ", video_type = " << request_.video_type << ", audio_type = " << request_.audio_type
               << ", allowed = " << allowed;
  content::PermissionStatus status =
      allowed ? content::PermissionStatus::GRANTED : content::PermissionStatus::DENIED;

  if (!allowed) {
    if (!callback_.is_null()) {
      AlloyMediaAccessRequest::microphone_permission_[requesting_origin_] = (int32_t)content::PermissionStatus::DENIED;
      AlloyMediaAccessRequest::camera_permission_[requesting_origin_] = (int32_t)content::PermissionStatus::DENIED;
      std::move(callback_).Run(
          blink::mojom::StreamDevicesSet(),
          blink::mojom::MediaStreamRequestResult::PERMISSION_DENIED,
          std::unique_ptr<content::MediaStreamUI>());
    }
    return;
  }

  if (callback_.is_null()) {
    return;
  }

  blink::mojom::StreamDevicesSet devices_set;
  devices_set.stream_devices.emplace_back(blink::mojom::StreamDevices::New());
  blink::mojom::StreamDevices& stream_devices = *devices_set.stream_devices[0];

  // Based on chrome/browser/media/media_stream_devices_controller.cc
  blink::mojom::MediaStreamRequestResult result =
      blink::mojom::MediaStreamRequestResult::NO_HARDWARE;

  if (request_.audio_type ==
      blink::mojom::MediaStreamType::DEVICE_AUDIO_CAPTURE) {
    if (!request_.requested_audio_device_ids.empty()) {
      LOG(INFO) << "RequestMediaAccessPermission requested_audio_device_id: "
                << request_.requested_audio_device_ids.front();
    }
    blink::MediaStreamDevices devices =
        MediaCaptureDevicesDispatcher::GetInstance()->GetAudioCaptureDevices();
    for (const auto &device: devices) {
      if (request_.requested_audio_device_ids.empty()) {
        break;
      }
      if (request_.requested_audio_device_ids.front() == device.id) {
        stream_devices.audio_device = device;
        result = blink::mojom::MediaStreamRequestResult::OK;
        AlloyMediaAccessRequest::microphone_permission_[requesting_origin_] = (int32_t)status;
      }
    }
  }

  if (request_.video_type ==
      blink::mojom::MediaStreamType::DEVICE_VIDEO_CAPTURE) {
    if (!request_.requested_video_device_ids.empty()) {
      LOG(INFO) << "RequestMediaAccessPermission requested_video_device_id: "
                << request_.requested_video_device_ids.front();
    }
    blink::MediaStreamDevices devices =
      MediaCaptureDevicesDispatcher::GetInstance()->GetVideoCaptureDevices();
    for (const auto &device: devices) {
      if (request_.requested_video_device_ids.empty()) {
        break;
      }
      if (request_.requested_video_device_ids.front() == device.id) {
        stream_devices.video_device = device;
        result = blink::mojom::MediaStreamRequestResult::OK;
        AlloyMediaAccessRequest::camera_permission_[requesting_origin_] = (int32_t)status;
      }
    }
  }
  bool has_video = stream_devices.video_device.has_value();
  bool has_audio = stream_devices.audio_device.has_value();
  auto media_stream_ui =
      browser_->GetMediaStreamRegistrar()->MaybeCreateMediaStreamUI(has_video,
                                                                    has_audio);
  std::move(callback_).Run(devices_set, result, std::move(media_stream_ui));
}

AlloyScreenCaptureAccessRequest::AlloyScreenCaptureAccessRequest(
    CefBrowserHostBase* const browser,
    const content::MediaStreamRequest& request,
    content::MediaResponseCallback callback)
    : browser_(browser),
      request_(request),
      callback_(std::move(callback)),
      mode_(cef_screen_capture_mode_t::CAPTURE_INVAILD_MODE),
      sourceId_(-1),
      audioSourceId_(kSystemAudioSourceId) {}

AlloyScreenCaptureAccessRequest::~AlloyScreenCaptureAccessRequest() {
  if (!callback_.is_null()) {
    std::move(callback_).Run(
        blink::mojom::StreamDevicesSet(),
        blink::mojom::MediaStreamRequestResult::PERMISSION_DENIED,
        std::unique_ptr<content::MediaStreamUI>());
  }
}

CefString AlloyScreenCaptureAccessRequest::Origin() {
  return request_.security_origin.spec();
}

void AlloyScreenCaptureAccessRequest::SetCaptureMode(int32_t mode) {
  LOG(INFO) << "AlloyScreenCaptureAccessRequest::SetCaptureMode, mode = " << mode;
  mode_ = mode;
}

void AlloyScreenCaptureAccessRequest::SetCaptureSourceId(int32_t sourceId) {
  sourceId_ = sourceId;
}

void AlloyScreenCaptureAccessRequest::ReportRequestResult(bool allowed) {
  LOG(WARNING) << "AlloyScreenCaptureAccessRequest::ReportRequestResult"
               << ", video_type = " << request_.video_type << ", audio_type = " << request_.audio_type
               << ", mode = " << mode_ << ", allowed = " << allowed;
  if (!allowed) {
    if (!callback_.is_null()) {
      std::move(callback_).Run(
          blink::mojom::StreamDevicesSet(),
          blink::mojom::MediaStreamRequestResult::PERMISSION_DENIED,
          std::unique_ptr<content::MediaStreamUI>());
    }
    return;
  }

  if (callback_.is_null()) {
    return;
  }

  blink::mojom::StreamDevicesSet devices_set;
  devices_set.stream_devices.emplace_back(blink::mojom::StreamDevices::New());
  blink::mojom::StreamDevices& stream_devices = *devices_set.stream_devices[0];

  // Based on chrome/browser/media/media_stream_devices_controller.cc
  if (request_.audio_type ==
      blink::mojom::MediaStreamType::DEVICE_AUDIO_CAPTURE) {
    blink::MediaStreamDevices devices =
        MediaCaptureDevicesDispatcher::GetInstance()->GetAudioCaptureDevices();
    if (!devices.empty()) {
      stream_devices.audio_device = devices[0];
    }
  }

  content::DesktopMediaID media_audio_id;
  if (request_.audio_type ==
      blink::mojom::MediaStreamType::DISPLAY_AUDIO_CAPTURE) {
    media_audio_id = content::DesktopMediaID(content::DesktopMediaID::TYPE_SCREEN, audioSourceId_);
    blink::MediaStreamDevices devices;
    stream_devices.audio_device =
      blink::MediaStreamDevice(request_.audio_type, media_audio_id.ToString(), "System Audio");
  }

  content::DesktopMediaID media_id;
  if (request_.requested_video_device_ids.empty() ||
      request_.requested_video_device_ids.front().empty()) {
    if (mode_ == cef_screen_capture_mode_t::CAPTURE_HOME_SCREEN_MODE) {
#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
      auto web_contents = browser_->GetWebContents();
      media_id = content::DesktopMediaID(content::DesktopMediaID::TYPE_SCREEN,
                                         web_contents->GetNWebId());
#else
      media_id = content::DesktopMediaID(content::DesktopMediaID::TYPE_SCREEN,
                                         -1 /* webrtc::kFullDesktopScreenId */);
#endif
    } else if (mode_ ==
               cef_screen_capture_mode_t::CAPTURE_SPECIFIED_SCREEN_MODE) {
      media_id = content::DesktopMediaID(content::DesktopMediaID::TYPE_SCREEN,
                                         sourceId_);
    } else if (mode_ ==
               cef_screen_capture_mode_t::CAPTURE_SPECIFIED_WINDOW_MODE) {
      media_id = content::DesktopMediaID(content::DesktopMediaID::TYPE_WINDOW,
                                         sourceId_);
    }
  } else {
    LOG(INFO) << "[screen_webrtc_logging]ReportRequestResult, requested_video_device_id is not empty";
    media_id = content::DesktopMediaID::Parse(
        request_.requested_video_device_ids.front());
  }
  if (media_id.is_null()) {
    LOG(INFO) << "[screen_webrtc_logging]ReportRequestResult, media_id is invalid(type == TYPE_NONE)";
    std::move(callback_).Run(devices_set,
                           blink::mojom::MediaStreamRequestResult::INVALID_STATE,
                           nullptr);
    return;
  }
  stream_devices.video_device = blink::MediaStreamDevice(
      request_.video_type, media_id.ToString(), "Screen");
  bool has_audio = stream_devices.audio_device.has_value();
  auto media_stream_ui =
      browser_->GetMediaStreamRegistrar()->MaybeCreateMediaStreamUI(true,
                                                                    has_audio);
  std::move(callback_).Run(devices_set,
                           blink::mojom::MediaStreamRequestResult::OK,
                           std::move(media_stream_ui));
}
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
