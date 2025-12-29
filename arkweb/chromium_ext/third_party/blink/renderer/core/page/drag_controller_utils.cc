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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/page/drag_controller_utils.h"

#include "third_party/blink/renderer/core/clipboard/data_transfer.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/core/dom/node_computed_style.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#include "third_party/blink/renderer/core/layout/hit_test_result.h"
#include "third_party/blink/renderer/core/layout/layout_image.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/core/page/drag_image.h"
#include "third_party/blink/renderer/core/page/drag_state.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/platform/graphics/compositing/paint_artifact_compositor.h"
#include "ui/display/screen_info.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/size.h"

namespace gfx {
class RectF;
}
namespace blink {
static const float kDragTextAlpha = 0.7f;
static const float kDragImageAlphaExt = 1.0f;
static const float kUxDragImageScale = 1.0f;
static const int kMaxOriginalImageArea = 1500 * 1500;

extern gfx::Rect DragRectForImage(const DragImage* drag_image,
                                  const gfx::Point& drag_initiation_location,
                                  const gfx::Point& image_element_location,
                                  const gfx::Size& image_element_size_in_pixels);
extern std::unique_ptr<DragImage> DragImageForImage(const Element& element,
                                                    float device_scale_factor,
                                                    const gfx::Size& image_element_size_in_pixels);

DragControllerUtils::DragControllerUtils(DragController* drag_controller)
    :drag_controller_(drag_controller) {}

#if BUILDFLAG(ARKWEB_DRAG_DROP)
gfx::Rect DragRectForSelectionDrag(const LocalFrame& frame,
                                   const gfx::RectF& visibleRect) {
#endif
  frame.View()->UpdateLifecycleToLayoutClean(DocumentUpdateReason::kSelection);
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  gfx::Rect dragging_rect;
  if (visibleRect.IsEmpty()) {
    dragging_rect =
        gfx::ToEnclosingRect(DragController::ClippedSelection(frame));
  } else {
    dragging_rect = gfx::ToEnclosingRect(
        IntersectRects(visibleRect, DragController::ClippedSelection(frame)));
  }
#endif
  int x1 = dragging_rect.x();
  int y1 = dragging_rect.y();
  int x2 = dragging_rect.right();
  int y2 = dragging_rect.bottom();
  gfx::Point origin(std::min(x1, x2), std::min(y1, y2));
  gfx::Size size(std::abs(x2 - x1), std::abs(y2 - y1));
  return gfx::Rect(origin, size);
}

std::unique_ptr<DragImage> DragImageForLink(const KURL& link_url, const String& link_text,
                                            float device_scale_factor, const Document* document) {
  auto doc = const_cast<Document*>(document);
  bool is_force_dark_mode =
      doc ? doc->GetStyleEngine().GetForceDarkModeEnabled() : false;
  return DragImage::Create(link_url, link_text, device_scale_factor,
                           is_force_dark_mode);
}

void ArkClampedImageScale(gfx::Vector2dF& image_scale, gfx::Size& image_size,
                          const gfx::Size& image_element_size_in_pixels) {
  // UX : scale image
  image_scale = DragImageExt::HwClampedImageScale(
      image_size, image_element_size_in_pixels, kUxDragImageScale);

  if (image_size.Area64() > kMaxOriginalImageArea) {
    LOG(WARNING) << "The image (" << image_size.width() << ","
                 << image_size.height() << ") is too big to support drag";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(WARNING) << "The image (" << image_size.width() << ","
                          << image_size.height()
                          << ") is too big to support drag";
#endif
  }
}

gfx::Rect GetImageRectFromImageNode(LocalFrame* frame,
                                    const HitTestResult& hit_test_result) {
  if (!frame || !frame->GetSettings()) {
    return hit_test_result.ImageRect();
  }
  return hit_test_result.imp_utils_->GetReplacedContentRect();
}

static std::unique_ptr<DragImage> ClippedDragImageForImage(
    const Element& element,
    const gfx::Rect& image_rect,
    const gfx::Rect& clipped_image_rect,
    float device_scale_factor,
    const gfx::Size& image_element_size_in_pixels) {
  auto* layout_image = To<LayoutImage>(element.GetLayoutObject());
  const LayoutImageResource& image_resource = *layout_image->ImageResource();
  scoped_refptr<Image> image =
      image_resource.GetImage(image_element_size_in_pixels);
  RespectImageOrientationEnum respect_orientation =
      image_resource.ImageOrientation();

  gfx::Size image_size = image->Size(respect_orientation);
  if (image_size.IsEmpty()) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO)
        << "DragDrop Try to get clipped drag image failed, the size is empty";
#endif
    return nullptr;
  }

  InterpolationQuality interpolation_quality = kInterpolationDefault;
  if (layout_image->StyleRef().ImageRendering() ==
      EImageRendering::kPixelated) {
    interpolation_quality = kInterpolationNone;
  }

  gfx::Vector2dF image_scale = DragImage::ClampedImageScale(
      image_size, image_element_size_in_pixels, image_element_size_in_pixels);

  gfx::SizeF image_scale_from_view(
      (float)image_rect.width() / image_size.width(),
      (float)image_rect.height() / image_size.height());

  gfx::Vector2d clip_origin_offset =
      clipped_image_rect.origin() - image_rect.origin();
  gfx::Rect clip_rect =
      gfx::Rect(clip_origin_offset.x() / image_scale_from_view.width(),
                clip_origin_offset.y() / image_scale_from_view.height(),
                clipped_image_rect.width() / image_scale_from_view.width(),
                clipped_image_rect.height() / image_scale_from_view.height());

  return DragImageExt::CreateClippedByVisualViewport(
      image.get(), clip_rect, respect_orientation, device_scale_factor,
      interpolation_quality, kDragImageAlphaExt, image_scale);
}

std::unique_ptr<DragImage> CreateClippedDragImageForImage(
    LocalFrame* src,
    const Element& element,
    const gfx::Rect& image_rect,
    const gfx::RectF& visibleRect,
    gfx::Rect& clipped_image_rect) {
  if (!src || !src->View() || !src->GetPage()) {
    return nullptr;
  }

  gfx::RectF image_rect_in_root_frame(
      src->View()->ConvertToRootFrame(image_rect));
  gfx::RectF viewport_in_root_frame =
      src->GetPage()->GetVisualViewport().VisibleRect();
  gfx::RectF clipped_image_rect_in_root_frame =
      IntersectRects(viewport_in_root_frame, image_rect_in_root_frame);
  if (!visibleRect.IsEmpty()) {
    clipped_image_rect_in_root_frame =
        IntersectRects(clipped_image_rect_in_root_frame, visibleRect);
  }

  clipped_image_rect = src->View()->ConvertFromRootFrame(
      ToEnclosedRect(clipped_image_rect_in_root_frame));

  gfx::Size image_size_in_pixels = image_rect.size();
  image_size_in_pixels =
      ScaleToFlooredSize(image_size_in_pixels,
                         src->GetPage()->InspectorDeviceScaleFactorOverride() *
                             src->GetPage()->GetVisualViewport().Scale());

  float screen_device_scale_factor =
      src->GetChromeClient().GetScreenInfo(*src).device_scale_factor;

  std::unique_ptr<DragImage> drag_image = ClippedDragImageForImage(
      element, image_rect, clipped_image_rect, screen_device_scale_factor,
      image_size_in_pixels);
  if (drag_image) {
    // When previewing a drag image with position information,
    // the drag image want to overlap with the original image in web page,
    // except the drag image customized by H5.
    clipped_image_rect_in_root_frame.Offset(
        -src->GetPage()->GetVisualViewport().GetScrollOffset());
    clipped_image_rect_in_root_frame.Scale(
        src->GetPage()->GetVisualViewport().Scale());
    if (auto* paint_artifact_compositor =
            src->View()->GetPaintArtifactCompositor()) {
      if (auto* root_cclayer = paint_artifact_compositor->RootLayer()) {
        gfx::RectF clipped_image_rectf =
            gfx::RectF(clipped_image_rect_in_root_frame);
        (void)root_cclayer->ScreenSpaceTransform().MapRect(clipped_image_rectf);
        drag_image->AsDragImageExt()->SetFromClippedMethod(true);
        drag_image->AsDragImageExt()->SetDragImageOriginPosition(
            gfx::PointF(clipped_image_rectf.x(), clipped_image_rectf.y()));
      }
    }
  }

  return drag_image;
}

void CreateImgAndRectForSelection(std::unique_ptr<DragImage>& drag_image, gfx::Rect& drag_obj_rect,
                                  LocalFrame* frame, const gfx::RectF& visibleRect) {
  gfx::RectF visible_rect_from_root_frame =
      gfx::RectF(frame->View()->ConvertFromRootFrame(
          gfx::ToEnclosingRect(visibleRect)));
  drag_image = DragController::DragImageForSelection(
      *frame, kDragTextAlpha, visible_rect_from_root_frame);
  drag_obj_rect =
      DragRectForSelectionDrag(*frame, visible_rect_from_root_frame);
}

void CreateImgAndRectForImage(std::unique_ptr<DragImage>& drag_image, gfx::Rect& drag_obj_rect,
                              LocalFrame* frame, const gfx::Rect& image_rect, const DragState& state,
                              const gfx::RectF& visibleRect,
                              const gfx::Point& effective_drag_initiation_location) {
  float device_scale_factor =
      frame->GetChromeClient().GetScreenInfo(*frame).device_scale_factor;
  auto* element = DynamicTo<Element>(state.drag_src_.Get());
  gfx::Size image_size_in_pixels = gfx::ScaleToFlooredSize(
      image_rect.size(), frame->GetPage()->GetVisualViewport().Scale());
  if (frame->GetSettings()) {
    // create clipped drag image
    LOG(INFO) << "DragDrop image drag create clipped drag image";
    gfx::Rect clipped_image_rect = image_rect;
    drag_image = CreateClippedDragImageForImage(
        frame, *element, image_rect, visibleRect, clipped_image_rect);
    drag_obj_rect = DragRectForImage(
        drag_image.get(), effective_drag_initiation_location,
        clipped_image_rect.origin(), image_size_in_pixels);
  } else {
    // Pass the selected image size in DIP becasue dragImageForImage clips
    // the image in DIP.  The coordinates of the locations are in Viewport
    // coordinates, and they're converted in the Blink client.
    // TODO(oshima): Currently, the dragged image on high DPI is scaled and
    // can be blurry because of this.  Consider to clip in the screen
    // coordinates to use high resolution image on high DPI screens.
    drag_image = DragImageForImage(*element, device_scale_factor,
                                  image_size_in_pixels);
    drag_obj_rect = DragRectForImage(
        drag_image.get(), effective_drag_initiation_location,
        image_rect.origin(), image_size_in_pixels);
  }
}
#if BUILDFLAG(ARKWEB_DRAG_DROP)
void DragControllerUtils::Trace(Visitor* visitor) const {
  visitor->Trace(drag_controller_);
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
}