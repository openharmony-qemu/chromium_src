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

#include "content/public/test/mock_navigation_handle.h"
#include "chrome/browser/chrome_content_browser_client.h"
#include "content/test/test_render_view_host.h"
#include "content/test/test_render_frame_host.h"

class ChromeContentBrowserClientForIncludeTest : public content::RenderViewHostImplTestHarness {
 protected:
  void SetUp() override {
    content::RenderViewHostTestHarness::SetUp();
    client_ = std::make_unique<ChromeContentBrowserClient>();
  }

  void TearDown() override {
    client_.reset();
    content::RenderViewHostTestHarness::TearDown();
  }

  std::string TestOverrideErrorPage(content::FrameTreeNodeId frame_tree_node_id,
                                    bool browser_initiated,
                                    const GURL& gurl,
                                    const std::string& request_method,
                                    bool has_user_gesture,
                                    bool is_redirect,
                                    bool is_outermost_main_frame,
                                    int error_code,
                                    const std::string& error_text,
                                    bool is_prerendering,
                                    ui::PageTransition transition,
                                    std::string* html) {
    return client_->OverrideErrorPage(frame_tree_node_id, browser_initiated, gurl,
                                      request_method, has_user_gesture, is_redirect,
                                      is_outermost_main_frame, error_code, error_text,
                                      is_prerendering, transition, html);
  }

  bool TestShouldOverrideUrlLoading(content::FrameTreeNodeId frame_tree_node_id,
                                    bool browser_initiated,
                                    const GURL& gurl,
                                    const std::string& request_method,
                                    bool has_user_gesture,
                                    bool is_redirect,
                                    bool is_outermost_main_frame,
                                    bool is_prerendering,
                                    ui::PageTransition transition,
                                    bool* ignore_navigation) {
    return client_->ShouldOverrideUrlLoading(frame_tree_node_id,
                                            browser_initiated, gurl, request_method,
                                            has_user_gesture, is_redirect,
                                            is_outermost_main_frame, is_prerendering,
                                            transition, ignore_navigation);
  }

  std::unique_ptr<ChromeContentBrowserClient> client_;
};

TEST_F(ChromeContentBrowserClientForIncludeTest, OverrideErrorPage_WebContentsIsNull) {
  content::FrameTreeNodeId invalid_id;
  GURL test_url("https://example.com");
  std::string request_method = "GET";
  bool has_user_gesture = true;
  bool is_redirect = false;
  bool is_outermost_main_frame = true;
  int error_code = 404;
  std::string error_text = "Not Found";
  bool is_prerendering = false;
  ui::PageTransition transition = ui::PAGE_TRANSITION_LINK;
  std::string html = "original_html";

  std::string result = TestOverrideErrorPage(
      invalid_id, true, test_url, request_method, has_user_gesture,
      is_redirect, is_outermost_main_frame, error_code, error_text,
      is_prerendering, transition, &html);

  EXPECT_EQ(result, "");
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading001) {
  bool ignore_navigation = true;
  
  bool result = TestShouldOverrideUrlLoading(
      main_rfh()->GetFrameTreeNodeId(),  // frame_tree_node_id
      false,                             // browser_initiated
      GURL("https://example.com"),       // gurl
      "POST",                            // request_method
      true,                              // has_user_gesture
      false,                             // is_redirect
      true,                              // is_outermost_main_frame
      false,                             // is_prerendering
      ui::PAGE_TRANSITION_LINK,          // transition
      &ignore_navigation);
  
  EXPECT_TRUE(result);
  EXPECT_FALSE(ignore_navigation);
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading002) {
  bool ignore_navigation = true;
  
  bool result = TestShouldOverrideUrlLoading(
      main_rfh()->GetFrameTreeNodeId(),
      false,
      GURL("chrome-extension://test/page.html"),
      "GET",
      true,
      false,
      true,
      false,
      ui::PAGE_TRANSITION_LINK,
      &ignore_navigation);
  
  EXPECT_TRUE(result);
  EXPECT_FALSE(ignore_navigation);
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading003) {
  bool ignore_navigation = true;
  
  bool result = TestShouldOverrideUrlLoading(
      main_rfh()->GetFrameTreeNodeId(),
      true,
      GURL("https://example.com"),
      "GET",
      true,
      false,
      true,
      false,
      ui::PAGE_TRANSITION_LINK,
      &ignore_navigation);
  
  EXPECT_TRUE(result);
  EXPECT_FALSE(ignore_navigation);
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading004) {
  bool ignore_navigation = true;
  
  bool result = TestShouldOverrideUrlLoading(
      content::FrameTreeNodeId(),
      true,
      GURL("https://example.com"),
      "GET",
      true,
      true,
      true,
      false,
      ui::PAGE_TRANSITION_LINK,
      &ignore_navigation);
  
  EXPECT_TRUE(result);
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading005) {
  bool ignore_navigation = true;
  
  bool result = TestShouldOverrideUrlLoading(
      main_rfh()->GetFrameTreeNodeId(),
      false,
      GURL("https://example.com"),
      "GET",
      true,
      false,
      false,
      false,
      ui::PAGE_TRANSITION_LINK,
      &ignore_navigation);
  
  EXPECT_TRUE(result);
  EXPECT_FALSE(ignore_navigation);
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading006) {
  bool ignore_navigation = true;
  
  bool result = TestShouldOverrideUrlLoading(
      main_rfh()->GetFrameTreeNodeId(),
      false,
      GURL("about:blank"),
      "GET",
      true,
      false,
      false,
      false,
      ui::PAGE_TRANSITION_LINK,
      &ignore_navigation);
  
  EXPECT_TRUE(result);
  EXPECT_FALSE(ignore_navigation);
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading007) {
  bool ignore_navigation = true;
  
  bool result = TestShouldOverrideUrlLoading(
      main_rfh()->GetFrameTreeNodeId(),
      false,
      GURL("uuid-in-package:example"),
      "GET",
      true,
      false,
      false,
      false,
      ui::PAGE_TRANSITION_LINK,
      &ignore_navigation);
  
  EXPECT_TRUE(result);
  EXPECT_FALSE(ignore_navigation);
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading008) {
  std::vector<ui::PageTransition> transitions = {
    ui::PAGE_TRANSITION_LINK,
    ui::PAGE_TRANSITION_TYPED,
    ui::PAGE_TRANSITION_AUTO_BOOKMARK,
    ui::PAGE_TRANSITION_AUTO_SUBFRAME,
    ui::PAGE_TRANSITION_MANUAL_SUBFRAME,
    ui::PAGE_TRANSITION_GENERATED,
    ui::PAGE_TRANSITION_AUTO_TOPLEVEL,
    ui::PAGE_TRANSITION_FORM_SUBMIT,
    ui::PAGE_TRANSITION_RELOAD,
    ui::PAGE_TRANSITION_KEYWORD,
    ui::PAGE_TRANSITION_KEYWORD_GENERATED
  };
  
  for (auto transition : transitions) {
    bool ignore_navigation = true;
    
    bool result = TestShouldOverrideUrlLoading(
        content::FrameTreeNodeId(),
        false,
        GURL("https://example.com"),
        "GET",
        true,
        false,
        true,
        false,
        transition,
        &ignore_navigation);
    EXPECT_TRUE(result);
    
    if (transition & ui::PAGE_TRANSITION_FORWARD_BACK) {
      EXPECT_FALSE(ignore_navigation);
    }
  }
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading009) {
  std::vector<std::string> methods = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH"};
  
  for (const auto& method : methods) {
    bool ignore_navigation = true;
    
    bool result = TestShouldOverrideUrlLoading(
        content::FrameTreeNodeId(),
        false,
        GURL("https://example.com"),
        method,
        true,
        false,
        true,
        false,
        ui::PAGE_TRANSITION_LINK,
        &ignore_navigation);
    
    EXPECT_TRUE(result);
    
    if (method != "GET") {
      EXPECT_FALSE(ignore_navigation);
    }
  }
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading010) {
  bool ignore_navigation = true;
  
  bool result = TestShouldOverrideUrlLoading(
      content::FrameTreeNodeId(),
      false,
      GURL("https://example.com"),
      "GET",
      true,
      false,
      true,
      false,
      ui::PAGE_TRANSITION_LINK,
      &ignore_navigation);
  
  EXPECT_TRUE(result);
  EXPECT_FALSE(ignore_navigation);
}

TEST_F(ChromeContentBrowserClientForIncludeTest, ShouldOverrideUrlLoading011) {
  bool ignore_navigation = true;
  
  bool result = TestShouldOverrideUrlLoading(
      main_rfh()->GetFrameTreeNodeId(),
      false,
      GURL("https://example.com"),
      "GET",
      true,
      false,
      true,
      false,
      ui::PAGE_TRANSITION_FORWARD_BACK,
      &ignore_navigation);
  
  EXPECT_TRUE(result);
  EXPECT_FALSE(ignore_navigation);
}
