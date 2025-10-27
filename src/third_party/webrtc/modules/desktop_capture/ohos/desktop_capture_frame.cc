// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/desktop_capture/ohos/desktop_capture_frame.h"

#include <utility>

#include "base/logging.h"
#include "rtc_base/checks.h"

namespace webrtc {

DesktopCaptureFrame::DesktopCaptureFrame(SharedMemoryFactory* factory)
    : factory_(factory) {}

DesktopCaptureFrame::~DesktopCaptureFrame() = default;

bool DesktopCaptureFrame::Prepare(DesktopSize size) {
  if (resolution_tracker_.SetResolution(size)) {
    // Once the output size changed, recreate the SharedDesktopFrame.
    frame_.reset();
  }

  if (!frame_) {
    std::unique_ptr<DesktopFrame> frame;
    if (factory_) {
      frame = SharedMemoryDesktopFrame::Create(size, factory_);
      if (!frame) {
        LOG(WARNING) << "cannot create a new DesktopFrame.";
        return false;
      }

      RTC_DCHECK_EQ(frame->stride(),
                    frame->size().width() * DesktopFrame::kBytesPerPixel);
    } else {
      frame.reset(new BasicDesktopFrame(size));
    }

    frame_ = SharedDesktopFrame::Wrap(std::move(frame));
  }

  return !!frame_;
}

SharedDesktopFrame* DesktopCaptureFrame::frame() const {
  RTC_DCHECK(frame_);
  return frame_.get();
}

}  // namespace webrtc
