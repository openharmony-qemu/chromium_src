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

#include "ui/ozone/platform/ohos/drag/ohos_window_drag_manager.h"

#include "base/logging.h"
#include "ohos/adapter/context/context_adapter.h"
#include "ohos/adapter/device_info/device_info.h"
#include "ohos/adapter/node_handle/node_handle_impl.h"
#include "ohos/adapter/window/app_window_adapter.h"
#include "ohos/adapter/xcomponent/event/window_event_filter_adapter.h"

namespace ui {
using WindowEventFilterAdapter = ohos::adapter::window::WindowEventFilterAdapter;
using AppWindowAdapter = ohos::adapter::window::AppWindowAdapter;

void OhosWindowDragManager::StartTabDragging(int32_t window_id) {
  RefreshEventAction();
  WindowEventFilterAdapter::GetInstance().SetDraggingTabWidgetId(window_id);
}

void OhosWindowDragManager::StartTabDraggingByTouch(int32_t window_id,
                                                    const int32_t finger_id) {
  LOG(INFO) << "[OhosTabDrag]" << __FUNCTION__ << ",finger_id:" << finger_id;
  touch_drag_tab_finger_id_ = finger_id;
  StartTabDragging(window_id);
}

void OhosWindowDragManager::ShiftWindowEvent(const int32_t source_id,
                                             const int32_t target_id) {
  shift_event_source_window_id_ = source_id;
  shift_event_target_window_id_ = target_id;
  WindowEventFilterAdapter::GetInstance().SetDraggingTabWidgetId(target_id);
  WindowEventFilterAdapter::GetInstance().CacheShiftEventWindowIds(source_id, target_id);
  bool result = false;
  if (IsTouchingDragTab()) {
    result = AppWindowAdapter::GetInstance().ShiftWindowTouchEvent(
        source_id, target_id, touch_drag_tab_finger_id_);
  } else {
    result = AppWindowAdapter::GetInstance().ShiftWindowMouseEvent(source_id,
                                                                   target_id);
  }
  if (!result) {
    LOG(ERROR) << "[OhosTabDrag] " << __FUNCTION__
               << ", " << GetTabDragMethod() << " fail, clear tab dragging params";
    ClearDraggingTabParams();
  }
}

void OhosWindowDragManager::ClearDraggingTabParams() {
  shift_event_source_window_id_ = -1;
  shift_event_target_window_id_ = -1;
  touch_drag_tab_finger_id_ = -1;
  WindowEventFilterAdapter::GetInstance().SetDraggingTabWidgetId(-1);
}

bool OhosWindowDragManager::IsSimulateEventWhenEventShift(
    int32_t origin_window_id,
    int32_t event_action) {
  if (shift_event_source_window_id_ < 0 || shift_event_target_window_id_ < 0) {
    return false;
  }

  if (shift_event_source_window_id_ == origin_window_id &&
      event_action == release_event_action_) {
    LOG(WARNING) << "[OhosTabDrag] " << __FUNCTION__ << " source window "
                 << GetTabDragMethod()
                 << " up is filtered, shift_event_source_window_id_:"
                 << shift_event_source_window_id_
                 << ",release_event_action_:" << release_event_action_;
    return true;
  }
  if (shift_event_target_window_id_ == origin_window_id &&
      event_action == press_event_action_) {
    LOG(WARNING) << "[OhosTabDrag] " << __FUNCTION__ << " target window "
                 << GetTabDragMethod()
                 << " down is filtered, shift_event_target_window_id_:"
                 << shift_event_target_window_id_
                 << ",press_event_action_:" << press_event_action_;
    return true;
  }
  return false;
}

bool OhosWindowDragManager::NeedSendWindowEventToUi(const int32_t widget_id,
                                                    const int32_t action,
                                                    int32_t event_id) {
  // When the event transfer fails, the previous event is not intercepted
  if (!WindowEventFilterAdapter::GetInstance().IsTabDragging()) {
    return true;
  }
  // When tab is dragging, after the touch event is transferred,
  // the events in the source window need to be intercepted,
  // and the target window cannot be affected
  if (widget_id !=
      WindowEventFilterAdapter::GetInstance().GetDraggingTabWidgetId()) {
    LOG(WARNING) << "[OhosTabDrag] " << __FUNCTION__ << GetTabDragMethod()
                 << " event is filtered when tab is dragging, widget_id:"
                 << widget_id << ", action:" << action;
    return false;
  }
  if (IsTouchingDragTab() && event_id != touch_drag_tab_finger_id_) {
    LOG(WARNING) << "[OhosTabDrag] " << __FUNCTION__
                 << " touch event is filtered when current fingerId is not "
                 << "shift fingerId, current finger id:" << event_id
                 << ", touch_drag_tab_finger_id_:" << touch_drag_tab_finger_id_;
    return false;
  }

  // When tab is dragging, the up event of source window and down event of
  // target window are simulated after the window event is transferred. The two
  // events cannot be sent to the Chromium
  if (IsSimulateEventWhenEventShift(widget_id, action)) {
    LOG(WARNING) << "[OhosTabDrag] " << __FUNCTION__ << " intercept simulated "
                 << GetTabDragMethod() << " events, widget_id:" << widget_id;
    return false;
  }
  return true;
}

std::string OhosWindowDragManager::GetTabDragMethod() {
  if (IsTouchingDragTab()) {
    return kTouchDragTab;
  }
  return kMouseDragTab;
}

void OhosWindowDragManager::RefreshEventAction() {
  // window event type enum:
  // OH_NATIVEXCOMPONENT_MOUSE_PRESS: 1
  // OH_NATIVEXCOMPONENT_MOUSE_RELEASE: 2
  // OH_NATIVEXCOMPONENT_DOWN: 0
  // OH_NATIVEXCOMPONENT_UP: 1
  if (ohos::adapter::nodeHandle::NodeHandleImpl::GetInstance()
          .IsSupportNodeHandle()) {
    if (IsTouchingDragTab()) {
      press_event_action_ = UI_TOUCH_EVENT_ACTION_DOWN;
      release_event_action_ = UI_TOUCH_EVENT_ACTION_UP;
    } else {
      press_event_action_ = UI_MOUSE_EVENT_ACTION_PRESS;
      release_event_action_ = UI_MOUSE_EVENT_ACTION_RELEASE;
    }
  } else {
    if (IsTouchingDragTab()) {
      release_event_action_ = OH_NATIVEXCOMPONENT_UP;
      press_event_action_ = OH_NATIVEXCOMPONENT_DOWN;
    } else {
      press_event_action_ = OH_NATIVEXCOMPONENT_MOUSE_PRESS;
      release_event_action_ = OH_NATIVEXCOMPONENT_MOUSE_RELEASE;
    }
  }
}

}  // namespace ui
