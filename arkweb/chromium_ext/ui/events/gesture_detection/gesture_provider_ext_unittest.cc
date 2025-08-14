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

#include "arkweb/chromium_ext/ui/events/gesture_detection/gesture_provider_ext.h"

namespace ui {

TEST_F(GestureProviderTest, GestureDetectorExtCancel) {
  ResetGestureDetection(false);

  should_process_double_tap_events_ = true;
  base::TimeTicks event_time = base::TimeTicks::Now();
  MockMotionEvent event =
      ObtainMotionEvent(event_time, MotionEvent::Action::DOWN);
  EXPECT_TRUE(gesture_provider_->OnTouchEvent(event));

  ResetGestureDetection(true);
}

}  // namespace ui