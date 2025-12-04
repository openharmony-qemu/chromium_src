/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include "ui/ozone/platform/ohos/host/ohos_event_source_base.h"

#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/pointer_details.h"
#include "ui/events/types/event_type.h"
#include "ui/gfx/geometry/point.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"

namespace ui {

OhosEventSourceBase::OhosEventSourceBase(
    OhosWindowManager* window_manager,
    OhosWindowDragManager* window_drag_manager)
    : window_manager_(window_manager),
      window_drag_manager_(window_drag_manager) {
  // Observes remove changes to know when touch points can be removed.
  window_manager_->AddObserver(this);
}

OhosEventSourceBase::~OhosEventSourceBase() {
  window_manager_->RemoveObserver(this);
}

void OhosEventSourceBase::UpdateKeyPressedState(EventFlags& key_flags,
                                                const int32_t key_code_left,
                                                const int32_t key_code_right,
                                                const int event_flag) {
  Input_KeyState* keyState_left = OH_Input_CreateKeyState();
  Input_KeyState* keyState_right = OH_Input_CreateKeyState();
  if (keyState_left == nullptr || keyState_right == nullptr) {
    LOG(WARNING)
        << "[OhosEventSourceBase::UpdateKeyPressedState]Create KeyState failed";
    return;
  }

  OH_Input_SetKeyCode(keyState_left, key_code_left);
  OH_Input_GetKeyState(keyState_left);
  OH_Input_SetKeyCode(keyState_right, key_code_right);
  OH_Input_GetKeyState(keyState_right);
  if (KEY_PRESSED == OH_Input_GetKeyPressed(keyState_left) ||
      KEY_PRESSED == OH_Input_GetKeyPressed(keyState_right)) {
    key_flags = static_cast<int>(key_flags) | event_flag;
  }
  OH_Input_DestroyKeyState(&keyState_left);
  OH_Input_DestroyKeyState(&keyState_right);
}

void OhosEventSourceBase::UpdateSwitchState(EventFlags& key_flags,
                                            const int32_t key_code,
                                            const int event_flag) {
  Input_KeyState* keyState = OH_Input_CreateKeyState();
  OH_Input_SetKeyCode(keyState, key_code);
  OH_Input_GetKeyState(keyState);
  if (KEY_SWITCH_ON == OH_Input_GetKeySwitch(keyState)) {
    key_flags = static_cast<int>(key_flags) | event_flag;
  }
  OH_Input_DestroyKeyState(&keyState);
}

void OhosEventSourceBase::UpdateKeyFlagsByOhKeyState(EventFlags& key_flags) {
  UpdateKeyPressedState(key_flags, KEYCODE_ALT_LEFT, KEYCODE_ALT_RIGHT,
                        EF_ALT_DOWN);
  UpdateKeyPressedState(key_flags, KEYCODE_SHIFT_LEFT, KEYCODE_SHIFT_RIGHT,
                        EF_SHIFT_DOWN);
  UpdateKeyPressedState(key_flags, KEYCODE_CTRL_LEFT, KEYCODE_CTRL_RIGHT,
                        EF_CONTROL_DOWN);
  UpdateKeyPressedState(key_flags, KEYCODE_META_LEFT, KEYCODE_META_RIGHT,
                        EF_COMMAND_DOWN);
  UpdateSwitchState(key_flags, KEYCODE_CAPS_LOCK, EF_CAPS_LOCK_ON);
  UpdateSwitchState(key_flags, KEYCODE_SCROLL_LOCK, EF_SCROLL_LOCK_ON);
  UpdateSwitchState(key_flags, KEYCODE_NUM_LOCK, EF_NUM_LOCK_ON);
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

  EventType type =
      is_hover ? EventType::kMouseEntered : EventType::kMouseExited;
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

void OhosEventSourceBase::UpdateKeyFlags(const EventFlags& key_flags) {
  key_flags_ = key_flags;
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

void OhosEventSourceBase::OnDragEnterEvent(
    const gfx::AcceleratedWidget widget_id,
    const ohos::adapter::OhosDropData& drop_data) {
  pointer_flags_ |= EF_LEFT_MOUSE_BUTTON;

  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag enter Event target nullptr";
    return;
  }
  ohos_window->GetDragManager()->DragEnter(drop_data, pointer_location_);
}

void OhosEventSourceBase::OnDragLeaveEvent(
    const gfx::AcceleratedWidget widget_id) {
  pointer_flags_ &= ~EF_LEFT_MOUSE_BUTTON;
  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag leave target window nullptr";
    return;
  }
  ohos_window->GetDragManager()->DragLeave();
}

void OhosEventSourceBase::OnDragMoveEvent(
    const gfx::AcceleratedWidget widget_id,
    const float window_x,
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

void OhosEventSourceBase::OnDragEndEvent(
    const gfx::AcceleratedWidget widget_id) {
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

void OhosEventSourceBase::ShiftWindowEvent(
    const gfx::AcceleratedWidget source_widget_id,
    const gfx::AcceleratedWidget target_widget_id) {
  window_drag_manager_->ShiftWindowEvent(source_widget_id, target_widget_id);
}

void OhosEventSourceBase::StartTabDragging(
    const gfx::AcceleratedWidget widget_id) {
  if (widget_id <= 0) {
    LOG(ERROR) << "[OhosTabDrag] " << __FUNCTION__ << ", widget_id is invalid";
    return;
  }
  window_drag_manager_->StartTabDragging(widget_id);
}

void OhosEventSourceBase::EndTabDragging() {
  LOG(INFO) << "[OhosTabDrag] " << __FUNCTION__;
  window_drag_manager_->ClearDraggingTabParams();
}

void OhosEventSourceBase::StartTabDraggingByTouch(
    const gfx::AcceleratedWidget widget_id,
    const int32_t finger_id) {
  if (widget_id <= 0) {
    LOG(ERROR) << "[OhosTabDrag] " << __FUNCTION__ << ", widget_id is invalid";
    return;
  }
  window_drag_manager_->StartTabDraggingByTouch(widget_id, finger_id);
}

}  // namespace ui
