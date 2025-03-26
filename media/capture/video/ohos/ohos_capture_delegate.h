// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Linux specific implementation of VideoCaptureDevice.
// V4L2 is used for capturing. V4L2 does not provide its own thread for
// capturing so this implementation uses a Chromium thread for fetching frames
// from V4L2.

#ifndef OHOS_CAPTURE_DELEGATE_H_
#define OHOS_CAPTURE_DELEGATE_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "base/containers/queue.h"
#include "base/files/file_util.h"
#include "base/files/scoped_file.h"
#include "base/threading/thread.h"
#include "camera_manager_adapter.h"
#include "display_manager_adapter.h"
#include "media/capture/mojom/image_capture_types.h"
#include "media/capture/video/video_capture_device.h"
#include "media/capture/video_capture_types.h"
#include "video_capture_surface_buffer_listener_ohos.h"
#include "video_capture_params_adapter_impl.h"

namespace media {
using namespace OHOS::NWeb;
using media::mojom::MeteringMode;

static const int kSuccessReturnValue = 0;
static const int kErrorReturnValue = -1;

class CAPTURE_EXPORT OHOSCaptureDelegate final {
 public:
  OHOSCaptureDelegate(
      const VideoCaptureDeviceDescriptor& device_descriptor,
      const scoped_refptr<base::SingleThreadTaskRunner>& capture_stask_runner,
      const VideoCaptureParams capture_params);

  OHOSCaptureDelegate(const OHOSCaptureDelegate&) = delete;
  OHOSCaptureDelegate& operator=(const OHOSCaptureDelegate&) = delete;

  ~OHOSCaptureDelegate();

  // Forward-to versions of VideoCaptureDevice virtual methods.
  void AllocateAndStart(std::unique_ptr<VideoCaptureDevice::Client> client);
  void StopAndDeAllocate();

  void TakePhoto(VideoCaptureDevice::TakePhotoCallback callback);

  void GetPhotoState(VideoCaptureDevice::GetPhotoStateCallback callback);
  void SetPhotoOptions(mojom::PhotoSettingsPtr settings,
                       VideoCaptureDevice::SetPhotoOptionsCallback callback);

  void MaybeSuspend();
  void Resume();
  void OnBufferAvailable(std::shared_ptr<CameraSurfaceAdapter> surface,
                         std::shared_ptr<CameraSurfaceBufferAdapter> buffer,
                         std::shared_ptr<CameraRotationInfoAdapter> rotation_info);

  void SetRotation(int rotation);

  base::WeakPtr<OHOSCaptureDelegate> GetWeakPtr();

 private:
  bool StartStream();
  void DoCapture();
  bool StopStream();

  void SetErrorState(VideoCaptureError error,
                     const base::Location& from_here,
                     const std::string& reason);

  int TransToOHOSCaptrueParams(const VideoCaptureParams& in,
                               std::shared_ptr<VideoCaptureParamsAdapterImpl>& out);
  int GetUsableExposureMode(ExposureModeAdapter& exposure_mode_adapter,
                            MeteringMode& exposure_mode);
  MeteringMode GetCurrentExposureMode(
      ExposureModeAdapter& exposure_mode_adapter);
  void GetExposureState(mojom::PhotoStatePtr& photo_capabilities);
  mojom::RangePtr RetrieveUserControlRange(RangeIDAdapter rangeID);
  void GetFocusState(mojom::PhotoStatePtr& photo_capabilities);
  void GetFlashState(mojom::PhotoStatePtr& photo_capabilities);
  int GetCameraRotation(int surface_rotation);
  int ConvertToPixel(const int byte_stride, const VideoPixelFormat& pixel_format);
  const scoped_refptr<base::SingleThreadTaskRunner> capture_stask_runner_;
  const VideoCaptureDeviceDescriptor device_descriptor_;
  int power_line_frequency_;

  // The following members are only known on AllocateAndStart().
  VideoCaptureFormat capture_format_;
  std::unique_ptr<VideoCaptureDevice::Client> client_;

  base::queue<VideoCaptureDevice::TakePhotoCallback> take_photo_callbacks_;

  bool is_capturing_;
  int timeout_count_;

  base::TimeTicks first_ref_time_;

  // Clockwise rotation in degrees. This value should be 0, 90, 180, or 270.
  int rotation_;

  base::WeakPtrFactory<OHOSCaptureDelegate> weak_factory_{this};

  const VideoCaptureParams capture_params_;
};

}  // namespace media

#endif  // OHOS_CAPTURE_DELEGATE_H_
