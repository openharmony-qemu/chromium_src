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

#include "arkweb/chromium_ext/content/browser/display_cutout/display_cutout_host_ohos.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "base/test/task_environment.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/test/test_renderer_host.h"
#include "third_party/blink/public/mojom/page/display_cutout.mojom.h"
#include "ui/gfx/geometry/insets.h"

namespace content {
class TestableDisplayCutoutHostOhos : public DisplayCutoutHostOhos {
 public:
  explicit TestableDisplayCutoutHostOhos(WebContentsImpl* web_contents)
      : DisplayCutoutHostOhos(web_contents) {}

  blink::mojom::ViewportFit GetMainFrameViewportFitForTest() const {
    return mainFrameViewportFit_;
  }

  void SetCurrentRenderFrameHostForTest(RenderFrameHost* rfh,blink::mojom::ViewportFit value) {
    SetCurrentRenderFrameHost(rfh, value);
  }

  gfx::Insets GetInsetsForTest() const { return insets_; }

  using DisplayCutoutHostOhos::SetCurrentRenderFrameHost;
};
class MockDisplayCutoutHost : public blink::mojom::DisplayCutoutHost {
 public:
  MockDisplayCutoutHost() = default;
  ~MockDisplayCutoutHost() override = default;
  MOCK_METHOD(void, SendSafeAreaToFrame, (RenderFrameHost * rfh, gfx::Insets insets));
};

class DisplayCutoutHostOhosTest : public RenderViewHostTestHarness {
 public:
  DisplayCutoutHostOhosTest()
      : RenderViewHostTestHarness(base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}
  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    web_contents_impl_ = static_cast<WebContentsImpl*>(web_contents());
    testable_display_cutout_host_ = 
        std::make_unique<TestableDisplayCutoutHostOhos>(web_contents_impl_);
  }

  void TearDown() override {
    testable_display_cutout_host_.reset();
    RenderViewHostTestHarness::TearDown();
  }

  TestableDisplayCutoutHostOhos* display_cutout_host() {
    return testable_display_cutout_host_.get();
  }
  WebContentsImpl* web_contents_impl() { return web_contents_impl_; }

 protected:
  WebContentsImpl* web_contents_impl_;
  std::unique_ptr<TestableDisplayCutoutHostOhos> testable_display_cutout_host_;
  std::unique_ptr<MockDisplayCutoutHost> mock_display_cutout_;
};

TEST_F(DisplayCutoutHostOhosTest, SetDisplayCutoutSafeArea_SameInsets) {
  gfx::Insets test_insets = 
      gfx::Insets().set_top(10).set_left(20).set_bottom(30).set_right(40);
  display_cutout_host()->SetDisplayCutoutSafeArea(test_insets);
  EXPECT_NO_FATAL_FAILURE( display_cutout_host()->SetDisplayCutoutSafeArea(test_insets));
  EXPECT_EQ(display_cutout_host()->GetInsetsForTest(), test_insets);
}

TEST_F(DisplayCutoutHostOhosTest, SetDisplayCutoutSafeArea_DifferentInsets) {
  gfx::Insets insets1 =
      gfx::Insets().set_top(10).set_left(20).set_bottom(30).set_right(40);
  gfx::Insets insets2 = 
      gfx::Insets().set_top(10).set_left(10).set_bottom(10).set_right(10);
  display_cutout_host()->SetDisplayCutoutSafeArea(insets1);
  EXPECT_NO_FATAL_FAILURE(display_cutout_host()->SetDisplayCutoutSafeArea(insets2));
  EXPECT_EQ(display_cutout_host()->GetInsetsForTest(), insets2);
}

TEST_F(DisplayCutoutHostOhosTest, SetCurrentRenderFrameHost_NullPointer) {
  EXPECT_NO_FATAL_FAILURE(display_cutout_host()->SetCurrentRenderFrameHost(
      nullptr, blink::mojom::ViewportFit::kAuto));
}

TEST_F(DisplayCutoutHostOhosTest, SetCurrentRenderFrameHost_MainFrameCover) {
  RenderFrameHostImpl* rfh = static_cast<RenderFrameHostImpl*>(main_rfh());
  EXPECT_NO_FATAL_FAILURE(
      display_cutout_host()->SetCurrentRenderFrameHostForTest(
          rfh, blink::mojom::ViewportFit::kCover));
  EXPECT_EQ(display_cutout_host()->GetMainFrameViewportFitForTest(),
            blink::mojom::ViewportFit::kCover);
}

TEST_F(DisplayCutoutHostOhosTest, SetCurrentRenderFrameHost_MainFrameNonCover) {
  RenderFrameHostImpl* rfh = static_cast<RenderFrameHostImpl*>(main_rfh());
  EXPECT_NO_FATAL_FAILURE(
      display_cutout_host()->SetCurrentRenderFrameHostForTest(
          rfh, blink::mojom::ViewportFit::kContain));

  EXPECT_EQ(display_cutout_host()->GetMainFrameViewportFitForTest(),
      blink::mojom::ViewportFit::kContain);
}

TEST_F(DisplayCutoutHostOhosTest, LargeInsetsValues) {
  gfx::Insets large_insets =
      gfx::Insets().set_top(999).set_left(888).set_bottom(777).set_right(666);
  EXPECT_NO_FATAL_FAILURE(
      display_cutout_host()->SetDisplayCutoutSafeArea(large_insets));
  EXPECT_EQ(display_cutout_host()->GetInsetsForTest(), large_insets);
}

TEST_F(DisplayCutoutHostOhosTest, NegativeInsetsValues) {
  gfx::Insets negative_insets =
      gfx::Insets().set_top(-10).set_left(-20).set_bottom(-30).set_right(-40);
  EXPECT_NO_FATAL_FAILURE(
      display_cutout_host()->SetDisplayCutoutSafeArea(negative_insets));
  EXPECT_EQ(display_cutout_host()->GetInsetsForTest(), negative_insets);
}

}  // namespace content