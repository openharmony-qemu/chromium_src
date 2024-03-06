// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "video_capture_common_ohos.h"

namespace media {
VideoCaptureTransportType VideoCaptureCommonOHOS::GetCameraTransportType(
    VideoTransportType trans_type) {
  auto item = TRANS_TYPE_MAP.find(trans_type);
  if (item == TRANS_TYPE_MAP.end()) {
    LOG(ERROR) << "concect type: " << static_cast<int>(trans_type)
               << " not found.";
    return VideoCaptureTransportType::OTHER_TRANSPORT;
  }
  return item->second;
}

VideoFacingMode VideoCaptureCommonOHOS::GetCameraFacingMode(
    VideoFacingModeAdapter facing_mode) {
  auto item = FACING_MODE_MAP.find(facing_mode);
  if (item == FACING_MODE_MAP.end()) {
    LOG(ERROR) << "facine mode: " << static_cast<int>(facing_mode)
               << " not found.";
    return MEDIA_VIDEO_FACING_NONE;
  }
  return item->second;
}

VideoPixelFormat VideoCaptureCommonOHOS::GetCameraPixelFormatType(
    VideoPixelFormatAdapter pixel_format) {
  auto item = PIXEL_FORMAT_MAP.find(pixel_format);
  if (item == PIXEL_FORMAT_MAP.end()) {
    LOG(ERROR) << "camera pixel format: " << static_cast<int>(pixel_format)
               << " not found.";
    return PIXEL_FORMAT_UNKNOWN;
  }
  return item->second;
}

VideoPixelFormatAdapter VideoCaptureCommonOHOS::GetAdapterCameraPixelFormatType(
    VideoPixelFormat pixel_format) {
  auto item = ADPATER_PIXEL_FORMAT_MAP.find(pixel_format);
  if (item == ADPATER_PIXEL_FORMAT_MAP.end()) {
    LOG(ERROR) << "adapter camera pixel format: "
               << static_cast<int>(pixel_format) << " not found.";
    return VideoPixelFormatAdapter::FORMAT_UNKNOWN;
  }
  return item->second;
}

VideoCaptureFormats VideoCaptureCommonOHOS::GetSupportedFormats(
    const std::vector<std::shared_ptr<FormatAdapter>>&
        capture_formats_adapter) {
  VideoCaptureFormats capture_formats;
  for (auto i : capture_formats_adapter) {
    if (!i) {
      continue;
    }
    VideoCaptureFormat format;
    format.frame_size.SetSize(i->GetWidth(), i->GetHeight());
    format.frame_rate = i->GetFrameRate();
    format.pixel_format = GetCameraPixelFormatType(i->GetPixelFormat());

    capture_formats.push_back(format);
  }
  return capture_formats;
}

}  // namespace media
