// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Linux specific implementation of VideoCaptureDevice.
// V4L2 is used for capturing. V4L2 does not provide its own thread for
// capturing so this implementation uses a Chromium thread for fetching frames
// from V4L2.

#ifndef MEDIA_CAPTURE_VIDEO_OHOS_VIDEO_CAPTURE_DEVICE_OHOS_H_
#define MEDIA_CAPTURE_VIDEO_OHOS_VIDEO_CAPTURE_DEVICE_OHOS_H_

#include <stdint.h>

#include <memory>
#include <vector>

#include "base/files/file_util.h"
#include "base/files/scoped_file.h"
#include "base/threading/thread.h"
#include "camera_manager_adapter.h"
#include "media/capture/video/video_capture_device.h"
#include "media/capture/video_capture_types.h"
#include "video_capture_surface_buffer_listener_ohos.h"

namespace media {
using namespace OHOS::NWeb;
class OHOSCaptureDelegate;

// OHOS capture implementation of VideoCaptureDevice.
class VideoCaptureDeviceOHOS : public VideoCaptureDevice {
 public:
  VideoCaptureDeviceOHOS() = delete;

  explicit VideoCaptureDeviceOHOS(
      const VideoCaptureDeviceDescriptor& device_descriptor);

  VideoCaptureDeviceOHOS(const VideoCaptureDeviceOHOS&) = delete;
  VideoCaptureDeviceOHOS& operator=(const VideoCaptureDeviceOHOS&) = delete;

  ~VideoCaptureDeviceOHOS() override;

  // VideoCaptureDevice implementation.
  void AllocateAndStart(const VideoCaptureParams& params,
                        std::unique_ptr<Client> client) override;
  void StopAndDeAllocate() override;
  void TakePhoto(TakePhotoCallback callback) override;
  void GetPhotoState(GetPhotoStateCallback callback) override;
  void SetPhotoOptions(mojom::PhotoSettingsPtr settings,
                       SetPhotoOptionsCallback callback) override;
  void MaybeSuspend() override;
  void Resume() override;

 protected:
  const VideoCaptureDeviceDescriptor device_descriptor_;

 private:
  std::unique_ptr<OHOSCaptureDelegate> capture_impl_;

  // Photo-related requests waiting for |capture_thread_| to be active.
  std::vector<base::OnceClosure> photo_requests_queue_;

  base::Thread
      capture_thread_;  // Thread used for reading data from the device.

  VideoCaptureFormat capture_format_;
  void TransToOHOSCaptrueParams(const VideoCaptureParams& in,
                                VideoCaptureParamsAdapter& out);
  std::shared_ptr<CameraManagerAdapter> camera_manager_adapter_;
  SEQUENCE_CHECKER(sequence_checker_);
};

}  // namespace media

#endif  // MEDIA_CAPTURE_VIDEO_OHOS_VIDEO_CAPTURE_DEVICE_OHOS_H_
