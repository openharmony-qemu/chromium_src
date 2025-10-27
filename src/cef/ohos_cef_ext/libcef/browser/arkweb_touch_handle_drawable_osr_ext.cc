// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/browser/arkweb_touch_handle_drawable_osr_ext.h"

#include <cmath>

#include "arkweb/build/features/features.h"
#include "cef/libcef/browser/osr/render_widget_host_view_osr.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"

#if BUILDFLAG(IS_ARKWEB)
#include "cef/ohos_cef_ext/include/arkweb_render_handler_ext.h"
#endif

namespace {
// Copied from touch_handle_drawable_aura.cc

// The distance by which a handle image is offset from the focal point (i.e.
// text baseline) downwards.
constexpr int kSelectionHandleVerticalVisualOffset = 2;

// The padding around the selection handle image can be used to extend the
// handle so that touch events near the selection handle image are
// targeted to the selection handle.
constexpr int kSelectionHandlePadding = 0;

}  // namespace

ArkWebCefTouchHandleDrawableOSRExt::ArkWebCefTouchHandleDrawableOSRExt(
    CefRenderWidgetHostViewOSR* rwhv)
    : CefTouchHandleDrawableOSR(rwhv) {}

void ArkWebCefTouchHandleDrawableOSRExt::SetOrigin(
    const gfx::PointF& position) {
  if (position == origin_position_) {
    return;
  }

  origin_position_ = position;

#if !BUILDFLAG(ARKWEB_MENU)
  CefTouchHandleState touch_handle_state;
  touch_handle_state.touch_handle_id = id_;
  touch_handle_state.flags = CEF_THS_FLAG_ORIGIN;
  touch_handle_state.origin = {static_cast<int>(std::round(position.x())),
                               static_cast<int>(std::round(position.y()))};
  TouchHandleStateChanged(touch_handle_state);
#endif  // !BUILDFLAG(ARKWEB_MENU)
}

void ArkWebCefTouchHandleDrawableOSRExt::SetAlpha(float alpha) {
  if (alpha == alpha_) {
    return;
  }

  alpha_ = alpha;

#if !BUILDFLAG(ARKWEB_MENU)
  CefTouchHandleState touch_handle_state;
  touch_handle_state.touch_handle_id = id_;
  touch_handle_state.flags = CEF_THS_FLAG_ALPHA;
  touch_handle_state.alpha = alpha_;
  TouchHandleStateChanged(touch_handle_state);
#endif  // !BUILDFLAG(ARKWEB_MENU)
}

gfx::RectF ArkWebCefTouchHandleDrawableOSRExt::GetVisibleBounds() const {
  gfx::RectF bounds = relative_bounds_;
  bounds.Offset(origin_position_.x(), origin_position_.y());

#if !BUILDFLAG(ARKWEB_MENU)
  bounds.Inset(gfx::InsetsF::TLBR(
      kSelectionHandlePadding,
      kSelectionHandlePadding + kSelectionHandleVerticalVisualOffset,
      kSelectionHandlePadding, kSelectionHandlePadding));
#endif  // BUILDFLAG(ARKWEB_MENU)
  return bounds;
}

#if BUILDFLAG(ARKWEB_MENU)
void ArkWebCefTouchHandleDrawableOSRExt::SetEdge(const gfx::PointF& top,
                                                 const gfx::PointF& bottom) {
  edge_height_ = std::abs(bottom.y() - top.y());

  UpdateVisiableBounds();
}

void ArkWebCefTouchHandleDrawableOSRExt::UpdateVisiableBounds() {
  CefSize size;
  auto browser = rwhv_->browser_impl();
  auto handler = browser->GetClient()->GetRenderHandler();
  handler->GetTouchHandleSize(
      browser.get(), static_cast<cef_horizontal_alignment_t>(orientation_),
      size);

  const gfx::Size& image_size = gfx::Size(size.width, size.height);
  int handle_width = image_size.width() + 2 * kSelectionHandlePadding;
  int handle_height = image_size.height() + 2 * kSelectionHandlePadding;
  if (orientation_ != ui::TouchHandleOrientation::LEFT) {
    relative_bounds_ = gfx::RectF(-handle_width / 2.0, -edge_height_,
                                  handle_width, handle_height + edge_height_);
  } else {
    relative_bounds_ =
        gfx::RectF(-handle_width / 2.0, -edge_height_ - handle_height,
                   handle_width, handle_height + edge_height_);
  }
}
#endif  // BUILDFLAG(ARKWEB_MENU)
