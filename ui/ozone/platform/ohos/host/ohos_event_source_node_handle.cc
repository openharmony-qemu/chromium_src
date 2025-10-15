// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_event_source_node_handle.h"

#include <ace/xcomponent/native_xcomponent_key_event.h>
#include <arkui/native_key_event.h>
#include <arkui/ui_input_event.h>

#include "base/functional/callback_forward.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "ohos/adapter/task_runner/main_thread_task_runner.h"
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

constexpr float kMouseMoveLimitDistance = 0.01;

void NodeHandleTouchEventCallback(const int32_t widget_id,
                                  const ArkUI_UIInputEvent* touch_event);

void NodeHandleMouseEventCallback(const int32_t widget_id,
                                  const ArkUI_UIInputEvent* mouse_event);

void NodeHandleKeyEventCallback(const int32_t widget_id,
                                const ArkUI_UIInputEvent* key_event);

void NodeHandleMouseHoverEventCallback(const int32_t widget_id,
                                       const bool is_hover);
void NodeHandleDragEnterEventCallback(
    const int32_t widget_id,
    const ohos::adapter::OhosDropData& drop_data);

void NodeHandleDragLeaveEventCallback(const int32_t widget_id);

void NodeHandleDropEventCallback(const int32_t widget_id,
                                 const ohos::adapter::OhosDropData& drop_data);

void NodeHandleDragEndEventCallback(const int32_t widget_id);

void NodeHandleDragMoveEventCallback(const int32_t widget_id,
                                     const float window_x,
                                     const float window_y);

bool InitInputTouchEvent(const ArkUI_UIInputEvent* touch_event,
                         ArkUI_TouchEventData& input_touch_event,
                         int index);

bool CheckTouchEventData(ArkUI_TouchEventData& input_touch_event);

void NodeHandlePanEventCallback(const ArkUI_GestureEventActionType action_type,
                                const int32_t widget_id,
                                const NodeHandlePanEvent& event);

void NodeHandlePinchEventCallback(
    const ArkUI_GestureEventActionType action_type,
    const int32_t widget_id,
    const NodeHandlePinchEvent& event);

void NodeHandleDoubleTapEventCallback(const int32_t widget_id,
                                      const NodeHandleTapEvent& event);

OhosEventSourceNodeHandle::OhosEventSourceNodeHandle(
    OhosWindowManager* window_manager)
    : OhosEventSourceBase(window_manager) {
  event_callback_ = std::make_shared<NodeHandleInputEventCallBack>();
  event_callback_->touchEventCallback = &NodeHandleTouchEventCallback;
  event_callback_->mouseEventCallback = &NodeHandleMouseEventCallback;
  event_callback_->keyEventCallback = &NodeHandleKeyEventCallback;
  event_callback_->mouseHoverEventCallback = &NodeHandleMouseHoverEventCallback;
  event_callback_->dragEnterEventCallback = &NodeHandleDragEnterEventCallback;
  event_callback_->dragLeaveEventCallback = &NodeHandleDragLeaveEventCallback;
  event_callback_->dragMoveEventCallback = &NodeHandleDragMoveEventCallback;
  event_callback_->dropEventCallback = &NodeHandleDropEventCallback;
  event_callback_->dragEndEventCallback = &NodeHandleDragEndEventCallback;
  event_callback_->panEventCallback = &NodeHandlePanEventCallback;
  event_callback_->pinchEventCallback = &NodeHandlePinchEventCallback;
  event_callback_->doubleTapEventCallback = &NodeHandleDoubleTapEventCallback;
}

void OhosEventSourceNodeHandle::OnWindowAdded(OhosWindow* window) {
  DCHECK(window);

  auto task = std::bind(&XComponentManager::RegisterNodeHandleInputEventCallBack,
                        XComponentManager::GetInstance(),
                        window->GetWidget(),
                        event_callback_);
  ohos::adapter::taskRunner::MainThreadTaskRunner::GetInstance().PostTask(task);
}

void OhosEventSourceNodeHandle::OnWindowRemoved(OhosWindow* window) {
  DCHECK(window);
}

void OhosEventSourceNodeHandle::OnTouchEvent(
    const gfx::AcceleratedWidget widget_id,
    const ArkUI_TouchEventData& touch_event_data) {
  EventType type = GetTouchAction(touch_event_data.touch_action);
  if (type == ET_UNKNOWN) {
    LOG(ERROR) << "OhosEventSourceNodeHandle::OnTouchEvent,unknow touch action";
    return;
  }
  cursor_screen_point_.SetPoint(touch_event_data.display_x,
                                touch_event_data.display_y);
  EventPointerType pointer_type = GetPointType(touch_event_data.tool_type);
  PointerDetails pointer_details(
      pointer_type, touch_event_data.id, 0.0, 0.0, touch_event_data.force, 0.0,
      touch_event_data.tilt_x, touch_event_data.tilt_y);
  gfx::Point touch_location(touch_event_data.x, touch_event_data.y);
  TouchEvent event(type, touch_location, EventTimeForNow(), pointer_details);
  SetTargetAndDispatchEvent(widget_id, event);
}

EventType OhosEventSourceNodeHandle::GetTouchAction(
    const int32_t touch_action) {
  EventType type = ET_UNKNOWN;
  switch (touch_action) {
    case UI_TOUCH_EVENT_ACTION_DOWN:
      type = ET_TOUCH_PRESSED;
      break;
    case UI_TOUCH_EVENT_ACTION_UP:
      type = ET_TOUCH_RELEASED;
      break;
    case UI_TOUCH_EVENT_ACTION_MOVE:
      type = ET_TOUCH_MOVED;
      break;
    case UI_TOUCH_EVENT_ACTION_CANCEL:
      type = ET_TOUCH_CANCELLED;
      break;
    default:
      type = ET_UNKNOWN;
      LOG(ERROR)
          << "OhosEventSourceNodeHandle::OnTouchEvent,unknow touch action";
  }
  return type;
}

EventPointerType OhosEventSourceNodeHandle::GetPointType(
    const int32_t tool_type) {
  EventPointerType pointer_type = EventPointerType::kUnknown;
  switch (tool_type) {
    case UI_INPUT_EVENT_TOOL_TYPE_FINGER:
      pointer_type = EventPointerType::kTouch;
      break;
    case UI_INPUT_EVENT_TOOL_TYPE_PEN:
      pointer_type = EventPointerType::kPen;
      break;
    case UI_INPUT_EVENT_TOOL_TYPE_TOUCHPAD:
    case UI_INPUT_EVENT_TOOL_TYPE_MOUSE:
      pointer_type = EventPointerType::kMouse;
      break;
    default:
      pointer_type = EventPointerType::kTouch;
      break;
  }
  return pointer_type;
}

void OhosEventSourceNodeHandle::OnMouseMoveEvent(
    const gfx::AcceleratedWidget widget_id,
    const ArkUI_MouseEventData& mouse_event_data,
    const gfx::PointF& original_location) {
  OhosWindow* ohos_window =
      window_manager_->GetWindow(static_cast<int32_t>(widget_id));
  if (ohos_window == nullptr) {
    LOG(WARNING) << "[multiinput]mouse move target window nullptr";
    return;
  }

  if (pointer_location_.IsWithinDistance(original_location,
                                         kMouseMoveLimitDistance)) {
    return;
  }
  if (mouse_event_data.screenX != 0 && mouse_event_data.screenY != 0) {
    // xcomponent BUG may send error data: screenX and screenY both 0
    cursor_screen_point_.SetPoint(mouse_event_data.screenX,
                                  mouse_event_data.screenY);
  }
  int flags = pointer_flags_ | key_flags_;
  MouseEvent event(ET_MOUSE_MOVED, pointer_location_, pointer_location_,
                   EventTimeForNow(), flags, 0);
  SetTargetAndDispatchEvent(widget_id, event);

  // Determine if the drag event needs to be ended if needed
  EndSourceDragIfNeeded();
}

void OhosEventSourceNodeHandle::OnMouseEvent(
    const gfx::AcceleratedWidget widget_id,
    const ArkUI_MouseEventData& mouse_event_data) {
  EventType type = ET_MOUSE_PRESSED;
  gfx::PointF original_pointer_location = pointer_location_;
  pointer_location_.SetPoint(mouse_event_data.x, mouse_event_data.y);
  if (mouse_event_data.action == UI_MOUSE_EVENT_ACTION_PRESS) {
    type = ET_MOUSE_PRESSED;
  } else if (mouse_event_data.action == UI_MOUSE_EVENT_ACTION_RELEASE) {
    type = ET_MOUSE_RELEASED;
  } else if (mouse_event_data.action == UI_MOUSE_EVENT_ACTION_MOVE) {
    OnMouseMoveEvent(widget_id, mouse_event_data, original_pointer_location);
    return;
  } else {
    LOG(WARNING) << "[multiinput]Unhandle action";
    return;
  }
  int changed_button = 0;
  switch (mouse_event_data.button) {
    case UI_MOUSE_EVENT_BUTTON_LEFT:
      changed_button = EF_LEFT_MOUSE_BUTTON;
      break;
    case UI_MOUSE_EVENT_BUTTON_RIGHT:
      changed_button = EF_RIGHT_MOUSE_BUTTON;
      break;
    case UI_MOUSE_EVENT_BUTTON_MIDDLE:
      changed_button = EF_MIDDLE_MOUSE_BUTTON;
      break;
    case UI_MOUSE_EVENT_BUTTON_BACK:
      changed_button = EF_BACK_MOUSE_BUTTON;
      break;
    case UI_MOUSE_EVENT_BUTTON_FORWARD:
      changed_button = EF_FORWARD_MOUSE_BUTTON;
      break;
    default:
      LOG(WARNING) << "[multiinput]Unhandle button";
      break;
  }
  pointer_flags_ = type == ET_MOUSE_PRESSED
                       ? (pointer_flags_ | changed_button)
                       : (pointer_flags_ & ~changed_button);
  EventFlags flags = pointer_flags_ | changed_button | key_flags_;
  MouseEvent event(type, pointer_location_, original_pointer_location,
                   EventTimeForNow(), flags, changed_button);
  SetTargetAndDispatchEvent(widget_id, event);
}

void OhosEventSourceNodeHandle::SimulateLeftButtonUp(
    const gfx::AcceleratedWidget widget_id) {
  ArkUI_MouseEventData mouse_event_data;
  mouse_event_data.x = pointer_location_.x();
  mouse_event_data.y = pointer_location_.y();
  mouse_event_data.action = UI_MOUSE_EVENT_ACTION_RELEASE;
  OnMouseEvent(widget_id, mouse_event_data);
}

void NodeHandleTouchEventCallback(const int32_t widget_id,
                                  const ArkUI_UIInputEvent* touch_event) {
  int32_t point_num = 0;
  point_num = OH_ArkUI_PointerEvent_GetPointerCount(touch_event);
  for (int index = 0; index < point_num; index++) {
    ArkUI_TouchEventData input_touch_event;
    bool init_result =
        InitInputTouchEvent(touch_event, input_touch_event, index);
    if (init_result == false) {
      LOG(ERROR) << "NodeHandleTouchEventCallback, TouchEvent data abnormal";
      return;
    }

    auto task = base::BindOnce(
        [](const int32_t widget_id,
           const ArkUI_TouchEventData& input_touch_event) {
          auto* event_source = PlatformEventSource::GetInstance();
          if (event_source == nullptr) {
            LOG(WARNING) << "[multiinput]register touch event callback before "
                            "even source created";
            return;
          }
          static_cast<OhosEventSourceNodeHandle*>(event_source)
              ->OnTouchEvent(widget_id, input_touch_event);
        },
        widget_id, std::move(input_touch_event));
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                            std::move(task));
  }
}

bool InitInputTouchEvent(const ArkUI_UIInputEvent* touch_event,
                         ArkUI_TouchEventData& input_touch_event,
                         int index) {
  input_touch_event.tool_type = OH_ArkUI_UIInputEvent_GetToolType(touch_event);
  input_touch_event.tilt_x = OH_ArkUI_PointerEvent_GetTiltX(touch_event, index);
  input_touch_event.tilt_y = OH_ArkUI_PointerEvent_GetTiltY(touch_event, index);
  input_touch_event.display_x =
      OH_ArkUI_PointerEvent_GetDisplayXByIndex(touch_event, index);
  input_touch_event.display_y =
      OH_ArkUI_PointerEvent_GetDisplayYByIndex(touch_event, index);
  input_touch_event.x =
      OH_ArkUI_PointerEvent_GetWindowXByIndex(touch_event, index);
  input_touch_event.y =
      OH_ArkUI_PointerEvent_GetWindowYByIndex(touch_event, index);
  input_touch_event.id = OH_ArkUI_PointerEvent_GetPointerId(touch_event, index);
  input_touch_event.force =
      OH_ArkUI_PointerEvent_GetPressure(touch_event, index);
  input_touch_event.touch_action = OH_ArkUI_UIInputEvent_GetAction(touch_event);
  return CheckTouchEventData(input_touch_event);
}

bool CheckTouchEventData(ArkUI_TouchEventData& input_touch_event) {
  bool check_result = true;
  if (input_touch_event.display_x == 0.0f ||
      input_touch_event.display_y == 0.0f) {
    LOG(ERROR) << "CheckTouchEventData, OH_ArkUI_UIInputEvent get display "
                  "location wrong";
    check_result = false;
  }
  if (input_touch_event.x == 0.0f || input_touch_event.y == 0.0f) {
    LOG(ERROR) << "CheckTouchEventData, OH_ArkUI_UIInputEvent get location int "
                  "window wrong";
    check_result = false;
  }
  if (input_touch_event.force == 0.0f) {
    LOG(ERROR)
        << "CheckTouchEventData, OH_ArkUI_UIInputEvent get pressure wrong";
    check_result = false;
  }
  return check_result;
}

void OhosEventSourceNodeHandle::OnPanEvent(
    const ArkUI_GestureEventActionType action_type,
    const gfx::AcceleratedWidget widget_id,
    const NodeHandlePanEvent& ohos_event) {
  EventFlags event_flags = pointer_flags_ | key_flags_;
  // Set EF_PRECISION_SCROLLING_DELTA only for events originating from touchpad
  // devices.
  if (key_flags_ == EF_NONE &&
      ohos_event.source_tool() == UI_INPUT_EVENT_TOOL_TYPE_TOUCHPAD) {
    event_flags |= EF_PRECISION_SCROLLING_DELTA;
  }
  if (is_fling_active_) {
    CreateAndDispatchFlingEvent(widget_id, ohos_event, event_flags, false);
  }
  if (action_type == GESTURE_EVENT_ACTION_END) {
    CreateAndDispatchFlingEvent(widget_id, ohos_event, event_flags, true);
  }

  if (action_type != GESTURE_EVENT_ACTION_UPDATE) {
    mouse_wheel_offset_.set_x(0.0);
    mouse_wheel_offset_.set_y(0.0);
    return;
  }
  ui::MouseWheelEvent event(
      gfx::Vector2d(ohos_event.offset_x() - mouse_wheel_offset_.x(),
                    ohos_event.offset_y() - mouse_wheel_offset_.y()),
      pointer_location_, pointer_location_, EventTimeForNow(), event_flags, 0);
  mouse_wheel_offset_.SetPoint(ohos_event.offset_x(), ohos_event.offset_y());
  event.InitializeNative();
  SetTargetAndDispatchEvent(widget_id, event);
}

void OhosEventSourceNodeHandle::OnPinchEvent(
    const ArkUI_GestureEventActionType action_type,
    const gfx::AcceleratedWidget widget_id,
    const NodeHandlePinchEvent& gesture_event) {
  ui::GestureEventDetails event_details;
  if (action_type == GESTURE_EVENT_ACTION_ACCEPT) {
    event_details = ui::GestureEventDetails(ui::ET_GESTURE_PINCH_BEGIN);
  } else if (action_type == GESTURE_EVENT_ACTION_UPDATE) {
    event_details = ui::GestureEventDetails(ui::ET_GESTURE_PINCH_UPDATE);
  } else if (action_type == GESTURE_EVENT_ACTION_END) {
    event_details = ui::GestureEventDetails(ui::ET_GESTURE_PINCH_END);
  } else {
    LOG(WARNING) << "[pinch-event]Unhandle action:" << action_type;
    return;
  }
  event_details.set_device_type(ui::GestureDeviceType::DEVICE_TOUCHPAD);
  event_details.set_scale(
      NearZero(last_scale_) ? 1.0f : gesture_event.scale() / last_scale_);
  last_scale_ = gesture_event.scale();

  ui::GestureEvent pinch_event(gesture_event.offset_x(),
                               gesture_event.offset_y(), 0,
                               ui::EventTimeForNow(), event_details);
  SetTargetAndDispatchEvent(widget_id, pinch_event);
}

void OhosEventSourceNodeHandle::OnDoubleTapEvent(
    const gfx::AcceleratedWidget widget_id,
    const NodeHandleTapEvent& event) {
  ui::GestureEventDetails event_details(ui::ET_GESTURE_DOUBLE_TAP,
                                        event.offset_x(), event.offset_y());
  event_details.set_tap_count(event.tap_count());
  ui::GestureEvent tap_event(event.offset_x(), event.offset_y(), key_flags_,
                             ui::EventTimeForNow(), event_details);
  SetTargetAndDispatchEvent(widget_id, tap_event);
}

void NodeHandleMouseEventCallback(const int32_t widget_id,
                                  const ArkUI_UIInputEvent* mouse_event) {
  auto timestamp = OH_ArkUI_UIInputEvent_GetEventTime(mouse_event);
  auto action = OH_ArkUI_UIInputEvent_GetAction(mouse_event);
  if (OhosEventFilter::GetInstance().CheckFilterMouseEvent(widget_id, timestamp,
                                                           action)) {
    DLOG(ERROR) << "mouse event is filtered,widget_id:" << widget_id
                << ",action:" << action;
    return;
  }

  ArkUI_MouseEventData mouse_event_data;
  mouse_event_data.x = OH_ArkUI_PointerEvent_GetX(mouse_event);
  mouse_event_data.y = OH_ArkUI_PointerEvent_GetY(mouse_event);
  mouse_event_data.screenX = OH_ArkUI_PointerEvent_GetDisplayX(mouse_event);
  mouse_event_data.screenY = OH_ArkUI_PointerEvent_GetDisplayY(mouse_event);
  mouse_event_data.button = OH_ArkUI_MouseEvent_GetMouseButton(mouse_event);
  mouse_event_data.timestamp = timestamp;
  mouse_event_data.action = action;

  auto task = base::BindOnce(
      [](const int32_t widget_id, const ArkUI_MouseEventData mouse_event_data) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]"
              << "register mouse event callback before even source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnMouseEvent(widget_id, mouse_event_data);
      },
      widget_id, mouse_event_data);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
  OhosEventFilter::GetInstance().RefreshMouseEvent(
      widget_id, mouse_event_data.timestamp, mouse_event_data.action);
}

void NodeHandleKeyEventCallback(const int32_t widget_id,
                                const ArkUI_UIInputEvent* key_event) {
  ArkUI_KeyEventType key_event_type = OH_ArkUI_KeyEvent_GetType(key_event);
  EventType type = ET_UNKNOWN;
  switch (key_event_type) {
    case ARKUI_KEY_EVENT_DOWN:
      type = ET_KEY_PRESSED;
      break;
    case ARKUI_KEY_EVENT_UP:
      type = ET_KEY_RELEASED;
      break;
    default:
      type = ET_UNKNOWN;
      break;
  }

  ArkUI_KeyCode key = ARKUI_KEYCODE_UNKNOWN;
  key = static_cast<ArkUI_KeyCode>(OH_ArkUI_KeyEvent_GetKeyCode(key_event));
  if (key == ARKUI_KEYCODE_UNKNOWN) {
    LOG(WARNING) << "[multiinput]get key code is unknow";
    return;
  }

  DomCode dom_code = KeycodeConverter::NativeKeycodeToDomCode(key);

  auto task = base::BindOnce(
      [](const int32_t widget_id, const DomCode dom_code,
         const EventType event_type) {
        auto* event_source = reinterpret_cast<OhosEventSourceNodeHandle*>(
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

void NodeHandleMouseHoverEventCallback(const int32_t widget_id,
                                       const bool is_hover) {
  auto task = base::BindOnce(
      [](const int32_t widget_id, const bool is_hover) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput]register mouse hover event callback"
                          " before event source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnMouseHoverEvent(widget_id, is_hover);
      },
      widget_id, is_hover);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void NodeHandleDragEnterEventCallback(
    const int32_t widget_id,
    const ohos::adapter::OhosDropData& drop_data) {
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const ohos::adapter::OhosDropData& drop_data) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput]register drag enter event callback"
                          " before event source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnDragEnterEvent(widget_id, drop_data);
      },
      widget_id, drop_data);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void NodeHandleDragMoveEventCallback(const int32_t widget_id,
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
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnDragMoveEvent(widget_id, window_x, window_y);
      },
      widget_id, window_x, window_y);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void NodeHandleDragLeaveEventCallback(const int32_t widget_id) {
  auto task = base::BindOnce(
      [](const int32_t widget_id) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]register drag leave event callback before even "
                 "source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnDragLeaveEvent(widget_id);
      },
      widget_id);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void NodeHandleDropEventCallback(const int32_t widget_id,
                                 const ohos::adapter::OhosDropData& drop_data) {
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const ohos::adapter::OhosDropData& drop_data) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput]register drop event callback"
                          " before event source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnDropEvent(widget_id, drop_data);
      },
      widget_id, drop_data);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void NodeHandleDragEndEventCallback(const int32_t widget_id) {
  auto task = base::BindOnce(
      [](const int32_t widget_id) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]register drag end event callback before even "
                 "source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnDragEndEvent(widget_id);
      },
      widget_id);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void NodeHandlePanEventCallback(const ArkUI_GestureEventActionType action_type,
                                const int32_t widget_id,
                                const NodeHandlePanEvent& event) {
  auto task = base::BindOnce(
      [](const ArkUI_GestureEventActionType action_type,
         const int32_t widget_id, const NodeHandlePanEvent& event) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]register mouse wheel event callback before even "
                 "source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnPanEvent(action_type, widget_id, event);
      },
      action_type, widget_id, event);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void NodeHandlePinchEventCallback(
    const ArkUI_GestureEventActionType action_type,
    const int32_t widget_id,
    const NodeHandlePinchEvent& event) {
  auto task = base::BindOnce(
      [](const ArkUI_GestureEventActionType action_type,
         const int32_t widget_id, const NodeHandlePinchEvent& event) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[pinch-event]register pinch event callback before even "
                 "source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnPinchEvent(action_type, widget_id, event);
      },
      action_type, widget_id, event);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void NodeHandleDoubleTapEventCallback(const int32_t widget_id,
                                      const NodeHandleTapEvent& event) {
  auto task = base::BindOnce(
      [](const int32_t widget_id, const NodeHandleTapEvent& event) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[double-tap-event]register double tap event callback "
                 "before event source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnDoubleTapEvent(widget_id, event);
      },
      widget_id, event);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

void OhosEventSourceNodeHandle::CreateAndDispatchFlingEvent(
    const gfx::AcceleratedWidget widget_id,
    const NodeHandlePanEvent& ohos_event,
    const EventFlags& event_flags,
    const bool is_start) {
  if (ohos_event.source_tool() != UI_INPUT_EVENT_TOOL_TYPE_TOUCHPAD) {
    return;
  }
  EventType event_type = is_start ? ui::EventType::ET_SCROLL_FLING_START
                                  : ui::EventType::ET_SCROLL_FLING_CANCEL;
  // adjust the initial speed of fling
  float velocity_x = ohos_event.velocity_x() * kFlingVelocityFactor;
  float velocity_y = ohos_event.velocity_y() * kFlingVelocityFactor;
  ScrollEvent fling_event(event_type, pointer_location_, pointer_location_,
                          EventTimeForNow(), event_flags, velocity_x,
                          velocity_y, velocity_x, velocity_y,
                          kTouchpadScrollFingerCount);
  is_fling_active_ = is_start;
  SetTargetAndDispatchEvent(widget_id, fling_event);
}

}  // namespace ui
