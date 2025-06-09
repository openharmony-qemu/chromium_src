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

#include "media/capture/video/ohos/video_capture_common_ohos.h"

namespace media {
VideoCaptureTransportType VideoCaptureCommonOHOS::GetCameraTransportType(
    Camera_Connection trans_type) {
  auto item = kTransTypeMap.find(trans_type);
  if (item == kTransTypeMap.end()) {
    LOG(ERROR) << "concect type: " << static_cast<int>(trans_type)
               << " not found.";
    return VideoCaptureTransportType::OTHER_TRANSPORT;
  }
  return item->second;
}

VideoFacingMode VideoCaptureCommonOHOS::GetCameraFacingMode(
    Camera_Position facing_mode) {
  auto item = kFracingModeMap.find(facing_mode);
  if (item == kFracingModeMap.end()) {
    LOG(ERROR) << "facine mode: " << static_cast<int>(facing_mode)
               << " not found.";
    return MEDIA_VIDEO_FACING_NONE;
  }
  return item->second;
}

VideoPixelFormat VideoCaptureCommonOHOS::GetCameraPixelFormatType(
    Camera_Format pixel_format) {
  auto item = kPixelFormatMap.find(pixel_format);
  if (item == kPixelFormatMap.end()) {
    LOG(ERROR) << "camera pixel format: " << static_cast<int>(pixel_format)
               << " not found.";
    return PIXEL_FORMAT_UNKNOWN;
  }
  return item->second;
}

int VideoCaptureCommonOHOS::GetAdapterCameraPixelFormatType(
    VideoPixelFormat pixel_format) {
  auto item = kAdapterPixelFormatMap.find(pixel_format);
  if (item == kAdapterPixelFormatMap.end()) {
    LOG(ERROR) << "adapter camera pixel format: "
               << static_cast<int>(pixel_format) << " not found.";
    return -1;
  }
  return item->second;
}

VideoCaptureFormats VideoCaptureCommonOHOS::GetSupportedFormats(
    Camera_Profile** previewProfiles,
    uint32_t previewProfilesSize) {
  VideoCaptureFormats capture_formats;
  if (!previewProfiles) {
    return capture_formats;
  }
  for (uint32_t i = 0; i < previewProfilesSize; i++) {
    VideoCaptureFormat format;
    if (!previewProfiles[i]) {
      continue;
    }
    format.frame_size.SetSize(previewProfiles[i]->size.width,
                              previewProfiles[i]->size.height);
    format.frame_rate = kFrameRate;
    format.pixel_format = GetCameraPixelFormatType(previewProfiles[i]->format);
    if (format.pixel_format != PIXEL_FORMAT_UNKNOWN) {
      capture_formats.push_back(format);
    }
  }
  return capture_formats;
}

}  // namespace media
