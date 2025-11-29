/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "ui/ozone/platform/ohos/host/ohos_toplevel_pad_window.h"
 
#include "ohos/adapter/context/context_adapter.h"

namespace ui {
OhosToplevelPadWindow::OhosToplevelPadWindow(PlatformWindowDelegate* delegate,
                                             OhosWindowManager* manager)
    : OhosToplevelWindow(delegate, manager) {
  if (ohos::adapter::ContextAdapter::GetInstance().IsNormalWindowMode()) {
    state_ = PlatformWindowState::kMaximized;
  }
}
 
void OhosToplevelPadWindow::HandleEvent(std::shared_ptr<XCEvent> event) {
  switch (event->type()) {
    case XCEventType::ET_DEVICE_MODE_CHANGED: {
      OnDeviceModeChanged(event);
      break;
    }
    default:
      OhosToplevelWindow::HandleEvent(event);
      break;
  }
}

void OhosToplevelPadWindow::OnDeviceModeChanged(std::shared_ptr<XCEvent> event) {
  auto window_event = static_pointer_cast<DeviceInfoChangeEvent>(event);
  auto change_event_type = window_event->change_event_type_;
  auto status = window_event->status_;

  switch (change_event_type) {
    case ChangeEventType::CHANGE_TO_NORMAL_MODE: {
      RecoverStateWhenDeviceModeChanged(PlatformWindowState::kMaximized);
      break;
    }
    case ChangeEventType::CHANGE_TO_FREE_MODE: {
      if (status == WindowStatusType::MAXIMIZE) {
        RecoverStateWhenDeviceModeChanged(PlatformWindowState::kMaximized);
      } else if (status == WindowStatusType::FLOATING) {
        RecoverStateWhenDeviceModeChanged(PlatformWindowState::kNormal);
      }
      break;
    }
    default:
      break;
  }
}

void OhosToplevelPadWindow::SetWindowState(PlatformWindowState new_state,
                                           bool isTrigger) {
  if (state_ == PlatformWindowState::kFullScreen &&
      !is_trigger_state_change_required_) {
    is_trigger_state_change_required_ = true;
    OhosToplevelWindow::SetWindowState(new_state, false);
  } else {
    OhosToplevelWindow::SetWindowState(new_state, isTrigger);
  }
}
 
void OhosToplevelPadWindow::RecoverStateWhenDeviceModeChanged(
    PlatformWindowState recover_state) {
  if (state_ == PlatformWindowState::kFullScreen) {
    // When switching modes in full-screen state, the system will alter the
    // window status. There is no need to call TriggerStateChanges to actively
    // change the window status on the ets side. This avoids potential errors in
    // window bound due to system timing issues.
    previous_state_ = recover_state;
    is_trigger_state_change_required_ = false;
    delegate()->OnFullscreenSwitched(false);
  } else {
    SetWindowState(recover_state, false);
  }
}
}  // namespace ui