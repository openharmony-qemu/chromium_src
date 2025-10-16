// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_event_source_base.h"

#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/pointer_details.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/types/event_type.h"
#include "ui/gfx/geometry/point.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"

namespace ui {

OhosEventSourceBase::OhosEventSourceBase(OhosWindowManager* window_manager)
    : window_manager_(window_manager) {
  // Observes remove changes to know when touch points can be removed.
  window_manager_->AddObserver(this);
}

OhosEventSourceBase::~OhosEventSourceBase() {
  window_manager_->RemoveObserver(this);
}

void OhosEventSourceBase::SetTargetAndDispatchEvent(
    const gfx::AcceleratedWidget widget_id,
    Event& event) {
  auto* target = window_manager_->GetWindow(static_cast<int32_t>(widget_id));
  if (target == nullptr) {
    LOG(WARNING) << "[multiinput]Event target nullptr";
    return;
  }
  Event::DispatcherApi(&event).set_target(target);
  DispatchEvent(&event);
}

void OhosEventSourceBase::OnMouseHoverEvent(
    const gfx::AcceleratedWidget widget_id,
    const bool is_hover) {
  if (is_hover) {
    window_manager_->SetPointerFocusedWindow(widget_id);
  }

  auto closure = is_hover ? base::NullCallback()
                          : base::BindOnce(
                              [](OhosWindowManager* wm) {
                                wm->SetPointerFocusedWindow(nullptr);
                              },
                              window_manager_);

  EventType type = is_hover ? ET_MOUSE_ENTERED : ET_MOUSE_EXITED;
  // mouse leave event, xcomponent will not get mouse position,
  // set a fake position outside xcomponent when mosue leave
  auto pointer_location = is_hover ? pointer_location_ : gfx::PointF(-1, -1);
  MouseEvent event(type, pointer_location, pointer_location, EventTimeForNow(),
                   pointer_flags_, 0);
  SetTargetAndDispatchEvent(widget_id, event);

  if (!closure.is_null()) {
    std::move(closure).Run();
  }
}

void OhosEventSourceBase::OnKeyEvent(const gfx::AcceleratedWidget widget_id,
                                     KeyEvent& key_event) {
  SetTargetAndDispatchEvent(widget_id, key_event);
}

void OhosEventSourceBase::UpdateKeyFlags() {
  key_flags_ = EF_NONE;
}

void OhosEventSourceBase::UpdateKeyFlags(const EventType type,
                                         const DomCode dom_code) {
  MouseEventFlags f = EF_NONE;
  switch (dom_code) {
    case DomCode::CONTROL_LEFT:
    case DomCode::CONTROL_RIGHT:
      f = EF_CONTROL_DOWN;
      break;
    case DomCode::SHIFT_LEFT:
    case DomCode::SHIFT_RIGHT:
      f = EF_SHIFT_DOWN;
      break;
    case DomCode::ALT_LEFT:
    case DomCode::ALT_RIGHT:
      f = EF_ALT_DOWN;
      break;
    case DomCode::META_LEFT:
    case DomCode::META_RIGHT:
      f = EF_COMMAND_DOWN;
      break;
    case DomCode::NUM_LOCK:
      f = EF_NUM_LOCK_ON;
      break;
    case DomCode::CAPS_LOCK:
      f = EF_CAPS_LOCK_ON;
      break;
    case DomCode::SCROLL_LOCK:
      f = EF_SCROLL_LOCK_ON;
      break;
    default:
      f = EF_NONE;
      break;
  }
  if (type == ET_KEY_PRESSED) {
    key_flags_ = static_cast<int>(key_flags_) | f;
  } else {
    key_flags_ = static_cast<int>(key_flags_) & ~f;
  }
}

gfx::Point OhosEventSourceBase::GetCursorScreenPoint() {
  return gfx::ToFlooredPoint(cursor_screen_point_);
}

void OhosEventSourceBase::EndSourceDragIfNeeded() {
  // If there is data in DragSourceWindow when the cursor is moved normally,
  // it indicates that the drag event of the window
  // that initiates the drag is not complete.
  if (window_manager_->GetDragSourceWindow() && pointer_flags_ == 0 &&
      pointer_location_.x() > 0 && pointer_location_.y() > 0) {
    window_manager_->GetDragSourceWindow()->GetDragManager()->DragEnd();
  }
}

void OhosEventSourceBase::OnDragEnterEvent(const gfx::AcceleratedWidget widget_id,
                                           const ohos::adapter::OhosDropData& drop_data) {
  pointer_flags_ |= EF_LEFT_MOUSE_BUTTON;

  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag enter Event target nullptr";
    return;
  }
  ohos_window->GetDragManager()->DragEnter(drop_data, pointer_location_);
}

void OhosEventSourceBase::OnDragLeaveEvent(const gfx::AcceleratedWidget widget_id) {
  pointer_flags_ &= ~EF_LEFT_MOUSE_BUTTON;
  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag leave target window nullptr";
    return;
  }
  ohos_window->GetDragManager()->DragLeave();
}

void OhosEventSourceBase::OnDragMoveEvent(const gfx::AcceleratedWidget widget_id,
                                          float window_x,
                                          const float window_y) {
  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag move target window nullptr";
    return;
  }
  gfx::Point window_point = gfx::Point(window_x, window_y);
  ohos_window->GetDragManager()->UpdateDrag(window_point);
}

void OhosEventSourceBase::OnDropEvent(
    const gfx::AcceleratedWidget widget_id,
    const ohos::adapter::OhosDropData& drop_data) {
  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]on drop Event target nullptr";
    return;
  }
  ohos_window->GetDragManager()->OnDrop(drop_data, pointer_location_);
}

void OhosEventSourceBase::OnDragEndEvent(const gfx::AcceleratedWidget widget_id) {
  pointer_flags_ &= ~EF_LEFT_MOUSE_BUTTON;
  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag end target window nullptr";
    return;
  }
  ohos_window->GetDragManager()->DragEnd();
}

bool OhosEventSourceBase::NearZero(const float num) {
  // Epsilon of 1e-10 at 0.
  return (std::fabs(num) < 1e-10);
}

}  // namespace ui
