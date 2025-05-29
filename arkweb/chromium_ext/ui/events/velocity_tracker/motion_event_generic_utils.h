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

#ifndef ARKWEB_CHROMIUM_EXT_UI_EVENTS_VELOCITY_TRACKER_MOTION_EVENT_GENERIC_UTILS_H_
#define ARKWEB_CHROMIUM_EXT_UI_EVENTS_VELOCITY_TRACKER_MOTION_EVENT_GENERIC_UTILS_H_

#include "ui/events/velocity_tracker/motion_event_generic.h"

namespace ui {
class MotionEventGeneric;
class MotionEventGenericUtils {
 public:
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  MotionEventGenericUtils(MotionEventGeneric* motion_event_generic);

  bool IsCancelByLostFocus();
  void SetCancelByLostFocus(bool is_lost_focus);

  bool cancel_by_lost_focus_ = false;

  MotionEventGeneric* motion_event_generic_ = nullptr;
#endif
};
}  // namespace ui
#endif  // ARKWEB_CHROMIUM_EXT_UI_EVENTS_VELOCITY_TRACKER_MOTION_EVENT_GENERIC_UTILS_H_
