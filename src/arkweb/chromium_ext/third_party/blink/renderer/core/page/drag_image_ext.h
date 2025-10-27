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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_DRAG_IMAGE_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_DRAG_IMAGE_EXT_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/page/drag_controller.h"
#include "third_party/blink/renderer/platform/graphics/image.h"
#include "third_party/blink/renderer/core/page/drag_image.h"
#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/point_f.h"


namespace gfx {
class RectF;
}

namespace blink {
class DragImage;

    class DragImageExt final : public DragImage {
 public:
  DragImageExt(const SkBitmap& bitmap,
               InterpolationQuality interpolation_quality);
  DragImageExt(const DragImageExt&) = delete;
  DragImageExt& operator=(const DragImageExt&) = delete;
  ~DragImageExt() override;
  DragImageExt* AsDragImageExt() override { return this; }

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  static WTF::String filterNonPrintable(const WTF::String& input);
  bool is_from_clipped_method_ = false;
  gfx::PointF image_origin_position_;
  void SetFromClippedMethod(bool from_clipped_method) {
    is_from_clipped_method_ = from_clipped_method;
  }
  void SetDragImageOriginPosition(const gfx::PointF& position) {
    image_origin_position_ = position;
  }
  static std::unique_ptr<DragImage> CreateClippedByVisualViewport(
      Image* img,
      const gfx::Rect& clip_rect,
      RespectImageOrientationEnum = kRespectImageOrientation,
      float device_scale_factor = 1,
      InterpolationQuality = kInterpolationDefault,
      float opacity = 1,
      gfx::Vector2dF image_scale = gfx::Vector2dF(1, 1));
  static gfx::Vector2dF HwClampedImageScale(const gfx::Size&,
                                            const gfx::Size&,
                                            const float target_scale);
#endif
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_DRAG_IMAGE_EXT_H_
