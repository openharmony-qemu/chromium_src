// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MODULES_DESKTOP_CAPTURE_OHOS_WINDOW_CAPTURER_EVENT_CALLBACK_H_
#define MODULES_DESKTOP_CAPTURE_OHOS_WINDOW_CAPTURER_EVENT_CALLBACK_H_

#include "multimedia/player_framework/native_avscreen_capture.h"

namespace webrtc {

struct CapturerEventCallBack {
 public:
  virtual void OnStateChange(OH_AVScreenCaptureStateCode stateCode,
                             void* userData) {}

  virtual void OnError(int32_t errorCode, void* userData) {}

  virtual void OnBufferAvailable(OH_AVBuffer* buffer,
                                 OH_AVScreenCaptureBufferType bufferType,
                                 int64_t timestamp,
                                 void* userData) {}
};

}  // namespace webrtc
#endif  // MODULES_DESKTOP_CAPTURE_OHOS_WINDOW_CAPTURER_EVENT_CALLBACK_H_
