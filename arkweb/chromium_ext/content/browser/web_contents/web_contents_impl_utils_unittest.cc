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

#include "content/browser/web_contents/web_contents_impl_utils.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/test/test_render_view_host.h"
#include "content/test/test_web_contents.h"
#include "content/public/test/test_browser_context.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "ui/base/ui_base_switches.h"
#define private public
#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_utils.h"
#undef private
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "content/public/test/mock_render_process_host.h"

namespace base {
namespace ohos {

static bool compute_language_by_region_for_test = false;

class LanguageMock {
 public:
  static LanguageMock& getInstance() { 
    static LanguageMock instance;
    return instance; 
  } 
  MOCK_METHOD(std::string, ComputeLanguageByRegionMock, (), ());
 private:
  LanguageMock() = default;
};

#ifdef __cplusplus
extern "C" {
#endif

std::string __real_ComputeLanguageByRegion();

std::string __wrap_ComputeLanguageByRegion() {
  if (compute_language_by_region_for_test) {
    return LanguageMock::getInstance().ComputeLanguageByRegionMock();
  } else {
    return __real_ComputeLanguageByRegion();
  }
}

#ifdef __cplusplus
}
#endif

}
}

namespace content {
class WebContentsImplUtilsTest : public RenderViewHostImplTestHarness {
 public:
  void SetUp() override {
    RenderViewHostImplTestHarness::SetUp();
    base::ohos::compute_language_by_region_for_test = true;
    utils_ = std::make_unique<WebContentsImplUtils>(contents());
  }

  void TearDown() override {
    utils_.reset();
    RenderViewHostImplTestHarness::TearDown();
  }

  std::unique_ptr<WebContentsImplUtils> utils_;
};

TEST_F(WebContentsImplUtilsTest, UpdateRenderAcceptLanguageIfNeed_NoLangSwitch) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->RemoveSwitch(::switches::kLang);

  auto& mock = base::ohos::LanguageMock::getInstance();
  EXPECT_CALL(mock, ComputeLanguageByRegionMock()).Times(0);

  utils_->UpdateRenderAcceptLanguageIfNeed("en-US");
}

TEST_F(WebContentsImplUtilsTest, UpdateRenderAcceptLanguageIfNeed_WithLangSwitch) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitchASCII(::switches::kLang, "en-US");

  utils_->webContentsImpl->renderer_preferences_.accept_languages = "zh-CN";
  auto& mock = base::ohos::LanguageMock::getInstance();
  EXPECT_CALL(mock, ComputeLanguageByRegionMock()).WillOnce(testing::Return(""));

  utils_->UpdateRenderAcceptLanguageIfNeed("zh-CN");
  EXPECT_TRUE(utils_->webContentsImpl->renderer_preferences_.accept_languages == "zh-CN");
}

TEST_F(WebContentsImplUtilsTest, UpdateRenderAcceptLanguageIfNeed_WithLangSwitch_NoRegion) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitchASCII(::switches::kLang, "en");

  utils_->webContentsImpl->renderer_preferences_.accept_languages = "zh-CN";
  auto& mock = base::ohos::LanguageMock::getInstance();
  EXPECT_CALL(mock, ComputeLanguageByRegionMock()).WillOnce(testing::Return(""));

  utils_->UpdateRenderAcceptLanguageIfNeed("zh-CN");
  EXPECT_TRUE(utils_->webContentsImpl->renderer_preferences_.accept_languages == "zh-CN");
}

TEST_F(WebContentsImplUtilsTest, UpdateRenderAcceptLanguageIfNeed_HasRegion_SameLanguage) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitchASCII(::switches::kLang, "en-US");

  utils_->webContentsImpl->renderer_preferences_.accept_languages = "zh-CN";
  auto& mock = base::ohos::LanguageMock::getInstance();
  EXPECT_CALL(mock, ComputeLanguageByRegionMock()).WillOnce(testing::Return("en-US"));

  utils_->UpdateRenderAcceptLanguageIfNeed("en-US");
  EXPECT_TRUE(utils_->webContentsImpl->renderer_preferences_.accept_languages == "zh-CN");
}

TEST_F(WebContentsImplUtilsTest, UpdateRenderAcceptLanguageIfNeed_HasRegion_CurrentLanguage) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitchASCII(::switches::kLang, "en-US");

  utils_->webContentsImpl->renderer_preferences_.accept_languages = "zh-CN";
  auto& mock = base::ohos::LanguageMock::getInstance();
  EXPECT_CALL(mock, ComputeLanguageByRegionMock()).WillOnce(testing::Return("en-US"));

  utils_->UpdateRenderAcceptLanguageIfNeed("zh-CN");
  EXPECT_TRUE(utils_->webContentsImpl->renderer_preferences_.accept_languages == "en-US");
}

TEST_F(WebContentsImplUtilsTest, RenderProcessShareInit_EmptyToken) {
  auto browser_context = contents()->GetBrowserContext();
  WebContents::CreateParams params(browser_context);
  scoped_refptr<SiteInstanceImpl> site_instance = SiteInstanceImpl::Create(browser_context);

  params.shared_render_process_token = "";
  ASSERT_NO_FATAL_FAILURE(utils_->renderProcessShareInit(params, site_instance));
}

TEST_F(WebContentsImplUtilsTest, RenderProcessShareInit_WithToken) {
  auto browser_context = contents()->GetBrowserContext();
  WebContents::CreateParams params(browser_context);
  scoped_refptr<SiteInstanceImpl> site_instance = SiteInstanceImpl::Create(browser_context);

  params.shared_render_process_token = "test-token";
  utils_->renderProcessShareInit(params, site_instance);
  EXPECT_EQ(utils_->webContentsImpl->AsWebContentsImplExt()->shared_render_process_token_, "test-token");
}

TEST_F(WebContentsImplUtilsTest, RenderProcessShareInit_WithToken_ExistingProcess) {
  auto browser_context = contents()->GetBrowserContext();
  WebContents::CreateParams params(browser_context);
  scoped_refptr<SiteInstanceImpl> site_instance = SiteInstanceImpl::Create(browser_context);

  StoragePartition* partition = browser_context->GetDefaultStoragePartition();
  params.shared_render_process_token = "test-token";
  auto renderProcessHost = 
      std::make_unique<MockRenderProcessHost>(browser_context);
  ArkwebRenderProcessHostImplUtils::RegisteProcessForSharedToken("test-token", renderProcessHost.get());
  EXPECT_EQ(ArkwebRenderProcessHostImplUtils::GetProcessForSharedToken("test-token"), renderProcessHost.get());
  utils_->renderProcessShareInit(params, site_instance);
  EXPECT_EQ(utils_->webContentsImpl->AsWebContentsImplExt()->shared_render_process_token_, "test-token");
}

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
TEST_F(WebContentsImplUtilsTest, UpdateMainFrameLoadingControlsState_NoSwitch) {
  FrameTreeNode* frame_tree_node = main_test_rfh()->frame_tree_node();
  EXPECT_TRUE(frame_tree_node);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->RemoveSwitch(switches::kEnableNwebExTopControls);
  ASSERT_NO_FATAL_FAILURE(utils_->UpdateMainFrameLoadingControlsState(frame_tree_node, true));
}

TEST_F(WebContentsImplUtilsTest, UpdateMainFrameLoadingControlsState_WithSwitch) {
  FrameTreeNode* frame_tree_node = main_test_rfh()->frame_tree_node();
  EXPECT_TRUE(frame_tree_node);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitch(switches::kEnableNwebExTopControls);
  ASSERT_NO_FATAL_FAILURE(utils_->UpdateMainFrameLoadingControlsState(frame_tree_node, false));
}

TEST_F(WebContentsImplUtilsTest, UpdateMainFrameLoadingControlsState_MainFrame_WithSwitch_ShowLoadingUI) {
  FrameTreeNode* frame_tree_node = main_test_rfh()->frame_tree_node();
  EXPECT_TRUE(frame_tree_node);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitch(switches::kEnableNwebExTopControls);
  ASSERT_NO_FATAL_FAILURE(utils_->UpdateMainFrameLoadingControlsState(frame_tree_node, true));
}
#endif

TEST_F(WebContentsImplUtilsTest, UpdateUserAgentOverride) {
  blink::UserAgentOverride ua_override;
  ua_override.ua_string_override = "Fake-UA";

  utils_->webContentsImpl->delayed_load_url_params_ = nullptr;
  utils_->UpdateUserAgentOverride(ua_override);
  EXPECT_EQ(utils_->webContentsImpl->AsWebContentsImplExt()->user_agent_, ua_override.ua_string_override);
}

TEST_F(WebContentsImplUtilsTest, UpdateUserAgentOverride_WithDelayedLoadUrlParams) {
  blink::UserAgentOverride ua_override;
  ua_override.ua_string_override = "Fake-UA";

  utils_->webContentsImpl->delayed_load_url_params_ =
      std::make_unique<NavigationController::LoadURLParams>(GURL("https://example.com/navigation.html"));
  utils_->UpdateUserAgentOverride(ua_override);

  EXPECT_EQ(utils_->webContentsImpl->AsWebContentsImplExt()->user_agent_, ua_override.ua_string_override);
  EXPECT_EQ(utils_->webContentsImpl->delayed_load_url_params_->override_user_agent,
      NavigationController::UA_OVERRIDE_TRUE);
}

TEST_F(WebContentsImplUtilsTest, JudgeIsPdfPageVisibilityChanged_VisibilityHidden) {
  ASSERT_NO_FATAL_FAILURE(utils_->JudgeIsPdfPageVisibilityChanged(Visibility::HIDDEN));
}

TEST_F(WebContentsImplUtilsTest, JudgeIsPdfPageVisibilityChanged_NotFirstVisible) {
  utils_->webContentsImpl->did_first_set_visible_ = true;
  ASSERT_NO_FATAL_FAILURE(utils_->JudgeIsPdfPageVisibilityChanged(Visibility::VISIBLE));
}

TEST_F(WebContentsImplUtilsTest, JudgeIsPdfPageVisibilityChanged_FirstVisiblePdf) {
  utils_->webContentsImpl->did_first_set_visible_ = false;
  ASSERT_NO_FATAL_FAILURE(utils_->JudgeIsPdfPageVisibilityChanged(Visibility::VISIBLE));
}

}  // namespace content