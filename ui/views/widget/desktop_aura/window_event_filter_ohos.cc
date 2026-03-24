// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/widget/desktop_aura/window_event_filter_ohos.h"

#include "base/logging.h"
#include "ohos/adapter/device_info/device_info.h"
#include "ui/aura/client/aura_constants.h"
#include "ui/aura/window.h"
#include "ui/aura/window_delegate.h"
#include "ui/events/event_constants.h"
#include "ui/events/event_target.h"
#include "ui/events/types/event_type.h"
#include "ui/platform_window/wm/wm_move_resize_handler.h"
#include "ui/views/widget/desktop_aura/desktop_window_tree_host_platform.h"

namespace views {

//kTouchSlop is Minimum distance for touch events
constexpr float kTouchSlop = 15.05;
constexpr float kSlidingEffectiveThresholdSquare = kTouchSlop * kTouchSlop;

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
  if (event->IsTouchEvent() &&
      HandleTouchEventWithHitTest(hit_test, event->AsTouchEvent())) {
    return;
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

bool WindowEventFilterOhos::HandleTouchEventWithHitTest(int hit_test,
                                                        ui::TouchEvent* event) {
  int previous_click_component = HTNOWHERE;
  if (event->type() == ui::EventType::kTouchPressed) {
    previous_click_component = click_component_;
    click_component_ = hit_test;
  }
  if (hit_test == HTCAPTION || is_touch_move_) {
    OnTouchedCaption(event, previous_click_component);
    return true;
  }
  return false;
}

void WindowEventFilterOhos::OnClickedCaption(ui::MouseEvent* event,
                                             int previous_click_component) {
  if (event->IsLeftMouseButton()) {
    // Refer to WindowEventFilterLinux for handling double-click title bar.
    if (event->flags() & ui::EF_IS_DOUBLE_CLICK) {
      click_component_ = HTNOWHERE;
      if (previous_click_component == HTCAPTION) {
        auto* content_window = desktop_window_tree_host_->GetContentWindow();
        MaybeToggleMaximizedState(content_window);
        event->SetHandled();
      }
      return;
    }

    if (ohos::adapter::device_info::DeviceInfo::SdkApi() >=
        ohos::adapter::device_info::SDK_VERSION_14) {
      if (event->type() == ui::EventType::kMousePressed) {
        is_dragging_ = true;
      } else if (event->type() == ui::EventType::kMouseReleased) {
        is_dragging_ = false;
      } else if (event->type() == ui::EventType::kMouseDragged && is_dragging_) {
        MaybeDispatchHostWindowDragMovement(HTCAPTION, event);
      }
    } else {
      if (event->type() == ui::EventType::kMousePressed) {
        is_dragging_ = true;
        // Tell the mouse position in the window at the beginning of window
        // dragging
        offset_ = event->AsLocatedEvent()->location();
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
    }
  } else {
    is_dragging_ = false;
  }
}

void WindowEventFilterOhos::OnTouchedCaption(ui::TouchEvent* event,
                                             int previous_click_component) {
  if (ohos::adapter::device_info::DeviceInfo::SdkApi() >=
      ohos::adapter::device_info::SDK_VERSION_14) {
    if (event->type() == ui::EventType::kTouchPressed) {
      touch_press_point_ = event->AsLocatedEvent()->location();
      is_touch_move_ = true;
    } else if (event->type() == ui::EventType::kTouchReleased) {
      is_touch_move_ = false;
    } else if (event->type() == ui::EventType::kTouchMoved && is_touch_move_) {
      gfx::Point move = event->AsLocatedEvent()->location();
      if (IsWithinDistance(touch_press_point_, move)) {
        MaybeDispatchHostWindowDragMovement(HTCAPTION, event);
      }
    } else {
      is_touch_move_ = false;
    }
  } else {
    if (event->type() == ui::EventType::kTouchPressed) {
      is_touch_move_ = true;
      offset_ = event->AsLocatedEvent()->location();
    } else if (event->type() == ui::EventType::kTouchReleased) {
      is_touch_move_ = false;
      LOG(INFO) << "[OhosDrag]WindowEventFilterOhos::OnTouchedCaption,no "
                   "client drag end.";
    } else if (event->type() == ui::EventType::kTouchMoved && is_touch_move_) {
      MaybeDispatchHostWindowDragMovement(HTCAPTION, event);
      offset_.SetPoint(-1, -1);
    } else {
      is_touch_move_ = false;
    }
  }
}

void WindowEventFilterOhos::MaybeDispatchHostWindowDragMovement(
    int hittest,
    ui::LocatedEvent* event) {
  if (!event->IsMouseEvent() && !event->IsGestureEvent() && !event->IsTouchEvent()) {
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

bool WindowEventFilterOhos::IsWithinDistance(const gfx::Point& press_location,
                                             const gfx::Point& move_location) {
  float dx = move_location.x() - press_location.x();
  float dy = move_location.y() - press_location.y();
  float distance = dx * dx + dy * dy;
  //distance is sliding effective threshold
  return distance > kSlidingEffectiveThresholdSquare;
}

void WindowEventFilterOhos::MaybeToggleMaximizedState(aura::Window* window) {
  if (!(window->GetProperty(aura::client::kResizeBehaviorKey) &
        aura::client::kResizeBehaviorCanMaximize)) {
    return;
  }

  if (desktop_window_tree_host_->IsMaximized()) {
    desktop_window_tree_host_->Restore();
  } else {
    desktop_window_tree_host_->Maximize();
  }
}

void WindowEventFilterOhos::OnGestureEvent(ui::GestureEvent* event) {
  auto* window = static_cast<aura::Window*>(event->target());
  int hit_test_code =
      window->delegate()
          ? window->delegate()->GetNonClientComponent(event->location())
          : HTNOWHERE;
  // Cosuming long press event, the shortcut menu is displayed
  // via a long tab event in root_view.cc.
  if (event->type() == ui::EventType::kGestureLongPress &&
      hit_test_code == HTCAPTION) {
    event->SetHandled();
    return;
  }
  // Double tap to maximize.
  if (event->type() == ui::EventType::kGestureTap) {
    int previous_click_component = click_component_;
    click_component_ = hit_test_code;

    if (click_component_ == HTCAPTION &&
        click_component_ == previous_click_component &&
        event->details().tap_count() == 2) {
      MaybeToggleMaximizedState(window);
      click_component_ = HTNOWHERE;
      event->StopPropagation();
    }
    return;
  }
}

}  // namespace views
