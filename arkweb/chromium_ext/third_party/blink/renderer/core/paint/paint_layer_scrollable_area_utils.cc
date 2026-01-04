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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/paint_layer_scrollable_area_utils.h"

#include "base/logging.h"
#include "third_party/blink/renderer/core/scroll/scroll_into_view_util.h"
#include "third_party/blink/renderer/core/scroll/scrollable_area.h"

namespace blink {

PaintLayerScrollableAreaUtils::PaintLayerScrollableAreaUtils(
    PaintLayerScrollableArea* scrollable_area)
    : scrollable_area_(scrollable_area) {}

void PaintLayerScrollableAreaUtils::ScaleSize(gfx::Size& visible_size) {
  auto scale_factor = scrollable_area_->AsPaintLayerScrollableAreaExt()
                          ->ComputeVisibleAreaScale();
  visible_size.set_width(visible_size.width() / scale_factor);
  visible_size.set_height(visible_size.height() / scale_factor);
}

// LCOV_EXCL_START
LayoutUnit PaintLayerScrollableAreaUtils::ScaleWidth() {
  auto scale_factor = scrollable_area_->AsPaintLayerScrollableAreaExt()
                          ->ComputeVisibleAreaScale();
  LayoutUnit client_width = LayoutUnit::FromFloatRound(
      scrollable_area_->LayoutContentRect(kIncludeScrollbars)
              .Width()
              .ToFloat() /
          scale_factor -
      scrollable_area_->VerticalScrollbarWidth(kIgnoreOverlayScrollbarSize));
  return client_width;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
LayoutUnit PaintLayerScrollableAreaUtils::ScaleHeight() {
  auto scale_factor = scrollable_area_->AsPaintLayerScrollableAreaExt()
                          ->ComputeVisibleAreaScale();
  LayoutUnit client_height = LayoutUnit::FromFloatRound(
      scrollable_area_->LayoutContentRect(kIncludeScrollbars)
              .Height()
              .ToFloat() /
          scale_factor -
      scrollable_area_->HorizontalScrollbarHeight(kIgnoreOverlayScrollbarSize));
  return client_height;
}
// LCOV_EXCL_STOP

ScrollOffset PaintLayerScrollableAreaUtils::GetScrollOffset(
    const mojom::blink::ScrollIntoViewParamsPtr& params,
    const PhysicalRect& local_expose_rect,
    const PhysicalBoxStrut& scroll_margin) {
  int32_t scroll_offset_limit =
      scrollable_area_->AsPaintLayerScrollableAreaExt()->ClampScrollOffsetLimit(
          params->scroll_offset_limit);
  ScrollOffset target_offset = scroll_into_view_util::GetScrollOffsetToExpose(
      *scrollable_area_, local_expose_rect, scroll_margin,
      *params->align_x.get(), *params->align_y.get(), scroll_offset_limit);
  return target_offset;
}

void PaintLayerScrollableAreaUtils::Trace(Visitor* visitor) const {
  visitor->Trace(scrollable_area_);
}
}  // namespace blink
