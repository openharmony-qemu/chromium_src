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

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/common/features.h"
#define private public
#include "third_party/blink/renderer/core/frame/frame.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/loader/subresource_filter.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/loader/arkweb_subresource_filter_ext.h"
#undef private
#include "third_party/blink/renderer/core/css/resolver/style_cascade.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/sim/sim_request.h"
#include "third_party/blink/renderer/core/testing/sim/sim_test.h"
#include "third_party/blink/renderer/core/frame/local_frame_client_impl.h"

using ::testing::NiceMock;

namespace blink {
class MockWebDocumentSubresourceFilter : public WebDocumentSubresourceFilter {
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

class MockLocalFrameClient : public blink::LocalFrameClient {
 public:
  MOCK_METHOD(bool, InShadowTree, (), (const, override));
  MOCK_METHOD(void, Detached, (blink::FrameDetachType), (override));
  MOCK_METHOD(unsigned, BackForwardLength, (), (override));
  MOCK_METHOD(bool, HasWebView, (), (const, override));
  MOCK_METHOD(base::UnguessableToken, GetDevToolsFrameToken, (), (const, override));
  MOCK_METHOD(void, WillBeDetached, (), (override));
  MOCK_METHOD(void, DispatchFinalizeRequest, (blink::ResourceRequest&), (override));
  MOCK_METHOD(std::optional<blink::KURL>, DispatchWillSendRequest, (
      const blink::KURL&,
      const scoped_refptr<const blink::SecurityOrigin>&,
      const net::SiteForCookies&,
      bool,
      const blink::KURL&), (override));
  MOCK_METHOD(void, DispatchDidLoadResourceFromMemoryCache, (
      const blink::ResourceRequest&,
      const blink::ResourceResponse&), (override));
  MOCK_METHOD(void, DispatchDidHandleOnloadEvents, (), (override));
  MOCK_METHOD(void, DispatchDidReceiveTitle, (const WTF::String&), (override));
  MOCK_METHOD(void, DispatchDidCommitLoad, (
      blink::HistoryItem*,
      blink::WebHistoryCommitType,
      bool,
      const blink::ParsedPermissionsPolicy&,
      const blink::DocumentPolicyFeatureState&), (override));
  MOCK_METHOD(void, DispatchDidFailLoad, (
      const blink::ResourceError&,
      blink::WebHistoryCommitType), (override));
  MOCK_METHOD(void, DispatchDidDispatchDOMContentLoadedEvent, (), (override));
  MOCK_METHOD(void, DispatchDidFinishLoad, (), (override));
  MOCK_METHOD(void, DispatchWillSendSubmitEvent, (blink::HTMLFormElement*), (override));
  MOCK_METHOD(void, DidStartLoading, (), (override));
  MOCK_METHOD(void, DidStopLoading, (), (override));
  MOCK_METHOD(bool, NavigateBackForward, (
      int,
      std::optional<blink::scheduler::TaskAttributionId>), (const, override));
  MOCK_METHOD(void, DidDispatchPingLoader, (const blink::KURL&), (override));
  MOCK_METHOD(void, SelectorMatchChanged, (
      const WTF::Vector<WTF::String>&,
      const WTF::Vector<WTF::String>&), (override));
  MOCK_METHOD(void, DidCreateDocumentLoader, (blink::DocumentLoader*), (override));
  MOCK_METHOD(WTF::String, UserAgentOverride, (), (override));
  MOCK_METHOD(WTF::String, UserAgent, (), (override));
  MOCK_METHOD(std::optional<blink::UserAgentMetadata>, UserAgentMetadata, (), (override));
  MOCK_METHOD(WTF::String, DoNotTrackValue, (), (override));
  MOCK_METHOD(void, TransitionToCommittedForNewPage, (), (override));
  MOCK_METHOD(blink::LocalFrame*, CreateFrame, (
      const WTF::AtomicString&,
      blink::HTMLFrameOwnerElement*), (override));
  MOCK_METHOD(blink::RemoteFrame*, CreateFencedFrame, (
      blink::HTMLFencedFrameElement*,
      mojo::PendingAssociatedReceiver<blink::mojom::blink::FencedFrameOwnerHost>), (override));
  MOCK_METHOD(blink::WebPluginContainerImpl*, CreatePlugin, (
      blink::HTMLPlugInElement&,
      const blink::KURL&,
      const WTF::Vector<WTF::String>&,
      const WTF::Vector<WTF::String>&,
      const WTF::String&,
      bool), (override));
  MOCK_METHOD(std::unique_ptr<blink::WebMediaPlayer>, CreateWebMediaPlayer, (
      blink::HTMLMediaElement&,
      const blink::WebMediaPlayerSource&,
      blink::WebMediaPlayerClient*), (override));
  MOCK_METHOD(blink::RemotePlaybackClient*, CreateRemotePlaybackClient, (
      blink::HTMLMediaElement&), (override));
  MOCK_METHOD(void, DidCommitDocumentReplacementNavigation, (blink::DocumentLoader*), (override));
  MOCK_METHOD(void, DispatchDidClearWindowObjectInMainWorld, (
      v8::Isolate*,
      v8::MicrotaskQueue*), (override));
  MOCK_METHOD(void, DocumentElementAvailable, (), (override));
  MOCK_METHOD(void, RunScriptsAtDocumentElementAvailable, (), (override));
  MOCK_METHOD(void, RunScriptsAtDocumentReady, (bool), (override));
  MOCK_METHOD(void, RunScriptsAtDocumentIdle, (), (override));
  MOCK_METHOD(void, DidCreateScriptContext, (
      v8::Local<v8::Context>,
      int32_t), (override));
  MOCK_METHOD(void, WillReleaseScriptContext, (
      v8::Local<v8::Context>,
      int32_t), (override));
  MOCK_METHOD(bool, AllowScriptExtensions, (), (override));
  MOCK_METHOD(std::unique_ptr<blink::WebServiceWorkerProvider>, CreateServiceWorkerProvider, (), (override));
  MOCK_METHOD(blink::WebContentSettingsClient*, GetContentSettingsClient, (), (override));
  MOCK_METHOD(blink::AssociatedInterfaceProvider*, GetRemoteNavigationAssociatedInterfaces, (), (override));
  MOCK_METHOD(blink::WebSpellCheckPanelHostClient*, SpellCheckPanelHostClient, (), (const, override));
  MOCK_METHOD(blink::WebTextCheckClient*, GetTextCheckerClient, (), (const, override));
  MOCK_METHOD(scoped_refptr<network::SharedURLLoaderFactory>, GetURLLoaderFactory, (), (override));
  MOCK_METHOD(std::unique_ptr<blink::URLLoader>, CreateURLLoaderForTesting, (), (override));
  MOCK_METHOD(blink::ChildURLLoaderFactoryBundle*, GetLoaderFactoryBundle, (), (override));
  MOCK_METHOD(scoped_refptr<blink::WebBackgroundResourceFetchAssets>,
      MaybeGetBackgroundResourceFetchAssets, (), (override));
  MOCK_METHOD(WTF::String, evaluateInInspectorOverlayForTesting, (const WTF::String&), (override));
  MOCK_METHOD(blink::Frame*, FindFrame, (const WTF::AtomicString&), (const, override));
  void BeginNavigation(
      const ResourceRequest&,
      const KURL& requestor_base_url,
      mojom::RequestContextFrameType,
      LocalDOMWindow* origin_window,
      DocumentLoader*,
      WebNavigationType,
      NavigationPolicy,
      WebFrameLoadType,
      mojom::blink::ForceHistoryPush,
      bool is_client_redirect,
      bool is_unfenced_top_navigation,
      mojom::blink::TriggeringEventInfo,
      HTMLFormElement*,
      network::mojom::CSPDisposition should_check_main_world_content_security_policy,
      mojo::PendingRemote<mojom::blink::BlobURLToken>,
      base::TimeTicks input_start_time,
      const String& href_translate,
      const std::optional<Impression>& impression,
      const LocalFrameToken* initiator_frame_token,
      std::unique_ptr<SourceLocation> source_location,
      mojo::PendingRemote<mojom::blink::NavigationStateKeepAliveHandle> initiator_navigation_state_keep_alive_handle,
      bool is_container_initiated,
      bool has_rel_opener) {};

#if BUILDFLAG(ARKWEB_ADBLOCK)
  MOCK_METHOD(void, DispatchDidSubresourceFiltered, (), (override));
  MOCK_METHOD(bool, GetGlobalAdblockEnabled, (), (override));
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  MOCK_METHOD(void, RunScriptsAtHeadElementAvailable, (), (override));
#endif

  ~MockLocalFrameClient() override = default;
  void Trace(blink::Visitor* visitor) const override {
    blink::LocalFrameClient::Trace(visitor);
  }
};

class ArkWebSubresourceFilterExtTest : public SimTest {
 public:
  void SetUp() {
    SimTest::SetUp();

    auto web_filter = std::make_unique<MockWebDocumentSubresourceFilter>();
    subresource_filter_ = MakeGarbageCollected<SubresourceFilter>(
        MainFrame().GetFrame()->DomWindow(), std::move(web_filter));
    EXPECT_NE(subresource_filter_, nullptr);
    subresource_filter_ext_ = std::make_unique<ArkWebSubresourceFilterExt>(subresource_filter_);
  }

  SubresourceFilter* subresource_filter_;
  std::unique_ptr<ArkWebSubresourceFilterExt> subresource_filter_ext_;
};

TEST_F(ArkWebSubresourceFilterExtTest, RequestSendStatistics) {
  subresource_filter_ext_->RequestSendStatistics(base::Milliseconds(1000));
  EXPECT_TRUE(subresource_filter_->statistics_timer_.IsActive());

  subresource_filter_ext_->RequestSendStatistics(base::Milliseconds(2000));
  EXPECT_TRUE(subresource_filter_->statistics_timer_.IsActive());
}

TEST_F(ArkWebSubresourceFilterExtTest, SendStatistics) {
  auto* window = DynamicTo<LocalDOMWindow>(subresource_filter_->execution_context_.Get());
  EXPECT_TRUE(window);
  LocalFrame* local_frame = window->GetFrame();
  auto* mock_client = MakeGarbageCollected<NiceMock<MockLocalFrameClient>>();
  EXPECT_CALL(*mock_client, DispatchDidSubresourceFiltered()).Times(1);
  local_frame->client_ = mock_client;
  subresource_filter_->SendStatistics(nullptr);

  subresource_filter_->execution_context_ = nullptr;
  EXPECT_CALL(*mock_client, DispatchDidSubresourceFiltered()).Times(0);
  subresource_filter_->SendStatistics(nullptr);
}

TEST_F(ArkWebSubresourceFilterExtTest, GetAdBlockEnabledByPage) {
  bool ret = subresource_filter_ext_->GetAdBlockEnabledByPage();
  EXPECT_FALSE(ret);

  auto* window = DynamicTo<LocalDOMWindow>(subresource_filter_->execution_context_.Get());
  EXPECT_TRUE(window);
  LocalFrame* local_frame = window->GetFrame();
  EXPECT_TRUE(local_frame);
  LocalFrame& local_frame_root = local_frame->LocalFrameRoot();
  auto* mock_client = MakeGarbageCollected<NiceMock<MockLocalFrameClient>>();
  EXPECT_CALL(*mock_client, GetGlobalAdblockEnabled()).WillRepeatedly(::testing::Return(true));
  local_frame_root.client_ = mock_client;
  ret = subresource_filter_ext_->GetAdBlockEnabledByPage();
  EXPECT_FALSE(ret);

  local_frame_root.adblock_enabled_for_site_ = true;
  ret = subresource_filter_ext_->GetAdBlockEnabledByPage();
  EXPECT_TRUE(ret);

  subresource_filter_->execution_context_ = nullptr;
  ret = subresource_filter_ext_->GetAdBlockEnabledByPage();
  EXPECT_FALSE(ret);
}

}  // namespace blink
