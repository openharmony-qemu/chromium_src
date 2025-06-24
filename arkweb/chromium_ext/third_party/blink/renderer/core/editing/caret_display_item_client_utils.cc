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
#include "third_party/blink/renderer/core/editing/local_caret_rect.h"
#include "third_party/blink/renderer/core/exported/web_view_impl.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#include "third_party/blink/renderer/core/layout/layout_block.h"
#include "third_party/blink/renderer/core/paint/paint_auto_dark_mode.h"
#include "third_party/blink/renderer/platform/graphics/dark_mode_filter.h"
#include "third_party/blink/renderer/platform/graphics/graphics_context.h"
namespace blink {

CaretDisplayItemClientUtils::CaretDisplayItemClientUtils(
    CaretDisplayItemClient* caret_display_item_client)
    : caret_display_item_client_(caret_display_item_client) {}

#if BUILDFLAG(ARKWEB_MENU)
bool CaretDisplayItemClientUtils::GetBlinkCaretRect(GraphicsContext& context,
                                                    const gfx::Rect& paint_rect,
                                                    LayoutBlock* layout_block,
                                                    const Color& color) {
  if (layout_block == nullptr) {
    return false;
  }

  LocalFrameView* frameView = layout_block->GetFrameView();
  if (!frameView || !frameView->GetChromeClient()) {
    return false;
  }

  WebView* webview = frameView->GetChromeClient()->GetWebView();
  if (!webview) {
    return false;
  }

  LocalFrame& frame = frameView->GetFrame();
  if (!frame.GetPage()) {
    return false;
  }
  VisualViewport& visual_viewport = frame.GetPage()->GetVisualViewport();
  if (!visual_viewport.IsActiveViewport()) {
    return false;
  }

  float scale = visual_viewport.Scale();
  float ratio = webview->ZoomFactorForViewportLayout();
  gfx::RectF paint_rectf(paint_rect);
  paint_rectf.set_width(2.0 * ratio / scale);
  paint_rectf.set_x(paint_rectf.x() - (ratio * 0.5));
  context.FillRect(paint_rectf, color,
                   PaintAutoDarkMode(layout_block->StyleRef(),
                                     DarkModeFilter::ElementRole::kForeground));
  return true;
}
#endif  // BUILDFLAG(ARKWEB_MENU)
}  // namespace blink
