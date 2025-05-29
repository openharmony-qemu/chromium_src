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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_view_utils.h"

#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/editing/frame_selection.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/layout/layout_box.h"
#include "third_party/blink/renderer/core/layout/layout_view.h"
#include "third_party/blink/renderer/core/page/focus_controller.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_fetcher.h"
#include "third_party/blink/renderer/platform/widget/frame_widget.h"
#include "ui/gfx/geometry/rect_f.h"

namespace blink {
LocalFrameViewUtils::LocalFrameViewUtils(LocalFrameView* local_frame_view)
    : local_frame_view_(local_frame_view) {}

#if BUILDFLAG(ARKWEB_MENU)
void LocalFrameViewUtils::UpdateCompositedSelectionIfNeed() {
  Page* page = local_frame_view_->GetFrame().GetPage();
  if (!page) {
    return;
  }

  LocalFrame* focus_frame = page->GetFocusController().FocusedFrame();
  LocalFrame* local_frame =
      (focus_frame && (focus_frame->LocalFrameRoot() ==
                       local_frame_view_->GetFrame().LocalFrameRoot()))
          ? focus_frame
          : nullptr;
  if (!local_frame) {
    return;
  }
  gfx::Rect clipped_selection_bounds =
      local_frame->Selection()
          .AsFrameSelectionExt()
          ->ClippedSelectionBoundsInRootFrame();
  if (auto* frame_widget = local_frame->GetWidgetForLocalRoot()) {
    frame_widget->RegisterClippedVisualViewportSelectionBounds(
        clipped_selection_bounds);
  }
}
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
void LocalFrameViewUtils::PerformLayoutOnPreload(Document* document) {
  if (document == nullptr) {
    LOG(ERROR) << "document is nullptr";
    return;
  }
  document->Fetcher()->UpdateAllImageResourcePriorities();
  if (!document->Url().GetString().Utf8().empty()) {
    LayoutBox* body_box = nullptr;
    const LayoutBox* html_box = nullptr;
    int body_height = 0;
    Element* document_element = document->documentElement();
    if (document_element && document_element->GetLayoutObject()) {
      html_box = DynamicTo<LayoutBox>(document_element->GetLayoutObject());
    }
    if (html_box && IsA<LayoutBox>(html_box) && html_box->SlowFirstChild()) {
      body_box = DynamicTo<LayoutBox>(html_box->SlowFirstChild());
    }
    if (body_box && IsA<LayoutBox>(body_box) &&
        (body_box->GetLayoutResults().size() > 0)) {
      body_height = body_box->ScrollHeight().ToInt();
    } else {
      body_height = -1;
    }
    if (body_height >
        (local_frame_view_->GetFrame()
             .ContentLayoutObject()
             ->ViewRect()
             .Height()
             .ToInt() *
         1.5)) {  // only record preload info in 1.5 viewport height
      document->Fetcher()->UpdateAllowPreloadRecord(false);
    } else {
      document->Fetcher()->UpdateAllowPreloadRecord(true);
    }
  }
}
#endif
}  // namespace blink
