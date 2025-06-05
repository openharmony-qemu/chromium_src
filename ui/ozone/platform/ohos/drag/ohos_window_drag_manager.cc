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
#include "ohos/adapter/window/app_window_adapter.h"
#include "ohos/adapter/xcomponent/event/window_event_filter_adapter.h"

namespace ui {
using WindowEventFilterAdapter = ohos::adapter::window::WindowEventFilterAdapter;
using AppWindowAdapter = ohos::adapter::window::AppWindowAdapter;

OhosWindowDragManager::OhosWindowDragManager() {
}

void OhosWindowDragManager::StartTabDragging(int32_t window_id) {
  WindowEventFilterAdapter::GetInstance().SetDraggingTabWidgetId(window_id);
}

void OhosWindowDragManager::ShiftWindowEvent(const int32_t source_id,
                                             const int32_t target_id) {
  shift_event_source_window_id_ = source_id;
  shift_event_target_window_id_ = target_id;
  LOG(INFO) << "[OhosTabDrag] ShiftWindowEvent, "
               "shift_event_source_window_id_:"
            << shift_event_source_window_id_
            << ", shift_event_target_window_id_:"
            << shift_event_target_window_id_;
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
      event_action == OH_NATIVEXCOMPONENT_MOUSE_RELEASE) {
    LOG(WARNING) << "[OhosTabDrag] IsSimulateEventWhenEventShift source window "
                    "mouse-up is filtered, shift_event_source_window_id_:"
                 << shift_event_source_window_id_;
    return true;
  }
  if (shift_event_target_window_id_ == origin_window_id &&
      event_action == OH_NATIVEXCOMPONENT_MOUSE_PRESS) {
    LOG(WARNING) << "[OhosTabDrag] IsSimulateEventWhenEventShift target window "
                    "mouse-down is filtered, shift_event_target_window_id_:"
                 << shift_event_target_window_id_;
    return true;
  }
  return false;
}

bool OhosWindowDragManager::NeedSendWindowMouseEventToUi(
    const int32_t widget_id,
    const std::shared_ptr<OH_NativeXComponent_MouseEvent>
        xcomponent_mouse_event) {
  // When the event transfer fails, the previous event is not intercepted
  if (!WindowEventFilterAdapter::GetInstance().IsTabDragging()) {
    return true;
  }
  int32_t action = xcomponent_mouse_event->action;
  // When tab is dragging, after the mouse event is transferred,
  // the events in the source window need to be intercepted,
  // and the target window cannot be affected
  if (widget_id != WindowEventFilterAdapter::GetInstance().GetDraggingTabWidgetId()) {
    LOG(WARNING) << "[OhosTabDrag] NeedSendWindowMouseEventToUi mouse event is "
                    "filtered when tab is dragging, widget_id:"
                 << widget_id << ", action:" << action;
    return false;
  }
  // When tab is dragging, the mouse up and mouse down events are simulated
  // after the window event is transferred. The events cannot be sent to the Chromium
  if (IsSimulateEventWhenEventShift(widget_id, action)) {
    LOG(WARNING) << "[OhosTabDrag] NeedSendWindowMouseEventToUi, intercept "
                    "simulated mouse "
                    "events, widget_id:"
                 << widget_id;
    return false;
  }
  return true;
}

}  // namespace ui
