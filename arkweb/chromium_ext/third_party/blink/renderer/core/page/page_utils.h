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
  
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_PAGE_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_PAGE_UTILS_H_

#include "third_party/blink/renderer/core/page/page.h"
#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
#include "ui/gfx/geometry/insets.h"
#endif

namespace blink {

class Page;

class PageUtils {
public:
  PageUtils(Page* page);
  ~PageUtils();

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
  bool SetScrollbarAvoidAreaTop(double margin);
  bool SetScrollbarAvoidAreaBottom(double margin);
#endif // ARKWEB_SCROLLBAR_AVOID_AREA
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  bool IsInTextDraging();
  bool IsHyperLinkDragging();
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
#if BUILDFLAG(IS_ARKWEB)
  // Ignore shrinks_viewport_contents_to_fit if browser zoom works.
  void ResetPageScaleConstraints(bool constraint_for_mobile);
  bool IsNotPageScaleLimits(PageScaleConstraints new_defaults);
#endif
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  gfx::Insets SafeAreaScaled() const;
  void SetSafeArea(gfx::Insets safe_area) { safe_area_ = safe_area; }
#endif
private:
  raw_ptr<Page> page_;
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  gfx::Insets safe_area_;
#endif
#if BUILDFLAG(IS_ARKWEB)
  bool enable_default_page_scale_limits_update_ = true;
#endif
};
}
#endif  //THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_PAGE_UTILS_H_