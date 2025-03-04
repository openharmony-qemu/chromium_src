// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_FILTER_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_FILTER_H_

#include <ace/xcomponent/native_interface_xcomponent.h>

#include "ui/gfx/native_widget_types.h"

namespace ui {

class OhosEventFilter {
 public:
  static OhosEventFilter& GetInstance();
  OhosEventFilter(const OhosEventFilter&) = delete;
  OhosEventFilter(const OhosEventFilter&&) = delete;
  OhosEventFilter operator=(const OhosEventFilter&) = delete;

  bool CheckFilterMouseEvent(const gfx::AcceleratedWidget widget_id,
                             const OH_NativeXComponent_MouseEvent& mouse_event);
  void RefreshMouseEvent(const gfx::AcceleratedWidget widget_id,
                         const OH_NativeXComponent_MouseEvent& mouse_event);

 private:
  OhosEventFilter() = default;
  ~OhosEventFilter() = default;

  gfx::AcceleratedWidget pre_widget_id_ = -1;
  int64_t pre_timestamp_ = 0;
  OH_NativeXComponent_MouseEventAction pre_event_action_ =
      OH_NATIVEXCOMPONENT_MOUSE_NONE;

  bool CheckMouseEventInfoForFilter(
      const gfx::AcceleratedWidget widget_id,
      const OH_NativeXComponent_MouseEvent& mouse_event);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_FILTER_H_
