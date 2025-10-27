// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MODULES_DESKTOP_CAPTURE_OHOS_NATIVE_FRAME_H_
#define MODULES_DESKTOP_CAPTURE_OHOS_NATIVE_FRAME_H_

#include <memory>

#include "multimedia/player_framework/native_avscreen_capture.h"
#include "native_buffer/native_buffer.h"

namespace webrtc {

class NativeFrame final {
 public:
  static std::unique_ptr<NativeFrame> Create(
      OH_AVScreenCapture* screen_capture);

  static std::unique_ptr<NativeFrame> Create(OH_AVBuffer* buffer);

  explicit NativeFrame(OH_AVScreenCapture* screen_capture);
  explicit NativeFrame(OH_AVBuffer* buffer);
  ~NativeFrame();

  OH_NativeBuffer* GetNativeBuffer() const;

  bool Init();
  uint8_t* Map();
  void UnMap();
  uint8_t* GetBuffer();
  int32_t GetCapacity();
  int32_t image_width() const { return config_.width; }
  int32_t image_height() const { return config_.height; }
  int32_t image_stride() const { return config_.stride; }

 private:
  bool InitWithScreenCapture();
  bool InitWithBuffer();
  OH_AVScreenCapture* screen_capture_ = nullptr;
  OH_AVBuffer* buffer_ = nullptr;
  OH_NativeBuffer* native_buffer_ = nullptr;
  OH_NativeBuffer_Config config_;
  uint8_t* src_data_ = nullptr;
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_OHOS_NATIVE_FRAME_H_
