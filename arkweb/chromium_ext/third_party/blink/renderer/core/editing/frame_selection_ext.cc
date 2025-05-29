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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/editing/frame_selection_ext.h"

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "third_party/blink/public/mojom/scroll/scroll_into_view_params.mojom-blink.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/editing/layout_selection.h"
#include "third_party/blink/renderer/core/editing/position.h"
#include "third_party/blink/renderer/core/editing/selection_editor.h"
#include "third_party/blink/renderer/core/editing/selection_template.h"
#include "third_party/blink/renderer/core/editing/visible_position.h"
#include "third_party/blink/renderer/core/editing/visible_selection.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/layout/hit_test_result.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/scroll/scroll_into_view_util.h"

#if BUILDFLAG(ARKWEB_MENU)
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#include "ui/gfx/geometry/rect_conversions.h"
#define AUTOSCROLL_RANGE_EDGES_SCREEN \
  (90.0f)  // Range of the upper and lower edges of the screen
#define AUTOSCROLL_OFFSET_LIMIT (20.0f)   // Offset of each scroll
#define AUTOSCROLL_OFFSET_INTERVALS (50)  // Rolling interval period
#endif

namespace blink {

FrameSelectionExt::FrameSelectionExt(LocalFrame& frame)
    : FrameSelection(frame) {
  last_autoscroll_time_ = base::TimeTicks::Now();
  selection_autoscroll_enabled_ = true;
}

FrameSelectionExt::~FrameSelectionExt() = default;

#if BUILDFLAG(ARKWEB_CLIPBOARD)
void FrameSelectionExt::ScrollRectToVisualIfClosestEdge(
    const gfx::Point& contents_point) {
  if (!GetFrame() || !GetFrame()->View() || !GetFrame()->GetPage()) {
    return;
  }
  gfx::Point point_in_viewport =
      GetFrame()->View()->FrameToViewport(contents_point);
  int32_t visual_height =
      GetFrame()->GetPage()->GetVisualViewport().Size().height();
  auto device_pixel_ratio = GetDocument().DevicePixelRatio();
  float limit_border = device_pixel_ratio * AUTOSCROLL_RANGE_EDGES_SCREEN;
  float autoscroll_limit = device_pixel_ratio * AUTOSCROLL_OFFSET_LIMIT;

  const Position& start = ComputeVisibleSelectionInDOMTree().Start();
  if (!start.AnchorNode() || !start.AnchorNode()->GetLayoutObject()) {
    return;
  }

  PhysicalRect selection_rect(ComputeRectToScroll(kRevealExtent));
  mojom::blink::ScrollAlignment alignment = ScrollAlignment::ToEdgeIfNeeded();
  auto params =
      scroll_into_view_util::CreateScrollIntoViewParams(alignment, alignment);
  if (!params) {
    return;
  }

  params->scroll_offset_limit = autoscroll_limit;
  // top edge
  if (point_in_viewport.y() < limit_border) {
    params->scroll_offset_limit += autoscroll_limit *
                                   (limit_border - point_in_viewport.y()) /
                                   limit_border;
    params->scroll_offset_limit *= -1;
    // bottom edge
  } else if (point_in_viewport.y() > visual_height - limit_border) {
    params->scroll_offset_limit +=
        autoscroll_limit *
        (point_in_viewport.y() - visual_height + limit_border) / limit_border;
    params->scroll_offset_limit *= 1;
  } else {
    return;
  }
  base::TimeDelta delta = base::TimeTicks::Now() - last_autoscroll_time_;
  if (delta.InMilliseconds() > AUTOSCROLL_OFFSET_INTERVALS) {
    scroll_into_view_util::ScrollRectToVisible(
        *start.AnchorNode()->GetLayoutObject(), selection_rect,
        std::move(params));
    last_autoscroll_time_ = base::TimeTicks::Now();
  }
}
#endif

#if BUILDFLAG(ARKWEB_MENU)
void FrameSelectionExt::MoveRangeSelection(const gfx::Point& point,
                                           bool is_base,
                                           TextGranularity granularity) {
  const VisiblePosition& position = CreateVisiblePosition(
      PositionForContentsPointRespectingEditingBoundary(point, GetFrame()));
  const VisibleSelection& selection = ComputeVisibleSelectionInDOMTree();
  if (is_base) {
    if (position.IsNull() || selection.Focus() == position.DeepEquivalent()) {
      LOG(INFO) << "The left and right handle information has been exchanged.";
      MoveRangeSelectionInternal(selection.AsSelection(), granularity);
      return;
    }
    LOG(INFO) << "Left and right handle information exchange, base:"
              << selection.Focus().ToAnchorTypeAndOffsetString().Utf8()
              << ", extend:"
              << position.DeepEquivalent().ToAnchorTypeAndOffsetString().Utf8()
              << ", is_base:" << is_base;
    MoveRangeSelectionInternal(
        SelectionInDOMTree::Builder()
            .SetBaseAndExtentDeprecated(selection.Focus(),
                                        position.DeepEquivalent())
            .SetAffinity(position.Affinity())
            .Build(),
        granularity);
  } else {
    if (position.IsNull() || selection.Anchor() == position.DeepEquivalent()) {
      LOG(INFO)
          << "Left and right handle information exchange, base:"
          << selection.Focus().ToAnchorTypeAndOffsetString().Utf8()
          << ", extend:"
          << position.DeepEquivalent().ToAnchorTypeAndOffsetString().Utf8()
          << ", is_base:" << is_base;
      MoveRangeSelectionInternal(
          SelectionInDOMTree::Builder()
              .SetBaseAndExtentDeprecated(selection.Focus(),
                                          position.DeepEquivalent())
              .SetAffinity(position.Affinity())
              .Build(),
          granularity);
      return;
    }
    MoveRangeSelectionInternal(
        SelectionInDOMTree::Builder()
            .SetBaseAndExtentDeprecated(selection.Anchor(),
                                        position.DeepEquivalent())
            .SetAffinity(position.Affinity())
            .Build(),
        granularity);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MENU)
void FrameSelectionExt::NeedUpdateCursorLocation() {
  if (!selection_editor_) {
    LOG(ERROR) << "selection_editor_ is nullptr";
    return;
  }
  selection_editor_->NeedRecalculateCursor();
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void FrameSelectionExt::InvalidateSelectionForDrag() {
  auto* view = GetDocument().GetLayoutView();
  if (view) {
    layout_selection_->InvalidateStyleAndPaintForSelection();
  }
}
#endif  //(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_MENU)
gfx::Rect FrameSelectionExt::ClippedSelectionBoundsInRootFrame() const {
  if (!frame_ || !frame_->GetPage() || !frame_->View()) {
    return gfx::Rect();
  }
  const VisibleSelection& selection = ComputeVisibleSelectionInDOMTree();
  if (selection.IsNone() || !selection.IsRange()) {
    return gfx::Rect();
  }

  gfx::Rect viewport_in_root_frame = gfx::ToEnclosingRect(
      frame_->GetPage()->GetVisualViewport().VisibleRect());
  gfx::Rect unclipped_bounds_in_root_frame = frame_->View()->ConvertToRootFrame(
      blink::ToPixelSnappedRect(frame_->Selection().AbsoluteUnclippedBounds()));
  gfx::Rect selection_bounds = gfx::IntersectRects(
      viewport_in_root_frame, unclipped_bounds_in_root_frame);

  selection_bounds.Offset(
      -(int)(frame_->GetPage()->GetVisualViewport().GetScrollOffset().x()),
      -(int)(frame_->GetPage()->GetVisualViewport().GetScrollOffset().y()));
  return gfx::ScaleToEnclosingRect(
      selection_bounds, frame_->GetPage()->GetVisualViewport().Scale());
}
#endif

bool FrameSelectionExt::GetSelectionMarkMaxLengthOverflow() {
  LOG(DEBUG) << "FrameSelectionExt::GetSelectionMarkMaxLengthOverflow "
             << is_max_length_overflow_;
  if (!is_max_length_overflow_) {
    return true;
  }
  is_max_length_overflow_ = false;
  return false;
}
}  // namespace blink
