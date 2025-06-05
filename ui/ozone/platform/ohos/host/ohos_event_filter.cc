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

#include "ui/ozone/platform/ohos/host/ohos_event_filter.h"

#include "ohos/adapter/xcomponent/event/window_event_filter_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"

namespace ui {

// Interval: microseconds
constexpr int64_t kDragTabMouseFilterTime = 15 * 1000 * 1000;
constexpr int64_t kMouseEventFilterTime = 50 * 1000 * 1000;

OhosEventFilter& OhosEventFilter::GetInstance() {
  static OhosEventFilter event_filter;
  return event_filter;
}

bool OhosEventFilter::CheckFilterMouseEvent(
    const gfx::AcceleratedWidget widget_id,
    const OH_NativeXComponent_MouseEvent& mouse_event) {
  if (CheckMouseEventInfoForFilter(widget_id, mouse_event)) {
    // When dragging tab,  the mouse event
     // filtering condition is within kDragTabMouseFilterTime ms
     // In other cases, the mouse event filtering condition is within kMouseEventFilterTime ms
    if (ohos::adapter::window::WindowEventFilterAdapter::GetInstance()
            .IsTabDragging()) {
      if ((mouse_event.timestamp - pre_timestamp_ < kDragTabMouseFilterTime)) {
        return true;
      }
    } else if (mouse_event.timestamp - pre_timestamp_ < kMouseEventFilterTime) {
      return true;
    }
  }
  return false;
}

void OhosEventFilter::RefreshMouseEvent(
    const gfx::AcceleratedWidget widget_id,
    const OH_NativeXComponent_MouseEvent& mouse_event) {
  pre_widget_id_ = widget_id;
  pre_timestamp_ = mouse_event.timestamp;
  pre_event_action_ = mouse_event.action;
}

bool OhosEventFilter::CheckMouseEventInfoForFilter(
    const gfx::AcceleratedWidget widget_id,
    const OH_NativeXComponent_MouseEvent& mouse_event) {
  // intercept mouse event only when mouse event type is move
  if (widget_id == pre_widget_id_ && mouse_event.action == pre_event_action_ &&
      mouse_event.action == OH_NATIVEXCOMPONENT_MOUSE_MOVE) {
    return true;
  }
  return false;
}

} // namespace ui
