// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/desktop_capture/ohos/native_frame.h"

#include <utility>

#include "base/logging.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "rtc_base/checks.h"

namespace webrtc {

// static
std::unique_ptr<NativeFrame> NativeFrame::Create(
    OH_AVScreenCapture* screen_capture) {
  RTC_DCHECK(screen_capture);

  std::unique_ptr<NativeFrame> native_frame =
      std::make_unique<NativeFrame>(screen_capture);
  if (!native_frame.get()->Init()) {
    return nullptr;
  }

  return native_frame;
}

// static
std::unique_ptr<NativeFrame> NativeFrame::Create(OH_AVBuffer* buffer) {
  RTC_DCHECK(buffer);

  std::unique_ptr<NativeFrame> native_frame =
      std::make_unique<NativeFrame>(buffer);
  if (!native_frame.get()->Init()) {
    return nullptr;
  }

  return native_frame;
}

NativeFrame::NativeFrame(OH_AVScreenCapture* screen_capture)
    : screen_capture_(screen_capture) {}

NativeFrame::NativeFrame(OH_AVBuffer* buffer) : buffer_(buffer) {}

NativeFrame::~NativeFrame() {
  if (native_buffer_) {
    OH_NativeBuffer_Unreference(native_buffer_);
  }
}

bool NativeFrame::Init() {
  if (screen_capture_) {
    return InitWithScreenCapture();
  } else if (buffer_) {
    return InitWithBuffer();
  }

  return false;
}

bool NativeFrame::InitWithScreenCapture() {
  int32_t fence = 0;
  int64_t timestamp = 0;
  OH_Rect region = {0};
  native_buffer_ = OH_AVScreenCapture_AcquireVideoBuffer(
      screen_capture_, &fence, &timestamp, &region);
  if (!native_buffer_) {
    LOG(ERROR) << "acquire video buffer failed";
    return false;
  }

  OH_NativeBuffer_GetConfig(native_buffer_, &config_);
  int32_t format = config_.format;
  if (format != OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888) {
    LOG(ERROR) << "buffer format error";
    return false;
  }

  return true;
}

bool NativeFrame::InitWithBuffer() {
  native_buffer_ = OH_AVBuffer_GetNativeBuffer(buffer_);
  if (!native_buffer_) {
    LOG(ERROR) << "get native buffer from video buffer failed";
    return false;
  }

  OH_NativeBuffer_GetConfig(native_buffer_, &config_);
  int32_t format = config_.format;
  if (format != OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888) {
    LOG(ERROR) << "buffer format error";
    return false;
  }

  return true;
}

OH_NativeBuffer* NativeFrame::GetNativeBuffer() const {
  RTC_DCHECK(native_buffer_);
  return native_buffer_;
}

uint8_t* NativeFrame::Map() {
  OH_NativeBuffer_Map(native_buffer_, (void**)&src_data_);
  if (!src_data_) {
    LOG(ERROR) << "native buffer map failed";
    return nullptr;
  }
  return src_data_;
}

void NativeFrame::UnMap() {
  if (native_buffer_) {
    OH_NativeBuffer_Unmap(native_buffer_);
  }
}

uint8_t* NativeFrame::GetBuffer() {
  RTC_DCHECK(buffer_);
  return OH_AVBuffer_GetAddr(buffer_);
}

int32_t NativeFrame::GetCapacity() {
  RTC_DCHECK(buffer_);
  return OH_AVBuffer_GetCapacity(buffer_);
}

}  // namespace webrtc
