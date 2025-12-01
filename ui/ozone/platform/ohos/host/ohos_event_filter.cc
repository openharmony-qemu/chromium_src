// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_event_filter.h"

#include "ohos/adapter/xcomponent/event/window_event_filter_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"
#include "ohos/adapter/node_handle/node_handle_impl.h"

namespace ui {

// Interval: microseconds
constexpr int64_t kDragTabMouseFilterTime = 15 * 1000 * 1000;
constexpr int64_t kMouseEventFilterTime = 5 * 1000 * 1000;

OhosEventFilter& OhosEventFilter::GetInstance() {
  static OhosEventFilter event_filter;
  return event_filter;
}

OhosEventFilter::OhosEventFilter() {
  if (ohos::adapter::nodeHandle::NodeHandleImpl::GetInstance()
          .IsSupportNodeHandle()) {
    mouse_move_action_ = UI_MOUSE_EVENT_ACTION_MOVE;
    pre_event_action_ = UI_MOUSE_EVENT_ACTION_UNKNOWN;
  } else {
    mouse_move_action_ = OH_NATIVEXCOMPONENT_MOUSE_MOVE;
    pre_event_action_ = OH_NATIVEXCOMPONENT_MOUSE_NONE;
  }
}

bool OhosEventFilter::CheckFilterMouseEvent(
    const gfx::AcceleratedWidget widget_id,
    MouseEventTimeStamp timestamp,
    MouseEventAction action) {
  if (CheckMouseEventInfoForFilter(widget_id, action)) {
    // When dragging tab,  the mouse event
     // filtering condition is within kDragTabMouseFilterTime ms
     // In other cases, the mouse event filtering condition is within kMouseEventFilterTime ms
    if (ohos::adapter::window::WindowEventFilterAdapter::GetInstance()
            .IsTabDragging()) {
      if ((timestamp - pre_timestamp_ < kDragTabMouseFilterTime)) {
        return true;
      }
    } else if (timestamp - pre_timestamp_ < kMouseEventFilterTime) {
      return true;
    }
  }
  return false;
}

void OhosEventFilter::RefreshMouseEvent(
    const gfx::AcceleratedWidget widget_id,
    MouseEventTimeStamp timestamp,
    MouseEventAction action) {
  pre_widget_id_ = widget_id;
  pre_timestamp_ = timestamp;
  pre_event_action_ = action;
}

bool OhosEventFilter::CheckMouseEventInfoForFilter(
    const gfx::AcceleratedWidget widget_id,
    MouseEventAction action) {
  // intercept mouse event only when mouse event type is move
  if (widget_id == pre_widget_id_ && action == pre_event_action_ &&
      action == mouse_move_action_) {
    return true;
  }
  return false;
}

} // namespace ui
