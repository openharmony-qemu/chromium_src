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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INPUT_GESTURE_MANAGER_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INPUT_GESTURE_MANAGER_UTILS_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/input/gesture_manager.h"

namespace blink {
class GestureManager;

class GestureManagerUtils {
  friend class GestureManager;

 public:
  GestureManagerUtils(GestureManager* gesture_manager);
#if BUILDFLAG(ARKWEB_AI)
  void CloseAIOverlay(const GestureEventWithHitTestResults& targeted_event);
#endif
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  WebInputEventResult HandleGestureDragLongPress(
      const GestureEventWithHitTestResults& targeted_event);
#endif
#if BUILDFLAG(ARKWEB_AI)
  WebInputEventResult HandleGestureCreateOverlay(
      const GestureEventWithHitTestResults& targeted_event);
  
  void UpdateContextMenuForAI(const HitTestResult& hit_test_result,
                              const HitTestLocation& location,
                              const WebGestureEvent& gesture_event);
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void UpdateContextMenuForFreeCopy(HitTestResult& hit_test_result, HitTestLocation& location);
#endif
  GestureManager* gesture_manager_;
};
}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INPUT_GESTURE_MANAGER_UTILS_H_
