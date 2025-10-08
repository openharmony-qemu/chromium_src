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

}

#if BUILDFLAG(ARKWEB_TEST)
#undef private
#endif