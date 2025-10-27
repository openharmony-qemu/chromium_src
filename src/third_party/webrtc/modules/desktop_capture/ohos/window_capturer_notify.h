// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MODULES_DESKTOP_CAPTURE_OHOS_WINDOW_CAPTURER_NOTIFY_H_
#define MODULES_DESKTOP_CAPTURE_OHOS_WINDOW_CAPTURER_NOTIFY_H_

#include <memory>
#include <unordered_map>

#include "modules/desktop_capture/ohos/capturer_event_callback.h"

namespace webrtc {

class hash_ {
 public:
  size_t operator()(OH_AVScreenCapture* val) const {
    return std::hash<unsigned long>()(reinterpret_cast<unsigned long>(val));
  }
};

class equal_ {
 public:
  bool operator()(OH_AVScreenCapture* val1, OH_AVScreenCapture* val2) const {
    return val1 == val2;
  }
};

class WindowCapturerNotify {
 public:
  WindowCapturerNotify();
  ~WindowCapturerNotify();
  WindowCapturerNotify(WindowCapturerNotify& other) = delete;
  void operator=(const WindowCapturerNotify&) = delete;

  static WindowCapturerNotify* GetInstance();

  CapturerEventCallBack* GetEventCallBack(OH_AVScreenCapture* capture);
  bool RegisterEventCallback(OH_AVScreenCapture* capture,
                             std::shared_ptr<CapturerEventCallBack> callback,
                             void* userData);
  void UnRegisterEventCallback(OH_AVScreenCapture* capture);

 private:
  std::unordered_map<OH_AVScreenCapture*,
                     std::shared_ptr<CapturerEventCallBack>,
                     hash_,
                     equal_>
      callback_map_;
};
}  // namespace webrtc
#endif  // MODULES_DESKTOP_CAPTURE_OHOS_WINDOW_CAPTURER_NOTIFY_H_
