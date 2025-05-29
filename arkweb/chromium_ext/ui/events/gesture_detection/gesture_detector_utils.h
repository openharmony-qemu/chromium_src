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

#ifndef UI_EVENTS_GESTURE_DETECTION_GESTURE_DETECTOR_UTILS_H_
#define UI_EVENTS_GESTURE_DETECTION_GESTURE_DETECTOR_UTILS_H_

#include "arkweb/build/features/features.h"
#include "ui/events/gesture_detection/gesture_detector.h"

namespace ui {
class GestureDetector;

class TimeoutGestureHandlerUtils {
 public:
  TimeoutGestureHandlerUtils(GestureDetector::TimeoutGestureHandler*);

  ~TimeoutGestureHandlerUtils();
  void Stop(bool is_lost_focus);

  GestureDetector::TimeoutGestureHandler* timeout_gesture_handler_;
};
}  // namespace ui

#endif  // UI_EVENTS_GESTURE_DETECTION_GESTURE_DETECTOR_UTILS_H_
