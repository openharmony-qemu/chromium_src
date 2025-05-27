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

#ifndef UI_EVENTS_GESTURE_DETECTION_GESTURE_DETECTOR_EXT_H_
#define UI_EVENTS_GESTURE_DETECTION_GESTURE_DETECTOR_EXT_H_

#include "arkweb/build/features/features.h"
#include "ui/events/gesture_detection/gesture_detector.h"

namespace ui {
class GestureDetector;

class GestureDetectorExt final : public GestureDetector {
 public:
  GestureDetectorExt(const Config& config,
                     GestureListener* listener,
                     DoubleTapListener* optional_double_tap_listener);

  ~GestureDetectorExt() override;

  GestureDetectorExt* AsGestureDetectorExt() override { return this; }

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void Cancel(bool is_lost_focus);
  void CancelTaps(bool is_lost_focus);
  void ActivateLongPressKeepDragTimeout(const MotionEvent& ev);
  void set_draglongpress_enabled(bool enabled) {
    draglongpress_enabled_ = enabled;
  }
  void StopDragLongPressGesture();
#endif  // ARKWEB_DRAG_DROP

#if BUILDFLAG(ARKWEB_AI)
  void StopCreateOverlayGesture();
  void OnAITextSelected();
#endif  // ARKWEB_AI
};
}  // namespace ui

#endif  // UI_EVENTS_GESTURE_DETECTION_GESTURE_DETECTOR_EXT_H_
