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

#include "page_utils.h"

#include "base/trace_event/trace_event.h"
#include "base/trace_event/traced_value.h"
#include "cc/input/scrollbar_controller.h"
#include "cc/layers/layer_impl.h"
#include "cc/trees/layer_tree_impl.h"
#include "third_party/blink/renderer/core/frame/page_scale_constraints_set.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/core/page/drag_controller_ext.h"

namespace blink {

PageUtils::PageUtils(Page* page) : page_(page) {}

PageUtils::~PageUtils() {}

#if BUILDFLAG(IS_ARKWEB)
bool PageUtils::IsNotPageScaleLimits(PageScaleConstraints new_defaults) {
  return !enable_default_page_scale_limits_update_ ||
      new_defaults == page_->GetPageScaleConstraintsSet().DefaultConstraints();
}

void PageUtils::ResetPageScaleConstraints(bool constraint_for_mobile) {
  enable_default_page_scale_limits_update_ = true;
  if (constraint_for_mobile) {
    page_->SetDefaultPageScaleLimits(0.25f, 5.0f);
  } else {
    page_->SetDefaultPageScaleLimits(1.0f, 4.0f);
    enable_default_page_scale_limits_update_ = false;
  }

  PageScaleConstraints constraints =
      page_->GetPageScaleConstraintsSet().UserAgentConstraints();
  constraints.minimum_scale =
      page_->GetPageScaleConstraintsSet().DefaultConstraints().minimum_scale;
  constraints.maximum_scale =
      page_->GetPageScaleConstraintsSet().DefaultConstraints().maximum_scale;

  page_->SetUserAgentPageScaleConstraints(constraints);
}
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
bool PageUtils::SetScrollbarAvoidAreaTop(double margin) {
  return page_->GetChromeClient().AsChromeClientExt()->SetScrollbarAvoidAreaTop(margin);
}

bool PageUtils::SetScrollbarAvoidAreaBottom(double margin) {
  return page_->GetChromeClient().AsChromeClientExt()->SetScrollbarAvoidAreaBottom(margin);
}
#endif // ARKWEB_SCROLLBAR_AVOID_AREA

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
gfx::Insets PageUtils::SafeAreaScaled() const {
  if (page_->PageScaleFactor() == 0.0f) {
    return safe_area_;
  }
  return gfx::ScaleToFlooredInsets(safe_area_, 1 / page_->PageScaleFactor());
}
#endif
#if BUILDFLAG(ARKWEB_DRAG_DROP)
bool PageUtils::IsHyperLinkDragging() {
  if (page_->drag_controller_) {
    return page_->drag_controller_->IsHyperLinkDragging();
  }
  return false;
}

bool PageUtils::IsInTextDraging() {
  if (page_->drag_controller_) {
    return page_->drag_controller_->IsInTextDraging();
  }
  return false;
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
}