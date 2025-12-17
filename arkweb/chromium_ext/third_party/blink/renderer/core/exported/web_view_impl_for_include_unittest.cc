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

#include "third_party/blink/renderer/core/exported/web_view_impl.h"

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#if BUILDFLAG(ARKWEB_TEST)
#define private public
#endif
#include "third_party/blink/renderer/platform/scheduler/main_thread/page_scheduler_impl.h"
#if BUILDFLAG(ARKWEB_TEST)
#undef private
#endif
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/platform/testing/url_test_helpers.h"
#include "third_party/blink/public/web/web_frame.h"

namespace blink {

class WebViewImplTest : public testing::Test {
 protected:
  void SetUp() override {
    web_view_impl_ = helper_.InitializeAndLoad("about:blank");
  }
  void TearDown() override {
    helper_.Reset();
  }
  void TestSetPinchSmoothMode(bool isEnable) {
    web_view_impl_->SetPinchSmoothMode(isEnable);
  }
  bool TestGetPinchSmoothMode(){
    return web_view_impl_->pinch_smooth_mode;
  }
  test::TaskEnvironment task_environment_;
  WebViewImpl* web_view_impl_;
  frame_test_helpers::WebViewHelper helper_;
  std::string base_url_{"http://www.test.com/"};
};

TEST_F(WebViewImplTest, ApplyWebPreferences_NoCustomMediaPlayer) {
  gfx::PointF position = gfx::PointF(0, 0);
  web_view_impl_->SetScrollOffset(position);
  web_pref::WebPreferences prefs;
  prefs.custom_media_player_enabled = false;
  prefs.media_resume_from_bfcache_page = true;
  WebViewImpl* web_view = helper_.InitializeAndLoad("about:blank");
  web_view->MainFrameViewWidget()->Resize(gfx::Size(500, 500)); // 500, 500: width and height
  WebView::ApplyWebPreferences(prefs, web_view);
  EXPECT_EQ(web_view->GetSettings()->GetMediaResumeFromBFCachePage(), true);
}

TEST_F(WebViewImplTest, ApplyWebPreferences_CustomMediaPlayer) {
  gfx::PointF position = gfx::PointF(0, 0);
  web_view_impl_->SetScrollOffset(position);
  web_pref::WebPreferences prefs;
  prefs.custom_media_player_enabled = true;
  prefs.media_resume_from_bfcache_page = true;
  WebViewImpl* web_view = helper_.InitializeAndLoad("about:blank");
  web_view->MainFrameViewWidget()->Resize(gfx::Size(500, 500)); // 500, 500: width and height
  WebView::ApplyWebPreferences(prefs, web_view);
  EXPECT_EQ(web_view->GetSettings()->GetMediaResumeFromBFCachePage(), true);
}

TEST_F(WebViewImplTest, ApplyWebPreferencesForInclude) {
  gfx::PointF position = gfx::PointF(0, 0);
  web_view_impl_->SetScrollOffset(position);
  web_pref::WebPreferences prefs;
  prefs.custom_media_player_enabled = false;
  WebViewImpl* web_view = helper_.InitializeAndLoad("about:blank");
  web_view->MainFrameViewWidget()->Resize(gfx::Size(500, 500)); // 500, 500: width and height
  WebView::ApplyWebPreferencesForInclude(prefs, web_view);
  EXPECT_EQ(RuntimeEnabledFeatures::InputMultipleFieldsUIEnabled(), false);
}

TEST_F(WebViewImplTest, GetScrollBottom) {
  gfx::PointF position = gfx::PointF(0, 0);
  web_view_impl_->SetScrollOffset(position);
  float scroll_bottom = web_view_impl_->GetScrollBottom();
  EXPECT_NE(scroll_bottom, -1.0f);
}

TEST_F(WebViewImplTest, SetPinchSmoothMode_False) {
  TestSetPinchSmoothMode(false);
  EXPECT_EQ(TestGetPinchSmoothMode(), false);
}

TEST_F(WebViewImplTest, SetPinchSmoothMode_SameModeReturnsEarly) {
  TestSetPinchSmoothMode(false);
  EXPECT_EQ(TestGetPinchSmoothMode(), false);
  TestSetPinchSmoothMode(false);
  EXPECT_EQ(TestGetPinchSmoothMode(), false);
  TestSetPinchSmoothMode(true);
  EXPECT_EQ(TestGetPinchSmoothMode(), true);
  TestSetPinchSmoothMode(true);
  EXPECT_EQ(TestGetPinchSmoothMode(), true);
}

TEST_F(WebViewImplTest, SetPinchSmoothMode_ChangeMode) {
  TestSetPinchSmoothMode(false);
  EXPECT_EQ(TestGetPinchSmoothMode(), false);
  TestSetPinchSmoothMode(true);
  EXPECT_EQ(TestGetPinchSmoothMode(), true);
  TestSetPinchSmoothMode(false);
  EXPECT_EQ(TestGetPinchSmoothMode(), false);
}

TEST_F(WebViewImplTest, OnSetAdBlockEnable) {
  web_view_impl_->OnSetAdBlockEnable(false);
  EXPECT_EQ(web_view_impl_->GetAdBlockEnableForSite(), false);
  web_view_impl_->OnSetAdBlockEnable(true);
  EXPECT_EQ(web_view_impl_->GetAdBlockEnableForSite(), true);
}

TEST_F(WebViewImplTest, SetDelayDurationForBackgroundTabFreezing) {
  auto* page = web_view_impl_->GetPage();
  scheduler::PageSchedulerImpl* scheduler = static_cast<scheduler::PageSchedulerImpl*>(page->GetPageScheduler());
  int64_t duration = 1;
  web_view_impl_->SetDelayDurationForBackgroundTabFreezing(duration);
  EXPECT_EQ(scheduler->is_tab_freezing_enable_force, true);
}

TEST_F(WebViewImplTest, SetDelayDurationForBackgroundTabFreezing_InvalidDuration) {
  auto* page = web_view_impl_->GetPage();
  scheduler::PageSchedulerImpl* scheduler = static_cast<scheduler::PageSchedulerImpl*>(page->GetPageScheduler());
  int64_t duration = -1;
  web_view_impl_->SetDelayDurationForBackgroundTabFreezing(duration);
  EXPECT_NE(scheduler->is_tab_freezing_enable_force, true);
}

TEST_F(WebViewImplTest, SetPinchSmoothMode_NullPageMainFrame) {
  auto* main_frame = web_view_impl_->GetPage()->MainFrame();
  web_view_impl_->GetPage()->SetMainFrame(nullptr);
  TestSetPinchSmoothMode(true);
  EXPECT_EQ(TestGetPinchSmoothMode(), false);
  web_view_impl_->GetPage()->SetMainFrame(main_frame);
}

TEST_F(WebViewImplTest, OnSetAdBlockEnable_NullPageMainFrame) {
  auto* main_frame = web_view_impl_->GetPage()->MainFrame();
  web_view_impl_->OnSetAdBlockEnable(true);
  EXPECT_EQ(web_view_impl_->GetAdBlockEnableForSite(), true);
  web_view_impl_->GetPage()->SetMainFrame(nullptr);
  web_view_impl_->OnSetAdBlockEnable(true);
  EXPECT_EQ(web_view_impl_->GetAdBlockEnableForSite(), false);
  web_view_impl_->GetPage()->SetMainFrame(main_frame);
}

TEST_F(WebViewImplTest, UpdateFlingVelocityLimitEq) {
  gfx::Vector2dF velocity1{std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max()};
  gfx::Vector2dF velocity2{std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max()};
  web_view_impl_->UpdateFlingVelocityLimit(velocity1);
  EXPECT_EQ(velocity1, velocity2);
}

TEST_F(WebViewImplTest, UpdateFlingVelocityLimitNEq) {
  gfx::Vector2dF velocity1{5000, 5000};
  gfx::Vector2dF velocity2{std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max()};
  web_view_impl_->UpdateFlingVelocityLimit(velocity1);
  EXPECT_NE(velocity1, velocity2);
}

}  // namespace blink