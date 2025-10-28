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

#include "arkweb/build/features/features.h"
#include "ui/events/velocity_tracker/motion_event_generic.h"
#include "ui/gfx/geometry/point_f.h"

namespace ui {
GestureProviderExt::GestureProviderExt(const Config& config,
                                       GestureProviderClient* client)
    : GestureProvider(config, client) {}

GestureProviderExt::~GestureProviderExt() {}

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void GestureProviderExt::ResetDetection(bool is_lost_focus) {
  MotionEventGeneric generic_cancel_event(
      MotionEvent::Action::CANCEL, base::TimeTicks::Now(), PointerProperties());
  generic_cancel_event.GetUtils()->SetCancelByLostFocus(is_lost_focus);
  OnTouchEvent(generic_cancel_event);
}
#endif

#if BUILDFLAG(ARKWEB_AI)
// LCOV_EXCL_START
void GestureProviderExt::OnAITextSelected() {
  LOG(INFO)
      << "GestureProviderExt::OnAITextSelected, stop arkweb generated gesture.";
  gesture_listener_->GetDetector().AsGestureDetectorExt()->OnAITextSelected();
}
// LCOV_EXCL_STOP
#endif

// LCOV_EXCL_START
void GestureProviderExt::StopArkwebGestures() {
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  gesture_listener_->GetDetector()
      .AsGestureDetectorExt()
      ->StopDragLongPressGesture();
#endif
#if BUILDFLAG(ARKWEB_AI)
  gesture_listener_->GetDetector()
      .AsGestureDetectorExt()
      ->StopCreateOverlayGesture();
#endif
}
// LCOV_EXCL_STOP

}  // namespace ui
