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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_LAYER_SCROLLABLE_AREA_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_LAYER_SCROLLABLE_AREA_EXT_H_

#include "third_party/blink/renderer/core/paint/paint_layer_scrollable_area.h"

#include "third_party/blink/renderer/core/paint/paint_layer.h"
#include "ui/gfx/geometry/rect.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace gfx {
class RectF;
}

namespace blink {
class PaintLayerScrollableArea;
class PaintLayer;

class PaintLayerScrollableAreaExt : public PaintLayerScrollableArea {
 public:
  explicit PaintLayerScrollableAreaExt(PaintLayer&);
  ~PaintLayerScrollableAreaExt();

  PaintLayerScrollableAreaExt* AsPaintLayerScrollableAreaExt() override { return this; }

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  float ComputeVisibleAreaScale() const;
  void UpdateScrollbarLengthOrCreateWidthScale();
  void UpdateScrollbar();
  void UpdateScrollbarProportions() override;
  gfx::Rect RectForHorizontalScrollbar() const override;
  gfx::Rect RectForVerticalScrollbar() const override;
  int32_t ClampScrollOffsetLimit(int32_t scroll_offset_limit) const;
  bool is_pinch_gesture_active_ = false;
#endif  // ARKWEB_SCROLLBAR
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
  bool SetScrollbarAvoidAreaTop(double margin);
  bool SetScrollbarAvoidAreaBottom(double margin);
  double GetScrollbarAvoidAreaTop() const override;
  double GetScrollbarAvoidAreaBottom() const override;
#endif  // ARKWEB_SCROLLBAR_AVOID_AREA
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  bool HasScrollbarAvoidCorner() const override;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void ComputeScrollbarExistence(
      bool& needs_horizontal_scrollbar,
      bool& needs_vertical_scrollbar,
      ComputeScrollbarExistenceOption = kDependsOnOverflow) const override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
  mutable double scrollbar_avoid_area_top_ = 0.0f;
  mutable double scrollbar_avoid_area_bottom_ = 0.0f;
  mutable double scrollbar_avoid_area_valid_top_ = 0.0f;
  mutable double scrollbar_avoid_area_valid_bottom_ = 0.0f;
#endif // ARKWEB_SCROLLBAR_AVOID_AREA
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_LAYER_SCROLLABLE_AREA_EXT_H_
