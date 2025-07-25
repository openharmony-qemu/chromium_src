// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/command_line.h"
#include "base/containers/contains.h"
#include "base/functional/bind.h"
#include "base/logging.h"

void DesktopMediaPickerController::OnDisplaySelected(uint64_t displayId) {
  (void)displayId;
  OnPickerDialogResults(std::string(), content::DesktopMediaID());
}

void DesktopMediaPickerController::Stop() {
  LOG(INFO) << "controller stop capture";

  if (params_.web_contents == nullptr) {
    LOG(ERROR) << "Stop fail, "
                  "params_.web_contents == nullptr";
    return;
  }
  int nweb_id = params_.web_contents->GetNWebId();
  std::unique_ptr<webrtc::DesktopCapturer> desktop_capturer =
      content::desktop_capture::CreateScreenCapturer(
          false, nweb_id, base::BindOnce([](uint64_t) {}));
  if (desktop_capturer == nullptr) {
    LOG(ERROR) << "Stop fail, "
                  "desktop_capturer == nullptr";
    return;
  }
  desktop_capturer->Stop();
  desktop_capturer = nullptr;
}