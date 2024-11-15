// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/ohos/ohos_capture_delegate.h"

#include <stddef.h>

#include <utility>

#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "media/capture/video/blob_utils.h"
#include "media/capture/video/ohos/ohos_capture_delegate.h"
#include "ohos_adapter_helper.h"
#include "third_party/libyuv/include/libyuv.h"
#include "video_capture_common_ohos.h"

namespace media {

OHOSCaptureDelegate::OHOSCaptureDelegate(
    const VideoCaptureDeviceDescriptor& device_descriptor,
    const scoped_refptr<base::SingleThreadTaskRunner>& capture_stask_runner,
    const VideoCaptureParams capture_params)
    : capture_stask_runner_(capture_stask_runner),
      device_descriptor_(device_descriptor),
      is_capturing_(false),
      timeout_count_(0),
      capture_params_(capture_params) {}

int OHOSCaptureDelegate::TransToOHOSCaptrueParams(
    const VideoCaptureParams& in,
    std::shared_ptr<VideoCaptureParamsAdapterImpl>& out) {
  if (!out) {
    LOG(ERROR) << "TransToOHOSCaptrueParams out is NULL";
    return kErrorReturnValue;
  }

  std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> devices_desc =
      OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .GetDevicesInfo();
  int req_width = in.requested_format.frame_size.width();
  int req_height = in.requested_format.frame_size.height();
  int min_diff = INT_MAX;
  int matched_width = req_width;
  int matched_height = req_height;
  VideoPixelFormatAdapter matched_pixel_format =
      VideoPixelFormatAdapter::FORMAT_UNKNOWN;
  ;
  std::string device_id = device_descriptor_.device_id;

  for (auto single_device_desc : devices_desc) {
    if (!single_device_desc) {
      continue;
    }
    if (single_device_desc->GetDeviceId() == device_id) {
      LOG(INFO) << "TransToOHOSCaptrueParams " << device_id;
      std::vector<std::shared_ptr<FormatAdapter>> supportCaptureFormats =
          single_device_desc->GetSupportCaptureFormats();
      for (auto format : supportCaptureFormats) {
        if (!format) {
          continue;
        }
        int format_support_width = (int)format->GetWidth();
        int format_support_height = (int)format->GetHeight();
        VideoPixelFormatAdapter format_support_format =
            format->GetPixelFormat();
        int diff = abs(format_support_width - req_width) +
                   abs(format_support_height - req_height);
        LOG(DEBUG) << "support width: " << format_support_width
                   << ", height: " << format_support_height
                   << ", diff: " << diff;
        if ((req_width == format_support_width) &&
            (req_height == format_support_height)) {
          LOG(INFO) << "no need match other size, current width: "
                    << format_support_width
                    << ", height: " << format_support_height;
          out->SetWidth(format_support_width);
          out->SetHeight(format_support_height);
          out->SetFrameRate(in.requested_format.frame_rate);
          out->SetPixelFormat(format_support_format);
          out->SetEnableFaceDetection(in.enable_face_detection);
          return kSuccessReturnValue;
        }
        if (diff < min_diff) {
          min_diff = diff;
          matched_width = format_support_width;
          matched_height = format_support_height;
          matched_pixel_format = format_support_format;
        }
      }
    }
  }

  if (min_diff == INT_MAX) {
    LOG(ERROR) << "can not find matched pixelformat";
    return kErrorReturnValue;
  }

  LOG(INFO) << "matched width: " << matched_width
            << ", height: " << matched_height;
  out->SetWidth(matched_width);
  out->SetHeight(matched_height);
  out->SetFrameRate(in.requested_format.frame_rate);
  out->SetPixelFormat(matched_pixel_format);
  out->SetEnableFaceDetection(in.enable_face_detection);
  return kSuccessReturnValue;
}

void OHOSCaptureDelegate::AllocateAndStart(
    std::unique_ptr<VideoCaptureDevice::Client> client) {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  DCHECK(client);
  client_ = std::move(client);

  if (!StartStream()) {
    LOG(ERROR) << "start stream failed";
    return;
  }

  client_->OnStarted();
}

void OHOSCaptureDelegate::StopAndDeAllocate() {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  LOG(INFO) << "OHOSCaptureDelegate::StopAndDeAllocate";
  StopStream();
  // At this point we can close the device.
  // This is also needed for correctly changing settings later via VIDIOC_S_FMT.
  client_.reset();
}

void OHOSCaptureDelegate::OnBufferAvailable(
    std::shared_ptr<CameraSurfaceAdapter> surface,
    std::shared_ptr<CameraSurfaceBufferAdapter> buffer,
    std::shared_ptr<CameraRotationInfoAdapter> roration_info) {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  const base::TimeTicks now = base::TimeTicks::Now();
  if (first_ref_time_.is_null()) {
    first_ref_time_ = now;
  }
  const base::TimeDelta timestamp = now - first_ref_time_;

  if (!surface || !buffer || !roration_info) {
    LOG(ERROR) << "OnBufferAvailable params is nullptr";
    return;
  }
  int32_t rotation = roration_info->GetRotation();
  if (client_ != nullptr) {
#if !defined(RK3568) && defined(OHOS_WEBRTC)
    capture_format_.stride = buffer->GetStride();
#endif        
    client_->OnIncomingCapturedData(
        buffer->GetBufferAddr(), buffer->GetSize(), capture_format_,
        gfx::ColorSpace(), rotation, roration_info->GetIsFlipY() /* flip_y */,
        now, timestamp);
  } else {
    LOG(DEBUG) << "OnBufferAvailable client is nullptr";
  }

  if (surface->ReleaseBuffer(std::move(buffer), -1) != kSuccessReturnValue) {
    LOG(DEBUG) << "OnBufferAvailable ReleaseBuffer failed";
  }
}

void OHOSCaptureDelegate::TakePhoto(
    VideoCaptureDevice::TakePhotoCallback callback) {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  take_photo_callbacks_.push(std::move(callback));
}

const std::unordered_map<ExposureModeAdapter, MeteringMode> EXP_MODE_MAP = {
    {ExposureModeAdapter::EXPOSURE_MODE_UNSUPPORTED, MeteringMode::NONE},
    {ExposureModeAdapter::EXPOSURE_MODE_LOCKED, MeteringMode::SINGLE_SHOT},
    {ExposureModeAdapter::EXPOSURE_MODE_AUTO, MeteringMode::CONTINUOUS},
    {ExposureModeAdapter::EXPOSURE_MODE_CONTINUOUS_AUTO,
     MeteringMode::CONTINUOUS},
};

int OHOSCaptureDelegate::GetUsableExposureMode(
    ExposureModeAdapter& exposure_mode_adapter,
    MeteringMode& exposure_mode) {
  if (exposure_mode_adapter != ExposureModeAdapter::EXPOSURE_MODE_UNSUPPORTED) {
    return kErrorReturnValue;
  }
  auto item = EXP_MODE_MAP.find(exposure_mode_adapter);
  if (item == EXP_MODE_MAP.end()) {
    LOG(ERROR) << "concect type: " << static_cast<int>(exposure_mode_adapter)
               << " not found.";
    exposure_mode = MeteringMode::NONE;
    return kErrorReturnValue;
  }
  exposure_mode = item->second;
  return kSuccessReturnValue;
}

MeteringMode OHOSCaptureDelegate::GetCurrentExposureMode(
    ExposureModeAdapter& exposure_mode_adapter) {
  auto item = EXP_MODE_MAP.find(exposure_mode_adapter);
  if (item == EXP_MODE_MAP.end()) {
    LOG(ERROR) << "concect type: " << static_cast<int>(exposure_mode_adapter)
               << " not found.";
    return MeteringMode::NONE;
  }
  return item->second;
}

mojom::RangePtr OHOSCaptureDelegate::RetrieveUserControlRange(
    RangeIDAdapter rangeID) {
  mojom::RangePtr capability = mojom::Range::New();
  std::shared_ptr<VideoCaptureRangeAdapter> rangeVal =
      OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .GetCaptionRangeById(rangeID);
  if (!rangeVal) {
    LOG(ERROR) << "get current caption range failed";
    return capability;
  }
  capability->max = rangeVal->GetMax();
  capability->min = rangeVal->GetMin();
  capability->step = rangeVal->GetStep();
  capability->current = rangeVal->GetCurrent();

  return capability;
}

void OHOSCaptureDelegate::GetExposureState(
    mojom::PhotoStatePtr& photo_capabilities) {
  std::vector<ExposureModeAdapter> exposure_modes_adapter;
  if (OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .GetExposureModes(exposure_modes_adapter) !=
      CameraManagerAdapterCode::CAMERA_OK) {
    LOG(ERROR) << "get exposure mode failed";
    return;
  }
  for (auto exposure_mode_adapter : exposure_modes_adapter) {
    MeteringMode exposure_mode;
    if (GetUsableExposureMode(exposure_mode_adapter, exposure_mode) ==
        kSuccessReturnValue) {
      photo_capabilities->supported_exposure_modes.push_back(exposure_mode);
    }
  }
  ExposureModeAdapter curr_exp_mode_adapter;
  if (OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .GetCurrentExposureMode(curr_exp_mode_adapter) !=
      CameraManagerAdapterCode::CAMERA_OK) {
    LOG(ERROR) << "get current exposure mode failed";
    return;
  }
  photo_capabilities->current_exposure_mode =
      GetCurrentExposureMode(curr_exp_mode_adapter);
  photo_capabilities->exposure_compensation =
      RetrieveUserControlRange(RangeIDAdapter::RANGE_ID_EXP_COMPENSATION);
}

void OHOSCaptureDelegate::GetFocusState(
    mojom::PhotoStatePtr& photo_capabilities) {
  if (OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .IsFocusModeSupported(FocusModeAdapter::FOCUS_MODE_MANUAL)) {
    photo_capabilities->supported_focus_modes.push_back(MeteringMode::MANUAL);
  }

  if (OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .IsFocusModeSupported(FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO)) {
    photo_capabilities->supported_focus_modes.push_back(
        MeteringMode::CONTINUOUS);
  }

  FocusModeAdapter focusMode = OhosAdapterHelper::GetInstance()
                                   .GetCameraManagerAdapter()
                                   .GetCurrentFocusMode();

  if (focusMode == FocusModeAdapter::FOCUS_MODE_MANUAL) {
    photo_capabilities->current_focus_mode = MeteringMode::MANUAL;
  } else if ((focusMode == FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO) ||
             (focusMode == FocusModeAdapter::FOCUS_MODE_AUTO)) {
    photo_capabilities->current_focus_mode = MeteringMode::CONTINUOUS;
  }
}

void OHOSCaptureDelegate::GetFlashState(
    mojom::PhotoStatePtr& photo_capabilities) {
  if (OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_CLOSE)) {
    photo_capabilities->fill_light_mode.push_back(mojom::FillLightMode::OFF);
  }

  if (OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_OPEN)) {
    photo_capabilities->fill_light_mode.push_back(mojom::FillLightMode::FLASH);
  }

  if (OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_AUTO)) {
    photo_capabilities->fill_light_mode.push_back(mojom::FillLightMode::AUTO);
  }
}

void OHOSCaptureDelegate::GetPhotoState(
    VideoCaptureDevice::GetPhotoStateCallback callback) {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  if (!is_capturing_) {
    return;
  }

  mojom::PhotoStatePtr photo_capabilities = mojo::CreateEmptyPhotoState();
  GetExposureState(photo_capabilities);
  GetFocusState(photo_capabilities);
  GetFlashState(photo_capabilities);

  photo_capabilities->height = mojom::Range::New(
      capture_format_.frame_size.height(), capture_format_.frame_size.height(),
      capture_format_.frame_size.height(), 0 /* step */);
  photo_capabilities->width = mojom::Range::New(
      capture_format_.frame_size.width(), capture_format_.frame_size.width(),
      capture_format_.frame_size.width(), 0 /* step */);

  std::move(callback).Run(std::move(photo_capabilities));
}

void OHOSCaptureDelegate::SetPhotoOptions(
    mojom::PhotoSettingsPtr settings,
    VideoCaptureDevice::SetPhotoOptionsCallback callback) {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  if (!is_capturing_) {
    return;
  }
  std::move(callback).Run(true);
}

void OHOSCaptureDelegate::MaybeSuspend() {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  if (!is_capturing_) {
    return;
  }
  LOG(INFO) << "MaybeSuspend";
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().StopSession(
      CameraStopType::NORMAL);
}

void OHOSCaptureDelegate::Resume() {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  if (!is_capturing_) {
    return;
  }
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().RestartSession();
}

base::WeakPtr<OHOSCaptureDelegate> OHOSCaptureDelegate::GetWeakPtr() {
  return weak_factory_.GetWeakPtr();
}

OHOSCaptureDelegate::~OHOSCaptureDelegate() = default;

bool OHOSCaptureDelegate::StartStream() {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  DCHECK(!is_capturing_);
  if (OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .GetCameraStatus() == CameraStatusAdapter::UNAVAILABLE) {
    LOG(ERROR) << "camera is not closed";
    return false;
  }

  std::shared_ptr<VideoCaptureParamsAdapterImpl> capture_params_adapter =
      std::make_shared<VideoCaptureParamsAdapterImpl>();
  if (TransToOHOSCaptrueParams(capture_params_, capture_params_adapter) !=
      kSuccessReturnValue) {
    LOG(ERROR) << "can not find matched parameter";
    return false;
  }

  auto listener = std::make_shared<VideoCaptureSufaceBufferListenerOHOS>(
      capture_stask_runner_, weak_factory_.GetWeakPtr());

  if (OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().StartStream(
          device_descriptor_.device_id, capture_params_adapter, listener) !=
      CameraManagerAdapterCode::CAMERA_OK) {
    LOG(ERROR) << "create and start session failed";
    return false;
  }
  LOG(INFO) << "create and start session success, deviceId = "
            << device_descriptor_.device_id;

  capture_format_.frame_size.SetSize(capture_params_adapter->GetWidth(),
                                     capture_params_adapter->GetHeight());
  capture_format_.frame_rate = capture_params_adapter->GetFrameRate();
  capture_format_.pixel_format =
      VideoCaptureCommonOHOS::GetCameraPixelFormatType(
          capture_params_adapter->GetPixelFormat());
  is_capturing_ = true;
  return true;
}

bool OHOSCaptureDelegate::StopStream() {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  if (!is_capturing_) {
    return false;
  }

  is_capturing_ = false;
  LOG(INFO) << "StopStream";
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().StopSession(
      CameraStopType::NORMAL);

  return true;
}

void OHOSCaptureDelegate::SetErrorState(VideoCaptureError error,
                                        const base::Location& from_here,
                                        const std::string& reason) {
  DCHECK(capture_stask_runner_->BelongsToCurrentThread());
  client_->OnError(error, from_here, reason);
}

}  // namespace media
