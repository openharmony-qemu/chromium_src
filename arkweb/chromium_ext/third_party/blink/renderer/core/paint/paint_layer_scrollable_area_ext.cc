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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/paint_layer_scrollable_area_ext.h"

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/logging.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_client.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/layout/layout_view.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/paint/paint_invalidator.h"
#include "third_party/blink/renderer/core/paint/paint_layer.h"
#include "third_party/blink/renderer/core/paint/paint_layer_fragment.h"
#include "third_party/blink/renderer/core/scroll/scrollbar.h"

#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/core/exported/web_view_impl.h"
#include "third_party/blink/public/web/web_view.h"
#include "third_party/blink/public/web/web_settings.h"

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS) || BUILDFLAG(ARKWEB_SCROLLBAR)
#include "third_party/blink/renderer/core/frame/browser_controls.h"
#endif

namespace blink {
const int kScrollbarMargin = 4;
extern bool CanHaveOverflowScrollbars(const LayoutBox& box);
extern const LayoutObject& ScrollbarStyleSource(const LayoutBox& layout_box);

PaintLayerScrollableAreaExt::PaintLayerScrollableAreaExt(PaintLayer& layer)
    : PaintLayerScrollableArea(layer) {}

PaintLayerScrollableAreaExt::~PaintLayerScrollableAreaExt() {}

#if BUILDFLAG(ARKWEB_SCROLLBAR)
// LCOV_EXCL_START
float PaintLayerScrollableAreaExt::ComputeVisibleAreaScale() const {
  Page* page = GetLayoutBox()->GetDocument().GetPage();
  DCHECK(page);
  float scale_factor = page->PageScaleFactor();
  bool will_be_overlay = GetPageScrollbarTheme().UsesOverlayScrollbars();
  if (is_pinch_gesture_active_ && !base::ohos::IsPcDevice() &&
      scale_factor > 1.f && layer_->IsRootLayer() && will_be_overlay) {
    return scale_factor;
  }
  return 1.f;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void PaintLayerScrollableAreaExt::UpdateScrollbarLengthOrCreateWidthScale() {
  is_pinch_gesture_active_ = true;
  if (HorizontalScrollbar() && VerticalScrollbar()) {
    UpdateScrollbarProportions();
  } else {
    bool needs_horizontal_scrollbar;
    bool needs_vertical_scrollbar;
    bool needs_notify_location = false;
    ComputeScrollbarExistence(needs_horizontal_scrollbar,
                              needs_vertical_scrollbar);
    if (needs_horizontal_scrollbar && !HasHorizontalScrollbar()) {
      SetHasHorizontalScrollbar(true);
      needs_notify_location = true;
    }
    if (needs_vertical_scrollbar && !HasVerticalScrollbar()) {
      SetHasVerticalScrollbar(true);
      needs_notify_location = true;
    }
    if (needs_notify_location) {
      UpdateScrollbarProportions();
      ClampScrollOffsetAfterOverflowChange();
      PositionOverflowControls();
    }
  }
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void PaintLayerScrollableAreaExt::UpdateScrollbar() {
  UpdateScrollbarProportions();
  ClampScrollOffsetAfterOverflowChange();
  PositionOverflowControls();
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void PaintLayerScrollableAreaExt::UpdateScrollbarProportions() {
  auto scale_factor = ComputeVisibleAreaScale();
  if (Scrollbar* horizontal_scrollbar = HorizontalScrollbar()) {
    horizontal_scrollbar->SetProportion(VisibleWidth() / scale_factor,
                                        ContentsSize().width());
  }
  if (Scrollbar* vertical_scrollbar = VerticalScrollbar()) {
    vertical_scrollbar->SetProportion(VisibleHeight() / scale_factor,
                                      ContentsSize().height());
  }
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
gfx::Rect PaintLayerScrollableAreaExt::RectForHorizontalScrollbar() const {
  if (!HasHorizontalScrollbar()) {
    return gfx::Rect();
  }

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  ChromeClient* client = GetLayoutBox()->GetFrameView()->GetChromeClient();
  if (!client) {
    return PaintLayerScrollableArea::RectForHorizontalScrollbar();
  }
  auto webview = client->GetWebView();
  if (!webview) {
    return PaintLayerScrollableArea::RectForHorizontalScrollbar();
  }
  auto setting = webview->GetSettings();
  if (!setting) {
    return PaintLayerScrollableArea::RectForHorizontalScrollbar();
  }

  auto borderRadiusBottomLeft =
      setting->GetBorderRadiusBottomLeft() * ScaleFromDIP();
  auto borderRadiusBottomRight =
      setting->GetBorderRadiusBottomRight() * ScaleFromDIP();
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

  const gfx::Rect& scroll_corner = ScrollCornerRect();
  gfx::Size border_box_size = PixelSnappedBorderBoxSize();

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  int marginBottom = 0;
  if (HorizontalScrollbar()->IsOverlayScrollbar()) {
    marginBottom = ScaleFromDIP() * kScrollbarMargin;
  }

  float controlsOffset = 0;
  if (GetLayoutBox()->GetFrame() && GetLayoutBox()->GetFrame()->GetPage()) {
    const BrowserControls& controls =
        GetLayoutBox()->GetFrame()->GetPage()->GetBrowserControls();
    controlsOffset = (controls.TopShownRatio() > 0 ? 0 : controls.TopHeight());
  }

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
  int rectX = HorizontalScrollbarStart();
  int rectWidth =
      border_box_size.width() -
      (GetLayoutBox()->BorderLeft() + GetLayoutBox()->BorderRight()).ToInt() -
      scroll_corner.width();
  int rectHeight = HorizontalScrollbar()->ScrollbarThickness();
  borderRadiusBottomRight = borderRadiusBottomRight > scroll_corner.width()
                                ? borderRadiusBottomRight
                                : 0.0f;
  if (layer_ && !layer_->IsRootLayer()) {
    borderRadiusBottomLeft = 0.0f;
    borderRadiusBottomRight = 0.0f;
  }
  if (rectWidth > rectHeight) {
    // Horizontal scrollbar rect
    return gfx::Rect(
        rectX + borderRadiusBottomLeft,
        border_box_size.height() - GetLayoutBox()->BorderBottom().ToInt() -
            HorizontalScrollbar()->ScrollbarThickness() - marginBottom +
            controlsOffset,
        rectWidth - borderRadiusBottomRight - borderRadiusBottomLeft,
        HorizontalScrollbar()->ScrollbarThickness());
  }
  scrollbar_avoid_area_valid_top_ = scrollbar_avoid_area_top_;
  scrollbar_avoid_area_valid_bottom_ = scrollbar_avoid_area_bottom_;
  if (scrollbar_avoid_area_top_ < 0.0f || scrollbar_avoid_area_bottom_ < 0.0f) {
    LOG(ERROR) << " func:" << __FUNCTION__ << " scrollbar rectWidth"
               << rectWidth << " scrollbar rectHeight" << rectHeight
               << " scrollbar_avoid_area_top_" << scrollbar_avoid_area_top_
               << " scrollbar_avoid_area_bottom_"
               << scrollbar_avoid_area_bottom_;
    scrollbar_avoid_area_valid_top_ = 0.0f;
    scrollbar_avoid_area_valid_bottom_ = 0.0f;
  }
  double scaleFromDIP = 1.0f;
  int rectXModified = rectX + scaleFromDIP * scrollbar_avoid_area_valid_top_;
  int rectWidthModified =
      rectWidth - scaleFromDIP * (scrollbar_avoid_area_valid_top_ +
                                  scrollbar_avoid_area_valid_bottom_);
  if (rectXModified > rectWidth || rectWidthModified < 0.0f) {
    LOG(ERROR) << " func:" << __FUNCTION__ << " invalid Scrollbar avoid area";
    // Restore original scrollbar rect
    rectXModified = rectX;
    rectWidthModified = rectWidth;
    scrollbar_avoid_area_valid_top_ = 0.0f;
    scrollbar_avoid_area_valid_bottom_ = 0.0f;
  }
  return gfx::Rect(
      rectXModified + borderRadiusBottomLeft,
      border_box_size.height() - GetLayoutBox()->BorderBottom().ToInt() -
          HorizontalScrollbar()->ScrollbarThickness() - marginBottom +
          controlsOffset,
      rectWidthModified - borderRadiusBottomRight - borderRadiusBottomLeft,
      HorizontalScrollbar()->ScrollbarThickness());
#else
  return gfx::Rect(
      HorizontalScrollbarStart(),
      border_box_size.height() - GetLayoutBox()->BorderBottom().ToInt() -
          HorizontalScrollbar()->ScrollbarThickness() - marginBottom +
          controlsOffset,
      border_box_size.width() -
          (GetLayoutBox()->BorderLeft() + GetLayoutBox()->BorderRight())
              .ToInt() -
          scroll_corner.width(),
      HorizontalScrollbar()->ScrollbarThickness());
#endif  // ARKWEB_SCROLLBAR_AVOID_AREA
#endif  // ARKWEB_SCROLLBAR
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
gfx::Rect PaintLayerScrollableAreaExt::RectForVerticalScrollbar() const {
  if (!HasVerticalScrollbar()) {
    return gfx::Rect();
  }

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  ChromeClient* client = GetLayoutBox()->GetFrameView()->GetChromeClient();
  if (!client) {
    return PaintLayerScrollableArea::RectForVerticalScrollbar();
  }
  auto webview = client->GetWebView();
  if (!webview) {
    return PaintLayerScrollableArea::RectForVerticalScrollbar();
  }
  auto setting = webview->GetSettings();
  if (!setting) {
    return PaintLayerScrollableArea::RectForVerticalScrollbar();
  }

  auto borderRadiusTopRight =
      setting->GetBorderRadiusTopRight() * ScaleFromDIP();
  auto borderRadiusBottomRight =
      setting->GetBorderRadiusBottomRight() * ScaleFromDIP();
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

  const gfx::Rect& scroll_corner = ScrollCornerRect();
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
  int rectY = GetLayoutBox()->BorderTop().ToInt();
  int rectWidth = VerticalScrollbar()->ScrollbarThickness();
  int rectHeight =
      PixelSnappedBorderBoxSize().height() -
      (GetLayoutBox()->BorderTop() + GetLayoutBox()->BorderBottom()).ToInt() -
      scroll_corner.height();
  borderRadiusBottomRight = borderRadiusBottomRight > scroll_corner.height()
                                ? borderRadiusBottomRight
                                : 0.0f;
  if (layer_ && !layer_->IsRootLayer()) {
    borderRadiusTopRight = 0.0f;
    borderRadiusBottomRight = 0.0f;
  }
  if (rectWidth > rectHeight) {
    // Horizontal scrollbar rect
    return gfx::Rect(
        VerticalScrollbarStart(), rectY + borderRadiusTopRight,
        VerticalScrollbar()->ScrollbarThickness(),
        rectHeight - borderRadiusTopRight - borderRadiusBottomRight);
  }

  scrollbar_avoid_area_valid_top_ = scrollbar_avoid_area_top_;
  scrollbar_avoid_area_valid_bottom_ = scrollbar_avoid_area_bottom_;
  if (scrollbar_avoid_area_top_ < 0.0f || scrollbar_avoid_area_bottom_ < 0.0f) {
    LOG(ERROR) << " func:" << __FUNCTION__ << " scrollbar rectWidth"
               << rectWidth << " scrollbar rectHeight" << rectHeight
               << " scrollbar_avoid_area_top_" << scrollbar_avoid_area_top_
               << " scrollbar_avoid_area_bottom_"
               << scrollbar_avoid_area_bottom_;
    scrollbar_avoid_area_valid_top_ = 0.0f;
    scrollbar_avoid_area_valid_bottom_ = 0.0f;
  }
  double scaleFromDIP = 1.0f;
  int rectYModified = rectY + scaleFromDIP * scrollbar_avoid_area_valid_top_;
  int rectHeightModified =
      rectHeight - scaleFromDIP * (scrollbar_avoid_area_valid_top_ +
                                   scrollbar_avoid_area_valid_bottom_);
  if (rectYModified > rectHeight || rectHeightModified < 0.0f) {
    LOG(ERROR) << " func:" << __FUNCTION__ << " invalid Scrollbar avoid area";
    // Restore original scrollbar rect
    rectYModified = rectY;
    rectHeightModified = rectHeight;
    scrollbar_avoid_area_valid_top_ = 0.0f;
    scrollbar_avoid_area_valid_bottom_ = 0.0f;
  }
  return gfx::Rect(
      VerticalScrollbarStart(), rectYModified + borderRadiusTopRight,
      VerticalScrollbar()->ScrollbarThickness(),
      rectHeightModified - borderRadiusTopRight - borderRadiusBottomRight);
#endif  // ARKWEB_SCROLLBAR_AVOID_AREA
}
// LCOV_EXCL_STOP

int32_t PaintLayerScrollableAreaExt::ClampScrollOffsetLimit(
    int32_t scroll_offset_limit) const {
  if (scroll_offset_limit == 0) {
    return 0;
  }

  PhysicalSize layer_size = Size();
  // 1. root frame可以scroll时不处理paint layer的滚动,以防止元素整体上移
  if (GetLayoutBox() && GetLayoutBox()->GetFrameView() &&
      layer_size.height < ScrollHeight()) {
    LocalFrameView* parentFrameView = GetLayoutBox()->GetFrameView();
    while (parentFrameView && parentFrameView->ParentFrameView()) {
      parentFrameView = parentFrameView->ParentFrameView();
    }
    ScrollableArea* scrollableArea =
        (parentFrameView ? parentFrameView->GetScrollableArea() : nullptr);
    if (scrollableArea && scrollableArea->MaximumScrollOffset() !=
                              scrollableArea->MinimumScrollOffset()) {
      return 0;
    }
  }

  // 2. 当前root layer无法scroll时也不处理paint layer的滚动,以防止元素整体上移
  if (Layer()) {
    PaintLayer* parentLayer = Layer();
    while (parentLayer && parentLayer->Parent()) {
      parentLayer = parentLayer->Parent();
    }
    if (parentLayer && parentLayer->GetLayoutBox() &&
        parentLayer->GetLayoutBox()->ClientHeight() ==
            parentLayer->GetLayoutBox()->ScrollHeight()) {
      return 0;
    }
  }
  return scroll_offset_limit;
}
#endif  // ARKWEB_SCROLLBAR

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
// LCOV_EXCL_START
bool PaintLayerScrollableAreaExt::SetScrollbarAvoidAreaTop(double margin) {
  bool ret = true;
  scrollbar_avoid_area_top_ = margin;
  return ret;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
bool PaintLayerScrollableAreaExt::SetScrollbarAvoidAreaBottom(double margin) {
  bool ret = true;
  scrollbar_avoid_area_bottom_ = margin;
  return ret;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
double PaintLayerScrollableAreaExt::GetScrollbarAvoidAreaTop() const
{
  return scrollbar_avoid_area_valid_top_;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
double PaintLayerScrollableAreaExt::GetScrollbarAvoidAreaBottom() const
{
  return scrollbar_avoid_area_valid_bottom_;
}
// LCOV_EXCL_STOP
#endif  // ARKWEB_SCROLLBAR_AVOID_AREA

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
// LCOV_EXCL_START
bool PaintLayerScrollableAreaExt::HasScrollbarAvoidCorner() const
{
  ChromeClient* client = GetLayoutBox()->GetFrameView()->GetChromeClient();
  if (!client || !client->GetWebView() ||
      !client->GetWebView()->GetSettings()) {
    return false;
  }

  auto setting = client->GetWebView()->GetSettings();
  auto RadiusBottomRight =
      setting->GetBorderRadiusBottomRight() * ScaleFromDIP();

  if (!HasHorizontalScrollbar() || !HasVerticalScrollbar()) {
    return false;
  }
  auto horizontal_thickness = VerticalScrollbar()->ScrollbarThickness();
  auto vertical_thickness = HorizontalScrollbar()->ScrollbarThickness();
  auto scrollbar_thinkness = horizontal_thickness > vertical_thickness
                                 ? horizontal_thickness
                                 : vertical_thickness;

  if ((RadiusBottomRight > 0.0f) && (RadiusBottomRight > scrollbar_thinkness)) {
    return true;
  }
  return false;
}
// LCOV_EXCL_STOP
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void PaintLayerScrollableAreaExt::ComputeScrollbarExistence(
    bool& needs_horizontal_scrollbar,
    bool& needs_vertical_scrollbar,
    ComputeScrollbarExistenceOption option) const {
  // Scrollbars may be hidden or provided by visual viewport or frame instead.
  DCHECK(GetLayoutBox()->GetFrame()->GetSettings());
  if (VisualViewportSuppliesScrollbars() ||
      GetLayoutBox()->GetDocument().ViewportDefiningElement() ==
          GetLayoutBox()->GetNode() ||
      GetLayoutBox()->GetFrame()->GetSettings()->GetHideScrollbars() ||
      GetLayoutBox()->IsFieldset() || GetLayoutBox()->IsFrameSet() ||
      GetLayoutBox()->StyleRef().UsedScrollbarWidth() ==
          EScrollbarWidth::kNone) {
    needs_horizontal_scrollbar = false;
    needs_vertical_scrollbar = false;
    return;
  }

  mojom::blink::ScrollbarMode h_mode = mojom::blink::ScrollbarMode::kAuto;
  mojom::blink::ScrollbarMode v_mode = mojom::blink::ScrollbarMode::kAuto;

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool is_vertical_scrollbars_hide =
      GetLayoutBox()->GetFrame()->GetSettings()->GetVerticalHideScrollbars();
  bool is_horizontal_scrollbars_hide =
      GetLayoutBox()->GetFrame()->GetSettings()->GetHorizontalHideScrollbars();
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

  // First, determine what behavior the scrollbars say they should have.
  {
    if (auto* layout_view = DynamicTo<LayoutView>(GetLayoutBox())) {
      // LayoutView is special as there's various quirks and settings that
      // style doesn't account for.
      layout_view->CalculateScrollbarModes(h_mode, v_mode);
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
      h_mode = is_horizontal_scrollbars_hide
                   ? mojom::blink::ScrollbarMode::kAlwaysOff
                   : h_mode;
      v_mode = is_vertical_scrollbars_hide
                   ? mojom::blink::ScrollbarMode::kAlwaysOff
                   : v_mode;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
    } else {
      auto overflow_x = GetLayoutBox()->StyleRef().OverflowX();
      if (overflow_x == EOverflow::kScroll) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
        h_mode = is_horizontal_scrollbars_hide
                     ? mojom::blink::ScrollbarMode::kAlwaysOff
                     : mojom::blink::ScrollbarMode::kAlwaysOn;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
      } else if (overflow_x == EOverflow::kHidden ||
                 overflow_x == EOverflow::kVisible) {
        h_mode = mojom::blink::ScrollbarMode::kAlwaysOff;
      }

      auto overflow_y = GetLayoutBox()->StyleRef().OverflowY();
      if (overflow_y == EOverflow::kScroll) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
        v_mode = is_vertical_scrollbars_hide
                     ? mojom::blink::ScrollbarMode::kAlwaysOff
                     : mojom::blink::ScrollbarMode::kAlwaysOn;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
      } else if (overflow_y == EOverflow::kHidden ||
                 overflow_y == EOverflow::kVisible) {
        v_mode = mojom::blink::ScrollbarMode::kAlwaysOff;
      }
    }

    // Since overlay scrollbars (the fade-in/out kind, not overflow: overlay)
    // only appear when scrolling, we don't create them if there isn't overflow
    // to scroll. Thus, overlay scrollbars can't be "always on". i.e.
    // |overlay:scroll| behaves like |overlay:auto|.
    bool has_custom_scrollbar_style =
        ScrollbarStyleSource(*GetLayoutBox())
            .StyleRef()
            .HasCustomScrollbarStyle(GetElementForScrollStart());
    bool will_be_overlay = GetPageScrollbarTheme().UsesOverlayScrollbars() &&
                           !has_custom_scrollbar_style;
    if (will_be_overlay) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
      if (!is_horizontal_scrollbars_hide &&
          h_mode == mojom::blink::ScrollbarMode::kAlwaysOn) {
        h_mode = mojom::blink::ScrollbarMode::kAuto;
      }
      if (!is_vertical_scrollbars_hide &&
          v_mode == mojom::blink::ScrollbarMode::kAlwaysOn) {
        v_mode = mojom::blink::ScrollbarMode::kAuto;
      }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
    }
  }

  // By default, don't make any changes.
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  needs_horizontal_scrollbar =
      is_horizontal_scrollbars_hide ? false : HasHorizontalScrollbar();
  needs_vertical_scrollbar =
      is_vertical_scrollbars_hide ? false : HasVerticalScrollbar();
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

  // If the behavior doesn't depend on overflow or any other information, we
  // can set it now.
  {
    if (h_mode == mojom::blink::ScrollbarMode::kAlwaysOn) {
      needs_horizontal_scrollbar = true;
    } else if (h_mode == mojom::blink::ScrollbarMode::kAlwaysOff) {
      needs_horizontal_scrollbar = false;
    }

    if (v_mode == mojom::blink::ScrollbarMode::kAlwaysOn) {
      needs_vertical_scrollbar = true;
    } else if (v_mode == mojom::blink::ScrollbarMode::kAlwaysOff) {
      needs_vertical_scrollbar = false;
    }
  }
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  // Force needs_horizontal_scrollbar/needs_vertical_scrollbar value is false
  // ,if scrollbarAccess(false)
  if (is_horizontal_scrollbars_hide) {
    needs_horizontal_scrollbar = false;
  }

  if (is_vertical_scrollbars_hide) {
    needs_vertical_scrollbar = false;
  }
#endif  // ARKWEB_INPUT_EVENTS
  // If this is being performed before layout, we want to only update scrollbar
  // existence if its based on purely style based reasons.
  if (option == kOverflowIndependent) {
    return;
  }

  // If we have clean layout, we can make a decision on any scrollbars that
  // depend on overflow.
  {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
    if (!is_horizontal_scrollbars_hide &&
        h_mode == mojom::blink::ScrollbarMode::kAuto) {
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
      // Don't add auto scrollbars if the box contents aren't visible.
      needs_horizontal_scrollbar =
          GetLayoutBox()->IsRooted() && HasHorizontalOverflow() &&
          VisibleContentRect(kIncludeScrollbars).height();
    }
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
    if (!is_vertical_scrollbars_hide &&
        v_mode == mojom::blink::ScrollbarMode::kAuto) {
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
      needs_vertical_scrollbar = GetLayoutBox()->IsRooted() &&
                                 HasVerticalOverflow() &&
                                 VisibleContentRect(kIncludeScrollbars).width();
    }
  }
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
}  // namespace blink
