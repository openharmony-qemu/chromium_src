// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

OhosWindowDragManager::OhosWindowDragManager() {
  if (ohos::adapter::nodeHandle::NodeHandleImpl::GetInstance()
          .IsSupportNodeHandle()) {
    mouse_press_action_ = UI_MOUSE_EVENT_ACTION_PRESS;
    mouse_release_action_ = UI_MOUSE_EVENT_ACTION_RELEASE;
  } else {
    mouse_press_action_ = OH_NATIVEXCOMPONENT_MOUSE_PRESS;
    mouse_release_action_ = OH_NATIVEXCOMPONENT_MOUSE_RELEASE;
  }
}

void OhosWindowDragManager::StartTabDragging(int32_t window_id) {
  WindowEventFilterAdapter::GetInstance().SetDraggingTabWidgetId(window_id);
}

void OhosWindowDragManager::ShiftWindowEvent(const int32_t source_id,
                                             const int32_t target_id) {
  shift_event_source_window_id_ = source_id;
  shift_event_target_window_id_ = target_id;
  WindowEventFilterAdapter::GetInstance().SetDraggingTabWidgetId(target_id);
  WindowEventFilterAdapter::GetInstance().CacheShiftEventWindowIds(source_id, target_id);
  bool result =
      AppWindowAdapter::GetInstance().ShiftWindowEvent(source_id, target_id);
  if (!result) {
    LOG(ERROR) << "[OhosTabDrag] ShiftWindowEvent fail ClearDraggingTabParams";
    ClearDraggingTabParams();
  }
}

void OhosWindowDragManager::ClearDraggingTabParams() {
  shift_event_source_window_id_ = -1;
  shift_event_target_window_id_ = -1;
  WindowEventFilterAdapter::GetInstance().SetDraggingTabWidgetId(-1);
}

bool OhosWindowDragManager::IsSimulateEventWhenEventShift(
    int32_t origin_window_id,
    int32_t event_action) {
  if (shift_event_source_window_id_ < 0 || shift_event_target_window_id_ < 0) {
    return false;
  }
  if (shift_event_source_window_id_ == origin_window_id &&
      event_action == mouse_release_action_) {
    LOG(WARNING) << "[OhosTabDrag] IsSimulateEventWhenEventShift source window "
                    "mouse-up is filtered, shift_event_source_window_id_:"
                 << shift_event_source_window_id_;
    return true;
  }
  if (shift_event_target_window_id_ == origin_window_id &&
      event_action == mouse_press_action_) {
    LOG(WARNING) << "[OhosTabDrag] IsSimulateEventWhenEventShift target window "
                    "mouse-down is filtered, shift_event_target_window_id_:"
                 << shift_event_target_window_id_;
    return true;
  }
  return false;
}

bool OhosWindowDragManager::NeedSendWindowMouseEventToUi(
    const int32_t widget_id,
    const int32_t mouse_action) {
  // When the event transfer fails, the previous event is not intercepted
  if (!WindowEventFilterAdapter::GetInstance().IsTabDragging()) {
    return true;
  }
  // When tab is dragging, after the mouse event is transferred,
  // the events in the source window need to be intercepted,
  // and the target window cannot be affected
  if (widget_id != WindowEventFilterAdapter::GetInstance().GetDraggingTabWidgetId()) {
    LOG(WARNING) << "[OhosTabDrag] NeedSendWindowMouseEventToUi mouse event is "
                    "filtered when tab is dragging, widget_id:"
                 << widget_id << ", action:" << mouse_action;
    return false;
  }
  // When tab is dragging, the mouse up and mouse down events are simulated
  // after the window event is transferred. The events cannot be sent to the Chromium
  if (IsSimulateEventWhenEventShift(widget_id, mouse_action)) {
    LOG(WARNING) << "[OhosTabDrag] NeedSendWindowMouseEventToUi, intercept "
                    "simulated mouse "
                    "events, widget_id:"
                 << widget_id;
    return false;
  }
  return true;
}

}  // namespace ui
