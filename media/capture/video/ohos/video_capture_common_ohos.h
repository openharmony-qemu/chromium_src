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

#ifndef MEDIA_CAPTURE_COMMON_OHOS_H_
#define MEDIA_CAPTURE_COMMON_OHOS_H_

#include "media/capture/video/video_capture_device_factory.h"
#include "media/capture/video_capture_types.h"

#include "ohcamera/camera.h"

namespace media {

const std::unordered_map<Camera_Connection, VideoCaptureTransportType>
  kTransTypeMap = {
    {Camera_Connection::CAMERA_CONNECTION_BUILT_IN,
        VideoCaptureTransportType::APPLE_USB_OR_BUILT_IN},
    {Camera_Connection::CAMERA_CONNECTION_USB_PLUGIN,
        VideoCaptureTransportType::APPLE_USB_OR_BUILT_IN},
    {Camera_Connection::CAMERA_CONNECTION_REMOTE,
        VideoCaptureTransportType::OTHER_TRANSPORT},
};

const std::unordered_map<Camera_Position, VideoFacingMode>
  kFracingModeMap = {
    {Camera_Position::CAMERA_POSITION_UNSPECIFIED, MEDIA_VIDEO_FACING_NONE},
    {Camera_Position::CAMERA_POSITION_BACK, MEDIA_VIDEO_FACING_ENVIRONMENT},
    {Camera_Position::CAMERA_POSITION_FRONT, MEDIA_VIDEO_FACING_USER},
};

const std::unordered_map<Camera_Format, VideoPixelFormat>
  kPixelFormatMap = {
    {Camera_Format::CAMERA_FORMAT_RGBA_8888, PIXEL_FORMAT_ABGR},
    {Camera_Format::CAMERA_FORMAT_YUV_420_SP, PIXEL_FORMAT_NV21},
    {Camera_Format::CAMERA_FORMAT_JPEG, PIXEL_FORMAT_MJPEG},
};

const std::unordered_map<VideoPixelFormat, Camera_Format>
  kAdapterPixelFormatMap = {
    {PIXEL_FORMAT_ABGR, Camera_Format::CAMERA_FORMAT_RGBA_8888},
    {PIXEL_FORMAT_NV21, Camera_Format::CAMERA_FORMAT_YUV_420_SP},
    {PIXEL_FORMAT_MJPEG, Camera_Format::CAMERA_FORMAT_JPEG},
};

class VideoCaptureCommonOHOS {
 public:
  VideoCaptureCommonOHOS();
  ~VideoCaptureCommonOHOS();

  static const int32_t kFrameRate = 30;

  static VideoCaptureTransportType GetCameraTransportType(
      Camera_Connection trans_type);
  static VideoFacingMode GetCameraFacingMode(Camera_Position facing_mode);
  static VideoPixelFormat GetCameraPixelFormatType(Camera_Format pixel_format);
  static VideoCaptureFormats GetSupportedFormats(
      Camera_Profile** previewProfiles,
      uint32_t previewProfilesSize);
  static int GetAdapterCameraPixelFormatType(
      VideoPixelFormat pixel_format);
};

}  // namespace media
#endif  // MEDIA_CAPTURE_COMMON_OHOS_H_
