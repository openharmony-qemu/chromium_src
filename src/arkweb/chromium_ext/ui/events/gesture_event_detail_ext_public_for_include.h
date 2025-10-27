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

#ifndef UI_EVENTS_GESTURE_EVENT_DETAILS_PUBLIC_FOR_INCLUDE_H_
#define UI_EVENTS_GESTURE_EVENT_DETAILS_PUBLIC_FOR_INCLUDE_H_

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void set_scroll_update_x(float x) {
    DCHECK_EQ(EventType::kGestureScrollUpdate, type_);
    data_.scroll_update.x = x;
  }

  void set_scroll_update_y(float y) {
    DCHECK_EQ(EventType::kGestureScrollUpdate, type_);
    data_.scroll_update.y = y;
  }

  void set_velocity_x(float xVelocity) {
    DCHECK_EQ(EventType::kScrollFlingStart, type_);
    data_.fling_velocity.x = xVelocity;
  }

  void set_velocity_y(float yVelocity) {
    DCHECK_EQ(EventType::kScrollFlingStart, type_);
    data_.fling_velocity.y = yVelocity;
  }
#endif
#endif  // defined(UI_EVENTS_GESTURE_EVENT_DETAILS_PUBLIC_FOR_INCLUDE_H_)