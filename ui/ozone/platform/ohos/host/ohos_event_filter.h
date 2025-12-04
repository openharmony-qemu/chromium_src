// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_FILTER_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_FILTER_H_

#include <ace/xcomponent/native_interface_xcomponent.h>
#include "ui/gfx/native_widget_types.h"

namespace ui {

using MouseEventAction = int32_t;
using MouseEventTimeStamp = int64_t;

using EventAction = int32_t;
using EventTimeStamp = int64_t;
using TouchEventFinger = int32_t;

class OhosEventFilter {
 public:
  static OhosEventFilter& GetInstance();
  OhosEventFilter(const OhosEventFilter&) = delete;
  OhosEventFilter(const OhosEventFilter&&) = delete;
  OhosEventFilter operator=(const OhosEventFilter&) = delete;

  bool CheckFilterMouseEvent(const gfx::AcceleratedWidget widget_id,
                             MouseEventTimeStamp timestamp,
                             MouseEventAction mouse_action);
  void RefreshMouseEvent(const gfx::AcceleratedWidget widget_id,
                         MouseEventTimeStamp timestamp,
                         MouseEventAction mouse_action);
  bool CheckFilterTouchEvent(const gfx::AcceleratedWidget widget_id,
                             EventTimeStamp timestamp,
                             EventAction touch_action,
                             TouchEventFinger finger_id);
  void
  RefreshTouchEvent(const gfx::AcceleratedWidget widget_id,
                    EventTimeStamp timestamp,
                    EventAction touch_action,
                    TouchEventFinger finger_id);

 private:
  OhosEventFilter();
  ~OhosEventFilter() = default;

  gfx::AcceleratedWidget pre_widget_id_ = -1;
  EventTimeStamp pre_timestamp_ = 0;
  EventAction pre_mouse_event_action_;
  EventAction pre_touch_event_action_;
  EventAction mouse_move_action_;
  EventAction touch_move_action_;
  TouchEventFinger pre_touch_finger_id_ = -1;

  bool CheckMouseEventInfoForFilter(gfx::AcceleratedWidget widget_id,
                                    EventAction mouse_action);
  bool CheckTouchEventInfoForFilter(gfx::AcceleratedWidget widget_id,
                                    EventAction touch_action,
                                    TouchEventFinger finger_id);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_FILTER_H_
