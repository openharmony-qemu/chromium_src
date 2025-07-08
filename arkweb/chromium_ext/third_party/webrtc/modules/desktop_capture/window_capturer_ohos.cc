// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/logging.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "ohos/base_window_capturer.h"

namespace webrtc {

// static
std::unique_ptr<DesktopCapturer> DesktopCapturer::CreateRawWindowCapturer(
    const DesktopCaptureOptions& options) {
  LOG(INFO) << "webrtc DesktopCapturer::CreateRawWindowCapturer";
  base::OnceCallback<void(uint64_t)> callback =
      base::BindOnce([](uint64_t value) {});
  return BaseWindowCapturer::CreateRawCapturer(
      options, BaseWindowCapturer::CaptureSourceType::kWindow,
      std::move(callback));
}

}  // namespace webrtc
