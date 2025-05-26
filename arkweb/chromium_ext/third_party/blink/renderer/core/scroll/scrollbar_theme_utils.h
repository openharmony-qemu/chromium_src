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

#ifndef ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLBAR_THEME_UTILS_H_
#define ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLBAR_THEME_UTILS_H_

#include "third_party/blink/renderer/core/scroll/scrollbar_theme.h"
#include "third_party/blink/renderer/core/scroll/scroll_types.h"

namespace blink {
class ScrollbarTheme;

class CORE_EXPORT ScrollbarThemeUtils {
 public:
  ScrollbarThemeUtils(ScrollbarTheme* scrollbar_theme);
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
  void OffsetPointForHitTest(const Scrollbar& scrollbar,
                             gfx::Point& in_out_test_position);

  ScrollbarPart HitTestUtils(const Scrollbar&, const gfx::Point&);
#endif  // ARKWEB_SCROLLBAR_AVOID_AREA
  ScrollbarTheme* scrollbar_theme_ = nullptr;
};
}  // namespace blink

#endif  // ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLBAR_THEME_UTILS_H_
