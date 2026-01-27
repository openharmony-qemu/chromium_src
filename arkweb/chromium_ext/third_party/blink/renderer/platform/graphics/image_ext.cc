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

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/graphics/image_ext.h"

#include "base/logging.h"
#include "third_party/blink/renderer/platform/graphics/skia/skia_utils.h"
#include "third_party/blink/renderer/platform/transforms/affine_transform.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColorSpace.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size_f.h"
#include "ui/gfx/geometry/skia_conversions.h"

namespace blink {
ImageExt::ImageExt() {}

ImageExt::~ImageExt() = default;

#if BUILDFLAG(ARKWEB_DRAG_DROP)
PaintImage ImageExt::ClipResizeAndOrientImage(
    const PaintImage& image,
    ImageOrientation orientation,
    const gfx::Rect& clip_rect,
    gfx::Vector2dF image_scale,
    float opacity,
    InterpolationQuality interpolation_quality) {
  gfx::Size size(image.width(), image.height());
  size = gfx::ScaleToFlooredSize(size, image_scale.x(), image_scale.y());

  gfx::Size clip_size(clip_rect.width(), clip_rect.height());
  clip_size =
      gfx::ScaleToFlooredSize(clip_size, image_scale.x(), image_scale.y());

  AffineTransform transform;
  if (orientation != ImageOrientationEnum::kDefault) {
    if (orientation.UsesWidthAsHeight()) {
      size.Transpose();
    }
    transform *= orientation.TransformFromDefault(gfx::SizeF(size));
  }
  transform.ScaleNonUniform(image_scale.x(), image_scale.y());

  if (size.IsEmpty()) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO)
        << "DragDrop Clip resize and orient image but the size is empty.";
#endif

    return PaintImage();
  }

  if (transform.IsIdentity() && opacity == 1 &&
      clip_rect.width() == image.width() &&
      clip_rect.height() == image.height()) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO)
        << "DragDrop Nothing to adjust drag image, just use the original";
#endif

    DCHECK_EQ(image.width(), size.width());
    DCHECK_EQ(image.height(), size.height());
    return image;
  }

  const SkImageInfo info =
      SkImageInfo::MakeN32(clip_size.width(), clip_size.height(),
                           kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
  sk_sp<SkSurface> surface = SkSurface::MakeRaster(info);
  if (!surface) {
    LOG(WARNING) << "DragDrop Make a SkImageInfo with w=" << clip_size.width()
                 << ", h=" << clip_size.height() << " failed.";

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(WARNING) << "DragDrop Make a SkImageInfo with w="
                          << clip_size.width() << ", h=" << clip_size.height()
                          << " failed.";
#endif

    return PaintImage();
  }

  SkPaint paint;
  DCHECK_GE(opacity, 0);
  DCHECK_LE(opacity, 1);
  paint.setAlpha(opacity * 255);
  SkSamplingOptions sampling;
  if (interpolation_quality != kInterpolationNone) {
    sampling = SkSamplingOptions(SkCubicResampler::CatmullRom());
  }

  SkCanvas* canvas = surface->getCanvas();
  canvas->concat(AffineTransformToSkMatrix(transform));

  SkRect dst_rect = SkRect::MakeIWH(clip_rect.width(), clip_rect.height());
  canvas->drawImageRect(image.GetSwSkImage(), gfx::RectToSkRect(clip_rect),
                        dst_rect, sampling, &paint,
                        SkCanvas::kFast_SrcRectConstraint);

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "DragDrop Create a clipped drag image("
                     << clip_rect.ToString() << ") from intrinsic("
                     << image.width() << "*" << image.height()
                     << ") to visual size(" << clip_size.width() << "*"
                     << clip_size.height() << "), opacity=" << opacity;
#endif

  return PaintImageBuilder::WithProperties(std::move(image))
      .set_image(surface->makeImageSnapshot(), PaintImage::GetNextContentId())
      .TakePaintImage();
}
#endif
}  // namespace blink
