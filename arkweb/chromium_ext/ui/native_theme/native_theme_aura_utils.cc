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

#include "arkweb/chromium_ext/ui/native_theme/native_theme_aura_utils.h"

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/logging.h"
#include "cc/paint/paint_canvas.h"
#include "cc/paint/paint_flags.h"
#include "ui/color/color_provider.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rrect_f.h"
#include "ui/gfx/image/image_skia.h"

namespace ui {
// 1 vp = 1.5 * px
constexpr int kForceScrollbarActiveWidth = 12;
constexpr int kForceScrollbarActiveRadius = 6;
constexpr int kForceScrollbarActiveOffset = 0;
constexpr int kForceScrollbarActiveHotSize = 4;
constexpr int kForceScrollbarInactiveWidth = 12;
constexpr int kForceScrollbarInactiveRadius = 3;
constexpr int kForceScrollbarInactiveOffset = 0;
constexpr int kForceScrollbarInactiveHotSize = 8;
// Scrollbar's width,include hot zone(20) + visible width(8) + marginRight(4)
constexpr int kOverlayScrollbarHotSize = 20;
constexpr int kOverlayScrollbarHotSizePc = 0;
int scrollbar_hot_size_ = kOverlayScrollbarHotSize;
constexpr int kOverlayScrollbarDoubleOrHalf = 2;
constexpr int kOverlayScrollbarMinimumLength = 48;
constexpr int kOverlayScrollbarBorderPatchWidth = 0;
constexpr float kOverlayScrollbarCornerRatio = 1.2f;

NativeThemeAuraUtils::NativeThemeAuraUtils(NativeThemeAura* native_theme_aura)
    : native_theme_aura_(native_theme_aura) {}

// LCOV_EXCL_START
void NativeThemeAuraUtils::SetScrollbarThumbWidth() {
  if (base::ohos::IsPcDevice()) {
    native_theme_aura_->scrollbar_width_ = kOverlayScrollbarThumbWidthPressedPc;
    scrollbar_hot_size_ = kOverlayScrollbarHotSizePc;
  } else {
    native_theme_aura_->scrollbar_width_ = kOverlayScrollbarThumbWidthPressed;
  }
}
// LCOV_EXCL_STOP

void NativeThemeAuraUtils::PaintOverlayScrollbarThumb(
    cc::PaintCanvas* canvas,
    const gfx::Rect& rect,
    SkColor scrollbar_color,
    NativeTheme::Part part,
    SkColor& thumb_color,
    cc::PaintFlags& fill_flags) {
  bool isPcDevice = base::ohos::IsPcDevice();
  float ratio = base::ohos::GetPixelRatio();
  gfx::Rect thumb_rect(rect);
  thumb_color =
      SK_ColorTRANSPARENT;  // The hot zone is a transparent rectangle.
  SkColor aroundColor = SkColorSetA(
      scrollbar_color,
      102);  // The color of the visible thumb, with an opacity of 40%
  cc::PaintFlags flags;
  flags.setColor(aroundColor);
  flags.setAntiAlias(true);
  gfx::Rect aroundRRect;  // Draw rect on aroundRRect's position.
  LOG(DEBUG) << "PaintScrollbarThumb GetVirtualPixelRatio ratio:" << ratio;
  int drawThumbThickness = native_theme_aura_->scrollbar_width_ * ratio -
                           scrollbar_hot_size_ * ratio;
  SkScalar radius =
      SkIntToScalar(drawThumbThickness / kOverlayScrollbarDoubleOrHalf);
  SkScalar radiusX = 0.0;
  if (part == NativeTheme::kScrollbarHorizontalThumb) {
    int horizontalX = isPcDevice ? thumb_rect.x() : 0;
    int horizontalY =
        isPcDevice ? thumb_rect.y() + scrollbar_hot_size_ : scrollbar_hot_size_;
    aroundRRect = gfx::Rect(horizontalX * ratio, horizontalY * ratio,
                            thumb_rect.width(), drawThumbThickness);
    radius = SkIntToScalar(radius * kOverlayScrollbarCornerRatio);
    radiusX = SkIntToScalar(drawThumbThickness / kOverlayScrollbarDoubleOrHalf);
  } else {
    int verticalX =
        isPcDevice ? thumb_rect.x() + scrollbar_hot_size_ : scrollbar_hot_size_;
    int verticalY = isPcDevice ? thumb_rect.y() : 0;
    aroundRRect = gfx::Rect(verticalX * ratio, verticalY * ratio,
                            drawThumbThickness, thumb_rect.height());
    radiusX = SkIntToScalar(radius * kOverlayScrollbarCornerRatio);
  }
  gfx::RRectF rounded_rect(gfx::RectF(aroundRRect), radiusX, radius, radiusX,
                           radius, radiusX, radius, radiusX, radius);
  fill_flags.setColor(thumb_color);
  canvas->drawRRect(static_cast<SkRRect>(rounded_rect), flags);
}

void NativeThemeAuraUtils::PaintScrollbarThumbWithColor(
    cc::PaintCanvas* canvas,
    const gfx::Rect& rect,
    SkColor scrollbar_color,
    NativeTheme::ColorScheme& color_scheme,
    NativeTheme::Part part,
    NativeTheme::State state,
    const NativeTheme::ScrollbarThumbExtraParams& extra_params) {
  cc::PaintFlags overflags;
  SkScalar radius;
  gfx::Rect aroundRRect;  // Draw rect on aroundRRect's position.
  gfx::Rect thumb_rect(rect);
  if (color_scheme == NativeTheme::ColorScheme::kDark) {
    overflags.setColor(SkColorSetA(SK_ColorWHITE, 102));
  } else {
    overflags.setColor(SkColorSetA(SK_ColorBLACK, 102));
  }
  float ratio = base::ohos::GetPixelRatio();
  if (extra_params.thumb_color.has_value()) {
    overflags.setColor(SkColor4f::FromColor(extra_params.thumb_color.value()));
  }
  if (state == NativeTheme::kHovered || state == NativeTheme::kPressed) {
    radius = SkIntToScalar(kForceScrollbarActiveRadius * ratio);
    if (part == NativeTheme::kScrollbarVerticalThumb) {
      thumb_rect.set_x(thumb_rect.x() + thumb_rect.width() -
                       kForceScrollbarActiveWidth * ratio -
                       kForceScrollbarActiveOffset * ratio);
      thumb_rect.set_width(kForceScrollbarActiveWidth * ratio);
      aroundRRect = gfx::Rect(kForceScrollbarActiveHotSize * ratio, 0,
                              kForceScrollbarActiveWidth * ratio -
                                  kForceScrollbarActiveHotSize * ratio,
                              thumb_rect.height());
    } else {
      thumb_rect.set_y(thumb_rect.y() + thumb_rect.height() -
                       kForceScrollbarActiveWidth * ratio -
                       kForceScrollbarActiveOffset * ratio);
      thumb_rect.set_height(kForceScrollbarActiveWidth * ratio);
      aroundRRect =
          gfx::Rect(0, kForceScrollbarActiveHotSize * ratio, thumb_rect.width(),
                    kForceScrollbarActiveWidth * ratio -
                        kForceScrollbarActiveHotSize * ratio);
    }
  } else {
    radius = SkIntToScalar(kForceScrollbarInactiveRadius * ratio);
    if (part == NativeTheme::kScrollbarVerticalThumb) {
      thumb_rect.set_x(thumb_rect.x() + thumb_rect.width() -
                       kForceScrollbarInactiveOffset * ratio);
      thumb_rect.set_width(kForceScrollbarInactiveWidth * ratio);
      aroundRRect = gfx::Rect(kForceScrollbarInactiveHotSize * ratio, 0,
                              kForceScrollbarInactiveWidth * ratio -
                                  kForceScrollbarInactiveHotSize * ratio,
                              thumb_rect.height());
    } else {
      thumb_rect.set_y(thumb_rect.y() + thumb_rect.height() -
                       kForceScrollbarInactiveOffset * ratio);
      thumb_rect.set_height(kForceScrollbarInactiveWidth * ratio);
      aroundRRect = gfx::Rect(0, kForceScrollbarInactiveHotSize * ratio,
                              thumb_rect.width(),
                              kForceScrollbarInactiveWidth * ratio -
                                  kForceScrollbarInactiveHotSize * ratio);
    }
  }
  gfx::RRectF rounded_rect(gfx::RectF(aroundRRect), radius, radius, radius,
                           radius, radius, radius, radius, radius);
  canvas->drawRRect(static_cast<SkRRect>(rounded_rect), overflags);
}

gfx::Size NativeThemeAuraUtils::GetPartSize(
    NativeTheme::Part part,
    NativeTheme::State state,
    const NativeTheme::ExtraParams& extra) const {
  if (native_theme_aura_->use_overlay_scrollbar()) {
    constexpr int minimum_length =
        kOverlayScrollbarMinimumLength + 2 * kOverlayScrollbarStrokeWidth;

#if BUILDFLAG(ARKWEB_SCROLLBAR)
    float ratio = base::ohos::GetPixelRatio();
    // Aura overlay scrollbars need a slight tweak from the base sizes.
    switch (part) {
      case NativeTheme::kScrollbarHorizontalThumb:
        return gfx::Size(minimum_length * ratio,
                         native_theme_aura_->scrollbar_width_);
      case NativeTheme::kScrollbarVerticalThumb:
        return gfx::Size(native_theme_aura_->scrollbar_width_,
                         minimum_length * ratio);

      default:
        // TODO(bokan): We should probably make sure code using overlay
        // scrollbars isn't asking for part sizes that don't exist.
        // crbug.com/657159.
        break;
    }
  } else {
    switch (part) {
      case NativeTheme::kScrollbarDownArrow:
      case NativeTheme::kScrollbarUpArrow:
        return gfx::Size(native_theme_aura_->scrollbar_width_, 0);
      case NativeTheme::kScrollbarLeftArrow:
      case NativeTheme::kScrollbarRightArrow:
        return gfx::Size(0, native_theme_aura_->scrollbar_width_);
      default:
        break;
    }
#endif  // BUILDFLAG(ARKWEB_SCROLLBAR)
  }
  return gfx::Size(0, 0);
}

gfx::Size NativeThemeAuraUtils::GetNinePatchCanvasSize(
    NativeTheme::Part part) const {
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  float ratio = base::ohos::GetPixelRatio();
  return gfx::Size(
      (kOverlayScrollbarBorderPatchWidth * kOverlayScrollbarDoubleOrHalf +
       native_theme_aura_->scrollbar_width_) *
          ratio,
      (kOverlayScrollbarBorderPatchWidth * kOverlayScrollbarDoubleOrHalf +
       native_theme_aura_->scrollbar_width_) *
          ratio);
#endif  // ARKWEB_SCROLLBAR
}

gfx::Rect NativeThemeAuraUtils::GetNinePatchAperture(
    NativeTheme::Part part) const {
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  float ratio = base::ohos::GetPixelRatio();
  if (part == NativeTheme::kScrollbarHorizontalThumb) {
    return gfx::Rect(native_theme_aura_->scrollbar_width_ * ratio /
                         kOverlayScrollbarDoubleOrHalf,
                     kOverlayScrollbarBorderPatchWidth * ratio, (int)ratio,
                     native_theme_aura_->scrollbar_width_ * ratio);
  } else {
    return gfx::Rect(kOverlayScrollbarBorderPatchWidth * ratio,
                     native_theme_aura_->scrollbar_width_ * ratio /
                         kOverlayScrollbarDoubleOrHalf,
                     native_theme_aura_->scrollbar_width_ * ratio, (int)ratio);
  }
#endif  // ARKWEB_SCROLLBAR
}

}  // namespace ui
