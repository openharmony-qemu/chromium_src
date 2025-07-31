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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_observer.h"
#define private public
#define protected public
#include "third_party/blink/renderer/core/page/drag_image.h"
#include "third_party/blink/renderer/core/page/drag_image_ext.h"
#include "third_party/blink/renderer/platform/graphics/bitmap_image.h"
#include "third_party/blink/renderer/platform/graphics/bitmap_image_test.cc"
#include "third_party/blink/renderer/platform/graphics/image.h"
#include "third_party/blink/renderer/platform/graphics/skia/skia_utils.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/gfx/geometry/size.h"
#undef private
#undef protected

namespace blink {
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

class DragImageExtTest : public ::testing::Test {
 public:
  void SetUp() override {}
  void TearDown() override {}

  void CreateImage(const gfx::Size& size) {
    image_ = OHOSMockImage::Create(size);
  }

  scoped_refptr<OHOSMockImage> image_;
};

/**
 * @tc.name: DragImageExtTest
 * @tc.desc: HwClampedImageScale
 * @tc.type: FUNC
 */
TEST_F(DragImageExtTest, HwClampedImageScale) {
  const gfx::Size image_size = gfx::Size(100, 100);
  const gfx::Size element_size = gfx::Size(100, 100);
  const float target_scale = 1.0;
  gfx::Vector2dF image_scale =
      DragImageExt::HwClampedImageScale(image_size, element_size, target_scale);
  EXPECT_EQ(1, image_scale.x());
  EXPECT_EQ(1, image_scale.y());
}
/**
 * @tc.name: DragImageExtTest
 * @tc.desc: filterNonPrintable
 * @tc.type: FUNC
 */
TEST_F(DragImageExtTest, filterNonPrintable) {
  const WTF::String input = WTF::String("asdfas\0\x01");
  WTF::String printable = DragImageExt::filterNonPrintable(input);
  EXPECT_EQ(6, printable.length());
}
/**
 * @tc.name: DragImageExtTest
 * @tc.desc: CreateClippedByVisualViewport
 * @tc.type: FUNC
 */
TEST_F(DragImageExtTest, CreateClippedByVisualViewport) {
  // create a SkBitmap with white color
  const gfx::Rect clip_rect = {0, 0, 100, 50};
  CreateImage(gfx::Size(100, 100));
  RespectImageOrientationEnum should_respect_image_orientation =
      RespectImageOrientationEnum::kDoNotRespectImageOrientation;
  float device_scale_factor = 1.0;
  InterpolationQuality interpolation_quality =
      InterpolationQuality::kInterpolationNone;
  float opacity = 0.5;
  gfx::Vector2dF image_scale = {1.0, 1.0};
  // branch line 61, !image
  std::unique_ptr<DragImage> drag_image =
      DragImageExt::CreateClippedByVisualViewport(
          nullptr, clip_rect, should_respect_image_orientation,
          device_scale_factor, interpolation_quality, opacity, image_scale);
  EXPECT_EQ(nullptr, drag_image);

  // branch line 72, !paint_image
  drag_image = DragImageExt::CreateClippedByVisualViewport(
      image_.get(), clip_rect, should_respect_image_orientation,
      device_scale_factor, interpolation_quality, opacity, image_scale);
  EXPECT_EQ(nullptr, drag_image);

  // branch line 84, !kRespectImageOrientation
  drag_image = DragImageExt::CreateClippedByVisualViewport(
      image_.get(), clip_rect, should_respect_image_orientation,
      device_scale_factor, interpolation_quality, opacity, image_scale);
  EXPECT_EQ(nullptr, drag_image);

  // branch line 85, !bitmap_image
  interpolation_quality = InterpolationQuality::kInterpolationNone;
  opacity = 1;
  drag_image = DragImageExt::CreateClippedByVisualViewport(
      image_.get(), clip_rect, should_respect_image_orientation,
      device_scale_factor, interpolation_quality, opacity, image_scale);
  EXPECT_EQ(nullptr, drag_image);
}
}  // namespace blink