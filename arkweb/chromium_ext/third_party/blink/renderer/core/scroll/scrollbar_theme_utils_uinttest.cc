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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#include "third_party/blink/renderer/core/scroll/scrollbar_theme_utils.h"
#include "third_party/blink/renderer/core/scroll/scrollbar.h"
#include "third_party/blink/renderer/core/scroll/scrollbar_test_suite.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/web_frame_widget_impl.h"
#include "third_party/blink/renderer/core/testing/sim/sim_request.h"
#include "third_party/blink/renderer/core/testing/sim/sim_test.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/page/focus_controller.h"
#include "third_party/blink/renderer/core/scroll/scrollbar_theme_aura.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"

namespace blink {

constexpr int POINT_VAL_A = 0;
constexpr int POINT_VAL_B = 10;
constexpr int POINT_VAL_C = 100;
constexpr int POINT_VAL_D = 150;
constexpr int POINT_VAL_E = 180;
constexpr int POINT_VAL_F = 200;
constexpr int POINT_VAL_G = 300;
constexpr int POINT_VAL_H = 400;
constexpr int POINT_VAL_I = 800;

namespace {

class ScrollbarThemeAuraButtonOverride final : public ScrollbarThemeAura {
 public:
  ScrollbarThemeAuraButtonOverride() = default;

  gfx::Rect TrackRect(const Scrollbar&) const override {
    return gfx::Rect(POINT_VAL_F, POINT_VAL_F, POINT_VAL_H, POINT_VAL_H);
  }

  gfx::Rect BackButtonRect(const Scrollbar&) const override {
    return gfx::Rect(POINT_VAL_D, POINT_VAL_D, POINT_VAL_B, POINT_VAL_B);
  }

  gfx::Rect ForwardButtonRect(const Scrollbar&) const override {
    return gfx::Rect(POINT_VAL_E, POINT_VAL_E, POINT_VAL_B, POINT_VAL_B);
  }

  void SplitTrack(const Scrollbar& scrollbar, const gfx::Rect& unconstrained_track_rect,
                  gfx::Rect& before_thumb_rect, gfx::Rect& thumb_rect,
                  gfx::Rect& after_thumb_rect) const override {
    before_thumb_rect = gfx::Rect(POINT_VAL_G, POINT_VAL_G, POINT_VAL_B, POINT_VAL_B);
    thumb_rect = gfx::Rect(POINT_VAL_F, POINT_VAL_F, POINT_VAL_B, POINT_VAL_B);
    after_thumb_rect = gfx::Rect(POINT_VAL_H, POINT_VAL_H, POINT_VAL_B, POINT_VAL_B);
  }
};

}  // namespace

class ScrollbarThemeUtilsTest: public SimTest {
public:
    HitTestResult HitTest(int x, int y) {
    ASSERT_TRUE(WebView().MainFrameViewWidget());
    return WebView().MainFrameViewWidget()->CoreHitTestResultAt(
        gfx::PointF(x, y));
  }
};

TEST_F(ScrollbarThemeUtilsTest, HitTestUtils001) {
    ASSERT_TRUE(WebView().MainFrameViewWidget());
    WebView().MainFrameViewWidget()->Resize(gfx::Size(POINT_VAL_F, POINT_VAL_F));
    SimRequest request("https://example.com/test.html", "text/html");
    LoadURL("https://example.com/test.html");
    request.Complete(R"HTML(
        <!DOCTYPE html>
        <style>
        .noscrollbars::-webkit-scrollbar { display: none; }
        #div{ height: 100px; width:100px; overflow:scroll; }
        .big{ height: 2000px; }
        body { overflow:scroll; }
        </style>
        <div id='div'>
        <div class='big'>
        </div>
        </div>
        <div class='big'>
        </div>
    )HTML");
    Compositor().BeginFrame();

    Document& document = GetDocument();
    Element* element = document.getElementById(AtomicString("div"));
    ASSERT_TRUE(element);
    auto* scrollable_div = element->GetLayoutBox()->GetScrollableArea();
    ASSERT_TRUE(scrollable_div);
    ASSERT_TRUE(GetDocument().View());
    ScrollableArea* scrollable_area = GetDocument().View()->LayoutViewport();
    ASSERT_TRUE(WebView().GetPage());
    ScrollbarTheme& theme = WebView().GetPage()->GetScrollbarTheme();
    auto utils = theme.GetUtils();
    ASSERT_TRUE(utils);
    Scrollbar* scrollbar = Scrollbar::CreateForTesting(scrollable_area, kHorizontalScrollbar, &theme);
    ASSERT_TRUE(scrollbar);
    gfx::Rect frame_rect = scrollbar->FrameRect();
    EXPECT_EQ(utils->HitTestUtils(*scrollbar, frame_rect.CenterPoint()), kScrollbarBGPart);

    gfx::Point bg_point(frame_rect.width() + frame_rect.width() , frame_rect.height() + frame_rect.height());
    EXPECT_EQ(utils->HitTestUtils(*scrollbar, bg_point), kNoPart);
}

TEST_F(ScrollbarThemeUtilsTest, HitTestUtils002) {
    WebView().MainFrameViewWidget()->Resize(gfx::Size(POINT_VAL_F, POINT_VAL_F));
    SimRequest request("https://example.com/test.html", "text/html");
    LoadURL("https://example.com/test.html");
    request.Complete(R"HTML(
        <!DOCTYPE html>
        <style>
        .noscrollbars::-webkit-scrollbar { display: none; }
        #div{ height: 100px; width:100px; overflow:scroll; }
        .big{ height: 2000px; }
        body { overflow:scroll; }
        </style>
        <div id='div'>
        <div class='big'>
        </div>
        </div>
        <div class='big'>
        </div>
    )HTML");
    Compositor().BeginFrame();

    Document& document = GetDocument();
    Element* element = document.getElementById(AtomicString("div"));
    ASSERT_TRUE(element);
    ASSERT_TRUE(element->GetLayoutBox());
    auto* scrollable_div = element->GetLayoutBox()->GetScrollableArea();
    ASSERT_TRUE(scrollable_div);
    ASSERT_TRUE(GetDocument().View());
    ScrollableArea* scrollable_area = GetDocument().View()->LayoutViewport();
    ASSERT_TRUE(WebView().GetPage());
    ScrollbarTheme& theme = WebView().GetPage()->GetScrollbarTheme();
    auto utils = theme.GetUtils();
    ASSERT_TRUE(utils);
    Scrollbar* scrollbar = Scrollbar::CreateForTesting(scrollable_area, kVerticalScrollbar, &theme);
    ASSERT_TRUE(scrollbar);
    scrollbar->SetFrameRect(gfx::Rect(POINT_VAL_A, POINT_VAL_A, POINT_VAL_C, POINT_VAL_C));

    gfx::Rect frame_rect = scrollbar->FrameRect();
    gfx::Rect track_rect = theme.TrackRect(*scrollbar);
    gfx::Rect thumb_rect = theme.ThumbRect(*scrollbar);
    gfx::Point p(track_rect.x(), track_rect.y());
    EXPECT_EQ(utils->HitTestUtils(*scrollbar, thumb_rect.CenterPoint()), kThumbPart);
}

TEST_F(ScrollbarThemeUtilsTest, HitTestUtils003) {
    ASSERT_TRUE(WebView().MainFrameViewWidget());
    WebView().MainFrameViewWidget()->Resize(gfx::Size(POINT_VAL_F, POINT_VAL_F));
    SimRequest request("https://example.com/test.html", "text/html");
    LoadURL("https://example.com/test.html");
    request.Complete(R"HTML(
        <!DOCTYPE html>
        <style>
        .noscrollbars::-webkit-scrollbar { display: none; }
        #div{ height: 100px; width:100px; overflow:scroll; }
        .big{ height: 2000px; }
        body { overflow:scroll; }
        </style>
        <div id='div'>
        <div class='big'>
        </div>
        </div>
        <div class='big'>
        </div>
    )HTML");
    Compositor().BeginFrame();

    Document& document = GetDocument();
    Element* element = document.getElementById(AtomicString("div"));
    ASSERT_TRUE(element);
    ASSERT_TRUE(element->GetLayoutBox());
    auto* scrollable_div = element->GetLayoutBox()->GetScrollableArea();
    ASSERT_TRUE(scrollable_div);
    ScrollableArea* scrollable_area = GetDocument().View()->LayoutViewport();
    ASSERT_TRUE(scrollable_area);
    ScrollbarThemeAuraButtonOverride theme;
    auto utils = theme.GetUtils();
    ASSERT_TRUE(utils);
    Scrollbar* scrollbar = Scrollbar::CreateForTesting(scrollable_area, kHorizontalScrollbar, &theme);
    ASSERT_TRUE(scrollbar);
    scrollbar->SetFrameRect(gfx::Rect(POINT_VAL_A, POINT_VAL_A, POINT_VAL_I, POINT_VAL_I));

    gfx::Rect track_rect = theme.TrackRect(*scrollbar);
    gfx::Rect before_thumb_rect;
    gfx::Rect thumb_rect;
    gfx::Rect after_thumb_rect;
    theme.SplitTrack(*scrollbar, track_rect, before_thumb_rect, thumb_rect, after_thumb_rect);

    gfx::Point thumb_point(thumb_rect.x() , thumb_rect.y());
    EXPECT_EQ(utils->HitTestUtils(*scrollbar, thumb_point), kThumbPart);

    gfx::Point before_thumb_point(before_thumb_rect.x(), before_thumb_rect.y());
    EXPECT_EQ(utils->HitTestUtils(*scrollbar, before_thumb_point), kBackTrackPart);

    gfx::Point after_thumb_point(after_thumb_rect.x(), after_thumb_rect.y());
    EXPECT_EQ(utils->HitTestUtils(*scrollbar, after_thumb_point), kForwardTrackPart);

    gfx::Point no_thumb_point(after_thumb_rect.x() + POINT_VAL_C, after_thumb_rect.y() + POINT_VAL_C);
    EXPECT_EQ(utils->HitTestUtils(*scrollbar, no_thumb_point), kTrackBGPart);

    gfx::Rect back_rect = theme.BackButtonRect(*scrollbar);
    gfx::Point back_point(back_rect.x(), back_rect.y());
    EXPECT_EQ(utils->HitTestUtils(*scrollbar, back_point), kBackButtonStartPart);

    gfx::Rect forward_rect = theme.ForwardButtonRect(*scrollbar);
    gfx::Point forward_point(forward_rect.x(), forward_rect.y());
    EXPECT_EQ(utils->HitTestUtils(*scrollbar, forward_point), kForwardButtonEndPart);
}

} // namespace blink