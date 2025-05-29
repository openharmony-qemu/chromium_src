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

#include "arkweb/chromium_ext/ui/events/gesture_detection/gesture_detector_ext.h"
#include "arkweb/build/features/features.h"

#include "ui/events/gesture_detection/gesture_listeners.h"

namespace ui {
GestureDetectorExt::GestureDetectorExt(
    const Config& config,
    GestureListener* listener,
    DoubleTapListener* optional_double_tap_listener)
    : GestureDetector(config, listener, optional_double_tap_listener) {}

GestureDetectorExt::~GestureDetectorExt() = default;

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void GestureDetectorExt::Cancel(bool is_lost_focus) {
  // Stop waiting for a second tap and send a GESTURE_TAP_CANCEL to keep the
  // gesture stream valid.
  if (timeout_handler_->HasTimeout(TAP)) {
    listener_->OnTapCancel(*current_down_event_);
  }
  CancelTaps(is_lost_focus);
  velocity_tracker_.Clear();
  all_pointers_within_slop_regions_ = false;
  still_down_ = false;
}

void GestureDetectorExt::CancelTaps(bool is_lost_focus) {
  timeout_handler_->GetUtils()->Stop(is_lost_focus);
  is_double_tapping_ = false;
  always_in_bigger_tap_region_ = false;
  defer_confirm_single_tap_ = false;
  is_down_candidate_for_repeated_single_tap_ = false;
  current_single_tap_repeat_count_ = 0;
}

void GestureDetectorExt::ActivateLongPressKeepDragTimeout(
    const MotionEvent& ev) {
  timeout_handler_->GetUtils()->Stop(true);
  defer_confirm_single_tap_ = false;
  listener_->OnLongPress(ev);
}

void GestureDetectorExt::StopDragLongPressGesture() {
  timeout_handler_->StopTimeout(DRAG_LONG_PRESS);
}
#endif  // ARKWEB_DRAG_DROP

#if BUILDFLAG(ARKWEB_AI)
void GestureDetectorExt::StopCreateOverlayGesture() {
  timeout_handler_->StopTimeout(CREATE_OVERLAY);
}

void GestureDetectorExt::OnAITextSelected() {}
#endif
}  // namespace ui
