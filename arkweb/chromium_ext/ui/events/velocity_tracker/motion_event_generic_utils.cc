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

#include "arkweb/chromium_ext/ui/events/velocity_tracker/motion_event_generic_utils.h"

namespace ui {

#if BUILDFLAG(ARKWEB_DRAG_DROP)
MotionEventGenericUtils::MotionEventGenericUtils(
    MotionEventGeneric* motion_event_generic)
    : motion_event_generic_(motion_event_generic) {}

bool MotionEventGenericUtils::IsCancelByLostFocus() {
  return cancel_by_lost_focus_;
}

void MotionEventGenericUtils::SetCancelByLostFocus(bool is_lost_focus) {
  cancel_by_lost_focus_ = is_lost_focus;
}
#endif
}  // namespace ui
