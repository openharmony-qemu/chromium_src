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
#include "third_party/blink/renderer/core/layout/layout_image.h"
#include "third_party/blink/renderer/core/layout/layout_image_resource.cc"
#include "third_party/blink/renderer/core/page/drag_controller_utils.h"
#undef private
#undef protected

namespace blink {

namespace {
constexpr char BASE64_RED_IMAGE_DATA_100_100[] =
    "iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAIAAAD/"
    "gAIDAAAA40lEQVR4nO3QsQEAIAyAsOr/"
    "P+"
    "sLZU9mJs4btu66xKzCrMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCs"
    "wKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArM"
    "CswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzA"
    "rMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArNn7il"
    "4Bx2GaB88AAAAASUVORK5CYII=";
}

class OHOSMockImage : public Image {
 public:
  bool no_paint_img_ = false;
  static scoped_refptr<OHOSMockImage> Create(sk_sp<SkImage> image) {
    return base::AdoptRef(new OHOSMockImage(image));
  }

  static scoped_refptr<OHOSMockImage> Create(const gfx::Size& size) {
    return base::AdoptRef(new OHOSMockImage(size));
  }

  gfx::Size SizeWithConfig(SizeConfig config) const override {
    if (config.apply_orientation) {
      return gfx::Size(100, 100);
    }
    return gfx::Size(0, 0);
  }

  bool CurrentFrameKnownToBeOpaque() override { return false; }

  void DestroyDecodedData() override {
    // Image pure virtual stub.
  }

  void Draw(cc::PaintCanvas*,
            const cc::PaintFlags&,
            const gfx::RectF& dest_rect,
            const gfx::RectF& src_rect,
            const ImageDrawOptions&) override {
    // Image pure virtual stub.
  }

  PaintImage PaintImageForCurrentFrame() override {
    if (!no_paint_img_ || !image_) {
      return PaintImage();
    }
    return CreatePaintImageBuilder()
        .set_image(image_, cc::PaintImage::GetNextContentId())
        .TakePaintImage();
  }

 private:
  explicit OHOSMockImage(sk_sp<SkImage> image) : image_(image) {}

  explicit OHOSMockImage(gfx::Size size) : image_(nullptr) {
    sk_sp<SkSurface> surface = CreateSkSurface(size);
    if (!surface) {
      return;
    }

    surface->getCanvas()->clear(SK_ColorTRANSPARENT);
    image_ = surface->makeImageSnapshot();
  }

  static sk_sp<SkSurface> CreateSkSurface(gfx::Size size) {
    return SkSurfaces::Raster(
        SkImageInfo::MakeN32(size.width(), size.height(), kPremul_SkAlphaType));
  }

  sk_sp<SkImage> image_;
};

class DragControllerUtilsTest : public RenderingTest {
 public:
  LocalFrame& GetFrame() const { return *GetDocument().GetFrame(); }

  static HitTestResult HitTestForOcclusion(const Element& target) {
    const LayoutObject* object = target.GetLayoutObject();
    return object->HitTestForOcclusion(VisualRectInDocument(*object));
  }

  void CreateImage(const gfx::Size& size) {
    image_ = OHOSMockImage::Create(size);
  }

  scoped_refptr<OHOSMockImage> image_;
};

/**
 * @tc.name: DragControllerUtilsTest
 * @tc.desc: DragRectForSelectionDrag
 * @tc.type: FUNC
 */
TEST_F(DragControllerUtilsTest, DragRectForSelectionDrag) {
  const gfx::RectF& visibleRect = {0, 0, 500, 500};
  // visibleRect not empty
  gfx::Rect rect = DragRectForSelectionDrag(GetFrame(), visibleRect);
  EXPECT_TRUE(rect.x() <= visibleRect.x());
  EXPECT_TRUE(rect.y() <= visibleRect.y());
}

/**
 * @tc.name: DragControllerUtilsTest
 * @tc.desc: DragImageForLink
 * @tc.type: FUNC
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
 * @tc.name: DragControllerUtilsTest
 * @tc.desc: ArkClampedImageScale
 * @tc.type: FUNC
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
 * @tc.name: DragControllerUtilsTest
 * @tc.desc: GetImageRectFromImageNode
 * @tc.type: FUNC
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

/**
 * @tc.name: DragControllerUtilsTest
 * @tc.desc: CreateClippedDragImageForImage
 * @tc.type: FUNC
 */
TEST_F(DragControllerUtilsTest, CreateClippedDragImageForImage) {
  SetBodyInnerHTML(R"HTML(
  <img id='drag' src="data:image/png;base64,)HTML" +
                   String(BASE64_RED_IMAGE_DATA_100_100) +
                   R"HTML(" width="100%" height="auto">
  )HTML");
  UpdateAllLifecyclePhasesForTest();
  LocalFrame* src = &GetFrame();
  Element* target = GetDocument().getElementById(AtomicString("drag"));
  CreateImage(gfx::Size(100, 100));
  ImageResourceContent* image_content =
      ImageResourceContent::CreateLoaded(image_);
  LayoutObject* obj = GetLayoutObjectByElementId("drag");
  ASSERT_NE(obj, nullptr);
  LayoutImage* img = DynamicTo<LayoutImage>(obj);
  ASSERT_NE(img, nullptr);
  LayoutImageResource* image_resource_ptr = img->ImageResource();
  image_resource_ptr->SetImageResource(image_content);
  gfx::Rect image_rect(0, 0, 100, 100);
  gfx::RectF visibleRect(0, 0, 0, 0);
  gfx::Rect clipped_image_rect(0, 0, 100, 100);

  const gfx::Rect c_image_rect = image_rect;
  std::unique_ptr<DragImage> result_img = CreateClippedDragImageForImage(
      nullptr, *target, c_image_rect, visibleRect, clipped_image_rect);
  EXPECT_EQ(nullptr, result_img);

  LocalFrameView* view = src->View();
  src->SetView(nullptr);
  result_img = CreateClippedDragImageForImage(src, *target, c_image_rect,
                                              visibleRect, clipped_image_rect);
  EXPECT_EQ(nullptr, result_img);

  src->SetView(view);
  result_img = CreateClippedDragImageForImage(src, *target, c_image_rect,
                                              visibleRect, clipped_image_rect);
  EXPECT_EQ(nullptr, result_img);

  visibleRect = gfx::RectF(0, 0, 100, 100);
  result_img = CreateClippedDragImageForImage(src, *target, c_image_rect,
                                              visibleRect, clipped_image_rect);
  EXPECT_EQ(nullptr, result_img);
}
}  // namespace blink