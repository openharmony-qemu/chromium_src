// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/widget/desktop_aura/window_event_filter_ohos.h"

#include "base/logging.h"
#include "ui/aura/client/aura_constants.h"
#include "ui/events/event_constants.h"
#include "ui/events/event_target.h"
#include "ui/events/types/event_type.h"
#include "ui/platform_window/wm/wm_move_resize_handler.h"
#include "ui/views/widget/desktop_aura/desktop_window_tree_host_platform.h"

namespace views {

WindowEventFilterOhos::WindowEventFilterOhos(
    DesktopWindowTreeHostPlatform* desktop_window_tree_host,
    ui::WmMoveResizeHandler* handler)
    : desktop_window_tree_host_(desktop_window_tree_host), handler_(handler) {
  desktop_window_tree_host_->window()->AddPreTargetHandler(this);
}

WindowEventFilterOhos::~WindowEventFilterOhos() {
  desktop_window_tree_host_->window()->RemovePreTargetHandler(this);
}

void WindowEventFilterOhos::HandleLocatedEventWithHitTest(
    int hit_test,
    ui::LocatedEvent* event) {
  if (event->IsMouseEvent() &&
      HandleMouseEventWithHitTest(hit_test, event->AsMouseEvent())) {
    return;
  }
  if (desktop_window_tree_host_->GetContentWindow()->GetProperty(
          aura::client::kResizeBehaviorKey) &
      aura::client::kResizeBehaviorCanResize) {
    MaybeDispatchHostWindowDragMovement(hit_test, event);
  }
}

bool WindowEventFilterOhos::HandleMouseEventWithHitTest(int hit_test,
                                                        ui::MouseEvent* event) {
  int previous_click_component = HTNOWHERE;
  if (event->IsLeftMouseButton()) {
    previous_click_component = click_component_;
    click_component_ = hit_test;
  }
  if (hit_test == HTCAPTION || is_dragging_) {
    OnClickedCaption(event, previous_click_component);
    return true;
  }
  return false;
}

void WindowEventFilterOhos::OnClickedCaption(ui::MouseEvent* event,
                                             int previous_click_component) {
  if (event->IsLeftMouseButton()) {
    if (event->type() == ui::EventType::kMousePressed) {
      is_dragging_ = true;
      // Tell the mouse position in the window at the beginning of window
      // dragging
      offset_ = event->AsLocatedEvent()->location();
      window_bounds_ =
          desktop_window_tree_host_->AsWindowTreeHost()->GetBoundsInPixels();
      LOG(INFO) << "[OhosDrag]WindowEventFilterOhos::OnClickedCaption,no "
                   "client drag running,offset_:"
                << offset_.ToString();
    } else if (event->type() == ui::EventType::kMouseReleased) {
      is_dragging_ = false;
      LOG(INFO) << "[OhosDrag]WindowEventFilterOhos::OnClickedCaption,no "
                   "client drag end.";
    } else if (event->type() == ui::EventType::kMouseDragged && is_dragging_) {
      MaybeDispatchHostWindowDragMovement(HTCAPTION, event);
      // when a window transitions from a maximized state to a normal window
      // state, the system continues to report the mouse position based on
      // the maximized window's coordinates.
      // So we just set the mouse position at the beginning of dragging.
      offset_.SetPoint(-1, -1);
    }
  } else {
    is_dragging_ = false;
  }
}

void WindowEventFilterOhos::MaybeDispatchHostWindowDragMovement(
    int hittest,
    ui::LocatedEvent* event) {
  if (!event->IsMouseEvent() && !event->IsGestureEvent()) {
    return;
  }
  if (event->IsMouseEvent() && !event->AsMouseEvent()->IsLeftMouseButton()) {
    return;
  }
  if (!handler_) {
    handler_ = reinterpret_cast<ui::WmMoveResizeHandler*>(event->target());
  }
  if (!handler_) {
    LOG(ERROR) << "WindowEventFilterOhos::MaybeDispatchHostWindowDragMovement "
                  "there is no WmMoveResizeHandler";
    return;
  }
  handler_->DispatchHostWindowDragMovement(hittest, offset_);
}

}  // namespace views
