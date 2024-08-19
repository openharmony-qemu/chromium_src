// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/ohos/video_capture_device_factory_ohos.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <utility>

#include "base/containers/contains.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/posix/eintr_wrapper.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/system/system_monitor.h"
#include "build/build_config.h"
#include "camera_manager_adapter.h"
#include "media/capture/video/ohos/video_capture_device_ohos.h"
#include "ohos_adapter_helper.h"
#include "video_capture_common_ohos.h"
#include "video_capture_status_callback_listener_ohos.h"

namespace media {

namespace {

bool CompareCaptureDevices(const VideoCaptureDeviceInfo& a,
                           const VideoCaptureDeviceInfo& b) {
  return a.descriptor < b.descriptor;
}
}  // namespace

VideoCaptureDeviceFactoryOHOS::VideoCaptureDeviceFactoryOHOS(
    scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner)
    : ui_task_runner_(ui_task_runner) {}

VideoCaptureDeviceFactoryOHOS::~VideoCaptureDeviceFactoryOHOS() {
  LOG(INFO) << "~VideoCaptureDeviceFactoryOHOS";
  if (is_camera_manager_created_) {
    OhosAdapterHelper::GetInstance()
      .GetCameraManagerAdapter()
      .ReleaseCameraManger();
    is_camera_manager_created_ = false;
  }
}

bool VideoCaptureDeviceFactoryOHOS::CheckAndInitCameraManager() {
  if (is_camera_manager_created_) {
    return true;
  }
  LOG(INFO) << "Create Camera Manager.";
  auto status_callback = std::make_shared<VideoCaptureCameraStatusCallbackListenerOHOS>(
    ui_task_runner_, weak_factory_.GetWeakPtr());
  auto ret = OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().Create(status_callback);
  if (ret != 0) {
    LOG(ERROR) << "create camera manager failed.";
    return false;
  }
  is_camera_manager_created_ = true;
  return true;
}

int VideoCaptureDeviceFactoryOHOS::CheckDeviceId(const std::string device_id) {
  std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> devices_desc =
      OhosAdapterHelper::GetInstance()
          .GetCameraManagerAdapter()
          .GetDevicesInfo();
  for (auto single_device_desc : devices_desc) {
    if (single_device_desc && device_id == single_device_desc->GetDeviceId()) {
      return kSuccessReturnValue;
    }
  }
  return kErrorReturnValue;
}

VideoCaptureErrorOrDevice VideoCaptureDeviceFactoryOHOS::CreateDevice(
    const VideoCaptureDeviceDescriptor& device_descriptor) {
  LOG(INFO) << "VideoCaptureDeviceFactoryOHOS::CreateDevice id: "
            << device_descriptor.device_id;
  DCHECK(thread_checker_.CalledOnValidThread());
  if (!CheckAndInitCameraManager()) {
    return VideoCaptureErrorOrDevice(VideoCaptureError::
      kVideoCaptureDeviceFactoryChromeOSCreateDeviceFailed);
  }
  if (CheckDeviceId(device_descriptor.device_id) != kSuccessReturnValue) {
    LOG(INFO) << "device_id can not be create";
    return VideoCaptureErrorOrDevice(
        VideoCaptureError::
            kVideoCaptureDeviceFactoryChromeOSCreateDeviceFailed);
  }
  // OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().StopSession();
  auto self = std::make_unique<VideoCaptureDeviceOHOS>(device_descriptor);

  return VideoCaptureErrorOrDevice(std::move(self));
}

void VideoCaptureDeviceFactoryOHOS::GetDevicesInfo(
    GetDevicesInfoCallback callback) {
  DCHECK(thread_checker_.CalledOnValidThread());
  if (!CheckAndInitCameraManager()) {
    return;
  }
  std::vector<VideoCaptureDeviceInfo> devices_info;

  std::vector<std::shared_ptr<OHOS::NWeb::VideoDeviceDescriptorAdapter>> devices_desc =
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().GetDevicesInfo();

  LOG(INFO) << "GetDevicesInfo size " << devices_desc.size();
  for (auto single_device_desc : devices_desc) {
    if (!single_device_desc || !(single_device_desc->GetControlSupport())) {
      continue;
    }
    VideoCaptureControlSupport control_support;
    control_support.pan = single_device_desc->GetControlSupport()->GetPan();
    control_support.tilt = single_device_desc->GetControlSupport()->GetTilt();
    control_support.zoom = single_device_desc->GetControlSupport()->GetZoom();
    VideoFacingMode facing_mode =
        VideoCaptureCommonOHOS::GetCameraFacingMode(single_device_desc->GetFacingMode());
    VideoCaptureDeviceInfo device_info(VideoCaptureDeviceDescriptor(
        single_device_desc->GetDisplayName(), single_device_desc->GetDeviceId(),
        "" /*model_id*/, VideoCaptureApi::LINUX_V4L2_SINGLE_PLANE,
        control_support,
        VideoCaptureCommonOHOS::GetCameraTransportType(
            single_device_desc->GetTransportType()),
        facing_mode));
    device_info.supported_formats = VideoCaptureCommonOHOS::GetSupportedFormats(
        single_device_desc->GetSupportCaptureFormats());
    LOG(INFO) << "GetDevicesInfo deviceId: " << single_device_desc->GetDeviceId()
              << ", pan: " << control_support.pan
              << ", tilt: " << control_support.tilt
              << ", zoom: " << control_support.zoom
              << ", facing_mode: " << facing_mode;
    if (facing_mode == MEDIA_VIDEO_FACING_USER) {
      devices_info.insert(devices_info.begin(), std::move(device_info));
    } else {
      devices_info.emplace_back(std::move(device_info));
    }
  }
  // This is required for some applications that rely on the stable ordering of
  // devices.
  std::sort(devices_info.begin(), devices_info.end(), CompareCaptureDevices);

  std::move(callback).Run(std::move(devices_info));
}

void VideoCaptureDeviceFactoryOHOS::OnCameraStatusChanged(
    CameraStatusAdapter camera_status,
    std::string callback_device_id) {
  DCHECK(thread_checker_.CalledOnValidThread());
  std::string current_device_Id = OhosAdapterHelper::GetInstance()
                                      .GetCameraManagerAdapter()
                                      .GetCurrentDeviceId();
  LOG(INFO) << "camera status changed, current_device_Id is "
            << current_device_Id << ", callback_device_id is "
            << callback_device_id;
  if ((camera_status == CameraStatusAdapter::DISAPPEAR) ||
      (camera_status == CameraStatusAdapter::APPEAR)) {
    if ((current_device_Id == callback_device_id) &&
        (camera_status == CameraStatusAdapter::DISAPPEAR)) {
      OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().StopSession(
          CameraStopType::NORMAL);
    }
    if (auto* monitor = base::SystemMonitor::Get()) {
      monitor->ProcessDevicesChanged(
          base::SystemMonitor::DEVTYPE_VIDEO_CAPTURE);
    }
  }
  LOG(INFO) << "camera status changed";
  if (camera_status == CameraStatusAdapter::AVAILABLE) {
    OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().SetCameraStatus(
        CameraStatusAdapter::AVAILABLE);
    if (!OhosAdapterHelper::GetInstance()
             .GetCameraManagerAdapter()
             .IsExistCaptureTask()) {
      return;
    }
    OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().RestartSession();
  }
}

}  // namespace media
