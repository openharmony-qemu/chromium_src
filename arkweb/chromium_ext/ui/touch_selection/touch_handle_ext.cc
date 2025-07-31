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

#include "arkweb/chromium_ext/ui/touch_selection/touch_handle_ext.h"

namespace ui {
TouchHandleExt::TouchHandleExt(TouchHandleClient* client,
                               TouchHandleOrientation orientation,
                               const gfx::RectF& viewport_rect)
    : TouchHandle(client, orientation, viewport_rect) {}

TouchHandleExt::~TouchHandleExt() {}

#if BUILDFLAG(ARKWEB_MENU)
void TouchHandleExt::SetEdge(const gfx::PointF& top, const gfx::PointF& bottom) {
  drawable_->SetEdge(top, bottom);
}

// LCOV_EXCL_START
void TouchHandleExt::ResetPositionAfterDragEnd() {
  if (!is_visible_ || !drawable_) {
    return;
  }

  drawable_->SetOrigin(ComputeHandleOrigin());
}
// LCOV_EXCL_STOP

bool TouchHandleExt::SetFocus(const gfx::PointF& top, const gfx::PointF& bottom) {
  if (focus_bottom_.y() < focus_top_.y()) {
    if (focus_top_ == bottom && focus_bottom_ == top)
      return true;
    focus_top_ = bottom;
    focus_bottom_ = top;
    SetUpdateLayoutRequired();
    return true;
  }
  return false;
}

void TouchHandleExt::SetInsertHandleAlpha(float alpha) {
  alpha = std::clamp(alpha, 0.0f, 1.0f);
  if (alpha_ == alpha)
    return;
  alpha_ = alpha;
  drawable_->SetAlpha(alpha);
}
#endif
}  // namespace ui
