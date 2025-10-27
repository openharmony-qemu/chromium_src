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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_IMAGE_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_IMAGE_EXT_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/platform/graphics/graphics_types.h"
#include "third_party/blink/renderer/platform/graphics/image.h"
#include "third_party/blink/renderer/platform/graphics/image_orientation.h"
#include "third_party/blink/renderer/platform/graphics/paint/paint_image.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size_f.h"

namespace blink {
class Image;

class PLATFORM_EXPORT ImageExt : public Image {
 public:
  ImageExt();
  ~ImageExt();

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  static PaintImage ClipResizeAndOrientImage(
      const PaintImage&,
      ImageOrientation,
      const gfx::Rect&,
      gfx::Vector2dF image_scale = gfx::Vector2dF(1, 1),
      float opacity = 1.0,
      InterpolationQuality interpolation_quality = kInterpolationNone);
#endif
};
}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_IMAGE_EXT_H_
