// Copyright 2023 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VIDEO_CAPTURE_SURFACE_BUFFER_LISTENER_OHOS_H_
#define VIDEO_CAPTURE_SURFACE_BUFFER_LISTENER_OHOS_H_

#include "base/logging.h"
#include "ohos_capture_delegate.h"
#include "video_capture_device_ohos.h"
namespace media {

class VideoCaptureSufaceBufferListenerOHOS
    : public OHOS::NWeb::CameraBufferListenerAdapter {
 public:
  VideoCaptureSufaceBufferListenerOHOS(
      const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
      base::WeakPtr<OHOSCaptureDelegate> media_player);

  VideoCaptureSufaceBufferListenerOHOS(
      const VideoCaptureSufaceBufferListenerOHOS&) = delete;
  VideoCaptureSufaceBufferListenerOHOS& operator=(
      const VideoCaptureSufaceBufferListenerOHOS&) = delete;

  virtual ~VideoCaptureSufaceBufferListenerOHOS();
  void OnBufferAvailable(
      std::shared_ptr<CameraSurfaceAdapter> surface,
      std::shared_ptr<OHOS::NWeb::CameraSurfaceBufferAdapter> buffer,
      std::shared_ptr<OHOS::NWeb::CameraRotationInfoAdapter> roration_info)
      override;

 private:
  std::unique_ptr<VideoCaptureDevice::Client> client_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::WeakPtr<OHOSCaptureDelegate> capture_delegate_;
};

}  // namespace media

#endif  // VIDEO_CAPTURE_SURFACE_BUFFER_LISTENER_OHOS_H_
