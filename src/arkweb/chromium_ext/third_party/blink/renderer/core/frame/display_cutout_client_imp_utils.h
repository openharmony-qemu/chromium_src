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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_DISPLAY_CUTOUT_CLIENT_IMPL_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_DISPLAY_CUTOUT_CLIENT_IMPL_UTILS_H_

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "ui/gfx/geometry/insets.h"

namespace blink {
class DisplayCutoutClientImpl;
class LocalFrame;

class DisplayCutoutClientImplUtils {
 public:
  DisplayCutoutClientImplUtils(DisplayCutoutClientImpl* impl);
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  static void UpdateSafeArea(LocalFrame* frame);
#endif
  void SetSafeArea(const gfx::Insets& safe_area);

 private:
  raw_ptr<DisplayCutoutClientImpl> impl_;
};
}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_DISPLAY_CUTOUT_CLIENT_IMPL_UTILS_H_