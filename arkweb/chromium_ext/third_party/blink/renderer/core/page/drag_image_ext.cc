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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/page/drag_image_ext.h"

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/graphics/image_ext.h"
#include "third_party/blink/renderer/platform/graphics/bitmap_image.h"
#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"
#include "third_party/blink/renderer/platform/wtf/text/unicode.h"

namespace blink {

DragImageExt::DragImageExt(const SkBitmap& bitmap,
                           InterpolationQuality interpolation_quality)
    : DragImage(bitmap, interpolation_quality) {}

DragImageExt::~DragImageExt() = default;

#if BUILDFLAG(ARKWEB_DRAG_DROP)
gfx::Vector2dF DragImageExt::HwClampedImageScale(const gfx::Size& image_size,
                                                 const gfx::Size& element_size,
                                                 const float target_scale) {
  // Non-uniform scaling for size mapping.
  gfx::Vector2dF image_scale(static_cast<float>(element_size.width()) *
                                 target_scale / image_size.width(),
                             static_cast<float>(element_size.height()) *
                                 target_scale / image_size.height());
  return image_scale;
}

WTF::String DragImageExt::filterNonPrintable(const WTF::String& input) {
  WTF::StringBuilder result;
  for (unsigned i = 0; i < input.length(); ++i) {
    if (WTF::unicode::IsPrintableChar(input[i])) {
      result.Append(input[i]);
    }
  }
  return result.ToString();
}

std::unique_ptr<DragImage> DragImageExt::CreateClippedByVisualViewport(
    Image* image,
    const gfx::Rect& clip_rect,
    RespectImageOrientationEnum should_respect_image_orientation,
    float device_scale_factor,
    InterpolationQuality interpolation_quality,
    float opacity,
    gfx::Vector2dF image_scale) {
  if (!image) {
    LOG(WARNING) << "DragDrop Invalid image input";

#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(WARNING) << "DragDrop Invalid image input";
#endif

    return nullptr;
  }

  PaintImage paint_image = image->PaintImageForCurrentFrame();
  if (!paint_image) {
    LOG(WARNING) << "DragDrop Invalid paint image";

#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(WARNING) << "DragDrop Invalid paint image";
#endif

    return nullptr;
  }

  ImageOrientation orientation;
  auto* bitmap_image = DynamicTo<BitmapImage>(image);
  if (should_respect_image_orientation == kRespectImageOrientation &&
      bitmap_image) {
    orientation = bitmap_image->CurrentFrameOrientation();
  }

  SkBitmap bm;
  paint_image = ImageExt::ClipResizeAndOrientImage(
      paint_image, orientation, clip_rect, image_scale, opacity,
      interpolation_quality);
  if (!paint_image || !paint_image.GetSwSkImage()->asLegacyBitmap(&bm)) {
    LOG(WARNING) << "DragDrop Invalid paint image or bitmap after clip";

#ifdef OHOS_LOGGER_REPORT
    LOG_FEEDBACK(WARNING)
        << "DragDrop Invalid paint image or bitmap after clip";
#endif

    return nullptr;
  }
  (void)device_scale_factor;
  return base::WrapUnique(new DragImageExt(bm, interpolation_quality));
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
}  // namespace blink
