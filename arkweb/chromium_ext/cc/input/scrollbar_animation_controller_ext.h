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

#ifndef CC_INPUT_SCROLLBAR_ANIMATION_CONTROLLER_EXT_H_
#define CC_INPUT_SCROLLBAR_ANIMATION_CONTROLLER_EXT_H_

#include "arkweb/build/features/features.h"
#include "base/time/time.h"
#include "cc/cc_export.h"
#include "cc/input/scrollbar_animation_controller.h"

namespace cc {
class CC_EXPORT ScrollbarAnimationControllerExt
    : public ScrollbarAnimationController {
 public:
  ScrollbarAnimationControllerExt(ElementId scroll_element_id,
                                  ScrollbarAnimationControllerClient* client,
                                  base::TimeDelta fade_delay,
                                  base::TimeDelta fade_duration,
                                  float initial_opacity);

  ScrollbarAnimationControllerExt(ElementId scroll_element_id,
                                  ScrollbarAnimationControllerClient* client,
                                  base::TimeDelta fade_delay,
                                  base::TimeDelta fade_duration,
                                  base::TimeDelta thinning_duration,
                                  float initial_opacity,
                                  float idle_thickness_scale);

  ~ScrollbarAnimationControllerExt();
  ScrollbarAnimationControllerExt* AsScrollbarAnimationControllerExt()
      override {
    return this;
  }

  void NeedThinningAnimation();
  bool IsDrawTooFast(base::TimeTicks now, float progress);

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  base::TimeTicks last_animate_time_;
#endif
};
}  // namespace cc
#endif  // CC_INPUT_SCROLLBAR_ANIMATION_CONTROLLER_EXT_H_
