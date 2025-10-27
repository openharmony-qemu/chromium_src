// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MODULES_DESKTOP_CAPTURE_OHOS_CAPTURE_FRAME_H_
#define MODULES_DESKTOP_CAPTURE_OHOS_CAPTURE_FRAME_H_

#include <memory>
#include <vector>

#include "modules/desktop_capture/desktop_capture_types.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_geometry.h"
#include "modules/desktop_capture/resolution_tracker.h"
#include "modules/desktop_capture/shared_desktop_frame.h"
#include "modules/desktop_capture/shared_memory.h"

namespace webrtc {

class DesktopCaptureFrame final {
 public:
  // DesktopCaptureFrame does not take ownership of `factory`, consumers should
  // ensure it outlives this instance. nullptr is acceptable.
  explicit DesktopCaptureFrame(SharedMemoryFactory* factory);
  ~DesktopCaptureFrame();

  // Should not be called if Prepare() is not executed or returns false.
  SharedDesktopFrame* frame() const;

  // Prepares current instance with desktop size.
  bool Prepare(DesktopSize size);

 private:
  SharedMemoryFactory* const factory_;
  ResolutionTracker resolution_tracker_;
  std::unique_ptr<SharedDesktopFrame> frame_;
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_OHOS_CAPTURE_FRAME_H_
