// Copyright 2023 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VIDEO_CAPTURE_STATUS_CALLBACK_LISTENER_OHOS_H_
#define VIDEO_CAPTURE_STATUS_CALLBACK_LISTENER_OHOS_H_

#include "base/logging.h"
#include "ohos_capture_delegate.h"
#include "video_capture_device_ohos.h"
namespace media {

class VideoCaptureCameraStatusCallbackListenerOHOS
    : public OHOS::NWeb::CameraStatusCallbackAdapter {
 public:
  VideoCaptureCameraStatusCallbackListenerOHOS(
      const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
      base::WeakPtr<VideoCaptureDeviceFactoryOHOS> capture_device_factory);

  VideoCaptureCameraStatusCallbackListenerOHOS(
      const VideoCaptureCameraStatusCallbackListenerOHOS&) = delete;
  VideoCaptureCameraStatusCallbackListenerOHOS& operator=(
      const VideoCaptureCameraStatusCallbackListenerOHOS&) = delete;

  virtual ~VideoCaptureCameraStatusCallbackListenerOHOS();
  void OnCameraStatusChanged(CameraStatusAdapter camera_status,
                             std::string callBackDeviceId) override;
  void OnFlashlightStatusChanged(FlashStatusAdapter flash_status,
                                 const std::string cameraID) override {}

 private:
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::WeakPtr<VideoCaptureDeviceFactoryOHOS> capture_device_factory_;
};

}  // namespace media

#endif  // VIDEO_CAPTURE_STATUS_CALLBACK_LISTENER_OHOS_H_
