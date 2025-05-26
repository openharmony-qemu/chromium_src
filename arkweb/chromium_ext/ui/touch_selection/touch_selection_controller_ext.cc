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

#include "arkweb/chromium_ext/ui/touch_selection/touch_selection_controller_ext.h"

#include "arkweb/chromium_ext/ui/touch_selection/touch_selection_controller_utils.h"
#include "base/logging.h"

namespace ui {

TouchSelectionControllerExt::TouchSelectionControllerExt(
    TouchSelectionControllerClient* client,
    const Config& config)
    : TouchSelectionController(client, config) {}

TouchSelectionControllerExt::~TouchSelectionControllerExt() {}

#if BUILDFLAG(ARKWEB_MENU)
void TouchSelectionControllerExt::UpdateSelectionChanged(
    const TouchSelectionDraggable& draggable) {
  if (&draggable != insertion_handle_.get()) {
    client_->OnSelectionEvent(SELECTION_HANDLES_UPDATEMENU);
  }
}

bool TouchSelectionControllerExt::IsLongPressDragSelectionActive() {
  return longpress_drag_selector_.IsActive();
}

void TouchSelectionControllerExt::ResetResponsePendingInputEvent() {
  response_pending_input_event_ = INPUT_EVENT_TYPE_NONE;
}

bool TouchSelectionControllerExt::SelectOverImg() {
  if (active_status_ == SELECTION_ACTIVE && start_selection_handle_ &&
      end_selection_handle_ && show_touch_handles_) {
    if (start_selection_handle_->IsActive() ||
        end_selection_handle_->IsActive()) {
      LOG(INFO) << "selection temporarily hide";
      utils_->SetResetSelectionTemporarily(true);
      return true;
    }
  }
  if (longpress_drag_selector_.IsDragging()) {
    LOG(INFO) << "long selection temporarily hide";
    utils_->SetResetSelectionTemporarily(true);
    return true;
  }
  return false;
}

bool TouchSelectionControllerExt::OnHandleSwap(bool need_swap,
                                               const gfx::SelectionBound& start,
                                               const gfx::SelectionBound& end) {
  bool handled_swap = need_swap;
  if (!need_swap) {
    handled_swap = (end_ == end && end_selection_handle_->IsActive()) ||
                   (start_ == start && start_selection_handle_->IsActive());
  }
  return handled_swap;
}

void TouchSelectionControllerExt::HandleIfEndNotVisible(
    const MotionEvent& event) {
  if (!end_selection_handle_->AsTouchHandleExt()->GetVisible() &&
      start_selection_handle_->AsTouchHandleExt()->GetVisible()) {
    LOG(INFO) << "Handle selection event, start is visible end is not visible.";
    return start_selection_handle_->WillHandleTouchEvent(event);
  }
}

void TouchSelectionControllerExt::ArkSelectBetweenCoordinates(
    const gfx::PointF& base,
    const gfx::PointF& extent) {
  if (!end_selection_handle_->AsTouchHandleExt()->GetVisible() &&
      end_.GetHeight() == 0 && start_.GetHeight() != 0) {
    LOG(INFO) << "The end handle information is empty.";
    client_->SelectBetweenCoordinatesV2(
        anchor_drag_to_selection_start_ ? extent : base, true);
  } else if (!start_selection_handle_->AsTouchHandleExt()->GetVisible() &&
             start_.GetHeight() == 0 && end_.GetHeight() != 0) {
    LOG(INFO) << "The start handle information is empty.";
    client_->SelectBetweenCoordinatesV2(
        anchor_drag_to_selection_start_ ? base : extent, false);
  } else {
    client_->SelectBetweenCoordinates(base, extent);
  }
}

void TouchSelectionControllerExt::ResetPositionAfterDragEnd(
    const TouchSelectionDraggable& draggable) {
  if (&draggable == start_selection_handle_.get()) {
    start_selection_handle_->AsTouchHandleExt()->ResetPositionAfterDragEnd();
  }
  if (&draggable == end_selection_handle_.get()) {
    end_selection_handle_->AsTouchHandleExt()->ResetPositionAfterDragEnd();
  }
}
#endif
}  // namespace ui
