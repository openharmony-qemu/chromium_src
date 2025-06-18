/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OHOS_CAPTURE_DELEGATE_H_
#define OHOS_CAPTURE_DELEGATE_H_

#include "base/containers/queue.h"
#include "base/threading/thread.h"
#include "media/capture/mojom/image_capture_types.h"
#include "media/capture/video/video_capture_device.h"
#include "media/capture/video_capture_types.h"
#include "ohos/adapter/media_manager/media_adapter.h"

#include "ohcamera/camera.h"
#include "ohcamera/camera_input.h"
#include "ohcamera/camera_manager.h"
#include "ohcamera/capture_session.h"

namespace media {
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

  OHOSCaptureDelegate() = default;

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

  void OnBufferAvailable(uint8_t* data, size_t data_size);

  void SetRotation(int rotation);

  base::WeakPtr<OHOSCaptureDelegate> GetWeakPtr();
  Camera_ErrorCode ReleaseSession();
  Camera_ErrorCode ReleaseSessionResource();

 private:
  bool StartStream();
  bool StopStream();

  Camera_ErrorCode InitCameraInput(uint32_t index);
  Camera_ErrorCode InitPreviewOutput(uint32_t index);
  Camera_ErrorCode InitCaptureSession();

  int GetUsableExposureMode(Camera_ExposureMode& exposure_mode_,
                            MeteringMode& exposure_mode);
  MeteringMode GetCurrentExposureMode(
      Camera_ExposureMode& exposure_mode_);
  void GetExposureState(mojom::PhotoStatePtr& photo_capabilities);
  mojom::RangePtr RetrieveUserControlRange();
  void GetFocusState(mojom::PhotoStatePtr& photo_capabilities);
  void GetFlashState(mojom::PhotoStatePtr& photo_capabilities);

  int GetIndexOfMatchedProfile();
  void SetErrorState(VideoCaptureError error,
                     const base::Location& from_here,
                     const std::string& reason);

  const scoped_refptr<base::SingleThreadTaskRunner> capture_task_runner_;
  const VideoCaptureDeviceDescriptor device_descriptor_;

  // The following members are only known on AllocateAndStart().
  VideoCaptureFormat capture_format_;
  std::unique_ptr<VideoCaptureDevice::Client> client_;

  base::queue<VideoCaptureDevice::TakePhotoCallback> take_photo_callbacks_;

  bool is_capturing_;

  base::TimeTicks first_ref_time_;

  // Clockwise rotation in degrees. This value should be 0, 90, 180, or 270.
  int rotation_;

  base::WeakPtrFactory<OHOSCaptureDelegate> weak_factory_{this};

  const VideoCaptureParams capture_params_;

  RAW_PTR_EXCLUSION Camera_Manager* camera_manager_ = nullptr;
  RAW_PTR_EXCLUSION Camera_CaptureSession* capture_session_ = nullptr;
  RAW_PTR_EXCLUSION Camera_OutputCapability* camera_output_capability_ = nullptr;
  RAW_PTR_EXCLUSION Camera_Device* cameras_ = nullptr;
  RAW_PTR_EXCLUSION Camera_Input* camera_input_ = nullptr;
  RAW_PTR_EXCLUSION Camera_PreviewOutput* preview_output_ = nullptr;
  uint32_t camera_size_;
};

}  // namespace media

#endif  // OHOS_CAPTURE_DELEGATE_H_
