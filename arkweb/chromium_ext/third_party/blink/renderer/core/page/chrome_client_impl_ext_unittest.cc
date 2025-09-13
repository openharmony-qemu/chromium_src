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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/page/chrome_client_impl_ext.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"

class ChromeClientImplExtTest : public testing::Test {
 protected:
  void SetUp() override {
    web_view_ = helper_.Initialize(&web_frame_client_);
    main_frame_ = helper_.LocalMainFrame();
    chrome_client_impl_ =
        To<ChromeClientImpl>(&web_view_->GetPage()->GetChromeClient());
    ASSERT_TRUE(chrome_client_impl_);
    chrome_client_impl_ext_ = chrome_client_impl_->AsChromeClientImplExt();
    ASSERT_TRUE(chrome_client_impl_ext_);
  }

  test::TaskEnvironment task_environment_;
  ViewCreatingClient web_frame_client_;
  frame_test_helpers::WebViewHelper helper_;
  WebViewImpl* web_view_;
  WebLocalFrame* main_frame_;
  Persistent<ChromeClientImpl> chrome_client_impl_;
  ChromeClientImplExt* chrome_client_impl_ext_;
};

TEST_F(ChromeClientImplExtTest, SetScrollbarAvoidAreaTop) {
  ScopedPagePauser pauser;
  LocalFrame* frame = To<WebLocalFrameImpl>(main_frame_)->GetFrame();
  ASSERT_TRUE(frame);
  FrameLoadRequest request(frame->DomWindow(), ResourceRequest());
  request.SetNavigationPolicy(kNavigationPolicyNewForegroundTab);
  EXPECT_TRUE(chrome_client_impl_ext_->SetScrollbarAvoidAreaTop(0.2));

  EXPECT_TRUE(chrome_client_impl_ext_->GetWebView());
  EXPECT_TRUE(chrome_client_impl_ext_->GetWebView()->MainFrameImpl());
  chrome_client_impl_ext_->GetWebView()->MainFrameImpl()->SetCoreFrame(nullptr);
  EXPECT_FALSE(chrome_client_impl_ext_->SetScrollbarAvoidAreaTop(0.2));
}

TEST_F(ChromeClientImplExtTest, SetScrollbarAvoidAreaBottom) {
  ScopedPagePauser pauser;
  LocalFrame* frame = To<WebLocalFrameImpl>(main_frame_)->GetFrame();
  ASSERT_TRUE(frame);
  FrameLoadRequest request(frame->DomWindow(), ResourceRequest());
  request.SetNavigationPolicy(kNavigationPolicyNewForegroundTab);
  EXPECT_TRUE(chrome_client_impl_ext_->SetScrollbarAvoidAreaBottom(0.2));

  EXPECT_TRUE(chrome_client_impl_ext_->GetWebView());
  EXPECT_TRUE(chrome_client_impl_ext_->GetWebView()->MainFrameImpl());
  chrome_client_impl_ext_->GetWebView()->MainFrameImpl()->SetCoreFrame(nullptr);
  EXPECT_FALSE(chrome_client_impl_ext_->SetScrollbarAvoidAreaBottom(0.2));
}

TEST_F(ChromeClientImplExtTest, GetVisibleRectToWeb) {
  ScopedPagePauser pauser;
  LocalFrame* frame = To<WebLocalFrameImpl>(main_frame_)->GetFrame();
  ASSERT_TRUE(frame);
  FrameLoadRequest request(frame->DomWindow(), ResourceRequest());
  request.SetNavigationPolicy(kNavigationPolicyNewForegroundTab);
  gfx::Rect rect = chrome_client_impl_ext_->GetVisibleRectToWeb(nullptr);
  EXPECT_EQ(rect.width(), 0);
}

TEST_F(ChromeClientImplExtTest, GetFoldStatus) {
  ScopedPagePauser pauser;
  LocalFrame* frame = To<WebLocalFrameImpl>(main_frame_)->GetFrame();
  ASSERT_TRUE(frame);
  FrameLoadRequest request(frame->DomWindow(), ResourceRequest());
  request.SetNavigationPolicy(kNavigationPolicyNewForegroundTab);
  EXPECT_EQ(chrome_client_impl_ext_->GetFoldStatus(nullptr), 0);
}
