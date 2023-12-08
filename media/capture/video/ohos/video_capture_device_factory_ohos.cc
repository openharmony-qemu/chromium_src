// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
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
    : ui_task_runner_(ui_task_runner) {
  auto status_callback = std::make_shared<VideoCaptureCameraStatusCallbackListenerOHOS>
    (ui_task_runner_, weak_factory_.GetWeakPtr());
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().Create(status_callback);
}

VideoCaptureDeviceFactoryOHOS::~VideoCaptureDeviceFactoryOHOS() {
  LOG(INFO) << "~VideoCaptureDeviceFactoryOHOS";
  OhosAdapterHelper::GetInstance()
      .GetCameraManagerAdapter()
      .ReleaseCameraManger();
}

VideoCaptureErrorOrDevice VideoCaptureDeviceFactoryOHOS::CreateDevice(
    const VideoCaptureDeviceDescriptor& device_descriptor) {
  LOG(INFO) << "VideoCaptureDeviceFactoryOHOS::CreateDevice id: "
            << device_descriptor.device_id;
  DCHECK(thread_checker_.CalledOnValidThread());
  // OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().StopSession();
  auto self = std::make_unique<VideoCaptureDeviceOHOS>(device_descriptor);

  return VideoCaptureErrorOrDevice(std::move(self));
}

void VideoCaptureDeviceFactoryOHOS::GetDevicesInfo(
    GetDevicesInfoCallback callback) {
  DCHECK(thread_checker_.CalledOnValidThread());
  std::vector<VideoCaptureDeviceInfo> devices_info;

  std::vector<VideoDeviceDescriptor> devices_desc;
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().GetDevicesInfo(
      devices_desc);

  LOG(INFO) << "GetDevicesInfo size " << devices_desc.size();
  for (auto single_device_desc : devices_desc) {
    VideoCaptureControlSupport control_support;
    control_support.pan = single_device_desc.controlSupport.pan;
    control_support.tilt = single_device_desc.controlSupport.tilt;
    control_support.zoom = single_device_desc.controlSupport.zoom;
    VideoFacingMode facing_mode =
        VideoCaptureCommonOHOS::GetCameraFacingMode(single_device_desc.facing);
    VideoCaptureDeviceInfo device_info(VideoCaptureDeviceDescriptor(
        single_device_desc.displayName, single_device_desc.deviceId,
        "" /*model_id*/, VideoCaptureApi::LINUX_V4L2_SINGLE_PLANE,
        control_support,
        VideoCaptureCommonOHOS::GetCameraTransportType(
            single_device_desc.transportType),
        facing_mode));
    device_info.supported_formats = VideoCaptureCommonOHOS::GetSupportedFormats(
        single_device_desc.supportCaptureFormats);
    LOG(INFO) << "GetDevicesInfo deviceId: " << single_device_desc.deviceId
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
    CameraStatusAdapter camera_status, std::string callback_device_id) {
  DCHECK(thread_checker_.CalledOnValidThread());
  std::string current_device_Id = OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().
      GetCurrentDeviceId();
  LOG(INFO) << "camera status changed, current_device_Id is " << current_device_Id
            << ", callback_device_id is " << callback_device_id;
  if ((camera_status == CameraStatusAdapter::DISAPPEAR) || (camera_status == CameraStatusAdapter::APPEAR)) {
    if ((current_device_Id == callback_device_id) && (camera_status == CameraStatusAdapter::DISAPPEAR)) {
        OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().StopSession(CameraStopType::NORMAL);
    }
    if(auto* monitor = base::SystemMonitor::Get()){
      monitor->ProcessDevicesChanged(base::SystemMonitor::DEVTYPE_VIDEO_CAPTURE);
    }
  }

  if (camera_status == CameraStatusAdapter::AVAILABLE) {
    OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().
      SetCameraStatus(CameraStatusAdapter::AVAILABLE);
    if (!OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().
        IsExistCaptureTask()) {
          return;
    }
    OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().
      RestartSession();
  }
}
}  // namespace media
