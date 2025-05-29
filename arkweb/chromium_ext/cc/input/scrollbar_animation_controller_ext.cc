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

#include "arkweb/chromium_ext/cc/input/scrollbar_animation_controller_ext.h"

namespace cc {
#if BUILDFLAG(ARKWEB_SCROLLBAR)
static const float kRefreshDeltaPerFrame = 66.7f;  // floor(1000 / 15f);
#endif

ScrollbarAnimationControllerExt::ScrollbarAnimationControllerExt(
    ElementId scroll_element_id,
    ScrollbarAnimationControllerClient* client,
    base::TimeDelta fade_delay,
    base::TimeDelta fade_duration,
    float initial_opacity)
    : ScrollbarAnimationController(scroll_element_id,
                                   client,
                                   fade_delay,
                                   fade_duration,
                                   initial_opacity) {}

ScrollbarAnimationControllerExt::ScrollbarAnimationControllerExt(
    ElementId scroll_element_id,
    ScrollbarAnimationControllerClient* client,
    base::TimeDelta fade_delay,
    base::TimeDelta fade_duration,
    base::TimeDelta thinning_duration,
    float initial_opacity,
    float idle_thickness_scale)
    : ScrollbarAnimationController(scroll_element_id,
                                   client,
                                   fade_delay,
                                   fade_duration,
                                   thinning_duration,
                                   initial_opacity,
                                   idle_thickness_scale) {}

ScrollbarAnimationControllerExt::~ScrollbarAnimationControllerExt() = default;

#if BUILDFLAG(ARKWEB_SCROLLBAR)
void ScrollbarAnimationControllerExt::NeedThinningAnimation() {
  if (need_thinning_animation_) {
    vertical_controller_->DidRequestShow();
    horizontal_controller_->DidRequestShow();
  }
}

bool ScrollbarAnimationControllerExt::IsDrawTooFast(base::TimeTicks now, float progress) {
  base::TimeDelta delta = now - last_animate_time_;
  // 50ms draw once
  if (!(delta.InMillisecondsF() < kRefreshDeltaPerFrame && progress < 1.0f)) {
    last_animate_time_ = now;
    return true;
  }
  return false;
}
#endif
}  // namespace cc
