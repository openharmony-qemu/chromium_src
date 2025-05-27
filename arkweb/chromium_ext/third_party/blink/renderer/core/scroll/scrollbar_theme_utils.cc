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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/scroll/scrollbar_theme_utils.h"
#include "third_party/blink/renderer/core/scroll/scrollbar.h"

#include "base/logging.h"
#include "third_party/blink/renderer/core/paint/paint_layer_scrollable_area.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_client.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/exported/web_view_impl.h"
#include "third_party/blink/public/web/web_settings.h"

namespace blink {
class ScrollableArea;
ScrollbarThemeUtils::ScrollbarThemeUtils(ScrollbarTheme* scrollbar_theme)
    : scrollbar_theme_(scrollbar_theme) {}

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
void ScrollbarThemeUtils::OffsetPointForHitTest(
    const Scrollbar& scrollbar,
    gfx::Point& in_out_test_position)
{
  double avoidAreaTopMargin = 0.0f;
  double RadiusTopRight = 0.0f;
  double RadiusBottomRight = 0.0f;

  ChromeClient* client =
      scrollbar.GetLayoutBox()->GetFrameView()->GetChromeClient();
  if (!client || !client->GetWebView() ||
      !client->GetWebView()->GetSettings()) {
    return;
  }
  auto setting = client->GetWebView()->GetSettings();
  auto scaleFromDIP = scrollbar.ScaleFromDIP();

  if (scrollbar.Orientation() == kVerticalScrollbar) {
    avoidAreaTopMargin = scrollbar.GetScrollbarAvoidAreaTop();
    RadiusTopRight = setting->GetBorderRadiusTopRight() * scaleFromDIP;
  } else if (scrollbar.Orientation() == kHorizontalScrollbar) {
    RadiusBottomRight = setting->GetBorderRadiusBottomRight() * scaleFromDIP;
  }

  in_out_test_position = gfx::Point(
      in_out_test_position.x() - RadiusBottomRight,
      in_out_test_position.y() - avoidAreaTopMargin - RadiusTopRight);
}

ScrollbarPart ScrollbarThemeUtils::HitTestUtils(
    const Scrollbar& scrollbar,
    const gfx::Point& test_position_original)
{
  auto test_position = test_position_original;
  OffsetPointForHitTest(scrollbar, test_position);

  if (!scrollbar.FrameRect().Contains(test_position)) {
    return kNoPart;
  }

  gfx::Rect track = scrollbar_theme_->TrackRect(scrollbar);
  if (track.Contains(test_position)) {
    gfx::Rect before_thumb_rect;
    gfx::Rect thumb_rect;
    gfx::Rect after_thumb_rect;
    scrollbar_theme_->SplitTrack(scrollbar, track, before_thumb_rect,
                                 thumb_rect, after_thumb_rect);
    if (thumb_rect.Contains(test_position)) {
      return kThumbPart;
    }
    if (before_thumb_rect.Contains(test_position)) {
      return kBackTrackPart;
    }
    if (after_thumb_rect.Contains(test_position)) {
      return kForwardTrackPart;
    }
    return kTrackBGPart;
  }

  if (scrollbar_theme_->BackButtonRect(scrollbar).Contains(test_position)) {
    return kBackButtonStartPart;
  }
  if (scrollbar_theme_->ForwardButtonRect(scrollbar).Contains(test_position)) {
    return kForwardButtonEndPart;
  }

  return kScrollbarBGPart;
}
#endif  // ARKWEB_SCROLLBAR_AVOID_AREA
}  // namespace blink
