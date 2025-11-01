/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UI_TOUCH_SELECTION_TOUCH_SELECTION_CONTROLLER_EXT_H_
#define UI_TOUCH_SELECTION_TOUCH_SELECTION_CONTROLLER_EXT_H_

#include "arkweb/build/features/features.h"
#include "ui/touch_selection/touch_selection_controller.h"
#include <queue>

namespace ui {
class TouchSelectionController;
struct PreTouchInfo {
  double x = -1;
  double y = -1;
  base::TimeTicks start;
};

class TouchSelectionControllerExt : public TouchSelectionController {
 public:
  TouchSelectionControllerExt(TouchSelectionControllerClient* client,
                              const Config& config);

  ~TouchSelectionControllerExt() override;
  TouchSelectionControllerExt* AsTouchSelectionControllerExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_VIBRATE)
  bool IsLongPressEvent() { return is_long_press_; }
  void SetLongPressEvent(bool is_long_press) { is_long_press_ = is_long_press; }
  void ResetLongPressEvent() { is_long_press_ = false; }
#endif  // BUILDFLAG(ARKWEB_VIBRATE)

#if BUILDFLAG(ARKWEB_MENU)
  const std::unique_ptr<TouchHandle>& GetInsertHandle() {
    return insertion_handle_;
  }
  const std::unique_ptr<TouchHandle>& GetStartSelectionHandle() {
    return start_selection_handle_;
  }
  const std::unique_ptr<TouchHandle>& GetEndSelectionHandle() {
    return end_selection_handle_;
  }
  void UpdateSelectionChanged(
      const TouchSelectionDraggable& draggable) override;
  bool IsLongPressDragSelectionActive();
  void ResetResponsePendingInputEvent();
  bool OnHandleSwap(bool need_swap,
                    const gfx::SelectionBound& start,
                    const gfx::SelectionBound& end);
  bool IsEndHandleNotVisible(const MotionEvent& event);
  void ArkSelectBetweenCoordinates(const gfx::PointF& base,
                                   const gfx::PointF& extent);
  void ResetPositionAfterDragEnd(const TouchSelectionDraggable& draggable);
  void OnInsertionChangedExt(const gfx::SelectionBound& start,
                             const gfx::SelectionBound& end);
  bool IsTapEvent() { return response_pending_input_event_ = TAP; }
#endif

  bool SelectOverImg();

#if BUILDFLAG(ARKWEB_VIBRATE)
  bool is_long_press_ = false;
#endif  // BUILDFLAG(ARKWEB_VIBRATE)

#if BUILDFLAG(ARKWEB_AI)
  static constexpr int32_t INVALID_CLICK_NUM = 0;
  static constexpr int32_t SINGLE_CLICK_NUM = 1;
  static constexpr int32_t DOUBLE_CLICK_NUM = 2;
  static constexpr int32_t TRIPLE_CLICK_NUM = 3;
  base::TimeDelta double_tap_timeout_ = base::Milliseconds(300);
  float double_tap_slop_square_ = 100 * 100;
  std::queue<PreTouchInfo> gestureTouchQueue_;
  int32_t GetTouchNums(const MotionEvent& event);
  bool IsContinuousEvent(const PreTouchInfo& first_down,
                         const PreTouchInfo& second_down,
                         bool should_process_double_tap);
  void SetTouchNumsForHandle(const MotionEvent& event);
#endif
};
}  // namespace ui
#endif  // UI_TOUCH_SELECTION_TOUCH_SELECTION_CONTROLLER_EXT_H_
