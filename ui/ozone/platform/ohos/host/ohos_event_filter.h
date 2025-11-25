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

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_FILTER_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_FILTER_H_

#include <ace/xcomponent/native_interface_xcomponent.h>
#include "ui/gfx/native_widget_types.h"

namespace ui {

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
                             EventTimeStamp timestamp,
                             EventAction mouse_action);
  void RefreshMouseEvent(const gfx::AcceleratedWidget widget_id,
                         EventTimeStamp timestamp,
                         EventAction mouse_action);
  bool CheckFilterTouchEvent(const gfx::AcceleratedWidget widget_id,
                             EventTimeStamp timestamp,
                             EventAction touch_action,
                             TouchEventFinger finger_id);
  void RefreshTouchEvent(const gfx::AcceleratedWidget widget_id,
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
