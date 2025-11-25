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

const static std::unordered_map<ArkUI_KeyCode, ArkUI_KeyCode> kNumLockOffMap = {
    {ARKUI_KEYCODE_NUMPAD_0, ARKUI_KEYCODE_INSERT},
    {ARKUI_KEYCODE_NUMPAD_1, ARKUI_KEYCODE_MOVE_END},
    {ARKUI_KEYCODE_NUMPAD_2, ARKUI_KEYCODE_DPAD_DOWN},
    {ARKUI_KEYCODE_NUMPAD_3, ARKUI_KEYCODE_PAGE_DOWN},
    {ARKUI_KEYCODE_NUMPAD_4, ARKUI_KEYCODE_DPAD_LEFT},
    {ARKUI_KEYCODE_NUMPAD_5, ARKUI_KEYCODE_UNKNOWN},
    {ARKUI_KEYCODE_NUMPAD_6, ARKUI_KEYCODE_DPAD_RIGHT},
    {ARKUI_KEYCODE_NUMPAD_7, ARKUI_KEYCODE_MOVE_HOME},
    {ARKUI_KEYCODE_NUMPAD_8, ARKUI_KEYCODE_DPAD_UP},
    {ARKUI_KEYCODE_NUMPAD_9, ARKUI_KEYCODE_PAGE_UP},
    {ARKUI_KEYCODE_NUMPAD_DOT, ARKUI_KEYCODE_FORWARD_DEL}};

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

void InitInputTouchEvent(const ArkUI_UIInputEvent* touch_event,
                         NodeHandleTouchEventData& input_touch_event,
                         int index);

bool CheckTouchEventData(NodeHandleTouchEventData& input_touch_event);

void NodeHandlePanEventCallback(const ArkUI_GestureEventActionType action_type,
                                const int32_t widget_id,
                                const NodeHandlePanEvent& event);

void NodeHandlePinchEventCallback(
    const ArkUI_GestureEventActionType action_type,
    const int32_t widget_id,
    const NodeHandlePinchEvent& event);

void NodeHandleSendWindowMouseEventForTabDragCallback(
    const int32_t widget_id,
    Input_MouseEvent* window_mouse_event);

void NodeHandleSendWindowTouchEventForTabDragCallback(
    const int32_t widget_id,
    Input_TouchEvent* window_touch_event);

OhosEventSourceNodeHandle::OhosEventSourceNodeHandle(
    OhosWindowManager* window_manager,
    OhosWindowDragManager* window_drag_manager)
    : OhosEventSourceBase(window_manager, window_drag_manager) {
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
  event_callback_->sendWindowMouseEventForTabDragCallback =
      &NodeHandleSendWindowMouseEventForTabDragCallback;
  event_callback_->sendWindowTouchEventForTabDragCallback =
      &NodeHandleSendWindowTouchEventForTabDragCallback;
}

void OhosEventSourceNodeHandle::OnWindowAdded(OhosWindow* window) {
  DCHECK(window);

  auto task = std::bind(
      &XComponentManager::RegisterNodeHandleInputEventCallBack,
      XComponentManager::GetInstance(), window->GetWidget(), event_callback_);
  ohos::adapter::taskRunner::MainThreadTaskRunner::GetInstance().PostTask(task);
}

void OhosEventSourceNodeHandle::OnWindowRemoved(OhosWindow* window) {
  DCHECK(window);
}

void OhosEventSourceNodeHandle::OnTouchEvent(
    const gfx::AcceleratedWidget widget_id,
    const NodeHandleTouchEventData & touch_event_data,
    const int32_t display_id) {
  EventType type = GetTouchAction(touch_event_data.touch_action);
  gfx::Point touch_location(touch_event_data.x, touch_event_data.y);
  cursor_screen_point_.SetPoint(touch_event_data.display_x,
                                touch_event_data.display_y);
  EventPointerType pointer_type = GetPointType(touch_event_data.tool_type);
  PointerDetails pointer_details(
      pointer_type, touch_event_data.id, 0.0, 0.0, touch_event_data.force, 0.0,
      touch_event_data.tilt_x, touch_event_data.tilt_y);
  TouchEvent event(type, touch_location, EventTimeForNow(), pointer_details);
  event.set_display_id(display_id);
  SetTargetAndDispatchEvent(widget_id, event);
}

EventType OhosEventSourceNodeHandle::GetTouchAction(
    const int32_t touch_action) {
  EventType type = EventType::kUnknown;
  switch (touch_action) {
    case UI_TOUCH_EVENT_ACTION_DOWN:
      type = EventType::kTouchPressed;
      break;
    case UI_TOUCH_EVENT_ACTION_UP:
      type = EventType::kTouchReleased;
      break;
    case UI_TOUCH_EVENT_ACTION_MOVE:
      type = EventType::kTouchMoved;
      break;
    case UI_TOUCH_EVENT_ACTION_CANCEL:
      type = EventType::kTouchCancelled;
      break;
    default:
      type = EventType::kUnknown;
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
    const NodeHandleMouseEventData& mouse_event_data,
    const gfx::PointF& original_location,
    const int32_t display_id) {
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
  if (mouse_event_data.screenX != 0 || mouse_event_data.screenY != 0) {
    // xcomponent BUG may send error data: screenX and screenY both 0
    cursor_screen_point_.SetPoint(mouse_event_data.screenX,
                                  mouse_event_data.screenY);
  }
  int flags = pointer_flags_ | key_flags_;
  MouseEvent event(EventType::kMouseMoved, pointer_location_, pointer_location_,
                   EventTimeForNow(), flags, 0);
  event.set_display_id(display_id);
  SetTargetAndDispatchEvent(widget_id, event);

  // Determine if the drag event needs to be ended if needed
  EndSourceDragIfNeeded();
}

void OhosEventSourceNodeHandle::OnMouseEvent(
    const gfx::AcceleratedWidget widget_id,
    const NodeHandleMouseEventData& mouse_event_data,
    const int32_t display_id,
    const EventFlags key_flags) {
  EventType type = EventType::kMousePressed;
  gfx::PointF original_pointer_location = pointer_location_;
  pointer_location_.SetPoint(mouse_event_data.x, mouse_event_data.y);
  if (mouse_event_data.action == UI_MOUSE_EVENT_ACTION_PRESS) {
    type = EventType::kMousePressed;
  } else if (mouse_event_data.action == UI_MOUSE_EVENT_ACTION_RELEASE) {
    type = EventType::kMouseReleased;
  } else if (mouse_event_data.action == UI_MOUSE_EVENT_ACTION_MOVE) {
    OnMouseMoveEvent(widget_id, mouse_event_data, original_pointer_location,
                     display_id);
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
  pointer_flags_ = type == EventType::kMousePressed
                       ? (pointer_flags_ | changed_button)
                       : (pointer_flags_ & ~changed_button);
  UpdateKeyFlags(key_flags);
  EventFlags flags = pointer_flags_ | changed_button | key_flags_;
  MouseEvent event(type, pointer_location_, original_pointer_location,
                   EventTimeForNow(), flags, changed_button);
  event.set_display_id(display_id);
  SetTargetAndDispatchEvent(widget_id, event);
}

void OhosEventSourceNodeHandle::SimulateTouchUp(
    const gfx::AcceleratedWidget widget_id) {
  NodeHandleTouchEventData  touch_event_data;
  touch_event_data.id = kSimulateTouchEventId;
  touch_event_data.display_x = cursor_screen_point_.x();
  touch_event_data.display_y = cursor_screen_point_.y();
  touch_event_data.touch_action = UI_TOUCH_EVENT_ACTION_UP;
  touch_event_data.tool_type = UI_INPUT_EVENT_TOOL_TYPE_FINGER;
  OnTouchEvent(widget_id, touch_event_data, display::kInvalidDisplayId);
}

void NodeHandleTouchEventCallback(const int32_t widget_id,
                                  const ArkUI_UIInputEvent* touch_event) {
  int32_t point_num = 0;
  point_num = OH_ArkUI_PointerEvent_GetPointerCount(touch_event);
  for (int index = 0; index < point_num; index++) {
    NodeHandleTouchEventData input_touch_event;
    InitInputTouchEvent(touch_event, input_touch_event, index);
    int32_t display_id = OH_ArkUI_UIInputEvent_GetTargetDisplayId(touch_event);
    auto task = base::BindOnce(
        [](const int32_t widget_id,
           const NodeHandleTouchEventData& input_touch_event,
           int32_t display_id) {
          auto* event_source = PlatformEventSource::GetInstance();
          if (event_source == nullptr) {
            LOG(WARNING) << "[multiinput]register touch event callback before "
                            "even source created";
            return;
          }
          static_cast<OhosEventSourceNodeHandle*>(event_source)
              ->OnTouchEvent(widget_id, input_touch_event, display_id);
        },
        widget_id, std::move(input_touch_event), display_id);
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                            std::move(task));
  }
}

void InitInputTouchEvent(const ArkUI_UIInputEvent* touch_event,
                         NodeHandleTouchEventData & input_touch_event,
                         int index) {
  input_touch_event.tool_type = OH_ArkUI_UIInputEvent_GetToolType(touch_event);
  input_touch_event.tilt_x = OH_ArkUI_PointerEvent_GetTiltX(touch_event, index);
  input_touch_event.tilt_y = OH_ArkUI_PointerEvent_GetTiltY(touch_event, index);
  input_touch_event.display_x =
      OH_ArkUI_PointerEvent_GetDisplayXByIndex(touch_event, index);
  input_touch_event.display_y =
      OH_ArkUI_PointerEvent_GetDisplayYByIndex(touch_event, index);
  input_touch_event.x = OH_ArkUI_PointerEvent_GetXByIndex(touch_event, index);
  input_touch_event.y = OH_ArkUI_PointerEvent_GetYByIndex(touch_event, index);
  input_touch_event.id = OH_ArkUI_PointerEvent_GetPointerId(touch_event, index);
  input_touch_event.force =
      OH_ArkUI_PointerEvent_GetPressure(touch_event, index);
  input_touch_event.touch_action = OH_ArkUI_UIInputEvent_GetAction(touch_event);
}

void OhosEventSourceNodeHandle::OnPanEvent(
    const ArkUI_GestureEventActionType action_type,
    const gfx::AcceleratedWidget widget_id,
    const NodeHandlePanEvent& ohos_event) {
  EventFlags event_flags = pointer_flags_ | key_flags_;
  // Set EF_PRECISION_SCROLLING_DELTA only for events originating from touchpad
  // devices.
  if (key_flags_ == EF_NONE &&
      ohos_event.tool_type() == UI_INPUT_EVENT_TOOL_TYPE_TOUCHPAD) {
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
    event_details = ui::GestureEventDetails(ui::EventType::kGesturePinchBegin);
  } else if (action_type == GESTURE_EVENT_ACTION_UPDATE) {
    event_details = ui::GestureEventDetails(ui::EventType::kGesturePinchUpdate);
  } else if (action_type == GESTURE_EVENT_ACTION_END) {
    event_details = ui::GestureEventDetails(ui::EventType::kGesturePinchEnd);
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

void OhosEventSourceNodeHandle::SendWindowMouseEventForTabDragNodeHandle(
    const gfx::AcceleratedWidget widget_id,
    std::shared_ptr<NodeHandleMouseEventData> mouse_event_data,
    const int32_t display_id,
    const EventFlags key_flags) {
  if (mouse_event_data == nullptr) {
    LOG(ERROR) << "[OhosTabDragNodeHandle] " << __FUNCTION__
               << ", mouse_event_data is null";
    return;
  }
  if (window_drag_manager_->NeedSendWindowEventToUi(
          widget_id, mouse_event_data->action)) {
    // Initialize internal coordinates of the component
    PrepareXcomponentPointForMouseEvent(widget_id, mouse_event_data,
                                        mouse_event_data->screenX,
                                        mouse_event_data->screenY);
    // When the tab page drag is complete, the tab page drag parameter is
    // cleared
    if (mouse_event_data->action == UI_MOUSE_EVENT_ACTION_RELEASE) {
      LOG(WARNING) << "[OhosTabDragNodeHandle] " << __FUNCTION__
                   << " tab dragging is ended, widget_id:" << widget_id;
      EndTabDragging();
    }
    OnMouseEvent(widget_id, *mouse_event_data, display_id, key_flags);
  }
}

void OhosEventSourceNodeHandle::SendWindowTouchEventForTabDragNodeHandle(
    const gfx::AcceleratedWidget widget_id,
    std::shared_ptr<NodeHandleTouchEventData > touch_event_data,
    const int32_t display_id) {
  if (touch_event_data == nullptr) {
    LOG(ERROR) << "[OhosTabDragNodeHandle] " << __FUNCTION__
               << ", touch_event_data is null";
    return;
  }
  if (window_drag_manager_->NeedSendWindowEventToUi(
          widget_id, touch_event_data->touch_action, touch_event_data->id)) {
    // Initialize internal coordinates of the component
    PrepareXcomponentPointForTouchEvent(widget_id, touch_event_data,
                                        touch_event_data->display_x,
                                        touch_event_data->display_y);
    // When the tab page drag is complete, the tab page drag parameter is
    // cleared
    if (touch_event_data->touch_action == UI_TOUCH_EVENT_ACTION_UP) {
      LOG(WARNING) << "[OhosTabDragNodeHandle] " << __FUNCTION__
                   << " tab dragging is ended"
                      ", widget_id:"
                   << widget_id;
      EndTabDragging();
    }
    OnTouchEvent(widget_id, *touch_event_data, display_id);
  }
}

void OhosEventSourceNodeHandle::PrepareXcomponentPointForTouchEvent(
    const gfx::AcceleratedWidget widget_id,
    std::shared_ptr<NodeHandleTouchEventData > touch_event_data,
    const float display_x,
    const float display_y) {
  OhosWindow* ohos_window = window_manager_->GetWindow(widget_id);
  if (ohos_window) {
    gfx::Rect window_bounds = ohos_window->GetBoundsInPixels();
    touch_event_data->x = touch_event_data->display_x - window_bounds.x();
    touch_event_data->y = touch_event_data->display_y - window_bounds.y();
  }
}
 
void OhosEventSourceNodeHandle::PrepareXcomponentPointForMouseEvent(
    const gfx::AcceleratedWidget widget_id,
    std::shared_ptr<NodeHandleMouseEventData> mouse_event_data,
    const float display_x,
    const float display_y) {
  OhosWindow* ohos_window = window_manager_->GetWindow(widget_id);
  if (ohos_window) {
    gfx::Rect window_bounds = ohos_window->GetBoundsInPixels();
    mouse_event_data->x = mouse_event_data->screenX - window_bounds.x();
    mouse_event_data->y = mouse_event_data->screenY - window_bounds.y();
  }
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

  EventFlags key_flags = EF_NONE;
  if (action != UI_MOUSE_EVENT_ACTION_MOVE) {
    OhosEventSourceBase::UpdateKeyFlagsByOhKeyState(key_flags);
  }

  NodeHandleMouseEventData mouse_event_data;
  mouse_event_data.x = OH_ArkUI_PointerEvent_GetX(mouse_event);
  mouse_event_data.y = OH_ArkUI_PointerEvent_GetY(mouse_event);
  mouse_event_data.screenX = OH_ArkUI_PointerEvent_GetDisplayX(mouse_event);
  mouse_event_data.screenY = OH_ArkUI_PointerEvent_GetDisplayY(mouse_event);
  mouse_event_data.button = OH_ArkUI_MouseEvent_GetMouseButton(mouse_event);
  mouse_event_data.timestamp = timestamp;
  mouse_event_data.action = action;

  int32_t display_id = OH_ArkUI_UIInputEvent_GetTargetDisplayId(mouse_event);
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const NodeHandleMouseEventData mouse_event_data, int32_t display_id,
         const EventFlags key_flags) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING)
              << "[multiinput]"
              << "register mouse event callback before even source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->OnMouseEvent(widget_id, mouse_event_data, display_id, key_flags);
      },
      widget_id, mouse_event_data, display_id, key_flags);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
  OhosEventFilter::GetInstance().RefreshMouseEvent(
      widget_id, mouse_event_data.timestamp, mouse_event_data.action);
}

ArkUI_KeyCode GetKeyWithNumLockOFF(ArkUI_KeyCode key, int flags) {
  if ((flags & EF_NUM_LOCK_ON) != EF_NUM_LOCK_ON) {
    auto iter = kNumLockOffMap.find(key);
    return (iter != kNumLockOffMap.end()) ? iter->second : key;
  }
  return key;
}

void NodeHandleKeyEventCallback(const int32_t widget_id,
                                const ArkUI_UIInputEvent* key_event) {
  ArkUI_KeyEventType key_event_type = OH_ArkUI_KeyEvent_GetType(key_event);
  EventType type = EventType::kUnknown;
  switch (key_event_type) {
    case ARKUI_KEY_EVENT_DOWN:
      type = EventType::kKeyPressed;
      break;
    case ARKUI_KEY_EVENT_UP:
      type = EventType::kKeyReleased;
      break;
    default:
      type = EventType::kUnknown;
      break;
  }
  if (type == EventType::kUnknown) {
    LOG(WARNING) << "[multiinput]get key event type is unknown";
    return;
  }

  ArkUI_KeyCode key = ARKUI_KEYCODE_UNKNOWN;
  key = static_cast<ArkUI_KeyCode>(OH_ArkUI_KeyEvent_GetKeyCode(key_event));
  if (key == ARKUI_KEYCODE_UNKNOWN) {
    LOG(WARNING) << "[multiinput]get key code is unknow";
    return;
  }

  EventFlags key_flags = EF_NONE;
  OhosEventSourceBase::UpdateKeyFlagsByOhKeyState(key_flags);
  auto task = base::BindOnce(
      [](const int32_t widget_id, ArkUI_KeyCode key, const EventType event_type,
         EventFlags key_flags) {
        auto* event_source = reinterpret_cast<OhosEventSourceNodeHandle*>(
            PlatformEventSource::GetInstance());
        if (event_source == nullptr) {
          LOG(WARNING) << "[multiinput] get event source failed";
          return;
        }
        event_source->UpdateKeyFlags(key_flags);
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
      widget_id, key, type, key_flags);
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

void OhosEventSourceNodeHandle::CreateAndDispatchFlingEvent(
    const gfx::AcceleratedWidget widget_id,
    const NodeHandlePanEvent& ohos_event,
    const EventFlags& event_flags,
    const bool is_start) {
  if (ohos_event.tool_type() != UI_INPUT_EVENT_TOOL_TYPE_TOUCHPAD) {
    return;
  }
  EventType event_type = is_start ? ui::EventType::kScrollFlingStart
                                  : ui::EventType::kScrollFlingCancel;
  // adjust the initial speed of fling
  float velocity_x = ohos_event.velocity_x();
  float velocity_y = ohos_event.velocity_y();
  ScrollEvent fling_event(event_type, pointer_location_, pointer_location_,
                          EventTimeForNow(), event_flags, velocity_x,
                          velocity_y, velocity_x, velocity_y,
                          kTouchpadScrollFingerCount);
  is_fling_active_ = is_start;
  SetTargetAndDispatchEvent(widget_id, fling_event);
}

int32_t ConvertInputEventActionFromWindowMouseEvent(
    int32_t window_mouse_action) {
  int32_t mouse_event_action = UI_MOUSE_EVENT_ACTION_UNKNOWN;
  switch (window_mouse_action) {
    case MOUSE_ACTION_MOVE:
      mouse_event_action = UI_MOUSE_EVENT_ACTION_MOVE;
      break;
    case MOUSE_ACTION_BUTTON_DOWN:
      mouse_event_action = UI_MOUSE_EVENT_ACTION_PRESS;
      break;
    case MOUSE_ACTION_BUTTON_UP:
      mouse_event_action = UI_MOUSE_EVENT_ACTION_RELEASE;
      break;
    default:
      DLOG(INFO) << __FUNCTION__ << " window_mouse_action is not useful:"
                 << window_mouse_action;
      break;
  }
  return mouse_event_action;
}

int32_t ConvertInputEventButtonFromWindowMouseEvent(
    int32_t window_mouse_button) {
  int32_t mouse_event_button = UI_MOUSE_EVENT_BUTTON_NONE;
  switch (window_mouse_button) {
    case MOUSE_BUTTON_LEFT:
      mouse_event_button = UI_MOUSE_EVENT_BUTTON_LEFT;
      break;
    case MOUSE_BUTTON_RIGHT:
      mouse_event_button = UI_MOUSE_EVENT_BUTTON_RIGHT;
      break;
    default:
      DLOG(INFO) << __FUNCTION__
                 << " mouse_button is not useful:" << window_mouse_button;
      break;
  }
  return mouse_event_button;
}

void ConvertWindowMouseEventToInputEvent(
    Input_MouseEvent* window_mouse_event,
    std::shared_ptr<NodeHandleMouseEventData> mouse_event_data) {
  WindowEventFilterAdapter& window_event_filter_adapter =
      WindowEventFilterAdapter::GetInstance();
  int32_t window_mouse_action =
      window_event_filter_adapter.GetWindowMouseEventAction(window_mouse_event);
  int32_t display_x = window_event_filter_adapter.GetWindowMouseEventDisplayX(
      window_mouse_event);
  int32_t display_y = window_event_filter_adapter.GetWindowMouseEventDisplayY(
      window_mouse_event);
  int32_t window_mouse_button =
      window_event_filter_adapter.GetWindowMouseEventButton(window_mouse_event);
  int64_t action_time =
      window_event_filter_adapter.GetWindowMouseEventActionTime(
          window_mouse_event);

  mouse_event_data->screenX = display_x;
  mouse_event_data->screenY = display_y;
  mouse_event_data->button =
      ConvertInputEventButtonFromWindowMouseEvent(window_mouse_button);
  mouse_event_data->action =
      ConvertInputEventActionFromWindowMouseEvent(window_mouse_action);
  // The unit of the mouse event timestamp from the window is millisecond.
  // The unit of the mouse event in the xcomponent is microsecond.
  mouse_event_data->timestamp =
      action_time * OhosEventSourceBase::kMicrosecondsUnit;
}

void NodeHandleSendWindowMouseEventForTabDragCallback(
    const int32_t widget_id,
    Input_MouseEvent* window_mouse_event) {
  if (window_mouse_event == nullptr) {
    LOG(ERROR) << __FUNCTION__
               << "[OhosTabDragNodeHandle]mouse event is null,widget_id:"
               << widget_id;
    return;
  }
  std::shared_ptr<NodeHandleMouseEventData> mouse_event_data =
      std::make_shared<NodeHandleMouseEventData>();
  ConvertWindowMouseEventToInputEvent(window_mouse_event, mouse_event_data);
  if (OhosEventFilter::GetInstance().CheckFilterMouseEvent(
          widget_id, mouse_event_data->timestamp, mouse_event_data->action)) {
    DLOG(ERROR) << __FUNCTION__
                << "[OhosTabDragNodeHandle]mouse event is filtered,widget_id:"
                << widget_id << ", action:" << mouse_event_data->action;
    return;
  }
  OhosEventFilter::GetInstance().RefreshMouseEvent(
      widget_id, mouse_event_data->timestamp, mouse_event_data->action);
  int32_t display_id =
      WindowEventFilterAdapter::GetInstance().GetWindowMouseEventDisplayId(
          window_mouse_event);

  EventFlags key_flags = EF_NONE;
  if (mouse_event_data->action != UI_MOUSE_EVENT_ACTION_MOVE) {
    OhosEventSourceBase::UpdateKeyFlagsByOhKeyState(key_flags);
  }
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const std::shared_ptr<NodeHandleMouseEventData> mouse_event_data,
         int32_t display_id, EventFlags key_flags) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << __FUNCTION__
                       << "[OhosTabDragNodeHandle]send window mouse event fail,"
                          " event_source is null";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->SendWindowMouseEventForTabDragNodeHandle(
                widget_id, mouse_event_data, display_id, key_flags);
      },
      widget_id, std::move(mouse_event_data), display_id, key_flags);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

int32_t ConvertTouchEventActionFromWindowTouchEvent(int32_t window_touch_action) {
  int32_t touch_event_action = UI_TOUCH_EVENT_ACTION_CANCEL;
  switch (window_touch_action) {
    case TOUCH_ACTION_MOVE:
      touch_event_action = UI_TOUCH_EVENT_ACTION_MOVE;
      break;
    case TOUCH_ACTION_DOWN:
      touch_event_action = UI_TOUCH_EVENT_ACTION_DOWN;
      break;
    case TOUCH_ACTION_UP:
      touch_event_action = UI_TOUCH_EVENT_ACTION_UP;
      break;
    case TOUCH_ACTION_CANCEL:
      touch_event_action = UI_TOUCH_EVENT_ACTION_CANCEL;
      break;
    default:
      DLOG(INFO) << __FUNCTION__
                 << "touch_event_action is not useful:"
                 << window_touch_action;
      break;
  }
  return touch_event_action;
}

void ConvertWindowTouchEventToInputEvent(
    Input_TouchEvent* window_touch_event,
    std::shared_ptr<NodeHandleTouchEventData > touch_event_data) {
  WindowEventFilterAdapter& window_event_filter_adapter =
      WindowEventFilterAdapter::GetInstance();
  int32_t window_touch_action =
      window_event_filter_adapter.GetWindowTouchEventAction(window_touch_event);
  int32_t display_x = window_event_filter_adapter.GetWindowTouchEventDisplayX(
      window_touch_event);
  int32_t display_y = window_event_filter_adapter.GetWindowTouchEventDisplayY(
      window_touch_event);
  int32_t window_touch_finger_id =
      window_event_filter_adapter.GetWindowTouchEventFingerId(
          window_touch_event);
  int64_t action_time =
      window_event_filter_adapter.GetWindowTouchEventActionTime(
          window_touch_event);
  touch_event_data->id = window_touch_finger_id;
  touch_event_data->touch_action =
      ConvertTouchEventActionFromWindowTouchEvent(window_touch_action);
  touch_event_data->display_x = display_x;
  touch_event_data->display_y = display_y;
  // The unit of the touch event timestamp from the window is millisecond.
  // The unit of the touch event in the xcomponent is microsecond.
  touch_event_data->timestamp =
      action_time * OhosEventSourceBase::kMicrosecondsUnit;
}

void NodeHandleSendWindowTouchEventForTabDragCallback(
    const int32_t widget_id,
    Input_TouchEvent* window_touch_event) {
  if (window_touch_event == nullptr) {
    LOG(ERROR) << __FUNCTION__
               << " touch event is null,widget_id:" << widget_id;
    return;
  }
  std::shared_ptr<NodeHandleTouchEventData> touch_event_data =
      std::make_shared<NodeHandleTouchEventData>();
  ConvertWindowTouchEventToInputEvent(window_touch_event, touch_event_data);
  if (OhosEventFilter::GetInstance().CheckFilterTouchEvent(
          widget_id, touch_event_data->timestamp,
          touch_event_data->touch_action, touch_event_data->id)) {
    DLOG(ERROR) << __FUNCTION__
                << " mouse event is filtered, widget_id:" << widget_id
                << ", action:" << touch_event_data->touch_action;
    return;
  }
  OhosEventFilter::GetInstance().RefreshTouchEvent(
      widget_id, touch_event_data->timestamp, touch_event_data->touch_action,
      touch_event_data->id);
  int32_t display_id =
      WindowEventFilterAdapter::GetInstance().GetWindowTouchEventDisplayId(
          window_touch_event);
  auto task = base::BindOnce(
      [](const int32_t widget_id,
         const std::shared_ptr<NodeHandleTouchEventData> touch_event_data,
         int32_t display_id) {
        auto* event_source = PlatformEventSource::GetInstance();
        if (event_source == nullptr) {
          LOG(WARNING) << "[OhosTabDragNodeHandle]register " << __FUNCTION__
                       << " before event source created";
          return;
        }
        reinterpret_cast<OhosEventSourceNodeHandle*>(event_source)
            ->SendWindowTouchEventForTabDragNodeHandle(
                widget_id, touch_event_data, display_id);
      },
      widget_id, std::move(touch_event_data), display_id);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}

}  // namespace ui
