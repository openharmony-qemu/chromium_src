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
#include "third_party/blink/renderer/platform/graphics/graphics_types.h"
#define private public
#define protected public
#include "cc/paint/paint_image.h"
#include "third_party/blink/renderer/platform/graphics/bitmap_image.h"
#include "third_party/blink/renderer/platform/graphics/image_ext.h"
#undef private
#undef protected

namespace blink {
class ImageExtTest : public ::testing::Test {
 public:
  void SetUp() override {}
  void TearDown() override {}
};

PaintImage CreateWhitePaintImage(int width, int height) {
  // create a SkBitmap with white color
  SkBitmap bitmap;
  bitmap.allocN32Pixels(width, height);
  bitmap.eraseColor(SK_ColorWHITE);
  return PaintImage::CreateFromBitmap(bitmap);
}

/**
 * @tc.name: ImageExtTest
 * @tc.desc: ClipResizeAndOrientImage
 * @tc.type: FUNC
 */
TEST_F(ImageExtTest, ClipResizeAndOrientImage) {
  PaintImage img_in = CreateWhitePaintImage(300, 300);
  // kDefault
  ImageOrientation orientation =
      ImageOrientation(ImageOrientationEnum::kOriginTopRight);
  // rect w == and h ==
  gfx::Rect rect = gfx::Rect(0, 0, 0, 300);
  gfx::Vector2dF scale_in = {1.0, 1.0};
  float opacity = 1.0;
  InterpolationQuality interpolation_quality =
      InterpolationQuality::kInterpolationNone;
  // branch line 52 , orientation != ImageOrientationEnum::kDefault
  PaintImage img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  // branch line 52 , orientation != ImageOrientationEnum::kDefault,
  // orientation.UsesWidthAsHeight()
  orientation = ImageOrientation(ImageOrientationEnum::kOriginLeftTop);
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);
  // branch line 52 , orientation == ImageOrientationEnum::kDefault
  orientation = ImageOrientation(ImageOrientationEnum::kDefault);
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  // branch line 60 , !size.IsEmpty()
  rect = gfx::Rect(0, 0, 300, 300);
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  EXPECT_EQ(300, rect.width());
  EXPECT_EQ(300, rect.height());
  // branch line 71 , !transform.IsIdentity()
  scale_in = {0.0, 0.0};
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  // branch line 71 , opacity != 1
  scale_in = {1.0, 1.0};
  opacity = 0.5;
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  // branch line 72, clip_rect.width() != image.width()
  opacity = 1.0;
  scale_in = {0.5, 1.0};
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  // branch line 73, clip_rect.height() != image.height()
  opacity = 1.0;
  scale_in = {1.0, 0.5};
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  // branch line 73, all pass
  scale_in = {1.0, 1.0};
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  // branch line 90, !surface
  scale_in = {0.0, 0.0};
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  // branch line 108, interpolation_quality = kInterpolationNone
  interpolation_quality = InterpolationQuality::kInterpolationMedium;
  scale_in = {2.0, 2.0};
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);

  rect.set_height(200);
  img_out = ImageExt::ClipResizeAndOrientImage(
      img_in, orientation, rect, scale_in, opacity, interpolation_quality);
}
}  // namespace blink