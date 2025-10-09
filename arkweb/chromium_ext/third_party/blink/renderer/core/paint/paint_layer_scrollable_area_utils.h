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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_LAYER_SCROLLABLE_AREA_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_LAYER_SCROLLABLE_AREA_UTILS_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/public/mojom/scroll/scroll_into_view_params.mojom-blink-forward.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/layout/geometry/box_strut.h"
#include "third_party/blink/renderer/core/layout/geometry/physical_rect.h"
#include "third_party/blink/renderer/core/paint/paint_layer_scrollable_area.h"
#include "third_party/blink/renderer/platform/geometry/layout_unit.h"
#include "third_party/blink/renderer/platform/graphics/overlay_scrollbar_clip_behavior.h"
#include "ui/gfx/geometry/size.h"

namespace blink {

class PaintLayerScrollableArea;

class PaintLayerScrollableAreaUtils {
 public:
  explicit PaintLayerScrollableAreaUtils(PaintLayerScrollableArea*);

  void ScaleSize(gfx::Size& visible_size);
  LayoutUnit ScaleWidth();
  LayoutUnit ScaleHeight();
  ScrollOffset GetScrollOffset(
      const mojom::blink::ScrollIntoViewParamsPtr& params,
      const PhysicalRect& local_expose_rect,
      const PhysicalBoxStrut& scroll_margin);

 private:
  PaintLayerScrollableArea* scrollable_area_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_LAYER_SCROLLABLE_AREA_UTILS_H_
