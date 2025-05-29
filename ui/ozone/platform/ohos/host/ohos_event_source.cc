// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_event_source.h"

#include <ace/xcomponent/native_xcomponent_key_event.h>

#include "base/functional/callback_forward.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
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

constexpr float kFlingVelocityFactor = 5.0;

constexpr float kMouseMoveLimitDistance = 0.01;

constexpr int32_t kTouchpadScrollFingerCount = 2;

void MouseEventCallback(const int32_t widget_id,
                        const OH_NativeXComponent_MouseEvent& mouse_event);

void MouseHoverEventCallback(const int32_t widget_id, const bool is_hover);

void KeyEventCallback(const int32_t widget_id,
                      OH_NativeXComponent_KeyEvent& key_event);

void TouchEventCallback(
    const int32_t widget_id,
    const OH_NativeXComponent_TouchEvent& ohos_touch_event,
    const OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type,
    const float tilt_x,
    const float tilt_y);

void PanEventCallback(const PanAction action,
                      const int32_t widget_id,
                      const PanEvent& event);

void PinchEventCallback(const std::string& pinch_step,
                        const int32_t widget_id,
                        const PinchEvent& event);

void DragEnterEventCallback(const int32_t widget_id,
                            const ohos::adapter::OhosDragInfo& drag_info);

void DragLeaveEventCallback(const int32_t widget_id);

void DropEventCallback(const int32_t widget_id,
                       const ohos::adapter::OhosDragInfo& drag_info);

void DragEndEventCallback(const int32_t widget_id);

void DragMoveEventCallback(const int32_t widget_id,
                           const float window_x,
                           const float window_y);

OhosEventSource::OhosEventSource(OhosWindowManager* window_manager)
    : window_manager_(window_manager) {
  event_callback_ = std::make_shared<InputEventCallBack>();
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

  // Observes remove changes to know when touch points can be removed.
  window_manager_->AddObserver(this);
}

OhosEventSource::~OhosEventSource() {
  window_manager_->RemoveObserver(this);
}

void OhosEventSource::OnWindowAdded(OhosWindow* window) {
  DCHECK(window);

  XComponentManager::GetInstance()->RegisterInputEventCallBack(
      window->GetWidget(), event_callback_);
}

void OhosEventSource::OnWindowRemoved(OhosWindow* window) {
  DCHECK(window);
  // TODO: remove event callback
}

void OhosEventSource::OnMouseMoveEvent(
    const gfx::AcceleratedWidget widget_id,
    const OH_NativeXComponent_MouseEvent& mouse_event) {
  OhosWindow* ohos_window =
      window_manager_->GetWindow(static_cast<int32_t>(widget_id));
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]mouse move target window nullptr";
    return;
  }

  gfx::PointF location(mouse_event.x, mouse_event.y);
  if (location.IsWithinDistance(pointer_location_, kMouseMoveLimitDistance)) {
    return;
  }
  pointer_location_ = location;
  if (mouse_event.screenX != 0 && mouse_event.screenY != 0) {
    // xcomponent BUG may send error data: screenX and screenY both 0
    cursor_screen_point_.SetPoint(mouse_event.screenX, mouse_event.screenY);
  }
  int flags = pointer_flags_ | key_flags_;
  MouseEvent event(EventType::kMouseMoved, pointer_location_, pointer_location_,
                   EventTimeForNow(), flags, 0);
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
    const OH_NativeXComponent_MouseEvent& mouse_event) {
  EventType type = EventType::kMousePressed;
  if (mouse_event.action == OH_NATIVEXCOMPONENT_MOUSE_PRESS) {
    type = EventType::kMousePressed;
  } else if (mouse_event.action == OH_NATIVEXCOMPONENT_MOUSE_RELEASE) {
    type = EventType::kMouseReleased;
  } else if (mouse_event.action == OH_NATIVEXCOMPONENT_MOUSE_MOVE) {
    OnMouseMoveEvent(widget_id, mouse_event);
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
  EventFlags flags = pointer_flags_ | changed_button | key_flags_;
  gfx::PointF new_pointer_location(mouse_event.x, mouse_event.y);
  MouseEvent event(type, new_pointer_location, pointer_location_,
                   EventTimeForNow(), flags, changed_button);
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

void OhosEventSource::OnTouchEvent(
    const gfx::AcceleratedWidget widget_id,
    const OH_NativeXComponent_TouchEvent& ohos_touch_event,
    const OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type,
    const float tilt_x,
    const float tilt_y) {
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
                                 ohos_touch_event.force, 0.0, tilt_x, tilt_y);

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
  EventType event_type = is_start ? ui::EventType::kScrollFlingStart
                                  : ui::EventType::kScrollFlingCancel;
  // adjust the initial speed of fling
  float velocity_x = ohos_event.velocity_x * kFlingVelocityFactor;
  float velocity_y = ohos_event.velocity_y * kFlingVelocityFactor;
  ScrollEvent fling_event(event_type, pointer_location_, pointer_location_,
                          EventTimeForNow(), event_flags, velocity_x,
                          velocity_y, velocity_x, velocity_y,
                          kTouchpadScrollFingerCount);
  is_fling_active_ = is_start;
  SetTargetAndDispatchEvent(widget_id, fling_event);
}

void OhosEventSource::OnPanEvent(const PanAction action,
                                 const gfx::AcceleratedWidget widget_id,
                                 const PanEvent& ohos_event) {
  EventFlags event_flags = pointer_flags_ | key_flags_;
  if (key_flags_ == EF_NONE) {
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

void OhosEventSource::OnPinchEvent(const std::string& pinch_step,
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
  event_details.set_scale(gesture_event.scale);
  ui::GestureEvent pinch_event(gesture_event.offset_x, gesture_event.offset_y,
                               0, ui::EventTimeForNow(), event_details);
  SetTargetAndDispatchEvent(widget_id, pinch_event);
}

void OhosEventSource::UpdateKeyFlags() {
  key_flags_ = EF_NONE;
}

void OhosEventSource::UpdateKeyFlags(const EventType type,
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
  if (type == EventType::kKeyPressed) {
    key_flags_ = static_cast<int>(key_flags_) | f;
  } else {
    key_flags_ = static_cast<int>(key_flags_) & ~f;
  }
}

gfx::Point OhosEventSource::GetCursorScreenPoint() {
  return gfx::ToFlooredPoint(cursor_screen_point_);
}

void OhosEventSource::OnDragEnterEvent(
    const gfx::AcceleratedWidget widget_id,
    const ohos::adapter::OhosDragInfo& drag_info) {
  pointer_flags_ |= EF_LEFT_MOUSE_BUTTON;

  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]drag enter Event target nullptr";
    return;
  }
  ohos_window->GetDragManager()->DragEnter(drag_info, pointer_location_);
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
    const ohos::adapter::OhosDragInfo& drag_info) {
  OhosWindow* ohos_window = window_manager_->GetWindow((int32_t)widget_id);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]on drop Event target nullptr";
    return;
  }
  ohos_window->GetDragManager()->OnDrop(drag_info, pointer_location_);
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

void OhosEventSource::SimulateLeftButtonUp(
    const gfx::AcceleratedWidget widget_id) {
  OH_NativeXComponent_MouseEvent mouse_event;
  mouse_event.x = pointer_location_.x();
  mouse_event.y = pointer_location_.y();
  mouse_event.action = OH_NATIVEXCOMPONENT_MOUSE_RELEASE;
  OnMouseEvent(widget_id, mouse_event);
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
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnMouseEvent(widget_id, mouse_event);
      },
      widget_id, mouse_event);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
  OhosEventFilter::GetInstance().RefreshMouseEvent(widget_id, mouse_event);
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

  DomCode dom_code = KeycodeConverter::NativeKeycodeToDomCode(key);

  auto task = base::BindOnce(
      [](const int32_t widget_id, const DomCode dom_code,
         const EventType event_type) {
        auto* event_source = reinterpret_cast<OhosEventSource*>(
            PlatformEventSource::GetInstance());
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput] get event source failed";
          return;
        }
        event_source->UpdateKeyFlags(event_type, dom_code);
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
      widget_id, dom_code, type);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void TouchEventCallback(
    const int32_t widget_id,
    const OH_NativeXComponent_TouchEvent& ohos_touch_event,
    const OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type,
    const float tilt_x,
    const float tilt_y) {
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const OH_NativeXComponent_TouchEvent& ohos_touch_event,
         const OH_NativeXComponent_TouchPointToolType
             ohos_touch_point_tool_type,
         const float tilt_x, const float tilt_y) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput]register touch event callback before "
                          "even source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnTouchEvent(widget_id, ohos_touch_event,
                           ohos_touch_point_tool_type, tilt_x, tilt_y);
      },
      widget_id, ohos_touch_event, ohos_touch_point_tool_type, tilt_x, tilt_y);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void PanEventCallback(const PanAction action,
                      const int32_t widget_id,
                      const PanEvent& event) {
  auto task = base::BindOnce(
      [](const PanAction action, const int32_t widget_id,
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
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void PinchEventCallback(const std::string& pinch_step,
                        const int32_t widget_id,
                        const PinchEvent& event) {
  auto task = base::BindOnce(
      [](const std::string& pinch_step, const int32_t widget_id,
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
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void DragEnterEventCallback(const int32_t widget_id,
                            const ohos::adapter::OhosDragInfo& drag_info) {
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const ohos::adapter::OhosDragInfo& drag_info) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput]register drag enter event callback"
                          " before event source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnDragEnterEvent(widget_id, drag_info);
      },
      widget_id, drag_info);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
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
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void DropEventCallback(const int32_t widget_id,
                       const ohos::adapter::OhosDragInfo& drag_info) {
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const ohos::adapter::OhosDragInfo& drag_info) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput]register drop event callback"
                          " before event source created";
          return;
        }
        reinterpret_cast<OhosEventSource*>(event_source)
            ->OnDropEvent(widget_id, drag_info);
      },
      widget_id, drag_info);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
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
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
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

}  // namespace ui
