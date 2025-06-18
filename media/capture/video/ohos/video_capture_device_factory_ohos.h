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

// OHOS implementation of the VideoCaptureDeviceFactory class

#ifndef MEDIA_CAPTURE_VIDEO_OHOS_VIDEO_CAPTURE_DEVICE_FACTORY_OHOS_H_
#define MEDIA_CAPTURE_VIDEO_OHOS_VIDEO_CAPTURE_DEVICE_FACTORY_OHOS_H_

#include "media/capture/video/video_capture_device_factory.h"

#include <memory>
#include <string>
#include <vector>

#include "media/capture/video_capture_types.h"
#include "ohcamera/camera.h"
#include "ohcamera/camera_manager.h"

namespace media {
// Extension of VideoCaptureDeviceFactory to create and manipulate OHOS
// devices.
class CAPTURE_EXPORT VideoCaptureDeviceFactoryOHOS
    : public VideoCaptureDeviceFactory {
 public:
  VideoCaptureDeviceFactoryOHOS();

  VideoCaptureDeviceFactoryOHOS(const VideoCaptureDeviceFactoryOHOS&) = delete;
  VideoCaptureDeviceFactoryOHOS& operator=(
      const VideoCaptureDeviceFactoryOHOS&) = delete;

  ~VideoCaptureDeviceFactoryOHOS() override;

  VideoCaptureErrorOrDevice CreateDevice(
      const VideoCaptureDeviceDescriptor& device_descriptor) override;
  void GetDevicesInfo(GetDevicesInfoCallback callback) override;

 private:
  RAW_PTR_EXCLUSION Camera_Manager* camera_manager_ = nullptr;
  RAW_PTR_EXCLUSION Camera_OutputCapability* camera_output_capability_ = nullptr;
  RAW_PTR_EXCLUSION Camera_Device* cameras_ = nullptr;
};

}  // namespace media
#endif  // MEDIA_CAPTURE_VIDEO_OHOS_VIDEO_CAPTURE_DEVICE_FACTORY_OHOS_H_
