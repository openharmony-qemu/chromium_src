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
const std::string kMouseDragTab = "mouse";
const std::string kTouchDragTab = "touch";

class OhosWindowDragManager {
 public:
  OhosWindowDragManager() = default;
  OhosWindowDragManager(OhosWindowDragManager&&) = delete;
  OhosWindowDragManager& operator=(OhosWindowDragManager&&) = delete;
  OhosWindowDragManager(const OhosWindowDragManager&) = delete;
  OhosWindowDragManager& operator=(const OhosWindowDragManager&) = delete;

  ~OhosWindowDragManager() = default;

  void StartTabDragging(int32_t window_id);
  void ShiftWindowEvent(const int32_t source_id, const int32_t target_id);
  void ClearDraggingTabParams();
  bool IsSimulateEventWhenEventShift(int32_t origin_window_id,
                                     int32_t event_action);
  int32_t GetTargetWindowIdAfterShiftEvent(int32_t origin_window_id);
  bool NeedSendWindowEventToUi(const int32_t widget_id,
                               const int32_t action,
                               int32_t event_id = -1);
  void StartTabDraggingByTouch(int32_t window_id,
                               const int32_t finger_id);

 private:
  // window ids before and after event transfer
  int32_t shift_event_source_window_id_ = -1;
  int32_t shift_event_target_window_id_ = -1;
  int32_t press_event_action_;
  int32_t release_event_action_;
  // The finger id that requires event shift when touching drag a tab
  int32_t touch_drag_tab_finger_id_ = -1;
  bool IsTouchingDragTab() {
    return touch_drag_tab_finger_id_ >= 0;
  }
  std::string GetTabDragMethod();
  void RefreshEventAction();
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_WINDOW_DRAG_MANAGER_H
