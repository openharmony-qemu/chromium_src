// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Implementation of a VideoCaptureDeviceFactoryOHOS class.

#ifndef MEDIA_CAPTURE_VIDEO_OHOS_VIDEO_CAPTURE_DEVICE_FACTORY_OHOS_H_
#define MEDIA_CAPTURE_VIDEO_OHOS_VIDEO_CAPTURE_DEVICE_FACTORY_OHOS_H_

#include "media/capture/video/video_capture_device_factory.h"

#include <memory>
#include <string>
#include <vector>

#include "base/task/single_thread_task_runner.h"
#include "camera_manager_adapter.h"
#include "media/capture/video_capture_types.h"

namespace media {
using namespace OHOS::NWeb;

// Extension of VideoCaptureDeviceFactory to create and manipulate OHOS
// devices.
class CAPTURE_EXPORT VideoCaptureDeviceFactoryOHOS
    : public VideoCaptureDeviceFactory {
 public:
  explicit VideoCaptureDeviceFactoryOHOS(
      scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner);

  VideoCaptureDeviceFactoryOHOS(const VideoCaptureDeviceFactoryOHOS&) = delete;
  VideoCaptureDeviceFactoryOHOS& operator=(
      const VideoCaptureDeviceFactoryOHOS&) = delete;

  ~VideoCaptureDeviceFactoryOHOS() override;

  VideoCaptureErrorOrDevice CreateDevice(
      const VideoCaptureDeviceDescriptor& device_descriptor) override;
  void GetDevicesInfo(GetDevicesInfoCallback callback) override;
  void OnCameraStatusChanged(CameraStatusAdapter camera_status, std::string callback_device_id);
 private:
  int CheckDeviceId(const std::string device_id);
  bool CheckAndInitCameraManager();
  scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner_;
  std::shared_ptr<CameraManagerAdapter> camera_manager_adapter_;
  base::WeakPtrFactory<VideoCaptureDeviceFactoryOHOS> weak_factory_{this};
  bool is_camera_manager_created_{false};
};

}  // namespace media
#endif  // MEDIA_CAPTURE_VIDEO_OHOS_VIDEO_CAPTURE_DEVICE_FACTORY_OHOS_H_
