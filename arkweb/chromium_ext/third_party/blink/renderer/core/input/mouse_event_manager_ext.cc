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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/input/mouse_event_manager_ext.h"
#include "base/logging.h"

#include "third_party/blink/renderer/core/frame/local_frame.h"

#if BUILDFLAG(ARKWEB_DRAG_DROP)
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/page/drag_controller.h"
#endif

#if BUILDFLAG(ARKWEB_AI)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/editing/selection_controller.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/html/html_image_element.h"
#include "third_party/blink/renderer/core/input/event_handler.h"
#include "third_party/blink/renderer/core/input/scroll_manager.h"
#include "third_party/blink/renderer/core/layout/hit_test_result.h"
#include "third_party/blink/renderer/core/layout/layout_view.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "ui/gfx/geometry/point_conversions.h"
#endif

namespace blink {
#if BUILDFLAG(ARKWEB_AI)
const int kMinAnalyzedImageWidth = 100;
const int kMinAnalyzedImageHeight = 100;
const int KMaxAnalyzedImageDimension = 1024;
const long long kMaxAnalyzedImageArea = 104857600; // 10240 * 10240
const double kMinAnalyzedImageToPageRatio = 0.8;
constexpr base::TimeDelta HOVER_CREATE_OVERLAY_TIME = base::Milliseconds(1000);
const unsigned long WORD_CORNER_NUM = 4ul;
#endif

MouseEventManagerExt::MouseEventManagerExt(LocalFrame& frame, ScrollManager& scroll_manager)
    : MouseEventManager(frame, scroll_manager) {
#if BUILDFLAG(ARKWEB_AI)
  create_overlay_timer_.Start(
      FROM_HERE, HOVER_CREATE_OVERLAY_TIME,
      WTF::BindRepeating(&MouseEventManagerExt::CreateOverlayCallback,
                         WrapPersistent(weak_factory_.GetWeakCell())));
  create_overlay_timer_.Stop();
#endif
}

MouseEventManagerExt::~MouseEventManagerExt() = default;

#if BUILDFLAG(ARKWEB_DRAG_DROP)
// LCOV_EXCL_START
bool MouseEventManagerExt::IsDraging() {
  DCHECK(frame_->GetPage());
  return frame_->GetPage()->GetDragController().AsDragControllerExt()->IsDraging();
}
// LCOV_EXCL_STOP
#endif

#if BUILDFLAG(ARKWEB_AI)
void MouseEventManagerExt::HandleCreateOverlayWhenDrag(const MouseEventWithHitTestResults& event) {
  last_mouse_drag_ = event.Event();
  if (!mouse_pressed_ && !is_mouse_position_unknown_ &&
      !scroll_manager_->MiddleClickAutoscrollInProgress()) {
    create_overlay_timer_.Reset();
  } else {
    create_overlay_timer_.Stop();
  }
}

// LCOV_EXCL_START
void MouseEventManagerExt::StopCreateOverlayTimer() {
  create_overlay_timer_.Stop();
}
// LCOV_EXCL_STOP

void MouseEventManagerExt::HandleGestureCreateOverlay(
    const WebGestureEvent& gesture_event) {
  HandleCreateOverlay(gesture_event);
}

// LCOV_EXCL_START
void MouseEventManagerExt::CreateOverlayCallback() {
  HandleCreateOverlay(last_mouse_drag_);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
bool MouseEventManagerExt::GetOverlayInProgress() {
  return overlay_in_progress_;
}
// LCOV_EXCL_STOP

bool MouseEventManagerExt::IsValidOverlayNode(Node* node) {
  auto image = HitTestResult::GetImage(node);
  if (!image || image->IsNull()) {
    LOG(INFO) << "IsOverlayNodeValid node is null";
    return false;
  }
  return true;
}

HitOverlayStatus MouseEventManagerExt::GetHitOverlayStatus(const HitTestResult& hit_test_result,
                                                           bool ignore_overlay_status) {
  auto status = HitOverlayStatus::kNone;
  auto node = hit_test_result.InnerNodeOrImageMapImage();
  if ((ignore_overlay_status || overlay_in_progress_) &&
      IsValidOverlayNode(node) && IsValidOverlayNode(hit_image_node_) &&
      node == hit_image_node_.Get()) {
    status = overlay_creating_ ? HitOverlayStatus::kCreating
                               : HitOverlayStatus::kCreated;
  }
  LOG(INFO) << "MouseEventManagerExt::GetHitOverlayStatus status: "
            << static_cast<uint32_t>(status);
  return status;
}

HitOverlayStatus MouseEventManagerExt::GetHitOverlayStatusFromMouseEvent(
    const MouseEventWithHitTestResults& event) {
  HitTestLocation location(frame_->View()->ConvertFromRootFrame(
      gfx::ToFlooredPoint(event.Event().PositionInRootFrame())));
  HitTestResult hit_test_result =
      frame_->GetEventHandler().HitTestResultAtLocation(location);
  return GetHitOverlayStatus(hit_test_result);
}

// LCOV_EXCL_START
void MouseEventManagerExt::CloseImageOverlay() {
  if (!overlay_in_progress_) {
    LOG(INFO)
        << "MouseEventManagerExt::CloseImageOverlay: No image overlay to closed.";
    return;
  }
  WebLocalFrameImpl* web_local_frame = WebLocalFrameImpl::FromFrame(frame_);
  if (web_local_frame && web_local_frame->Client()) {
    LOG(INFO) << "MouseEventManagerExt::CloseImageOverlay: start.";
    web_local_frame->Client()->AsWebLocalFrameClientExt()->CloseImageOverlaySelection();
  }
}
// LCOV_EXCL_STOP

void MouseEventManagerExt::GetAbsImageRect(gfx::RectF& abs_rect) {
  abs_rect = gfx::RectF();
  if (!IsValidOverlayNode(hit_image_node_)) {
    LOG(INFO) << "cannot get image from hit_image_node_.";
    hit_image_node_ = nullptr;
    return;
  }
  if (hit_image_node_ && hit_image_node_->isConnected() &&
      hit_image_node_->GetLayoutBox()) {
    LOG(INFO) << "getting layout box rect from hit_image_node_";
    abs_rect =
        hit_image_node_->GetLayoutBox()->AbsoluteContentQuad().BoundingBox();
  } else {
    LOG(INFO) << "hit_image_node_ is nullptr or disconnected.";
    hit_image_node_ = nullptr;
  }
}

// LCOV_EXCL_START
void MouseEventManagerExt::SetOverlayInProgress(bool flag) {
  LOG(INFO) << "MouseEventManagerExt::SetOverlayInProgress, flag == " << flag;
  overlay_in_progress_ = flag;
  if (!flag) {
    last_analyzed_image_ = nullptr;
    hit_image_node_ = nullptr;
  }
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void MouseEventManagerExt::SetOverlayCreatingStatus(bool flag) {
  LOG(INFO) << "MouseEventManagerExt::SetOverlayCreatingStatus, flag == " << flag;
  overlay_creating_ = flag;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void MouseEventManagerExt::OnDestroyImageAnalyzerOverlay() {
  LOG(INFO) << "MouseEventManagerExt::OnDestroyImageAnalyzerOverlay";
  overlay_in_progress_ = false;
  overlay_creating_ = false;
  last_analyzed_image_ = nullptr;
  hit_image_node_ = nullptr;
}
// LCOV_EXCL_STOP

void MouseEventManagerExt::OnFoldStatusChanged(uint32_t foldstatus) {
  LOG(INFO) << "MouseEventManagerExt::OnFoldStatusChanged foldstatus = "
            << foldstatus;
  FoldStatus foldStatusType = static_cast<FoldStatus>(foldstatus);
  if (foldStatusType == FoldStatus::FULL ||
      foldStatusType == FoldStatus::MAIN) {
    fold_screen_status_ = 2.0;
  } else {
    fold_screen_status_ = 1.0;
  }
}

template <typename T>
void MouseEventManagerExt::HandleCreateOverlay(T const& targeted_event) {
  if (!frame_ || !frame_->View()) {
    LOG(ERROR) << "MouseEventManagerExt::HandleCreateOverlay, frame_ is nullptr!";
    return;
  }
  HitTestLocation location(frame_->View()->ConvertFromRootFrame(
      gfx::ToFlooredPoint(targeted_event.PositionInRootFrame())));
  HitTestResult hit_test_result =
      frame_->GetEventHandler().HitTestResultAtLocation(location);

  Image* image = hit_test_result.GetImage();
  Node* inner_node = hit_test_result.InnerNodeOrImageMapImage();
  if (hit_test_result.AbsoluteImageURL().IsEmpty() || !image ||
      !IsValidOverlayNode(inner_node)) {
    LOG(INFO)
        << "MouseEventManagerExt::HandleCreateOverlay, invalid or has no image";
    return;
  }

  if (GetHitOverlayStatus(hit_test_result, true) != HitOverlayStatus::kNone) {
    LOG(INFO)
        << "MouseEventManagerExt::HandleCreateOverlay, hit last analyzer image";
    return;
  }

  if (hit_test_result.InnerNode() &&
      hit_test_result.InnerNode()->GetLayoutObject()) {
    if (!hit_test_result.InnerNode()
             ->GetLayoutObject()
             ->StyleRef()
             .IsSelectable()) {
      LOG(INFO)
          << "MouseEventManagerExt::HandleCreateOverlay image is not selectable";
      return;
    }
  }
  overlay_in_progress_ = false;
  last_analyzed_image_ = image;
  frame_->GetEventHandler().GetSelectionController().SetImageOverlayHitTest(
      hit_test_result);
  OnFoldStatusChanged(frame_->GetChromeClient().AsChromeClientExt()->GetFoldStatus(frame_));
  LOG(INFO) << "MouseEventManagerExt::HandleCreateOverlay fold_screen_status_ is "
            << fold_screen_status_;
  gfx::Rect image_rect =
      frame_->View()->FrameToDocument(hit_test_result.imp_utils_->ImageRect());
  gfx::Point touch_point = frame_->View()->FrameToDocument(
      gfx::ToRoundedPoint(targeted_event.PositionInRootFrame()));
  gfx::Rect view_rect = frame_->View()->FrameToDocument(
      ToEnclosingRect(frame_->View()->GetLayoutView()->ViewRect()));
  auto image_to_page_width_ratio =
      fold_screen_status_ * image_rect.width() / view_rect.width();
  LOG(INFO) << "MouseEventManagerExt::HandleCreateOverlay width ratio is "
            << image_to_page_width_ratio;
  int image_width = image->width();
  int image_height = image->height();
  if (1ll * image_width * image_height > kMaxAnalyzedImageArea) {
    LOG(ERROR) << "MouseEventManagerExt::HandleCreateOverlay, image is too large!";
    return;
  }
  if (image_width >= kMinAnalyzedImageWidth &&
      image_height >= kMinAnalyzedImageHeight &&
      (base::ohos::IsPcDevice() ||
       image_to_page_width_ratio > kMinAnalyzedImageToPageRatio)) {
    LOG(INFO)
        << "MouseEventManagerExt::HandleCreateOverlay, start, image w x h: "
        << image_width << " x " << image_height;
    PaintImage paint_image = image->PaintImageForCurrentFrame();
    if (!paint_image.GetSwSkImage()) {
      LOG(ERROR) << "MouseEventManagerExt::HandleCreateOverlay, "
                    "paint_image.GetSwSkImage() is null";
      return;
    }
    float shrink_ratio =
        std::min(1.0f, KMaxAnalyzedImageDimension * 1.0f /
                           std::max(image_width, image_height));
    paint_image = Image::ResizeAndOrientImage(
        paint_image, image->CurrentFrameOrientation(),
        gfx::Vector2dF(shrink_ratio, shrink_ratio));
    if (!paint_image.GetSwSkImage()) {
      LOG(ERROR) << "MouseEventManagerExt::CreateOverlay, downsampling failed.";
      return;
    }
    SetOverlayCreatingStatus(true);
    SkBitmap bm;
    paint_image.GetSwSkImage()->asLegacyBitmap(&bm);
    hit_image_node_ = inner_node;
    frame_->GetChromeClient().AsChromeClientExt()->CreateOverlay(
        frame_, bm,
        gfx::Point(touch_point.x() - image_rect.x(),
                   touch_point.y() - image_rect.y()),
        WTF::BindRepeating(&MouseEventManagerExt::GetAbsImageRect,
                           WrapPersistent(weak_factory_.GetWeakCell())),
        WTF::BindRepeating(&MouseEventManagerExt::SetOverlayInProgress,
                           WrapPersistent(weak_factory_.GetWeakCell())),
        WTF::BindRepeating(&MouseEventManagerExt::OnDestroyImageAnalyzerOverlay,
                           WrapPersistent(weak_factory_.GetWeakCell())));
  }
}

void MouseEventManagerExt::CloseImageOverlayWhenMousePress(const MouseEventWithHitTestResults& event) {
  if (GetHitOverlayStatusFromMouseEvent(event) == HitOverlayStatus::kNone) {
    LOG(INFO) << "HandleMousePressEvent CloseImageOverlay";
    CloseImageOverlay();
  }
}

// LCOV_EXCL_START
void MouseEventManagerExt::Trace(Visitor* visitor) const {
  MouseEventManager::Trace(visitor);
  visitor->Trace(frame_);
  visitor->Trace(scroll_manager_);
  visitor->Trace(element_under_mouse_);
  visitor->Trace(mouse_press_node_);
  visitor->Trace(mousedown_element_);
#if BUILDFLAG(ARKWEB_AI)
  visitor->Trace(hit_image_node_);
#endif
  visitor->Trace(weak_factory_);
  SynchronousMutationObserver::Trace(visitor);
}
// LCOV_EXCL_STOP

MouseEventManagerExt::OverLayerMouseLeaveEventListener::OverLayerMouseLeaveEventListener(blink::Element* element)
    : element_(element) {}

void MouseEventManagerExt::OverLayerMouseLeaveEventListener::Invoke(
    ExecutionContext*,
    Event* event) {
  if (element_ == nullptr) {
    LOG(ERROR) << "MouseEventManagerExt::OverLayerMouseLeaveEventListener : "
                  "element is nullptr";
    return;
  }
  if (event->type() == blink::event_type_names::kMouseleave) {
    element_->GetLayoutObject()->Parent()->GetNode()->removeChild(element_);
  }
}

// LCOV_EXCL_START
void MouseEventManagerExt::OverLayerMouseLeaveEventListener::Trace(
    Visitor* visitor) const {
  visitor->Trace(element_);
  NativeEventListener::Trace(visitor);
}
// LCOV_EXCL_STOP

#endif
}