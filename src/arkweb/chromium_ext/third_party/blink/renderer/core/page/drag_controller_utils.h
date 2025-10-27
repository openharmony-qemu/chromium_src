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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_DRAG_CONTROLLER_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_DRAG_CONTROLLER_UTILS_H_

#include "third_party/blink/renderer/core/page/drag_controller.h"
#include "arkweb/build/features/features.h"

namespace blink {

class DragController;

class DragControllerUtils : public GarbageCollected<DragControllerUtils> {
 public:
  explicit DragControllerUtils(DragController*);

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void Trace(Visitor*) const;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
 private:
  Member<DragController> drag_controller_;
};

  std::unique_ptr<DragImage> DragImageForLink(const KURL& link_url, const String& link_text,
                                              float device_scale_factor, const Document* document);
  gfx::Rect GetImageRectFromImageNode(LocalFrame* frame, const HitTestResult& hit_test_result);
  void ArkClampedImageScale(gfx::Vector2dF& image_scale, gfx::Size& image_size,
                            const gfx::Size& image_element_size_in_pixels);
  void CreateImgAndRectForSelection(std::unique_ptr<DragImage>& drag_image, gfx::Rect& drag_obj_rect,
                                    LocalFrame* frame, const gfx::RectF& visibleRect);
  void CreateImgAndRectForImage(std::unique_ptr<DragImage>& drag_image, gfx::Rect& drag_obj_rect,
                                LocalFrame* frame, const gfx::Rect& image_rect, const DragState& state,
                                const gfx::RectF& visibleRect,
                                const gfx::Point& effective_drag_initiation_location);
#if BUILDFLAG(ARKWEB_UNITTESTS)
  gfx::Rect DragRectForSelectionDrag(const LocalFrame& frame,
                                     const gfx::RectF& visibleRect);
  std::unique_ptr<DragImage> CreateClippedDragImageForImage(
    LocalFrame* src,
    const Element& element,
    const gfx::Rect& image_rect,
    const gfx::RectF& visibleRect,
    gfx::Rect& clipped_image_rect);
#endif
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_DRAG_CONTROLLER_UTILS_H_
