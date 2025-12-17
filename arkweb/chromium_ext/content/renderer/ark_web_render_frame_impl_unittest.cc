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

#include "content/renderer/render_frame_impl.h"
#include "arkweb/chromium_ext/content/renderer/ark_web_render_frame_impl.h"

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "base/command_line.h"
#include "content/common/frame.mojom.h"
#include "content/common/frame_messages.mojom.h"
#include "content/public/common/content_client.h"
#include "content/public/common/url_constants.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "mojo/public/cpp/bindings/pending_associated_remote.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/base/data_url.h"
#include "services/network/public/cpp/not_implemented_url_loader_factory.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/common/permissions_policy/permissions_policy.h"
#include "third_party/blink/public/mojom/frame/frame.mojom.h"
#include "third_party/blink/public/mojom/frame/frame_replication_state.mojom.h"
#include "third_party/blink/public/mojom/navigation/navigation_params.mojom.h"
#include "third_party/blink/public/mojom/service_worker/controller_service_worker.mojom.h"
#include "third_party/blink/public/mojom/service_worker/service_worker_container.mojom.h"
#include "third_party/blink/public/web/web_document_loader.h"
#include "third_party/blink/public/web/web_navigation_control.h"
#include "third_party/blink/public/web/web_window_features.h"
#include "third_party/blink/public/platform/web_document_subresource_filter.h"
#include "third_party/blink/public/platform/modules/service_worker/web_service_worker_network_provider.h"
#define private public
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#undef private

namespace content {
using ::testing::_;
using testing::Return;
using testing::SetArgPointee;
using testing::DoAll;
using ::testing::Matcher;
class MockFrameHostForArkWeb : public mojom::FrameHost {
 public:
  MOCK_METHOD(void, CreateNewWindow, (mojom::CreateNewWindowParamsPtr params,
      CreateNewWindowCallback callback));
  MOCK_METHOD(bool,
              GetCreateNewWindow,
              (const ::GURL& target_url,
               ::WindowOpenDisposition disposition,
               bool allow_popup,
               ::blink::mojom::WindowFeaturesPtr window_features,
               mojom::CreateNewWindowStatus* out_status));
  using GetCreateNewWindowCallback = base::OnceCallback<void(mojom::CreateNewWindowStatus)>;
  MOCK_METHOD(void,
              GetCreateNewWindow,
              (const ::GURL& target_url,
               ::WindowOpenDisposition disposition,
               bool allow_popup,
               ::blink::mojom::WindowFeaturesPtr window_features,
               GetCreateNewWindowCallback callback));
  MOCK_METHOD(void, CreateChildFrame, (const blink::LocalFrameToken& child_frame_token,
      mojo::PendingAssociatedRemote<mojom::Frame> frame,
      mojo::PendingReceiver<blink::mojom::BrowserInterfaceBroker> browser_interface_broker,
      blink::mojom::PolicyContainerBindParamsPtr policy_container_bind_params,
      mojo::PendingAssociatedReceiver<blink::mojom::AssociatedInterfaceProvider> associated_interface_provider,
      blink::mojom::TreeScopeType scope, const std::string& frame_name, const std::string& frame_unique_name,
      bool is_created_by_script, const blink::FramePolicy& frame_policy,
      blink::mojom::FrameOwnerPropertiesPtr frame_owner_properties,
      blink::FrameOwnerElementType child_frame_owner_element_type, int64_t document_ukm_source_id));
  MOCK_METHOD(void, DidCommitProvisionalLoad, (mojom::DidCommitProvisionalLoadParamsPtr params,
      mojom::DidCommitProvisionalLoadInterfaceParamsPtr interface_params));
  MOCK_METHOD(void, DidCommitSameDocumentNavigation, (mojom::DidCommitProvisionalLoadParamsPtr params,
      mojom::DidCommitSameDocumentNavigationParamsPtr same_document_params));
  MOCK_METHOD(void, DidOpenDocumentInputStream, (const ::GURL& url));
  MOCK_METHOD(void, BeginNavigation, (blink::mojom::CommonNavigationParamsPtr common_params,
      blink::mojom::BeginNavigationParamsPtr begin_params,
      mojo::PendingRemote<blink::mojom::BlobURLToken> blob_url_token,
      mojo::PendingAssociatedRemote<mojom::NavigationClient> navigation_client,
      mojo::PendingRemote<blink::mojom::NavigationStateKeepAliveHandle> initiator_navigation_state_keep_alive_handle,
      mojo::PendingReceiver<mojom::NavigationRendererCancellationListener> renderer_cancellation_listener));
  MOCK_METHOD(void, SubresourceResponseStarted, (const url::SchemeHostPort& final_response_url, uint32_t cert_status));
  MOCK_METHOD(void, ResourceLoadComplete, (blink::mojom::ResourceLoadInfoPtr url_load_info));
  MOCK_METHOD(void, DidChangeName, (const std::string& name, const std::string& unique_name));
  MOCK_METHOD(void, CancelInitialHistoryLoad, ());
  MOCK_METHOD(void, UpdateEncoding, (const std::string& encoding_name));
  MOCK_METHOD(void, UpdateState, (const blink::PageState& state));
  MOCK_METHOD(void, OpenURL, (blink::mojom::OpenURLParamsPtr params));
  MOCK_METHOD(void, DidStopLoading, ());
  MOCK_METHOD(void, MouseSelectMenuShow, (bool show));
  MOCK_METHOD(void, ChangeVisibilityOfQuickMenu, ());
  MOCK_METHOD(void, HideQuickMenu, ());
  MOCK_METHOD(void, OnClearContextMenu, ());
  MOCK_METHOD(void, CloseImageOverlaySelection, ());
  MOCK_METHOD(void, OnPdfScrollAtBottom, (const std::string& url));
  MOCK_METHOD(void, OnPdfLoadEvent, (int32_t result, const std::string& url));
#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  MOCK_METHOD(bool, IsVideoLoadOptimizationEnabled, (const std::string& url));
  MOCK_METHOD(int, GetVideoPreloadTimeDefault, (), (const));
  MOCK_METHOD(int, GetVideoMinCacheTimeDefault, (), (const));
  MOCK_METHOD(int, GetVideoMaxCacheTimeDefault, (), (const));
  MOCK_METHOD(int, GetVideoMoovSizeDefault, (), (const));
  MOCK_METHOD(int, GetVideoBitrateDefault, (), (const));
  MOCK_METHOD(bool, SetNewsFeedPageFitted, ());
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION
#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  MOCK_METHOD(void, OnDocumentEndReady, ());
#endif
};

class MockWebDocumentSubresourceFilter : public blink::WebDocumentSubresourceFilter {
 public:
  MockWebDocumentSubresourceFilter() = default;
  ~MockWebDocumentSubresourceFilter() override = default;
  MOCK_METHOD(blink::WebDocumentSubresourceFilter::LoadPolicy, GetLoadPolicy,
              (const blink::WebURL& resource_url, network::mojom::RequestDestination), (override));
  MOCK_METHOD(blink::WebDocumentSubresourceFilter::LoadPolicy, GetLoadPolicyForWebSocketConnect,
              (const blink::WebURL&), (override));
  MOCK_METHOD(blink::WebDocumentSubresourceFilter::LoadPolicy, GetLoadPolicyForWebTransportConnect,
              (const blink::WebURL&), (override));
  MOCK_METHOD(void, ReportDisallowedLoad, (), (override));
  MOCK_METHOD(bool, ShouldLogToConsole, (), (override));
  MOCK_METHOD(void, ClearStatistics, (), (override));
  MOCK_METHOD(std::unique_ptr<std::string>, GetElementHidingSelectors,
              (const blink::WebURL& document_url, bool need_common_selectors), (override));
  MOCK_METHOD(bool, HasGenericHideTypeOption,
              (const blink::WebURL& document_url, const url::Origin& parent_document_origin), (override));
  MOCK_METHOD(bool, HasElemHideTypeOption,
              (const blink::WebURL& document_url, const url::Origin& parent_document_origin), (override));
  MOCK_METHOD(bool, HasDocumentTypeOption,
              (const blink::WebURL& document_url, const url::Origin& parent_document_origin), (override));
  MOCK_METHOD(void, DidMatchCssRule,
              (const blink::WebURL& document_url, const std::string& dom_path, bool is_for_report), (override));
  MOCK_METHOD(void, SetDidFinishLoad, (bool did_load_finished), (override));
  MOCK_METHOD(bool, GetDidFinishLoad, (), (override));
  MOCK_METHOD(std::unique_ptr<std::vector<std::string>>, GetUserDomPathSelectors,
              (const blink::WebURL& document_url, bool need_generic_selectors), (override));
  MOCK_METHOD(void, set_activation_state, (bool enabled), (override));
};

class MockWebDocumentLoader : public blink::WebDocumentLoader {
 public:
  MockWebDocumentLoader() = default;
  ~MockWebDocumentLoader() = default;
  MOCK_METHOD(blink::WebString, OriginalReferrer, (), (const));
  MOCK_METHOD(blink::WebURL, GetUrl, (), (const));
  MOCK_METHOD(blink::WebString, HttpMethod, (), (const));
  MOCK_METHOD(blink::WebString, Referrer, (), (const));
  MOCK_METHOD(const blink::WebURLResponse&, GetWebResponse, (), (const));
  MOCK_METHOD(bool, HasUnreachableURL, (), (const));
  MOCK_METHOD(blink::WebURL, UnreachableWebURL, (), (const));
  MOCK_METHOD(bool, IsClientRedirect, (), (const));
  MOCK_METHOD(bool, ReplacesCurrentHistoryItem, (), (const));
  MOCK_METHOD(blink::WebNavigationType, GetNavigationType, (), (const));
  MOCK_METHOD(ExtraData*, GetExtraData, (), (const));
  MOCK_METHOD(std::unique_ptr<ExtraData>, CloneExtraData, ());
  MOCK_METHOD(void, SetExtraData, (std::unique_ptr<ExtraData>));
  MOCK_METHOD(blink::WebDocumentSubresourceFilter*, GetWebSubresourceFilter, ());
  MOCK_METHOD(blink::WebDocumentSubresourceFilter*, GetWebUserSubresourceFilter, ());
  MOCK_METHOD(void, SetSubresourceFilter, (blink::WebDocumentSubresourceFilter* filter), ());
  MOCK_METHOD(void, SetWebUserSubresourceFilter, (blink::WebDocumentSubresourceFilter* filter), ());
  MOCK_METHOD(blink::WebServiceWorkerNetworkProvider*, GetServiceWorkerNetworkProvider, ());
  MOCK_METHOD(void, SetServiceWorkerNetworkProvider, (std::unique_ptr<blink::WebServiceWorkerNetworkProvider>), ());
  MOCK_METHOD(void, BlockParser, (), ());
  MOCK_METHOD(void, ResumeParser, (), ());
  MOCK_METHOD(bool, HasBeenLoadedAsWebArchive, (), (const));
  MOCK_METHOD(blink::WebArchiveInfo, GetArchiveInfo, (), (const));
  MOCK_METHOD(bool, LastNavigationHadTransientUserActivation, (), (const));
  using CodeCacheHostRemote = blink::CrossVariantMojoRemote<blink::mojom::CodeCacheHostInterfaceBase>;
  MOCK_METHOD(void, SetCodeCacheHost, (CodeCacheHostRemote code_cache_host,
      CodeCacheHostRemote code_cache_host_for_background), ());
  MOCK_METHOD(blink::WebString, OriginCalculationDebugInfo, (), (const));
  MOCK_METHOD(bool, HasLoadedNonInitialEmptyDocument, (), (const));
};

class ArkWebRenderFrameImplTest : public RenderFrameImplTest {
 public:
  void TestDidSubresourceFiltered() {
    GetMainRenderFrame()->DidSubresourceFiltered();
  }
  void TestOnUpdateAdBlockEnabledToRender(bool site_adblock_enabled) {
    GetMainRenderFrame()->OnUpdateAdBlockEnabledToRender(site_adblock_enabled);
  }
  std::vector<RenderFrameObserver*> TestGetObservers() {
    std::vector<RenderFrameObserver*> observers;
    for (auto& observer : GetMainRenderFrame()->observers_) {
        observers.push_back(&observer);
    }
    return observers;
  }
  void TestObserversClear() {
    GetMainRenderFrame()->observers_.Clear();
  }
  void TestAddObserver(RenderFrameObserver* observer) {
    GetMainRenderFrame()->observers_.AddObserver(observer);
  }
  uint32_t TestGetNWebId() {
    return GetMainRenderFrame()->nweb_id_;
  }
  blink::WebDocumentLoader* TestGetDocumentLoader() {
    return GetMainRenderFrame()->frame_->GetDocumentLoader();
  }
  void TestSetDocumentLoader(blink::WebDocumentLoader* document_loader) {
    GetMainRenderFrame()->frame_->SetDocumentLoaderForTest(document_loader);
  }
  bool TestGetNewWindowWebView(const GURL& target_url,
                               blink::WebNavigationPolicy policy,
                               bool allow_popup,
                               const blink::WebWindowFeatures& features) {
    return GetMainRenderFrame()->GetNewWindowWebView(target_url, policy,
                                                     allow_popup, features);
  }
  void SetFrameHostForTest(mojom::FrameHost* frame_host) {
    GetMainRenderFrame()->SetFrameHostForTest(frame_host);
  }
  mojom::FrameHost* TestGetFrameHost() {
    return GetMainRenderFrame()->GetFrameHost();
  }
};

TEST_F(ArkWebRenderFrameImplTest, RunScriptsAtHeadReady_WithContentClient) {
  auto* client_renderer = SetRendererClientForTesting(nullptr);
  GetMainRenderFrame()->RunScriptsAtHeadReady();
  SetRendererClientForTesting(client_renderer);
}

TEST_F(ArkWebRenderFrameImplTest, DidSubresourceFiltered) {
  RenderFrameTestObserver observer1(GetMainRenderFrame());
  auto observers = TestGetObservers();
  TestObserversClear();
  EXPECT_CALL(observer1, DidSubresourceFiltered()).Times(0);
  TestDidSubresourceFiltered();
  for (auto& observer : observers) {
    TestAddObserver(observer);
  }
  EXPECT_CALL(observer1, DidSubresourceFiltered()).Times(1);
  TestDidSubresourceFiltered();
}

TEST_F(ArkWebRenderFrameImplTest, OnUpdateAdBlockEnabledToRender_NoUserFilter) {
  EXPECT_EQ(GetMainRenderFrame()->GetWebView()->GetAdBlockEnableForSite(), false);
  TestOnUpdateAdBlockEnabledToRender(false);
  EXPECT_EQ(GetMainRenderFrame()->GetWebView()->GetAdBlockEnableForSite(), true);
  TestOnUpdateAdBlockEnabledToRender(false);
  EXPECT_EQ(GetMainRenderFrame()->GetWebView()->GetAdBlockEnableForSite(), true);
}

TEST_F(ArkWebRenderFrameImplTest, OnUpdateAdBlockEnabledToRender_NoDocumentLoader) {
  blink::WebDocumentLoader* document_loader = TestGetDocumentLoader();
  MockWebDocumentLoader document_loader_mock;
  TestSetDocumentLoader(nullptr);
  EXPECT_CALL(document_loader_mock, GetWebSubresourceFilter()).Times(0);
  TestOnUpdateAdBlockEnabledToRender(true);
  TestSetDocumentLoader(document_loader);
}

TEST_F(ArkWebRenderFrameImplTest, OnUpdateAdBlockEnabledToRender_Filter) {
  blink::WebDocumentLoader* document_loader = TestGetDocumentLoader();
  MockWebDocumentLoader document_loader_mock;
  MockWebDocumentSubresourceFilter mock_filter;
  TestSetDocumentLoader(&document_loader_mock);
  EXPECT_CALL(document_loader_mock, GetWebSubresourceFilter()).WillOnce(testing::Return(&mock_filter));
  EXPECT_CALL(mock_filter, set_activation_state(true)).Times(1);
  TestOnUpdateAdBlockEnabledToRender(true);
  TestSetDocumentLoader(document_loader);
}

TEST_F(ArkWebRenderFrameImplTest, OnUpdateAdBlockEnabledToRender_UserFilter) {
  blink::WebDocumentLoader* document_loader = TestGetDocumentLoader();
  MockWebDocumentLoader document_loader_mock;
  MockWebDocumentSubresourceFilter mock_filter;
  TestSetDocumentLoader(&document_loader_mock);
  EXPECT_CALL(document_loader_mock, GetWebUserSubresourceFilter()).WillOnce(testing::Return(&mock_filter));
  EXPECT_CALL(mock_filter, set_activation_state(true)).Times(1);
  TestOnUpdateAdBlockEnabledToRender(true);
  TestSetDocumentLoader(document_loader);
}

TEST_F(ArkWebRenderFrameImplTest, OnUpdateAdBlockEnabledToRender_WithoutView) {
  auto* web_view = GetMainRenderFrame()->GetWebView();
  GetMainRenderFrame()->SetWebViewForTest(nullptr);
  blink::WebDocumentLoader* document_loader = TestGetDocumentLoader();
  MockWebDocumentLoader document_loader_mock;
  TestSetDocumentLoader(&document_loader_mock);
  EXPECT_CALL(document_loader_mock, GetWebSubresourceFilter()).WillOnce(testing::Return(nullptr));
  TestOnUpdateAdBlockEnabledToRender(true);
  GetMainRenderFrame()->SetWebViewForTest(web_view);
  TestSetDocumentLoader(document_loader);
}

TEST_F(ArkWebRenderFrameImplTest, AddNamedObject) {
  auto observers = TestGetObservers();
  TestObserversClear();
  GetMainRenderFrame()->AddNamedObject("test_name", 1, {}, true);
  for (auto& observer : observers) {
    TestAddObserver(observer);
  }
  GetMainRenderFrame()->AddNamedObject("test_name", 1, {}, true);
}

TEST_F(ArkWebRenderFrameImplTest, NotifyLcpForBlankless_BlanklessKey) {
  uint64_t blankless_key = base::ohos::BlanklessController::INVALID_BLANKLESS_KEY;
  GetMainRenderFrame()->SendBlanklessKeyToRenderFrame(1, blankless_key, 3, 4);
  GetMainRenderFrame()->NotifyLcpForBlankless();
  EXPECT_EQ(TestGetNWebId(), 1);
}

TEST_F(ArkWebRenderFrameImplTest, NotifyLcpForBlankless_NoBlanklessKey) {
  GetMainRenderFrame()->SendBlanklessKeyToRenderFrame(1, 1, 3, 4);
  GetMainRenderFrame()->NotifyLcpForBlankless();
  EXPECT_EQ(TestGetNWebId(), 1);
}

TEST_F(ArkWebRenderFrameImplTest, PageLoadStartLoggerReport_False) {
  blink::WebDocumentLoader* document_loader = TestGetDocumentLoader();
  EXPECT_NE(document_loader, nullptr);
  GetMainRenderFrame()->GetWebView()->SetStrictLogMode(false);
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  EXPECT_FALSE(base::CommandLine::Init(0, nullptr));
  command_line->AppendSwitch("enable-nweb-logger-report");
  GetMainRenderFrame()->PageLoadStartLoggerReport(document_loader);
  GetMainRenderFrame()->PageLoadFinishedLoggerReport();
}

TEST_F(ArkWebRenderFrameImplTest, PageLoadStartLoggerReport_True) {
  blink::WebDocumentLoader* document_loader = TestGetDocumentLoader();
  EXPECT_NE(document_loader, nullptr);
  GetMainRenderFrame()->GetWebView()->SetStrictLogMode(true);
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  EXPECT_FALSE(base::CommandLine::Init(0, nullptr));
  command_line->AppendSwitch("enable-nweb-logger-report");
  GetMainRenderFrame()->PageLoadStartLoggerReport(document_loader);
  GetMainRenderFrame()->PageLoadFinishedLoggerReport();
}

TEST_F(ArkWebRenderFrameImplTest, PageLoadStartLoggerReport_WithoutView) {
  auto* web_view = GetMainRenderFrame()->GetWebView();
  blink::WebDocumentLoader* document_loader = TestGetDocumentLoader();
  EXPECT_NE(document_loader, nullptr);
  GetMainRenderFrame()->SetWebViewForTest(nullptr);
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  EXPECT_FALSE(base::CommandLine::Init(0, nullptr));
  command_line->AppendSwitch("enable-nweb-logger-report");
  GetMainRenderFrame()->PageLoadStartLoggerReport(document_loader);
  GetMainRenderFrame()->PageLoadFinishedLoggerReport();
  GetMainRenderFrame()->SetWebViewForTest(web_view);
}

TEST_F(ArkWebRenderFrameImplTest, PageLoadFinishedLoggerReport_WithoutView) {
  auto* web_view = GetMainRenderFrame()->GetWebView();
  GetMainRenderFrame()->SetWebViewForTest(nullptr);
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  EXPECT_FALSE(base::CommandLine::Init(0, nullptr));
  command_line->AppendSwitch("enable-nweb-logger-report");
  GetMainRenderFrame()->PageLoadFinishedLoggerReport();
  GetMainRenderFrame()->SetWebViewForTest(web_view);
}

TEST_F(ArkWebRenderFrameImplTest, ContentLoadFailedLoggerReport_False) {
  GetMainRenderFrame()->GetWebView()->SetStrictLogMode(false);
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  EXPECT_FALSE(base::CommandLine::Init(0, nullptr));
  command_line->AppendSwitch("enable-nweb-logger-report");
  GetMainRenderFrame()->ContentLoadFailedLoggerReport();
}

TEST_F(ArkWebRenderFrameImplTest, ContentLoadFailedLoggerReport_True) {
  GetMainRenderFrame()->GetWebView()->SetStrictLogMode(true);
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  EXPECT_FALSE(base::CommandLine::Init(0, nullptr));
  command_line->AppendSwitch("enable-nweb-logger-report");
  GetMainRenderFrame()->ContentLoadFailedLoggerReport();
}

TEST_F(ArkWebRenderFrameImplTest, ContentLoadFailedLoggerReport_WithoutView) {
  auto* web_view = GetMainRenderFrame()->GetWebView();
  GetMainRenderFrame()->SetWebViewForTest(nullptr);
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  EXPECT_FALSE(base::CommandLine::Init(0, nullptr));
  command_line->AppendSwitch("enable-nweb-logger-report");
  GetMainRenderFrame()->ContentLoadFailedLoggerReport();
  GetMainRenderFrame()->SetWebViewForTest(web_view);
}

TEST_F(ArkWebRenderFrameImplTest, ReportRenderInitBlock) {
  RenderFrameImplUtils utils(GetMainRenderFrame());
  utils.ChangeCompleteInitialize(true);
  EXPECT_EQ(utils.IsCompleteInitialize(), true);
  utils.ReportRenderInitBlock();
  EXPECT_EQ(utils.IsCompleteInitialize(), false);
}

TEST_F(ArkWebRenderFrameImplTest, CloseImageOverlaySelection) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->CloseImageOverlaySelection();
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->CloseImageOverlaySelection();
}

TEST_F(ArkWebRenderFrameImplTest, SetZoomLevel) {
  auto* web_frame_widget = GetMainRenderFrame()->GetLocalRootWebFrameWidget();
  GetMainRenderFrame()->SetLocalRootWebFrameWidgetForTest(nullptr);
  GetMainRenderFrame()->SetZoomLevel(1.5, gfx::Point(10, 20));
  GetMainRenderFrame()->SetLocalRootWebFrameWidgetForTest(web_frame_widget);
  GetMainRenderFrame()->SetZoomLevel(1.5, gfx::Point(10, 20));
}

TEST_F(ArkWebRenderFrameImplTest, SetOverscrollMode) {
  auto* web_frame_widget = GetMainRenderFrame()->GetLocalRootWebFrameWidget();
  GetMainRenderFrame()->SetLocalRootWebFrameWidgetForTest(nullptr);
  GetMainRenderFrame()->SetOverscrollMode(0);
  GetMainRenderFrame()->SetLocalRootWebFrameWidgetForTest(web_frame_widget);
  GetMainRenderFrame()->SetOverscrollMode(1);
}

TEST_F(ArkWebRenderFrameImplTest, GetOverScrollOffset) {
  auto* web_frame_widget = GetMainRenderFrame()->GetLocalRootWebFrameWidget();
  GetMainRenderFrame()->SetLocalRootWebFrameWidgetForTest(nullptr);
  auto result1 = GetMainRenderFrame()->GetOverScrollOffset();
  GetMainRenderFrame()->SetLocalRootWebFrameWidgetForTest(web_frame_widget);
  auto result2 = GetMainRenderFrame()->GetOverScrollOffset();
  EXPECT_EQ(result1.x(), 0);
  EXPECT_EQ(result1.y(), 0);
  EXPECT_EQ(result2.x(), 0);
  EXPECT_EQ(result2.y(), 0);
}

TEST_F(ArkWebRenderFrameImplTest, MouseSelectMenuShow) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->MouseSelectMenuShow(false);
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->MouseSelectMenuShow(true);
}

TEST_F(ArkWebRenderFrameImplTest, ChangeVisibilityOfQuickMenu) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->ChangeVisibilityOfQuickMenu();
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->ChangeVisibilityOfQuickMenu();
}

TEST_F(ArkWebRenderFrameImplTest, OnPdfScrollAtBottom) {
  std::string url = "https://www.google.com";
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->OnPdfScrollAtBottom(url);
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->OnPdfScrollAtBottom(url);
}

TEST_F(ArkWebRenderFrameImplTest, OnPdfLoadEvent) {
  std::string url = "https://www.google.com";
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->OnPdfLoadEvent(0, url);
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->OnPdfLoadEvent(1, url);
}

TEST_F(ArkWebRenderFrameImplTest, GetNewWindowWebView_WithoutFrameHost) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GURL target_url("https://www.google.com");
  bool allow_popup = true;
  blink::WebWindowFeatures features;
  auto result = TestGetNewWindowWebView(
      target_url, blink::WebNavigationPolicy::kWebNavigationPolicyNewWindow,
      allow_popup, features);
  EXPECT_EQ(result, false);
  SetFrameHostForTest(frame_host);
}

TEST_F(ArkWebRenderFrameImplTest, GetNewWindowWebView_ReturnFalse) {
  MockFrameHostForArkWeb test_frame_host;
  SetFrameHostForTest(&test_frame_host);
  GURL target_url("https://www.google.com");
  bool allow_popup = true;
  blink::WebWindowFeatures features;
  EXPECT_CALL(
      test_frame_host,
      GetCreateNewWindow(Matcher<const GURL&>(_),
                         Matcher<::WindowOpenDisposition>(_), Matcher<bool>(_),
                         Matcher<::blink::mojom::WindowFeaturesPtr>(_),
                         Matcher<mojom::CreateNewWindowStatus*>(_)))
      .WillOnce(Return(false));
  auto result = TestGetNewWindowWebView(
      target_url, blink::WebNavigationPolicy::kWebNavigationPolicyNewWindow,
      allow_popup, features);
  EXPECT_EQ(result, false);
}

TEST_F(ArkWebRenderFrameImplTest, GetNewWindowWebView_NoSuccess) {
  MockFrameHostForArkWeb test_frame_host;
  SetFrameHostForTest(&test_frame_host);
  GURL target_url("https://www.google.com");
  bool allow_popup = true;
  blink::WebWindowFeatures features;
  EXPECT_CALL(
      test_frame_host,
      GetCreateNewWindow(Matcher<const GURL&>(_),
                         Matcher<::WindowOpenDisposition>(_), Matcher<bool>(_),
                         Matcher<::blink::mojom::WindowFeaturesPtr>(_),
                         Matcher<mojom::CreateNewWindowStatus*>(_)))
      .WillOnce(Return(true));
  auto result = TestGetNewWindowWebView(
      target_url, blink::WebNavigationPolicy::kWebNavigationPolicyNewWindow,
      allow_popup, features);
  EXPECT_EQ(result, false);
}

TEST_F(ArkWebRenderFrameImplTest, GetNewWindowWebView_Success) {
  MockFrameHostForArkWeb test_frame_host;
  SetFrameHostForTest(&test_frame_host);
  GURL target_url("https://www.google.com");
  bool allow_popup = true;
  blink::WebWindowFeatures features;
  EXPECT_CALL(
      test_frame_host,
      GetCreateNewWindow(Matcher<const GURL&>(_),
                         Matcher<::WindowOpenDisposition>(_), Matcher<bool>(_),
                         Matcher<::blink::mojom::WindowFeaturesPtr>(_),
                         Matcher<mojom::CreateNewWindowStatus*>(_)))
      .WillOnce(DoAll(SetArgPointee<4>(mojom::CreateNewWindowStatus::kSuccess),
                      Return(true)));
  auto result = TestGetNewWindowWebView(
      target_url, blink::WebNavigationPolicy::kWebNavigationPolicyNewWindow,
      allow_popup, features);
  EXPECT_EQ(result, true);
}

TEST_F(ArkWebRenderFrameImplTest, GetNewWindowWebView_True) {
  MockFrameHostForArkWeb test_frame_host;
  SetFrameHostForTest(&test_frame_host);
  GURL target_url("https://www.google.com");
  bool allow_popup = true;
  blink::WebWindowFeatures features;
  EXPECT_CALL(
      test_frame_host,
      GetCreateNewWindow(Matcher<const GURL&>(_),
                         Matcher<::WindowOpenDisposition>(_), Matcher<bool>(_),
                         Matcher<::blink::mojom::WindowFeaturesPtr>(_),
                         Matcher<mojom::CreateNewWindowStatus*>(_)))
      .WillOnce(DoAll(SetArgPointee<4>(mojom::CreateNewWindowStatus::kBlocked),
                      Return(true)));
  auto result = TestGetNewWindowWebView(
      target_url, blink::WebNavigationPolicy::kWebNavigationPolicyNewWindow,
      allow_popup, features);
  EXPECT_EQ(result, false);
}

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
TEST_F(ArkWebRenderFrameImplTest, VideoLoadOpt_IsVideoLoadOptimizationEnabledTest) {
  std::string url = "https://www.baidu.com/";
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->IsVideoLoadOptimizationEnabled(url);
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->IsVideoLoadOptimizationEnabled(url);
}

TEST_F(ArkWebRenderFrameImplTest, VideoLoadOpt_GetVideoPreloadTimeDefaultTest) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->GetVideoPreloadTimeDefault();
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->GetVideoPreloadTimeDefault();
}

TEST_F(ArkWebRenderFrameImplTest, VideoLoadOpt_GetVideoMinCacheTimeDefaultTest) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->GetVideoMinCacheTimeDefault();
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->GetVideoMinCacheTimeDefault();
}

TEST_F(ArkWebRenderFrameImplTest, VideoLoadOpt_GetVideoMaxCacheTimeDefaultTest) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->GetVideoMaxCacheTimeDefault();
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->GetVideoMaxCacheTimeDefault();
}

TEST_F(ArkWebRenderFrameImplTest, VideoLoadOpt_GetVideoMoovSizeDefaultTest) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->GetVideoMoovSizeDefault();
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->GetVideoMoovSizeDefault();
}

TEST_F(ArkWebRenderFrameImplTest, VideoLoadOpt_GetVideoBitrateDefaultTest) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->GetVideoBitrateDefault();
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->GetVideoBitrateDefault();
}

TEST_F(ArkWebRenderFrameImplTest, VideoLoadOpt_SetNewsFeedPageFittedTest) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->SetNewsFeedPageFitted();
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->SetNewsFeedPageFitted();
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
TEST_F(ArkWebRenderFrameImplTest, OnDocumentEndReady) {
  auto* frame_host = TestGetFrameHost();
  SetFrameHostForTest(nullptr);
  GetMainRenderFrame()->OnDocumentEndReady();
  SetFrameHostForTest(frame_host);
  GetMainRenderFrame()->OnDocumentEndReady();
}
#endif

}  // namespace content
