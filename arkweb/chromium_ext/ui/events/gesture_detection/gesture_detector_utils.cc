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

#include "arkweb/chromium_ext/ui/events/gesture_detection/gesture_detector_utils.h"

#include "arkweb/build/features/features.h"

namespace ui {
TimeoutGestureHandlerUtils::TimeoutGestureHandlerUtils(
    GestureDetector::TimeoutGestureHandler* timeout_gesture_handler)
    : timeout_gesture_handler_(timeout_gesture_handler) {}

TimeoutGestureHandlerUtils::~TimeoutGestureHandlerUtils() = default;

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void TimeoutGestureHandlerUtils::Stop(bool is_lost_focus) {
  for (size_t i = TimeoutEvent::SHOW_PRESS; i < TimeoutEvent::TIMEOUT_EVENT_COUNT; ++i) {
    // The longpress show contextmeu on UI will trigger focus changed and
    // resetGestureDetector in GestureListenerManagerImpl.java,
    // then draglongpress gesture will be stopped.
    // so, for draglongpress working, it will be continue in this Stop
    // and ACTION_CANCEL; ACTION_UP will stop draglongpress timer.
    if (i == TimeoutEvent::DRAG_LONG_PRESS && is_lost_focus) {
      continue;
    }
    timeout_gesture_handler_->timeout_timers_[i].Stop();
  }
}
#endif
}  // namespace ui
