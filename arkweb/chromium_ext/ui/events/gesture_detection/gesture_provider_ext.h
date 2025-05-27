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

#ifndef UI_EVENTS_GESTURE_DETECTION_GESTURE_PROVIDER_EXT_H_
#define UI_EVENTS_GESTURE_DETECTION_GESTURE_PROVIDER_EXT_H_

#include "arkweb/build/features/features.h"
#include "ui/events/gesture_detection/gesture_provider.h"

namespace ui {
class GestureProvider;

class GestureProviderExt final : public GestureProvider {
 public:
  GestureProviderExt(const Config& config, GestureProviderClient* client);
  ~GestureProviderExt() override;

  GestureProviderExt* AsGestureProviderExt() override { return this; }

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void ResetDetection(bool is_lost_focus);
#endif
#if BUILDFLAG(ARKWEB_AI)
  void OnAITextSelected();
#endif

  void StopArkwebGestures();
};
}  // namespace ui

#endif  // UI_EVENTS_GESTURE_DETECTION_GESTURE_PROVIDER_EXT_H_
