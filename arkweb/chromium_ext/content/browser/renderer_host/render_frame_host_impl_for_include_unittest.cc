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

#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/common/frame.mojom.h"
#include "content/common/frame_messages.mojom.h"
#include "content/common/frame_messages.mojom-forward.h"
#include "content/public/test/test_browser_context.h"
#include "content/public/common/content_client.h"
#include "content/public/browser/disallow_activation_reason.h"
#include "content/browser/loader/file_url_loader_factory.h"
#include "content/public/common/alternative_error_page_override_info.mojom.h"
#include "third_party/blink/public/common/navigation/navigation_params.h"

namespace content {

extern void CommitNavigationExt(const std::string& effective_scheme,
    ContentBrowserClient::NonNetworkURLLoaderFactoryMap& non_network_factories,
    BrowserContext* browser_context);

TEST_F(RenderFrameHostImplTest, SendAccessibilityEvent_BasicCall) {
  int64_t accessibilityId = 123;
  int32_t eventType = 1;
  std::string argument = "test argument";

  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(rfh->SendAccessibilityEvent(accessibilityId, eventType, argument));
}

TEST_F(RenderFrameHostImplTest, GetCreateNewWindow_NoDelegate) {
  GURL target_url = GURL("https://parent.example.test/");
  WindowOpenDisposition disposition = WindowOpenDisposition::NEW_POPUP;
  bool allow_popup = true;
  content::mojom::FrameHost::GetCreateNewWindowCallback callback = base::DoNothing();
  
  RenderFrameHostImpl* rfh = main_test_rfh();
  auto delegate = rfh->delegate_;
  rfh->delegate_ = nullptr;
  ASSERT_FALSE(rfh->delegate());
  rfh->GetCreateNewWindow(target_url, disposition, allow_popup, std::move(callback));
  rfh->delegate_ = delegate;
  ASSERT_TRUE(rfh->delegate());
}

TEST_F(RenderFrameHostImplTest, GetCreateNewWindow_AllowPopupTrue) {
  GURL target_url = GURL("https://parent.example.test/");
  WindowOpenDisposition disposition = WindowOpenDisposition::NEW_POPUP;
  bool allow_popup = true;
  content::mojom::FrameHost::GetCreateNewWindowCallback callback = base::DoNothing();
  
  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(rfh->GetCreateNewWindow(target_url, disposition, allow_popup, std::move(callback)));
}

TEST_F(RenderFrameHostImplTest, GetCreateNewWindow_AllowPopupFalse_HasTransientActivation) {
  GURL target_url = GURL("https://parent.example.test/");
  WindowOpenDisposition disposition = WindowOpenDisposition::NEW_POPUP;
  bool allow_popup = false;
  content::mojom::FrameHost::GetCreateNewWindowCallback callback = base::DoNothing();
  
  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(rfh->GetCreateNewWindow(target_url, disposition, allow_popup, std::move(callback)));
}

TEST_F(RenderFrameHostImplTest, GetCreateNewWindow_AllowPopupFalse_NoTransientActivation) {
  GURL target_url = GURL("https://parent.example.test/");
  WindowOpenDisposition disposition = WindowOpenDisposition::NEW_POPUP;
  bool allow_popup = false;
  content::mojom::FrameHost::GetCreateNewWindowCallback callback = base::DoNothing();

  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(rfh->GetCreateNewWindow(target_url, disposition, allow_popup, std::move(callback)));
}

TEST_F(RenderFrameHostImplTest, GenerateCodeCache_OptionsConversion) {
  const std::string test_url = "https://example.com/script.js";
  const std::string test_script = "console.log('Hello World');";

  std::map<std::string, std::string> response_headers = {
      {"Content-Type", "application/javascript"},
      {"Cache-Control", "max-age=3600"}
  };
  auto cache_options = std::make_shared<oh_code_cache::CacheOptions>(response_headers);

  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(rfh->GenerateCodeCache(test_url, test_script, cache_options, base::DoNothing()));
}

TEST_F(RenderFrameHostImplTest, GenerateCodeCache_NoResponseHeaders) {
  const std::string test_url = "https://example.com/script.js";
  const std::string test_script = "console.log('Hello World');";

  std::map<std::string, std::string> empty_headers;
  auto cache_options = std::make_shared<oh_code_cache::CacheOptions>(empty_headers);

  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(rfh->GenerateCodeCache(test_url, test_script, cache_options, base::DoNothing()));
}

TEST_F(RenderFrameHostImplTest, OnClearContextMenu_ReadyToBeDeleted) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  rfh->SetLifecycleState(RenderFrameHostImpl::LifecycleStateImpl::kReadyToBeDeleted);
  EXPECT_TRUE(rfh->IsInactiveAndDisallowActivation(DisallowActivationReasonId::kShowContextMenu));
  rfh->OnClearContextMenu();
}

TEST_F(RenderFrameHostImplTest, OnClearContextMenu_Active) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  rfh->SetLifecycleState(RenderFrameHostImpl::LifecycleStateImpl::kActive);
  EXPECT_FALSE(rfh->IsInactiveAndDisallowActivation(DisallowActivationReasonId::kShowContextMenu));
  rfh->OnClearContextMenu();
}

TEST_F(RenderFrameHostImplTest, UpdateAdBlockEnabledToRender_SiteAdblockEnableTrue) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  bool site_adblock_enabled = true;
  ASSERT_NO_FATAL_FAILURE(rfh->UpdateAdBlockEnabledToRender(site_adblock_enabled));
}

TEST_F(RenderFrameHostImplTest, UpdateAdBlockEnabledToRender_SiteAdblockEnableFalse) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  bool site_adblock_enabled = false;
  ASSERT_NO_FATAL_FAILURE(rfh->UpdateAdBlockEnabledToRender(site_adblock_enabled));
}

TEST_F(RenderFrameHostImplTest, AddNamedObject_WithFrame) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  base::Value::List list;
  ASSERT_NO_FATAL_FAILURE(rfh->AddNamedObject("test", 123, list, true));
}

TEST_F(RenderFrameHostImplTest, AddNamedObject_NoFrame) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  base::Value::List list;

  rfh->frame_ = mojo::AssociatedRemote<mojom::Frame>();
  ASSERT_NO_FATAL_FAILURE(rfh->AddNamedObject("test", 123, list, true));
}

TEST_F(RenderFrameHostImplTest, MouseSelectMenuShow_WithDelegate) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(rfh->MouseSelectMenuShow(true));
  ASSERT_NO_FATAL_FAILURE(rfh->MouseSelectMenuShow(false));
}

TEST_F(RenderFrameHostImplTest, MouseSelectMenuShow_NoDelegate) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  auto delegate = rfh->delegate_;
  rfh->delegate_ = nullptr;
  ASSERT_FALSE(rfh->delegate());
  rfh->MouseSelectMenuShow(true);
  rfh->delegate_ = delegate;
  ASSERT_TRUE(rfh->delegate());
}

TEST_F(RenderFrameHostImplTest, ChangeVisibilityOfQuickMenu_WithDelegate) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(rfh->ChangeVisibilityOfQuickMenu());
}

TEST_F(RenderFrameHostImplTest, ChangeVisibilityOfQuickMenu_NoDelegate) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  auto delegate = rfh->delegate_;
  rfh->delegate_ = nullptr;
  ASSERT_FALSE(rfh->delegate());
  rfh->ChangeVisibilityOfQuickMenu();
  rfh->delegate_ = delegate;
  ASSERT_TRUE(rfh->delegate());
}

TEST_F(RenderFrameHostImplTest, CloseImageOverlaySelection_WithDelegate) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(rfh->CloseImageOverlaySelection());
}

TEST_F(RenderFrameHostImplTest, CloseImageOverlaySelection_NoDelegate) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  auto delegate = rfh->delegate_;
  rfh->delegate_ = nullptr;
  ASSERT_FALSE(rfh->delegate());
  rfh->CloseImageOverlaySelection();
  rfh->delegate_ = delegate;
  ASSERT_TRUE(rfh->delegate());
}

TEST_F(RenderFrameHostImplTest, IsJsDialogShowOrBeforeUnloadTimedOut_NoDelegate) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  auto delegate = rfh->delegate_;
  rfh->delegate_ = nullptr;
  ASSERT_FALSE(rfh->delegate());
  EXPECT_FALSE(rfh->IsJsDialogShowOrBeforeUnloadTimedOut());
  rfh->delegate_ = delegate;
  ASSERT_TRUE(rfh->delegate());
}

TEST_F(RenderFrameHostImplTest, IsJsDialogShowOrBeforeUnloadTimedOut_WithDelegate) {
  RenderFrameHostImpl* rfh = main_test_rfh();
  rfh->IsJsDialogShowOrBeforeUnloadTimedOut();
  EXPECT_TRUE(rfh->page_close_state_ == RenderFrameHostImpl::PageCloseState::kNotClosing);
}

TEST_F(RenderFrameHostImplTest, CommitNavigationExt_WithResourcesScheme) {
  const std::string effective_scheme = url::kResourcesScheme;
  ContentBrowserClient::NonNetworkURLLoaderFactoryMap non_network_factories;
  
  EXPECT_TRUE(non_network_factories.empty());
  std::unique_ptr<TestBrowserContext> browser_context = std::make_unique<TestBrowserContext>();
  CommitNavigationExt(effective_scheme, non_network_factories, browser_context.get());
  
  EXPECT_EQ(non_network_factories.size(), 1u);
  EXPECT_TRUE(non_network_factories.find(url::kResourcesScheme) != non_network_factories.end());
  EXPECT_TRUE(non_network_factories[url::kResourcesScheme].is_valid());
}

TEST_F(RenderFrameHostImplTest, CommitNavigationExt_WithNonResourcesScheme) {
  const std::string effective_scheme = "http";
  ContentBrowserClient::NonNetworkURLLoaderFactoryMap non_network_factories;

  std::unique_ptr<TestBrowserContext> browser_context = std::make_unique<TestBrowserContext>();
  CommitNavigationExt(effective_scheme, non_network_factories, browser_context.get());

  EXPECT_TRUE(non_network_factories.empty());
}

TEST_F(RenderFrameHostImplTest, GetWorldId_EmptyName) {
  const std::string empty_world_name = "";
  int32_t world_id = 0;
    
  RenderFrameHostImpl* rfh = main_test_rfh();
  bool result = rfh->GetWorldId(empty_world_name, &world_id);
  
  EXPECT_FALSE(result);
}

TEST_F(RenderFrameHostImplTest, GetWorldId_NewName) {
  const std::string world_name = "test_world";
  int32_t world_id = 0;
  
  RenderFrameHostImpl* rfh = main_test_rfh();
  bool result = rfh->GetWorldId(world_name, &world_id);
  
  EXPECT_TRUE(result);
}

TEST_F(RenderFrameHostImplTest, GetWorldId_ExistingName) {
  const std::string world_name = "test_world";
  int32_t first_world_id = 0;
  int32_t second_world_id = 0;

  RenderFrameHostImpl* rfh = main_test_rfh();
  rfh->GetWorldId(world_name, &first_world_id);
  bool result = rfh->GetWorldId(world_name, &second_world_id);
  
  EXPECT_TRUE(result);
  EXPECT_EQ(first_world_id, second_world_id);
}

TEST_F(RenderFrameHostImplTest, GetWorldId_IncrementalNames) {
  const std::string world_name1 = "test_world_1";
  const std::string world_name2 = "test_world_2";
  const std::string world_name3 = "test_world_3";
  int32_t world_id1 = 0;
  int32_t world_id2 = 0;
  int32_t world_id3 = 0;

  RenderFrameHostImpl* rfh = main_test_rfh();
  rfh->GetWorldId(world_name1, &world_id1);
  
  rfh->GetWorldId(world_name2, &world_id2);
  
  rfh->GetWorldId(world_name3, &world_id3);
  
  EXPECT_EQ(world_id2, world_id1 + 1);
  EXPECT_EQ(world_id3, world_id2 + 1);
}

TEST_F(RenderFrameHostImplTest, ExecuteJavaScriptInFrames_WithWorldName) {
  const std::string world_name = "test_world";
  int32_t world_id = 0;

  RenderFrameHostImpl* rfh = main_test_rfh();
  bool result = rfh->GetWorldId(world_name, &world_id);
  EXPECT_TRUE(result);

  std::u16string javascript = u"alert('Hello World');";
  bool recursive = true;
  std::string worldName = "test_world";
  RenderFrameHost::JavaScriptResultCallback callback = base::DoNothing();

  rfh->ExecuteJavaScriptInFrames(javascript, recursive, worldName, std::move(callback));
}

TEST_F(RenderFrameHostImplTest, ExecuteJavaScriptInFrames_WithoutWorldName) {
  std::u16string javascript = u"alert('Hello World');";
  bool recursive = true;
  std::string empty_world_name = "";
  RenderFrameHost::JavaScriptResultCallback callback = base::DoNothing();

  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(
      rfh->ExecuteJavaScriptInFrames(javascript, recursive, empty_world_name, std::move(callback)));
}

TEST_F(RenderFrameHostImplTest, ExecuteJavaScriptInFrames_NonRecursive) {
  std::u16string javascript = u"alert('Hello World');";
  bool recursive = false;
  std::string empty_world_name = "";
  RenderFrameHost::JavaScriptResultCallback callback = base::DoNothing();

  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_NO_FATAL_FAILURE(
      rfh->ExecuteJavaScriptInFrames(javascript, recursive, empty_world_name, std::move(callback)));
}

TEST_F(RenderFrameHostImplTest, OnPdfScrollAtBottom_WithDelegate) {
  std::string url = "https://example.com/document.pdf";
  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_TRUE(rfh->delegate());
  rfh->OnPdfScrollAtBottom(url);
}

TEST_F(RenderFrameHostImplTest, OnPdfScrollAtBottom_NoDelegate) {
  std::string url = "https://example.com/document.pdf";
  RenderFrameHostImpl* rfh = main_test_rfh();
  auto delegate = rfh->delegate_;
  rfh->delegate_ = nullptr;
  ASSERT_FALSE(rfh->delegate());
  rfh->OnPdfScrollAtBottom(url);
  rfh->delegate_ = delegate;
  ASSERT_TRUE(rfh->delegate());
}

TEST_F(RenderFrameHostImplTest, OnPdfLoadEvent_WithDelegate) {
  std::string url = "https://example.com/document.pdf";
  int32_t result = 0;
  RenderFrameHostImpl* rfh = main_test_rfh();
  ASSERT_TRUE(rfh->delegate());
  rfh->OnPdfLoadEvent(result, url);
}

TEST_F(RenderFrameHostImplTest, OnPdfLoadEvent_NoDelegate) {
  std::string url = "https://example.com/document.pdf";
  int32_t result = 0;
  RenderFrameHostImpl* rfh = main_test_rfh();
  auto delegate = rfh->delegate_;
  rfh->delegate_ = nullptr;
  ASSERT_FALSE(rfh->delegate());
  rfh->OnPdfLoadEvent(result, url);
  rfh->delegate_ = delegate;
  ASSERT_TRUE(rfh->delegate());
}

}  // namespace content