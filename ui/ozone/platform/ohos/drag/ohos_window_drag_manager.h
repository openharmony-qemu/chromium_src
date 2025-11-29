// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_WINDOW_DRAG_MANAGER_H
#define UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_WINDOW_DRAG_MANAGER_H

#include <cstdint>
#include <string>

#include <multimodalinput/oh_input_manager.h>

#include "ohos/adapter/window/window_common.h"
#include "ui/gfx/native_widget_types.h"

namespace ui {

class OhosWindowDragManager {
 public:
  OhosWindowDragManager();
  OhosWindowDragManager(OhosWindowDragManager&&) = delete;
  OhosWindowDragManager& operator=(OhosWindowDragManager&&) = delete;
  OhosWindowDragManager(const OhosWindowDragManager&) = delete;
  OhosWindowDragManager& operator=(const OhosWindowDragManager&) = delete;

  ~OhosWindowDragManager() = default;

  int32_t GetTabDraggingWindowId() {
    return tab_dragging_window_id_;
  }
  void StartTabDragging(int32_t window_id);
  void ShiftWindowEvent(const int32_t source_id,
                        const int32_t target_id);
  void ClearDraggingTabParams();
  bool IsSimulateEventWhenEventShift(int32_t origin_window_id,
                                     int32_t event_action);
  int32_t GetTargetWindowIdAfterShiftEvent(int32_t origin_window_id);
  bool NeedSendWindowMouseEventToUi(
      const int32_t widget_id,
      const int32_t mouse_action);

 private:
  // window ids before and after event transfer
  int32_t shift_event_source_window_id_ = -1;
  int32_t shift_event_target_window_id_ = -1;
  // id of the tab that is being dragged
  int32_t tab_dragging_window_id_ = -1;
  int32_t mouse_press_action_;
  int32_t mouse_release_action_;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_WINDOW_DRAG_MANAGER_H
