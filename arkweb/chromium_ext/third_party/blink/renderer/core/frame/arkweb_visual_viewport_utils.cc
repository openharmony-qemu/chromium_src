/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/arkweb_visual_viewport_utils.h"
 
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/paint/paint_layer_scrollable_area.h"
#include "third_party/blink/renderer/core/scroll/scrollbar.h"
 
namespace blink {
 
ArkWebVisualViewportUtils::ArkWebVisualViewportUtils(VisualViewport* viewport)
    : viewport_(viewport) {
  DCHECK(viewport);
}
 
ArkWebVisualViewportUtils::~ArkWebVisualViewportUtils() = default;
 
void ArkWebVisualViewportUtils::
    TriggerUpdateAfterLayoutForNonOverlayScrollbar() {
  if (!viewport_) {
    return;
  }
 
  const Document* document = viewport_->GetDocument();
  if (!document) {
    return;
  }
 
  const LocalFrame* local_frame = document->GetFrame();
  if (!local_frame) {
    return;
  }
 
  LocalFrameView* view = local_frame->View();
  if (!view) {
    return;
  }
 
  PaintLayerScrollableArea* layout_viewport = view->LayoutViewport();
  if (!layout_viewport) {
    return;
  }
 
  bool has_non_overlay_scrollbar = false;
 
  if (Scrollbar* horizontal_scrollbar =
          layout_viewport->HorizontalScrollbar()) {
    if (!horizontal_scrollbar->IsOverlayScrollbar()) {
      has_non_overlay_scrollbar = true;
    }
  }
 
  if (!has_non_overlay_scrollbar) {
    if (Scrollbar* vertical_scrollbar = layout_viewport->VerticalScrollbar()) {
      if (!vertical_scrollbar->IsOverlayScrollbar()) {
        has_non_overlay_scrollbar = true;
      }
    }
  }
 
  if (has_non_overlay_scrollbar) {
    view->SetNeedsLayout();
  }
}
void ArkWebVisualViewportUtils::Trace(Visitor* visitor) const {
  visitor->Trace(viewport_);
}
}  // namespace blink