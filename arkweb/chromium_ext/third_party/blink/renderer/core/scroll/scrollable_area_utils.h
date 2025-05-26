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

#ifndef ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLABLE_AREA_UTILS_H_
#define ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLABLE_AREA_UTILS_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/public/mojom/frame/color_scheme.mojom-blink-forward.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/scroll/scrollable_area.h"
#include "third_party/skia/include/core/SkColor.h"

namespace blink {
class ScrollableArea;

class CORE_EXPORT ScrollableAreaUtils {
 public:
  ScrollableAreaUtils(ScrollableArea* scrollable_area);

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  void SetScrollbarColor(SkColor);

  SkColor GetScrollBarColor() const {
    return static_cast<SkColor>(scrollbar_color_);
  }
  void SetOverlayScrollbarColor();

  SkColor scrollbar_color_ = SK_ColorTRANSPARENT;
#endif  // ARKWEB_SCROLLBAR

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool GetScrollable();
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

  ScrollableArea* scrollable_area_ = nullptr;
};
}  // namespace blink

#endif  // ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLABLE_AREA_UTILS_H_
