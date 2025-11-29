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

class OhosEventFilter {
 public:
  static OhosEventFilter& GetInstance();
  OhosEventFilter(const OhosEventFilter&) = delete;
  OhosEventFilter(const OhosEventFilter&&) = delete;
  OhosEventFilter operator=(const OhosEventFilter&) = delete;

  bool CheckFilterMouseEvent(const gfx::AcceleratedWidget widget_id,
                             MouseEventTimeStamp timestamp,
                             MouseEventAction action);
  void RefreshMouseEvent(const gfx::AcceleratedWidget widget_id,
                         MouseEventTimeStamp timestamp,
                         MouseEventAction action);

 private:
  OhosEventFilter();
  ~OhosEventFilter() = default;

  gfx::AcceleratedWidget pre_widget_id_ = -1;
  MouseEventTimeStamp pre_timestamp_ = 0;
  MouseEventAction pre_event_action_;
  MouseEventAction mouse_move_action_;

  bool CheckMouseEventInfoForFilter(
      const gfx::AcceleratedWidget widget_id,
      MouseEventAction action);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_FILTER_H_
