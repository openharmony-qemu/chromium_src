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

#define private public
#define protected public
#include "third_party/blink/renderer/core/layout/hit_test_result.h"
#include "third_party/blink/renderer/core/layout/hit_test_result_utils.cc"
#include "third_party/blink/renderer/core/page/drag_controller_utils.h"
#undef private
#undef protected

namespace blink {

namespace {
constexpr char BASE64_RED_IMAGE_DATA_3_3[] =
    "iVBORw0KGgoAAAANSUhEUgAAAAMAAAADCAIAAADZSiLoAAAAEUlEQVR4n"
    "GP8zwAFTDAGEgsAHVsBBeg3qMUAAAAASUVORK5CYII=";
}

class DragControllerUtilsTest : public RenderingTest {
 public:
  LocalFrame& GetFrame() const { return *GetDocument().GetFrame(); }

  static HitTestResult HitTestForOcclusion(const Element& target) {
    const LayoutObject* object = target.GetLayoutObject();
    return object->HitTestForOcclusion(VisualRectInDocument(*object));
  }
};

/**

@tc.name: DragControllerUtilsTest
@tc.desc: DragRectForSelectionDrag
@tc.type: FUNC
*/
TEST_F(DragControllerUtilsTest, DragRectForSelectionDrag) {
  const gfx::RectF& visibleRect = {0, 0, 500, 500};
  // visibleRect not empty
  gfx::Rect rect = DragRectForSelectionDrag(GetFrame(), visibleRect);
  EXPECT_TRUE(rect.x() <= visibleRect.x());
  EXPECT_TRUE(rect.y() <= visibleRect.y());
}
/**

@tc.name: DragControllerUtilsTest
@tc.desc: DragImageForLink
@tc.type: FUNC
*/
TEST_F(DragControllerUtilsTest, DragImageForLink) {
  const KURL url = KURL("www.xxx.com");
  const String link_text = String("www.xxx.com");
  float device_scale_factor = 1.0;
  const Document* document = &GetDocument();
  // not force darkmode
  std::unique_ptr<DragImage> drag_img =
      DragImageForLink(url, link_text, device_scale_factor, document);
  EXPECT_NE(nullptr, drag_img);
}
/**

@tc.name: DragControllerUtilsTest
@tc.desc: ArkClampedImageScale
@tc.type: FUNC
*/
TEST_F(DragControllerUtilsTest, ArkClampedImageScale) {
  gfx::Vector2dF image_scale = gfx::Vector2dF(0.5, 0.5);
  gfx::Size image_size = gfx::Size(100, 100);
  gfx::Size image_element_size_in_pixels = gfx::Size(100, 100);
  // image_size.Area64() <= kMaxOriginalImageArea
  ArkClampedImageScale(image_scale, image_size, image_element_size_in_pixels);
  // image_size.Area64() > kMaxOriginalImageArea
  image_size = gfx::Size(1600, 1600);
  ArkClampedImageScale(image_scale, image_size, image_element_size_in_pixels);
}
/**

@tc.name: DragControllerUtilsTest
@tc.desc: GetImageRectFromImageNode
@tc.type: FUNC
*/
TEST_F(DragControllerUtilsTest, GetImageRectFromImageNode) {
  SetBodyInnerHTML(R"HTML(
<img id='drag' src="data:image/png;base64,)HTML" +
                   String(BASE64_RED_IMAGE_DATA_3_3) +
                   R"HTML(" width="100%" height="auto">
)HTML");
  Element* target = GetDocument().getElementById(AtomicString("drag"));
  HitTestResult hit_test_result = HitTestForOcclusion(*target);
  // !localframe
  gfx::Rect rect = GetImageRectFromImageNode(nullptr, hit_test_result);
  EXPECT_EQ(rect.width(), 0);
  // localframe !GetSettings()
  rect = GetImageRectFromImageNode(&GetFrame(), hit_test_result);
  EXPECT_EQ(rect.width(), 0);
}
}  // namespace blink