// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_event_filter.h"

#include "ohos/adapter/xcomponent/xcomponent_manager.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"

namespace ui {

constexpr int64_t kMouseEventFilterTime = 50 * 1000 * 1000;

OhosEventFilter& OhosEventFilter::GetInstance() {
  static OhosEventFilter event_filter;
  return event_filter;
}

bool OhosEventFilter::CheckFilterMouseEvent(
    const gfx::AcceleratedWidget widget_id,
    const OH_NativeXComponent_MouseEvent& mouse_event) {
  if (CheckMouseEventInfoForFilter(widget_id, mouse_event)) {
    // filter events when a window is creating
    // or the difference between two move events is within
    // kMouseEventFilterTime
    if (ohos::adapter::xcomponent::XComponentManager::GetInstance()
            ->WindowIsCreating(widget_id) ||
        (mouse_event.timestamp - pre_timestamp_ < kMouseEventFilterTime)) {
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

}  // namespace ui
