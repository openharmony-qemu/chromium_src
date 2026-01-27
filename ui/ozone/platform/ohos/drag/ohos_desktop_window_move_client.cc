/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
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

#include "ui/ozone/platform/ohos/drag/ohos_desktop_window_move_client.h"

#include "ui/display/screen.h"
#include "ui/events/event.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source_base.h"

namespace ui {

OhosDesktopWindowMoveClient::Delegate::~Delegate() = default;

OhosDesktopWindowMoveClient::OhosDesktopWindowMoveClient(Delegate* delegate)
    : window_delegate_(delegate) {}

OhosDesktopWindowMoveClient::~OhosDesktopWindowMoveClient() = default;

void OhosDesktopWindowMoveClient::OnMoveEnd() {
  EndMoveLoop();
}

void OhosDesktopWindowMoveClient::OnTabMoveForStartMoving() {
  if (!tab_start_moving_) {
    LOG(INFO) << "[OhosTabDrag] " << __FUNCTION__
              << ", tab startMoving, offset_x:" << window_offset_.x()
              << ", offset_y:" << window_offset_.y();
    window_delegate_->StartWindowMovingWithOffset(
        gfx::PointF(window_offset_.x(), window_offset_.y()));
    tab_start_moving_ = true;
  }
}

void OhosDesktopWindowMoveClient::OnTouchMove() {
  auto* event_source = PlatformEventSource::GetInstance();
  if (event_source == nullptr) {
    LOG(ERROR) << "[OhosTabDrag]" << __FUNCTION__ << " fail,no event_source";
    return;
  }
  gfx::Point cursor_pointer =
      reinterpret_cast<OhosEventSourceBase*>(event_source)
          ->GetCursorScreenPoint();
  gfx::Point system_loc = cursor_pointer - window_offset_;
  if (window_size_.IsZero()) {
    window_size_ =  window_delegate_->GetSize();
  }
  window_delegate_->SetBoundsOnMove(
      gfx::Rect(system_loc, window_size_));
}

void OhosDesktopWindowMoveClient::OnMoveLoopEnded() {
  tab_start_moving_ = false;
}

bool OhosDesktopWindowMoveClient::RunMoveLoop(
    bool can_grab_pointer,
    const gfx::Vector2d& drag_offset) {
  display::Display current_display = window_delegate_->GetCurrentDisplay();
  float device_scale_factor = current_display.device_scale_factor();
  window_offset_.set_x(drag_offset.x() * device_scale_factor);
  window_offset_.set_y(drag_offset.y() * device_scale_factor);
  return move_loop_.RunMoveLoop(can_grab_pointer,
                                window_delegate_->GetLastCursor(),
                                window_delegate_->GetLastCursor());
}

void OhosDesktopWindowMoveClient::EndMoveLoop() {
  window_size_ = gfx::Size();
  move_loop_.EndMoveLoop();
  tab_start_moving_ = false;
}

}  // namespace ui
