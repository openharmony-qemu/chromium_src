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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/scroll/scrollable_area_utils.h"

#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/layout/layout_box.h"

namespace blink {
ScrollableAreaUtils::ScrollableAreaUtils(ScrollableArea* scrollable_area)
    : scrollable_area_(scrollable_area) {}

#if BUILDFLAG(ARKWEB_SCROLLBAR)
void ScrollableAreaUtils::SetScrollbarColor(SkColor colorValue) {
  if (colorValue != scrollbar_color_) {
    scrollbar_color_ = colorValue;
  }
}

void ScrollableAreaUtils::SetOverlayScrollbarColor() {
  if (scrollable_area_->HasOverlayScrollbars()) {
    // set scrollbar color for overlay color theme
    if (scrollable_area_->overlay_scrollbar_color_scheme__ ==
        static_cast<unsigned>(mojom::blink::ColorScheme::kLight)) {
      SkColor skDark = SkColorSetRGB(0x18, 0x24, 0x31);
      SetScrollbarColor(skDark);
      LOG(INFO) << "ScrollableArea::SetOverlayScrollbarColorScheme"
                << " scrollbar color is RGB:0x18, 0x24, 0x31";
    } else {
      SkColor skLight = SkColorSetRGB(0xFF, 0xFF, 0xFF);
      SetScrollbarColor(skLight);
      LOG(INFO) << "ScrollableArea::SetOverlayScrollbarColorScheme"
                << " scrollbar color is RGB:0xFF, 0xFF, 0xFF";
    }
  }
}
#endif  // ARKWEB_SCROLLBAR

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
bool ScrollableAreaUtils::GetScrollable() {
  DCHECK(scrollable_area_->GetLayoutBox());
  if (scrollable_area_->GetLayoutBox() &&
      scrollable_area_->GetLayoutBox()->GetFrame()) {
    if (!scrollable_area_->GetLayoutBox()
             ->GetFrame()
             ->GetSettings()
             ->GetScrollable()) {
      LOG(DEBUG) << "can not SetScrollOffset, scroll is disabled";
      return false;
    }
  }
  return true;
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
}  // namespace blink
