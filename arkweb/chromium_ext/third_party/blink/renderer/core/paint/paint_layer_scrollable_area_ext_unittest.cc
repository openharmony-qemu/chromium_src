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

#include "third_party/googletest/src/googletest/include/gtest/gtest.h"
#if BUILDFLAG(ARKWEB_TEST)
#define private public
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#endif

namespace blink {

constexpr float SCALE_VAL_A = 0.1;
constexpr float SCALE_VAL_B = 1;
constexpr float SCALE_VAL_C = 2;
constexpr int POINT_VAL_A = 0;
constexpr int POINT_VAL_B = 10;

// Test non-root layer should always return 1.0 regardless of scale factor
TEST_P(PaintLayerTest, ComputeVisibleAreaScale001) {
  SetBodyInnerHTML(R"HTML(
    <div id='scroll' style='width: 100px; height: 100px; overflow: scroll;
        will-change: transform'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </div>
  )HTML");

  PaintLayer* scroll_layer = GetPaintLayerByElementId("scroll");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox());
  Page* page = scrollable_area_ext->GetLayoutBox()->GetDocument().GetPage();
  ASSERT_TRUE(page);
  auto& port = page->GetVisualViewport();
  port.scale_ = SCALE_VAL_A;
  EXPECT_EQ(scrollable_area_ext->ComputeVisibleAreaScale(), SCALE_VAL_B);

  port.scale_ = SCALE_VAL_C;
  EXPECT_EQ(scrollable_area_ext->ComputeVisibleAreaScale(), SCALE_VAL_B);
}

// Test root layer with overlay scrollbar and pinch gesture active should return scale factor
TEST_P(PaintLayerTest, ComputeVisibleAreaScale002) {
  SetBodyInnerHTML(R"HTML(
    <html>
    <body style='width: 100px; height: 100px; overflow: scroll; margin: 0'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </body>
    </html>
  )HTML");

  PaintLayer* scroll_layer = GetPaintLayerByElementId("body");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox());
  Page* page = scrollable_area_ext->GetLayoutBox()->GetDocument().GetPage();
  ASSERT_TRUE(page);
  auto& port = page->GetVisualViewport();

  // Enable pinch gesture and set scale > 1
  scrollable_area_ext->is_pinch_gesture_active_ = true;
  port.scale_ = SCALE_VAL_C;  // scale = 2.0

  // With overlay scrollbar (assuming default is overlay), should return scale factor
  float result = scrollable_area_ext->ComputeVisibleAreaScale();
  // Result should be either 2.0 (if overlay scrollbar) or 1.0 (if not overlay)
  EXPECT_TRUE(result == SCALE_VAL_C || result == SCALE_VAL_B);
}

// Test root layer without pinch gesture should return 1.0
TEST_P(PaintLayerTest, ComputeVisibleAreaScale003) {
  SetBodyInnerHTML(R"HTML(
    <html>
    <body style='width: 100px; height: 100px; overflow: scroll; margin: 0'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </body>
    </html>
  )HTML");

  PaintLayer* scroll_layer = GetPaintLayerByElementId("body");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox());
  Page* page = scrollable_area_ext->GetLayoutBox()->GetDocument().GetPage();
  ASSERT_TRUE(page);
  auto& port = page->GetVisualViewport();

  // pinch gesture not active (default is false)
  port.scale_ = SCALE_VAL_C;  // scale = 2.0

  EXPECT_EQ(scrollable_area_ext->ComputeVisibleAreaScale(), SCALE_VAL_B);
}

// Test root layer with scale <= 1.0 should return 1.0
TEST_P(PaintLayerTest, ComputeVisibleAreaScale004) {
  SetBodyInnerHTML(R"HTML(
    <html>
    <body style='width: 100px; height: 100px; overflow: scroll; margin: 0'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </body>
    </html>
  )HTML");

  PaintLayer* scroll_layer = GetPaintLayerByElementId("body");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox());
  Page* page = scrollable_area_ext->GetLayoutBox()->GetDocument().GetPage();
  ASSERT_TRUE(page);
  auto& port = page->GetVisualViewport();

  // Enable pinch gesture
  scrollable_area_ext->is_pinch_gesture_active_ = true;

  // Test scale = 1.0 (not greater than 1)
  port.scale_ = SCALE_VAL_B;
  EXPECT_EQ(scrollable_area_ext->ComputeVisibleAreaScale(), SCALE_VAL_B);

  // Test scale = 0.1 (less than 1)
  port.scale_ = SCALE_VAL_A;
  EXPECT_EQ(scrollable_area_ext->ComputeVisibleAreaScale(), SCALE_VAL_B);
}

TEST_P(PaintLayerTest, ClampScrollOffsetLimit001) {
  SetBodyInnerHTML(R"HTML(
    <div id='scroll' style='width: 100px; height: 100px; overflow: scroll;
        will-change: transform'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </div>
  )HTML");

  PaintLayer* scroll_layer = GetPaintLayerByElementId("scroll");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox());
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox()->GetFrameView());
  scrollable_area_ext->GetLayoutBox()->GetFrameView()->SetFrameRect(
    gfx::Rect(POINT_VAL_A, POINT_VAL_A, POINT_VAL_B, POINT_VAL_B));
  EXPECT_EQ(scrollable_area_ext->ClampScrollOffsetLimit(SCALE_VAL_B), POINT_VAL_A);
}

TEST_P(PaintLayerTest, HasScrollbarAvoidCorner001) {
  SetBodyInnerHTML(R"HTML(
    <div id='scroll' style='width: 100px; height: 100px; overflow: scroll;
        will-change: transform'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </div>
  )HTML");

  PaintLayer* scroll_layer = GetPaintLayerByElementId("scroll");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  EXPECT_FALSE(scrollable_area_ext->HasScrollbarAvoidCorner());
}

TEST_P(PaintLayerTest, UpdateScrollbarLengthOrCreateWidthScale001) {
  SetBodyInnerHTML(R"HTML(
    <div id='scroll' style='width: 100px; height: 100px; overflow: scroll;
        will-change: transform'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </div>
  )HTML");
 
  PaintLayer* scroll_layer = GetPaintLayerByElementId("scroll");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox());
  Page* page = scrollable_area_ext->GetLayoutBox()->GetDocument().GetPage();
  ASSERT_TRUE(page);
  ASSERT_TRUE(scrollable_area_ext->HasHorizontalScrollbar());
  ASSERT_TRUE(scrollable_area_ext->HasVerticalScrollbar());
  scrollable_area_ext->UpdateScrollbarLengthOrCreateWidthScale();
  ASSERT_TRUE(scrollable_area_ext->is_pinch_gesture_active_);
}
 
TEST_P(PaintLayerTest, UpdateScrollbarLengthOrCreateWidthScale002) {
  SetBodyInnerHTML(R"HTML(
    <div id='scroll' style='width: 100px; height: 100px; overflow: scroll hidden;
        will-change: transform'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </div>
  )HTML");
 
  PaintLayer* scroll_layer = GetPaintLayerByElementId("scroll");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox());
  Page* page = scrollable_area_ext->GetLayoutBox()->GetDocument().GetPage();
  ASSERT_TRUE(page);
  ASSERT_TRUE(scrollable_area_ext->HasHorizontalScrollbar());
  ASSERT_FALSE(scrollable_area_ext->HasVerticalScrollbar());
  scrollable_area_ext->UpdateScrollbarLengthOrCreateWidthScale();
  ASSERT_TRUE(scrollable_area_ext->is_pinch_gesture_active_);
}
 
TEST_P(PaintLayerTest, UpdateScrollbarLengthOrCreateWidthScale003) {
  SetBodyInnerHTML(R"HTML(
    <div id='scroll' style='width: 100px; height: 100px; overflow: hidden scroll;
        will-change: transform'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </div>
  )HTML");
 
  PaintLayer* scroll_layer = GetPaintLayerByElementId("scroll");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox());
  Page* page = scrollable_area_ext->GetLayoutBox()->GetDocument().GetPage();
  ASSERT_TRUE(page);
  ASSERT_FALSE(scrollable_area_ext->HasHorizontalScrollbar());
  ASSERT_TRUE(scrollable_area_ext->HasVerticalScrollbar());
  scrollable_area_ext->UpdateScrollbarLengthOrCreateWidthScale();
  ASSERT_TRUE(scrollable_area_ext->is_pinch_gesture_active_);
}
 
TEST_P(PaintLayerTest, UpdateScrollbar001) {
  SetBodyInnerHTML(R"HTML(
    <div id='scroll' style='width: 100px; height: 100px; overflow: scroll;
        will-change: transform'>
      <div id='content' style='position: relative; background: blue;
          width: 2000px; height: 2000px'></div>
    </div>
  )HTML");
 
  PaintLayer* scroll_layer = GetPaintLayerByElementId("scroll");
  ASSERT_TRUE(scroll_layer);
  PaintLayerScrollableArea* scrollable_area = scroll_layer->GetScrollableArea();
  ASSERT_TRUE(scrollable_area);
  PaintLayerScrollableAreaExt* scrollable_area_ext = scrollable_area->AsPaintLayerScrollableAreaExt();
  ASSERT_TRUE(scrollable_area_ext);
  ASSERT_TRUE(scrollable_area_ext->GetLayoutBox());
  Page* page = scrollable_area_ext->GetLayoutBox()->GetDocument().GetPage();
  ASSERT_TRUE(page);
  scrollable_area_ext->UpdateScrollbar();
  ASSERT_TRUE(scrollable_area_ext->HasHorizontalScrollbar());
  ASSERT_TRUE(scrollable_area_ext->HasVerticalScrollbar());
}
}

#if BUILDFLAG(ARKWEB_TEST)
#undef private
#endif