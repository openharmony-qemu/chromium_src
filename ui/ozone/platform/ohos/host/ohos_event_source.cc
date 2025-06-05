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

#include "ui/ozone/platform/ohos/host/ohos_event_source.h"

#include <multimodalinput/oh_input_manager.h>

#include "base/functional/callback_forward.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/event/window_event_filter_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
#include "ui/display/screen.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/keycodes/keyboard_codes_posix.h"
#include "ui/events/ozone/layout/keyboard_layout_engine.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/events/pointer_details.h"
#include "ui/events/types/event_type.h"
#include "ui/gfx/geometry/point.h"
#include "ui/ozone/platform/ohos/host/ohos_event_filter.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"
#include "ui/ozone/public/ozone_platform.h"

namespace ui {

constexpr float kMouseMoveLimitDistance = 0.01;

constexpr int32_t kTouchpadScrollFingerCount = 2;
constexpr int32_t kMicrosecondsUnit = 1000;
constexpr int32_t kSimulateTouchEventId = 0;;

const static std::unordered_map<OH_NativeXComponent_KeyCode,
                                OH_NativeXComponent_KeyCode>
    kNumLockOffMap = {
        {KEY_NUMPAD_0, KEY_INSERT},       {KEY_NUMPAD_1, KEY_MOVE_END},
        {KEY_NUMPAD_2, KEY_DPAD_DOWN},    {KEY_NUMPAD_3, KEY_PAGE_DOWN},
        {KEY_NUMPAD_4, KEY_DPAD_LEFT},    {KEY_NUMPAD_5, KEY_UNKNOWN},
        {KEY_NUMPAD_6, KEY_DPAD_RIGHT},   {KEY_NUMPAD_7, KEY_HOME},
        {KEY_NUMPAD_8, KEY_DPAD_UP},      {KEY_NUMPAD_9, KEY_PAGE_UP},
        {KEY_NUMPAD_DOT, KEY_FORWARD_DEL}};

void MouseEventCallback(const int32_t widget_id,
                        const OH_NativeXComponent_MouseEvent& mouse_event);

void MouseHoverEventCallback(const int32_t widget_id, const bool is_hover);

void KeyEventCallback(const int32_t widget_id,
                      OH_NativeXComponent_KeyEvent& key_event);

void TouchEventCallback(
    const int32_t widget_id,
    const OH_NativeXComponent_TouchEvent& ohos_touch_event,
    const OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type,
    const TouchPointCoordinate& coordinate);

void PanEventCallback(
    const PanAction action,
    const int32_t widget_id,
    const PanEvent& event);

void PinchEventCallback(
    const std::string& pinch_step,
    const int32_t widget_id,
    const PinchEvent& event);

void DragEnterEventCallback(const int32_t widget_id,
                            const ohos::adapter::OhosDropData& drop_data);

void DragLeaveEventCallback(const int32_t widget_id);

void DropEventCallback(const int32_t widget_id,
                       const ohos::adapter::OhosDropData& drop_data);

void DragEndEventCallback(const int32_t widget_id);

void DragMoveEventCallback(const int32_t widget_id,
                           const float window_x,
                           const float window_y);
void SendWindowMouseEventForTabDragCallback(
    const int32_t widget_id,
    Input_MouseEvent* window_mouse_event);

OhosEventSource::OhosEventSource(OhosWindowManager* window_manager,
                                 OhosWindowDragManager* window_drag_manager)
    : window_manager_(window_manager),
      window_drag_manager_(window_drag_manager) {
  event_callback_ =
      std::make_shared<InputEventCallBack>();
  event_callback_->mouseEventCallback = &MouseEventCallback;
  event_callback_->keyEventCallback = &KeyEventCallback;
  event_callback_->touchEventCallback = &TouchEventCallback;
  event_callback_->pan_event_callback = &PanEventCallback;
  event_callback_->pinch_event_callback = &PinchEventCallback;
  event_callback_->dragEnterEventCallback = &DragEnterEventCallback;
  event_callback_->dragLeaveEventCallback = &DragLeaveEventCallback;
  event_callback_->dragMoveEventCallback = &DragMoveEventCallback;
  event_callback_->dropEventCallback = &DropEventCallback;
  event_callback_->dragEndEventCallback = &DragEndEventCallback;
  event_callback_->mouse_hover_event_callback = &MouseHoverEventCallback;
  event_callback_->sendWindowMouseEventForTabDragCallback =
      &SendWindowMouseEventForTabDragCallback;

  // Observes remove changes to know when touch points can be removed.
  window_manager_->AddObserver(this);
}

OhosEventSource::~OhosEventSource() {
  window_manager_->RemoveObserver(this);
}

void OhosEventSource::OnWindowAdded(OhosWindow* window) {
  DCHECK(window);

  XComponentManager::GetInstance()
      ->RegisterInputEventCallBack(window->GetWidget(), event_callback_);
}

void OhosEventSource::OnWindowRemoved(OhosWindow* window) {
  DCHECK(window);
  // TODO: remove event callback
}

void OhosEventSource::OnMouseMoveEvent(
    const gfx::AcceleratedWidget widget_id,
    const OH_NativeXComponent_MouseEvent& mouse_event,
    const gfx::PointF& original_location,
    const int32_t display_id) {
  OhosWindow* ohos_window = window_manager_->GetWindow(static_cast<int32_t>(widget_id));
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]mouse move target window nullptr";
    return;
  }

  if (pointer_location_.IsWithinDistance(original_location, kMouseMoveLimitDistance)) {
    return;
  }
  if (mouse_event.screenX != 0 && mouse_event.screenY != 0) {
    // xcomponent BUG may send error data: screenX and screenY both 0
    cursor_screen_point_.SetPoint(mouse_event.screenX, mouse_event.screenY);
  }
  int flags = pointer_flags_ | key_flags_;
  MouseEvent event(EventType::kMouseMoved, pointer_location_, pointer_location_,
                   EventTimeForNow(), flags, 0);
  event.set_display_id(display_id);
  SetTargetAndDispatchEvent(widget_id, event);

  // Determine if the drag event needs to be ended if needed
  EndSourceDragIfNeeded();
}

void OhosEventSource::SetTargetAndDispatchEvent(
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

void OhosEventSource::OnMouseEvent(
    const gfx::AcceleratedWidget widget_id,
    const OH_NativeXComponent_MouseEvent& mouse_event,
    const int32_t display_id) {
  EventType type = EventType::kMousePressed;
  gfx::PointF original_pointer_location = pointer_location_;
  pointer_location_.SetPoint(mouse_event.x, mouse_event.y);
  if (mouse_event.action == OH_NATIVEXCOMPONENT_MOUSE_PRESS) {
    type = EventType::kMousePressed;
  } else if (mouse_event.action == OH_NATIVEXCOMPONENT_MOUSE_RELEASE) {
    type = EventType::kMouseReleased;
  } else if (mouse_event.action == OH_NATIVEXCOMPONENT_MOUSE_MOVE) {
    OnMouseMoveEvent(widget_id, mouse_event, original_pointer_location, display_id);
    return;
  } else {
    LOG(WARNING) << "[multiinput]Unhandle action";
    return;
  }
  int changed_button = 0;
  switch (mouse_event.button) {
    case OH_NATIVEXCOMPONENT_LEFT_BUTTON:
      changed_button = EF_LEFT_MOUSE_BUTTON;
      break;
    case OH_NATIVEXCOMPONENT_RIGHT_BUTTON:
      changed_button = EF_RIGHT_MOUSE_BUTTON;
      break;
    case OH_NATIVEXCOMPONENT_MIDDLE_BUTTON:
      changed_button = EF_MIDDLE_MOUSE_BUTTON;
      break;
    case OH_NATIVEXCOMPONENT_BACK_BUTTON:
      changed_button = EF_BACK_MOUSE_BUTTON;
      break;
    case OH_NATIVEXCOMPONENT_FORWARD_BUTTON:
      changed_button = EF_FORWARD_MOUSE_BUTTON;
      break;
    default:
      LOG(WARNING) << "[multiinput]Unhandle button";
      break;
  }
  pointer_flags_ = type == EventType::kMousePressed
                       ? (pointer_flags_ | changed_button)
                       : (pointer_flags_ & ~changed_button);
  UpdateKeyFlagsByOhKeyState();
  EventFlags flags = pointer_flags_ | changed_button | key_flags_;
  MouseEvent event(type, pointer_location_, original_pointer_location,
                   EventTimeForNow(), flags, changed_button);
  event.set_display_id(display_id);
  SetTargetAndDispatchEvent(widget_id, event);
}

void OhosEventSource::OnMouseHoverEvent(const gfx::AcceleratedWidget widget_id,
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

  EventType type = is_hover ? EventType::kMouseEntered : EventType::kMouseExited;
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

void OhosEventSource::OnTouchEvent(
    const gfx::AcceleratedWidget widget_id,
    const OH_NativeXComponent_TouchEvent& ohos_touch_event,
    const OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type,
    const TouchPointCoordinate& coordinate) {
  EventType type = EventType::kUnknown;
  switch (ohos_touch_event.type) {
    case OH_NATIVEXCOMPONENT_DOWN:
      type = EventType::kTouchPressed;
      break;
    case OH_NATIVEXCOMPONENT_UP:
      type = EventType::kTouchReleased;
      break;
    case OH_NATIVEXCOMPONENT_MOVE:
      type = EventType::kTouchMoved;
      break;
    case OH_NATIVEXCOMPONENT_CANCEL:
      type = EventType::kTouchCancelled;
      break;
    default:
      type = EventType::kUnknown;
      break;
  }
  gfx::Point touch_location(ohos_touch_event.x, ohos_touch_event.y);
  cursor_screen_point_.SetPoint(coordinate.display_x, coordinate.display_y);

  EventPointerType pointer_type = EventPointerType::kUnknown;
  switch (ohos_touch_point_tool_type) {
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_FINGER:
      pointer_type = EventPointerType::kTouch;
      break;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_PEN:
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_BRUSH:
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_PENCIL:
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_AIRBRUSH:
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_LENS:
      pointer_type = EventPointerType::kPen;
      break;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_RUBBER:
      pointer_type = EventPointerType::kEraser;
      break;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_MOUSE:
      pointer_type = EventPointerType::kMouse;
      break;
    default:
      pointer_type = EventPointerType::kTouch;
      break;
  }
  PointerDetails pointer_details(pointer_type, ohos_touch_event.id, 0.0, 0.0,
                                 ohos_touch_event.force, 0.0, coordinate.tilt_x,
                                 coordinate.tilt_y);

  TouchEvent event(type, touch_location, EventTimeForNow(), pointer_details);
  SetTargetAndDispatchEvent(widget_id, event);
}

void OhosEventSource::CreateAndDispatchFlingEvent(
    const gfx::AcceleratedWidget widget_id,
    const PanEvent& ohos_event,
    const EventFlags& event_flags,
    const bool is_start) {
  if (ohos_event.source_tool != GestureEventSourceTool::kTouchpad) {
    return;
  }
  EventType event_type = is_start ? ui::EventType::kScrollFlingStart :
                                 ui::EventType::kScrollFlingCancel;
  float velocity_x = ohos_event.velocity_x;
  float velocity_y = ohos_event.velocity_y;
  ScrollEvent fling_event(event_type, pointer_location_, pointer_location_,
                          EventTimeForNow(), event_flags,
                          velocity_x, velocity_y,
                          velocity_x, velocity_y,
                          kTouchpadScrollFingerCount);
  is_fling_active_ = is_start;
  SetTargetAndDispatchEvent(widget_id, fling_event);
}

void OhosEventSource::OnPanEvent(
    const PanAction action,
    const gfx::AcceleratedWidget widget_id,
    const PanEvent& ohos_event) {
  UpdateKeyFlagsByOhKeyState();
  EventFlags event_flags = pointer_flags_ | key_flags_;
  // Set EF_PRECISION_SCROLLING_DELTA only for events originating from touchpad
  // devices.
  if (key_flags_ == EF_NONE &&
      ohos_event.source_tool == GestureEventSourceTool::kTouchpad) {
    event_flags |= EF_PRECISION_SCROLLING_DELTA;
  }
  if (is_fling_active_) {
    CreateAndDispatchFlingEvent(widget_id, ohos_event, event_flags, false);
  }
  if (action == PanAction::kEnd) {
    CreateAndDispatchFlingEvent(widget_id, ohos_event, event_flags, true);
  }

  if (action != PanAction::kUpdate) {
    mouse_wheel_offset_.set_x(0.0);
    mouse_wheel_offset_.set_y(0.0);
    return;
  }
  ui::MouseWheelEvent event(
      gfx::Vector2d(ohos_event.offset_x - mouse_wheel_offset_.x(),
                    ohos_event.offset_y - mouse_wheel_offset_.y()),
      pointer_location_, pointer_location_, EventTimeForNow(), event_flags, 0);
  mouse_wheel_offset_.SetPoint(ohos_event.offset_x, ohos_event.offset_y);
  event.InitializeNative();
  SetTargetAndDispatchEvent(widget_id, event);
}

void OhosEventSource::OnKeyEvent(const gfx::AcceleratedWidget widget_id,
                                 KeyEvent& key_event) {
  SetTargetAndDispatchEvent(widget_id, key_event);
}

bool NearZero(const float num) {
  // Epsilon of 1e-10 at 0.
  return (std::fabs(num) < 1e-10);
}

void OhosEventSource::OnPinchEvent(
    const std::string& pinch_step,
    const gfx::AcceleratedWidget widget_id,
    const PinchEvent& gesture_event) {
    ui::GestureEventDetails event_details;
    if (pinch_step == "start") {
      event_details = ui::GestureEventDetails(ui::EventType::kGesturePinchBegin);
    } else if (pinch_step == "update") {
      event_details = ui::GestureEventDetails(ui::EventType::kGesturePinchUpdate);
    } else if (pinch_step == "end") {
      event_details = ui::GestureEventDetails(ui::EventType::kGesturePinchEnd);
    } else {
      LOG(WARNING) << "[pinch-event]Unhandle action";
      return;
    }
    event_details.set_device_type(ui::GestureDeviceType::DEVICE_TOUCHPAD);
    event_details.set_scale(
        NearZero(last_scale_) ? 1.0f : gesture_event.scale / last_scale_);
    last_scale_ = gesture_event.scale;

    ui::GestureEvent pinch_event(gesture_event.offset_x, gesture_event.offset_y, 0,
                                 ui::EventTimeForNow(), event_details);
    SetTargetAndDispatchEvent(widget_id, pinch_event);
}

void OhosEventSource::UpdateKeyFlags() {
  key_flags_ = EF_NONE;
}

void OhosEventSource::UpdateKeyPressedState(const int32_t key_code_left,
                                            const int32_t key_code_right,
                                            const int event_flag) {
  Input_KeyState* keyState_left = OH_Input_CreateKeyState();
  Input_KeyState* keyState_right = OH_Input_CreateKeyState();
  OH_Input_SetKeyCode(keyState_left, key_code_left);
  OH_Input_GetKeyState(keyState_left);
  OH_Input_SetKeyCode(keyState_right, key_code_right);
  OH_Input_GetKeyState(keyState_right);
  if (KEY_PRESSED == OH_Input_GetKeyPressed(keyState_left) ||
      KEY_PRESSED == OH_Input_GetKeyPressed(keyState_right)) {
    key_flags_ = static_cast<int>(key_flags_) | event_flag;
  }
  OH_Input_DestroyKeyState(&keyState_left);
  OH_Input_DestroyKeyState(&keyState_right);
}
void OhosEventSource::UpdateSwitchState(const int32_t key_code,
                                        const int event_flag) {
  Input_KeyState* keyState = OH_Input_CreateKeyState();
  OH_Input_SetKeyCode(keyState, key_code);
  OH_Input_GetKeyState(keyState);
  if (KEY_SWITCH_ON == OH_Input_GetKeySwitch(keyState)) {
    key_flags_ = static_cast<int>(key_flags_) | event_flag;
  }
  OH_Input_DestroyKeyState(&keyState);
}
void OhosEventSource::UpdateKeyFlagsByOhKeyState() {
  key_flags_ = EF_NONE;
  UpdateKeyPressedState(KEYCODE_ALT_LEFT, KEYCODE_ALT_RIGHT, EF_ALT_DOWN);
  UpdateKeyPressedState(KEYCODE_SHIFT_LEFT, KEYCODE_SHIFT_RIGHT, EF_SHIFT_DOWN);
  UpdateKeyPressedState(KEYCODE_CTRL_LEFT, KEYCODE_CTRL_RIGHT, EF_CONTROL_DOWN);
  UpdateKeyPressedState(KEYCODE_META_LEFT, KEYCODE_META_RIGHT, EF_COMMAND_DOWN);
  UpdateSwitchState(KEYCODE_CAPS_LOCK, EF_CAPS_LOCK_ON);
  UpdateSwitchState(KEYCODE_SCROLL_LOCK, EF_SCROLL_LOCK_ON);
  UpdateSwitchState(KEYCODE_NUM_LOCK, EF_NUM_LOCK_ON);
}

gfx::Point OhosEventSource::GetCursorScreenPoint() {
  return gfx::ToFlooredPoint(cursor_screen_point_);
}

void OhosEventSource::OnDragEnterEvent(const gfx::AcceleratedWidget widget_id,
                                       const ohos::adapter::OhosDropData& drop_data) {
  pointer_flags_ |= EF_LEFT_MOUSE_BUTTON;

  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag enter Event target nullptr";
    return;
  }
  ohos_window->GetDragManager()->DragEnter(drop_data, pointer_location_);
}

void OhosEventSource::OnDragLeaveEvent(const gfx::AcceleratedWidget widget_id) {
  pointer_flags_ &= ~EF_LEFT_MOUSE_BUTTON;
  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag leave target window nullptr";
    return;
  }
  ohos_window->GetDragManager()->DragLeave();
}

void OhosEventSource::OnDragMoveEvent(const gfx::AcceleratedWidget widget_id,
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

void OhosEventSource::OnDropEvent(
    const gfx::AcceleratedWidget widget_id,
    const ohos::adapter::OhosDropData& drop_data) {
  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]on drop Event target nullptr";
    return;
  }
  ohos_window->GetDragManager()->OnDrop(drop_data, pointer_location_);
}

void OhosEventSource::OnDragEndEvent(const gfx::AcceleratedWidget widget_id) {
  pointer_flags_ &= ~EF_LEFT_MOUSE_BUTTON;
  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag end target window nullptr";
    return;
  }
  ohos_window->GetDragManager()->DragEnd();
}

void OhosEventSource::EndSourceDragIfNeeded() {
  // If there is data in DragSourceWindow when the cursor is moved normally,
  // it indicates that the drag event of the window
  // that initiates the drag is not complete.
  if (window_manager_->GetDragSourceWindow() && pointer_flags_ == 0 &&
      pointer_location_.x() > 0 && pointer_location_.y() > 0) {
    window_manager_->GetDragSourceWindow()->GetDragManager()->DragEnd();
  }
}

void OhosEventSource::SimulateTouchUp(const gfx::AcceleratedWidget widget_id) {
  OH_NativeXComponent_TouchEvent touch_event;
  touch_event.id = kSimulateTouchEventId;
  touch_event.type = OH_NATIVEXCOMPONENT_UP;
  OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type =
      OH_NATIVEXCOMPONENT_TOOL_TYPE_FINGER;
  TouchPointCoordinate coordinate;
  coordinate.display_x = cursor_screen_point_.x();
  coordinate.display_y = cursor_screen_point_.y();
  OnTouchEvent(widget_id, touch_event, ohos_touch_point_tool_type, coordinate);
}

void OhosEventSource::ShiftWindowEvent(
    const gfx::AcceleratedWidget source_widget_id,
    const gfx::AcceleratedWidget target_widget_id) {
  window_drag_manager_->ShiftWindowEvent(source_widget_id, target_widget_id);
}

void OhosEventSource::SendWindowMouseEventForTabDrag(
    const gfx::AcceleratedWidget widget_id,
    std::shared_ptr<OH_NativeXComponent_MouseEvent> xcomponent_mouse_event,
    const int32_t display_id) {
  if (xcomponent_mouse_event == nullptr) {
    LOG(ERROR) << "[OhosTabDrag] " << __FUNCTION__
               << ", xcomponent_mouse_event is null";
    return;
  }
  if (window_drag_manager_->NeedSendWindowMouseEventToUi(
      widget_id, xcomponent_mouse_event)) {
    OhosWindow* ohos_window = window_manager_->GetWindow(widget_id);
    if (ohos_window) {
      gfx::Rect window_bounds = ohos_window->GetBoundsInPixels();
      xcomponent_mouse_event->x =
          xcomponent_mouse_event->screenX - window_bounds.x();
      xcomponent_mouse_event->y =
          xcomponent_mouse_event->screenY - window_bounds.y();
    }
    // When the tab page drag is complete, the tab page drag parameter is cleared
    if (xcomponent_mouse_event->action == OH_NATIVEXCOMPONENT_MOUSE_RELEASE) {
      LOG(WARNING)
          << "[OhosTabDrag] SendWindowMouseEventForTabDrag tab dragging is ended"
              ", widget_id:"
          << widget_id;
      EndTabDragging();
    }
    OnMouseEvent(widget_id, *xcomponent_mouse_event, display_id);
  }
}

void OhosEventSource::StartTabDragging(const gfx::AcceleratedWidget widget_id) {
  if (widget_id <= 0) {
    LOG(ERROR) << "[OhosTabDrag] " << __FUNCTION__
               << ", widget_id is invalid";
    return;
  }
  window_drag_manager_->StartTabDragging(widget_id);
}

void OhosEventSource::EndTabDragging() {
  LOG(INFO) << "[OhosTabDrag] " << __FUNCTION__;
  window_drag_manager_->ClearDraggingTabParams();
}

void MouseEventCallback(const int32_t widget_id,
                        const OH_NativeXComponent_MouseEvent& mouse_event) {
  if (OhosEventFilter::GetInstance().CheckFilterMouseEvent(widget_id,
                                                           mouse_event)) {
    DLOG(ERROR) << "mouse event is filtered,widget_id:" << widget_id
                << ",action:" << static_cast<int>(mouse_event.action);
    return;
  }
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const OH_NativeXComponent_MouseEvent& mouse_event) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]"
              << "register mouse event callback before even source created";
          return;
        }
        int64_t default_display_id = display::Screen::GetScreen()->GetPrimaryDisplay().id();
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnMouseEvent(widget_id, mouse_event, static_cast<int32_t>(default_display_id));
      },
      widget_id, mouse_event);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
  OhosEventFilter::GetInstance().RefreshMouseEvent(widget_id, mouse_event);
}

OH_NativeXComponent_KeyCode GetKeyWithNumLockOFF(
    OH_NativeXComponent_KeyCode key,
    int flags) {
  if ((flags & EF_NUM_LOCK_ON) != EF_NUM_LOCK_ON) {
    auto iter = kNumLockOffMap.find(key);
    return (iter != kNumLockOffMap.end()) ? iter->second : key;
  }
  return key;
}

void KeyEventCallback(const int32_t widget_id,
                      OH_NativeXComponent_KeyEvent& key_event) {
  OH_NativeXComponent_KeyAction key_action =
      OH_NATIVEXCOMPONENT_KEY_ACTION_UNKNOWN;
  EventType type = EventType::kUnknown;
  if (OH_NativeXComponent_GetKeyEventAction(&key_event, &key_action) ==
      OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    type = (key_action == OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN)
               ? EventType::kKeyPressed
               : EventType::kKeyReleased;
  } else {
    LOG(WARNING) << "[multiinput]get key event action failed";
    return;
  }
  OH_NativeXComponent_KeyCode key = KEY_UNKNOWN;
  if (OH_NativeXComponent_GetKeyEventCode(&key_event, &key) !=
          OH_NATIVEXCOMPONENT_RESULT_SUCCESS ||
      key == KEY_UNKNOWN) {
    LOG(WARNING) << "[multiinput]get key code is unknow";
    return;
  }

  auto task = base::BindOnce(
      [](const int32_t widget_id, OH_NativeXComponent_KeyCode key,
         const EventType event_type) {
        auto* event_source = reinterpret_cast<OhosEventSource*>(
            PlatformEventSource::GetInstance());
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput] get event source failed";
          return;
        }
        event_source->UpdateKeyFlagsByOhKeyState();
        key = GetKeyWithNumLockOFF(key, event_source->GetKeyFlags());
        DomCode dom_code = KeycodeConverter::NativeKeycodeToDomCode(key);
        DomKey dom_key;
        KeyboardCode key_code = VKEY_UNKNOWN;
        auto* layout_engine =
            KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();
        if (layout_engine == nullptr ||
            !layout_engine->Lookup(dom_code, event_source->GetKeyFlags(),
                                   &dom_key, &key_code)) {
          LOG(WARNING) << "[multiinput]failed to decode key_code: " << key_code;
          return;
        }

        KeyEvent event(event_type, key_code, dom_code,
                       event_source->GetKeyFlags(), dom_key, EventTimeForNow());
        event_source->OnKeyEvent(widget_id, event);
      },
      widget_id, key, type);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
}

void TouchEventCallback(
    const int32_t widget_id,
    const OH_NativeXComponent_TouchEvent& ohos_touch_event,
    const OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type,
    const TouchPointCoordinate& coordinate) {
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const OH_NativeXComponent_TouchEvent& ohos_touch_event,
         const OH_NativeXComponent_TouchPointToolType
             ohos_touch_point_tool_type,
         const TouchPointCoordinate& coordinate) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput]register touch event callback before "
                          "even source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnTouchEvent(widget_id, ohos_touch_event,
                           ohos_touch_point_tool_type, coordinate);
      },
      widget_id, ohos_touch_event, ohos_touch_point_tool_type, coordinate);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
}

void PanEventCallback(
    const PanAction action,
    const int32_t widget_id,
    const PanEvent& event) {
  auto task = base::BindOnce(
      [](const PanAction action,
         const int32_t widget_id,
         const PanEvent& event) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]register mouse wheel event callback before even "
                 "source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnPanEvent(action, widget_id, event);
      },
      action, widget_id, event);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
}

void PinchEventCallback(const std::string& pinch_step,
                        const int32_t widget_id,
                        const PinchEvent& event) {
  auto task = base::BindOnce(
      [](const std::string& pinch_step,
         const int32_t widget_id,
         const PinchEvent& event) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[pinch-event]register pinch event callback before even "
                 "source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnPinchEvent(pinch_step, widget_id, event);
      },
      pinch_step, widget_id, event);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
}

void DragEnterEventCallback(const int32_t widget_id,
                            const ohos::adapter::OhosDropData& drop_data) {
  auto task = base::BindOnce(
      [](const int32_t widget_id, const ohos::adapter::OhosDropData& drop_data) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]register drag enter event callback"
                 " before event source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnDragEnterEvent(widget_id, drop_data);
      },
      widget_id, drop_data);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
}

void DragMoveEventCallback(const int32_t widget_id,
                           const float window_x,
                           const float window_y) {
  auto task = base::BindOnce(
      [](const int32_t widget_id, const float window_x, const float window_y) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]register drag move event callback before even "
                 "source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnDragMoveEvent(widget_id, window_x, window_y);
      },
      widget_id, window_x, window_y);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void DragLeaveEventCallback(const int32_t widget_id) {
  auto task = base::BindOnce(
      [](const int32_t widget_id) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]register drag leave event callback before even "
                 "source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnDragLeaveEvent(widget_id);
      },
      widget_id);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
}

void DropEventCallback(const int32_t widget_id,
                       const ohos::adapter::OhosDropData& drop_data) {
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const ohos::adapter::OhosDropData& drop_data) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]register drop event callback"
                 " before event source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnDropEvent(widget_id, drop_data);
      },
      widget_id, drop_data);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
}

void DragEndEventCallback(const int32_t widget_id) {
  auto task = base::BindOnce(
      [](const int32_t widget_id) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]register drag end event callback before even "
                 "source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnDragEndEvent(widget_id);
      },
      widget_id);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
}

void MouseHoverEventCallback(const int32_t widget_id, const bool is_hover) {
  auto task = base::BindOnce(
      [](const int32_t widget_id, const bool is_hover) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput]register mouse hover event callback"
                          " before event source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnMouseHoverEvent(widget_id, is_hover);
      },
      widget_id, is_hover);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

OH_NativeXComponent_MouseEventAction ConvertMouseEventActionFromWindowMouseEvent(
    int32_t window_mouse_action) {
  OH_NativeXComponent_MouseEventAction mouse_event_action =
      OH_NATIVEXCOMPONENT_MOUSE_NONE;
  switch (window_mouse_action) {
    case MOUSE_ACTION_MOVE:
      mouse_event_action = OH_NATIVEXCOMPONENT_MOUSE_MOVE;
      break;
    case MOUSE_ACTION_BUTTON_DOWN:
      mouse_event_action = OH_NATIVEXCOMPONENT_MOUSE_PRESS;
      break;
    case MOUSE_ACTION_BUTTON_UP:
      mouse_event_action = OH_NATIVEXCOMPONENT_MOUSE_RELEASE;
      break;
    default:
      DLOG(INFO) << "ConvertMouseEventActionFromWindowMouseEvent "
                    "window_mouse_action is not useful:"
                 << window_mouse_action;
      break;
  }
  return mouse_event_action;
}

OH_NativeXComponent_MouseEventButton ConvertMouseEventButtonFromWindowMouseEvent(
    int32_t window_mouse_button) {
  OH_NativeXComponent_MouseEventButton mouse_event_button =
      OH_NATIVEXCOMPONENT_NONE_BUTTON;
  switch (window_mouse_button) {
    case MOUSE_BUTTON_LEFT:
      mouse_event_button = OH_NATIVEXCOMPONENT_LEFT_BUTTON;
      break;
    case MOUSE_BUTTON_RIGHT:
      mouse_event_button = OH_NATIVEXCOMPONENT_RIGHT_BUTTON;
      break;
    default:
      DLOG(INFO) << "ConvertMouseEventButtonFromWindowMouseEvent mouse_button is "
                    "not useful:"
                 << window_mouse_button;
      break;
  }
  return mouse_event_button;
}

void ConvertWindowMouseEventToXcomponentEvent(
    Input_MouseEvent* window_mouse_event,
    std::shared_ptr<OH_NativeXComponent_MouseEvent> xcomponent_mouse_event) {
  int32_t window_mouse_action =
      WindowEventFilterAdapter::GetWindowMouseEventAction(window_mouse_event);
  int32_t display_x =
      WindowEventFilterAdapter::GetWindowMouseEventDisplayX(window_mouse_event);
  int32_t display_y =
      WindowEventFilterAdapter::GetWindowMouseEventDisplayY(window_mouse_event);
  int32_t window_mouse_button =
      WindowEventFilterAdapter::GetWindowMouseEventButton(window_mouse_event);
  int64_t action_time = WindowEventFilterAdapter::GetWindowMouseEventActionTime(
      window_mouse_event);
  xcomponent_mouse_event->action =
      ConvertMouseEventActionFromWindowMouseEvent(window_mouse_action);
  xcomponent_mouse_event->button =
      ConvertMouseEventButtonFromWindowMouseEvent(window_mouse_button);
  xcomponent_mouse_event->screenX = display_x;
  xcomponent_mouse_event->screenY = display_y;
  // The unit of the mouse event timestamp from the window is millisecond.
  // The unit of the mouse event in the xcomponent is microsecond.
  xcomponent_mouse_event->timestamp = action_time * kMicrosecondsUnit;
}

void SendWindowMouseEventForTabDragCallback(
    const int32_t widget_id,
    Input_MouseEvent* window_mouse_event) {
  if (window_mouse_event == nullptr) {
    LOG(ERROR) << "SendWindowMouseEventForTabDragCallback mouse event is "
              "null,widget_id:"
            << widget_id;
    return;
  }
  std::shared_ptr<OH_NativeXComponent_MouseEvent> xcomponent_mouse_event =
      std::make_shared<OH_NativeXComponent_MouseEvent>();
  ConvertWindowMouseEventToXcomponentEvent(window_mouse_event,
                                           xcomponent_mouse_event);
  if (OhosEventFilter::GetInstance().CheckFilterMouseEvent(
      widget_id, *xcomponent_mouse_event)) {
    DLOG(ERROR) << "SendWindowMouseEventForTabDragCallback mouse event is "
                   "filtered,widget_id:"
                << widget_id << ", action:" << xcomponent_mouse_event->action;
    return;
  }
  OhosEventFilter::GetInstance().RefreshMouseEvent(widget_id,
                                                   *xcomponent_mouse_event);
  int32_t display_id = WindowEventFilterAdapter::GetWindowMouseEventDisplayId(
      window_mouse_event);
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const std::shared_ptr<OH_NativeXComponent_MouseEvent>
             xcomponent_mouse_event, int32_t display_id) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[OhosTabDrag]register "
                          "SendWindowMouseEventForTabDragCallback before event "
                          "source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->SendWindowMouseEventForTabDrag(widget_id, xcomponent_mouse_event, display_id);
      },
      widget_id, std::move(xcomponent_mouse_event), display_id);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

}  // namespace ui
