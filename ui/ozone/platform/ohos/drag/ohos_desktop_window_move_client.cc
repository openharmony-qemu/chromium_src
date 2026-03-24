// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
