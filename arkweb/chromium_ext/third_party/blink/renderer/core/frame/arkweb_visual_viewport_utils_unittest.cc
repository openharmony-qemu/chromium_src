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
 
#include "arkweb/build/features/features.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"
#if BUILDFLAG(ARKWEB_TEST)
#define private public
#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/arkweb_visual_viewport_utils.h"
#endif
 
namespace blink {
 
namespace {
#if BUILDFLAG(ARKWEB_TEST)
TEST_P(VisualViewportTest,
       TriggerUpdateAfterLayoutForNonOverlayScrollbar_NullViewport) {
  auto utils = MakeGarbageCollected<ArkWebVisualViewportUtils>(nullptr);
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
}
 
TEST_P(VisualViewportTest,
       TriggerUpdateAfterLayoutForNonOverlayScrollbar_WithNonOverlayScrollbar) {
  InitializeWithDesktopSettings();
  WebView()->MainFrameViewWidget()->Resize(gfx::Size(400, 300));
  WebView()->ResizeWithBrowserControls(
      gfx::Size(400, 300), gfx::Size(400, 300),
      WebView()->GetBrowserControls().Params());
  UpdateAllLifecyclePhases();
 
  NavigateTo("about:blank");
  ForceFullCompositingUpdate();
 
  LocalFrameView* view = WebView()->MainFrameImpl()->GetFrameView();
  ASSERT_NE(view, nullptr);
 
  PaintLayerScrollableArea* layout_viewport = view->LayoutViewport();
  ASSERT_NE(layout_viewport, nullptr);
 
  bool has_scrollbar_before = layout_viewport->HasScrollbar();
  ASSERT_TRUE(has_scrollbar_before);
 
  bool needs_layout_before = view->NeedsLayout();
 
  VisualViewport& visual_viewport = GetFrame()->GetPage()->GetVisualViewport();
  auto utils =
      MakeGarbageCollected<ArkWebVisualViewportUtils>(&visual_viewport);
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
 
  bool needs_layout_after = view->NeedsLayout();
  ASSERT_TRUE(needs_layout_after);
 
  if (!needs_layout_before) {
    ASSERT_TRUE(needs_layout_after);
  }
}
 
TEST_P(VisualViewportTest,
       TriggerUpdateAfterLayoutForNonOverlayScrollbar_WithOverlayScrollbar) {
  InitializeWithDesktopSettings();
  WebView()->MainFrameViewWidget()->Resize(gfx::Size(400, 300));
  WebView()->ResizeWithBrowserControls(
      gfx::Size(400, 300), gfx::Size(400, 300),
      WebView()->GetBrowserControls().Params());
  UpdateAllLifecyclePhases();
 
  NavigateTo("about:blank");
  ForceFullCompositingUpdate();
 
  LocalFrameView* view = WebView()->MainFrameImpl()->GetFrameView();
  ASSERT_NE(view, nullptr);
 
  PaintLayerScrollableArea* layout_viewport = view->LayoutViewport();
  ASSERT_NE(layout_viewport, nullptr);
 
  bool needs_layout_before = view->NeedsLayout();
 
  VisualViewport& visual_viewport = GetFrame()->GetPage()->GetVisualViewport();
  auto utils =
      MakeGarbageCollected<ArkWebVisualViewportUtils>(&visual_viewport);
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
 
  bool needs_layout_after = view->NeedsLayout();
 
  ASSERT_EQ(needs_layout_before, needs_layout_after);
}
 
TEST_P(VisualViewportTest,
       TriggerUpdateAfterLayoutForNonOverlayScrollbar_WithoutScrollbar) {
  InitializeWithDesktopSettings();
  WebView()->MainFrameViewWidget()->Resize(gfx::Size(400, 300));
  WebView()->ResizeWithBrowserControls(
      gfx::Size(400, 300), gfx::Size(400, 300),
      WebView()->GetBrowserControls().Params());
  UpdateAllLifecyclePhases();
 
  NavigateTo("about:blank");
  ForceFullCompositingUpdate();
 
  LocalFrameView* view = WebView()->MainFrameImpl()->GetFrameView();
  ASSERT_NE(view, nullptr);
 
  PaintLayerScrollableArea* layout_viewport = view->LayoutViewport();
  ASSERT_NE(layout_viewport, nullptr);
 
  bool has_scrollbar_before = layout_viewport->HasScrollbar();
  ASSERT_FALSE(has_scrollbar_before);
 
  bool needs_layout_before = view->NeedsLayout();
 
  VisualViewport& visual_viewport = GetFrame()->GetPage()->GetVisualViewport();
  auto utils =
      MakeGarbageCollected<ArkWebVisualViewportUtils>(&visual_viewport);
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
 
  bool needs_layout_after = view->NeedsLayout();
 
  ASSERT_EQ(needs_layout_before, needs_layout_after);
}
 
TEST_P(VisualViewportTest,
       TriggerUpdateAfterLayoutForNonOverlayScrollbar_MultipleCalls) {
  InitializeWithDesktopSettings();
  WebView()->MainFrameViewWidget()->Resize(gfx::Size(400, 300));
  WebView()->ResizeWithBrowserControls(
      gfx::Size(400, 300), gfx::Size(400, 300),
      WebView()->GetBrowserControls().Params());
  UpdateAllLifecyclePhases();
 
  NavigateTo("about:blank");
  ForceFullCompositingUpdate();
 
  LocalFrameView* view = WebView()->MainFrameImpl()->GetFrameView();
  ASSERT_NE(view, nullptr);
 
  PaintLayerScrollableArea* layout_viewport = view->LayoutViewport();
  ASSERT_NE(layout_viewport, nullptr);
 
  bool has_scrollbar = layout_viewport->HasScrollbar();
  ASSERT_TRUE(has_scrollbar);
 
  VisualViewport& visual_viewport = GetFrame()->GetPage()->GetVisualViewport();
  auto utils =
      MakeGarbageCollected<ArkWebVisualViewportUtils>(&visual_viewport);
 
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
  ASSERT_TRUE(view->NeedsLayout());
 
  UpdateAllLifecyclePhases();
 
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
  ASSERT_TRUE(view->NeedsLayout());
 
  UpdateAllLifecyclePhases();
 
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
  ASSERT_TRUE(view->NeedsLayout());
}
 
TEST_P(VisualViewportTest,
       TriggerUpdateAfterLayoutForNonOverlayScrollbar_LifecycleUpdate) {
  InitializeWithDesktopSettings();
  WebView()->MainFrameViewWidget()->Resize(gfx::Size(400, 300));
  WebView()->ResizeWithBrowserControls(
      gfx::Size(400, 300), gfx::Size(400, 300),
      WebView()->GetBrowserControls().Params());
  UpdateAllLifecyclePhases();
 
  NavigateTo("about:blank");
  ForceFullCompositingUpdate();
 
  LocalFrameView* view = WebView()->MainFrameImpl()->GetFrameView();
  ASSERT_NE(view, nullptr);
 
  PaintLayerScrollableArea* layout_viewport = view->LayoutViewport();
  ASSERT_NE(layout_viewport, nullptr);
 
  bool has_scrollbar = layout_viewport->HasScrollbar();
  ASSERT_TRUE(has_scrollbar);
 
  VisualViewport& visual_viewport = GetFrame()->GetPage()->GetVisualViewport();
  auto utils =
      MakeGarbageCollected<ArkWebVisualViewportUtils>(&visual_viewport);
 
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
 
  ASSERT_TRUE(view->NeedsLayout());
 
  UpdateAllLifecyclePhases();
 
  ASSERT_FALSE(view->NeedsLayout());
 
  bool has_scrollbar_after = layout_viewport->HasScrollbar();
  ASSERT_TRUE(has_scrollbar_after);
}
 
TEST_P(VisualViewportTest,
       TriggerUpdateAfterLayoutForNonOverlayScrollbar_HorizontalScrollbarOnly) {
  InitializeWithDesktopSettings();
  WebView()->MainFrameViewWidget()->Resize(gfx::Size(400, 300));
  WebView()->ResizeWithBrowserControls(
      gfx::Size(400, 300), gfx::Size(400, 300),
      WebView()->GetBrowserControls().Params());
  UpdateAllLifecyclePhases();
 
  NavigateTo("aboutabout:blank");
  ForceFullCompositingUpdate();
 
  LocalFrameView* view = WebView()->MainFrameImpl()->GetFrameView();
  ASSERT_NE(view, nullptr);
 
  PaintLayerScrollableArea* layout_viewport = view->LayoutViewport();
  ASSERT_NE(layout_viewport, nullptr);
 
  bool has_horizontal_scrollbar = layout_viewport->HasHorizontalScrollbar();
  ASSERT_TRUE(has_horizontal_scrollbar);
 
  VisualViewport& visual_viewport = GetFrame()->GetPage()->GetVisualViewport();
  auto utils =
      MakeGarbageCollected<ArkWebVisualViewportUtils>(&visual_viewport);
 
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
 
  ASSERT_TRUE(view->NeedsLayout());
}
 
TEST_P(VisualViewportTest,
       TriggerUpdateAfterLayoutForNonOverlayScrollbar_VerticalScrollbarOnly) {
  InitializeWithDesktopSettings();
  WebView()->MainFrameViewWidget()->Resize(gfx::Size(400, 300));
  WebView()->ResizeWithBrowserControls(
      gfx::Size(400, 300), gfx::Size(400, 300),
      WebView()->GetBrowserControls().Params());
  UpdateAllLifecyclePhases();
 
  NavigateTo("about:blank");
  ForceFullCompositingUpdate();
 
  LocalFrameView* view = WebView()->MainFrameImpl()->GetFrameView();
  ASSERT_NE(view, nullptr);
 
  PaintLayerScrollableArea* layout_viewport = view->LayoutViewport();
  ASSERT_NE(layout_viewport, nullptr);
 
  bool has_vertical_scrollbar = layout_viewport->HasVerticalScrollbar();
  ASSERT_TRUE(has_vertical_scrollbar);
 
  VisualViewport& visual_viewport = GetFrame()->GetPage()->GetVisualViewport();
  auto utils =
      MakeGarbageCollected<ArkWebVisualViewportUtils>(&visual_viewport);
 
  utils->TriggerUpdateAfterLayoutForNonOverlayScrollbar();
 
  ASSERT_TRUE(view->NeedsLayout());
}
#endif
}  // namespace
}  // namespace blink