/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef USER_SUBRESOURCE_FILTER_AGENT_TEST_H_
#define USER_SUBRESOURCE_FILTER_AGENT_TEST_H_

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <string>
#include <utility>

#define private public
#define protected public
#include "components/subresource_filter/content/renderer/subresource_filter_agent.h"
#include "arkweb/chromium_ext/components/subresource_filter/core/common/document_subresource_filter_ext.h"
#include "components/subresource_filter/content/renderer/arkweb_subresource_filter_agent_ext.h"
#include "components/subresource_filter/content/renderer/user_subresource_filter_agent.h"
#include "components/subresource_filter/content/renderer/web_document_subresource_filter_impl.h"
#include "content/public/renderer/render_frame.h"
#include "content/renderer/render_frame_impl.h"
#include "third_party/blink/public/web/web_local_frame.h"
#undef protected
#undef private
#include "url/gurl.h"
#include "third_party/blink/public/common/frame/frame_ad_evidence.h"
#include "third_party/blink/public/common/page_state/page_state.h"
#include "third_party/blink/public/platform/web_document_subresource_filter.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/platform/web_worker_fetch_context.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_document_loader.h"
#include "third_party/blink/public/web/web_performance_metrics_for_nested_contexts.h"
#include "third_party/blink/public/web/web_performance_metrics_for_reporting.h"
#include "third_party/blink/public/web/web_print_page_description.h"
#include "third_party/blink/public/web/web_range.h"

namespace subresource_filter {
class Message;
class MockWebLocalFrame;

class MockWebSecurityOrigin : public blink::WebSecurityOrigin {
 public:
  ~MockWebSecurityOrigin() {}
  MockWebSecurityOrigin() = default;
  MOCK_METHOD(bool, IsSameOriginWith, (const WebSecurityOrigin&), (const));
};

class MockWebDocumentLoader final : public blink::WebDocumentLoader {
 public:
  ~MockWebDocumentLoader() = default;
  MOCK_METHOD(void,
              SetWebUserSubresourceFilter,
              (blink::WebDocumentSubresourceFilter*),
              (override));
  MOCK_METHOD(blink::WebDocumentSubresourceFilter*,
              GetWebUserSubresourceFilter,
              (),
              (override));
  MOCK_METHOD(const blink::WebURLResponse&,
              GetWebResponse,
              (),
              (const override));

  // Returns the http referrer of original request which initited this load.
  blink::WebString OriginalReferrer() const override {
    return blink::WebString();
  }
  blink::WebURL GetUrl() const override { return blink::WebURL(); }
  blink::WebString HttpMethod() const override { return blink::WebString(); }
  blink::WebString Referrer() const override { return blink::WebString(); }
  bool HasUnreachableURL() const override { return false; }
  blink::WebURL UnreachableWebURL() const override { return blink::WebURL(); }
  bool IsClientRedirect() const override { return false; }
  bool ReplacesCurrentHistoryItem() const override { return false; }
  blink::WebNavigationType GetNavigationType() const override {
    return blink::WebNavigationType::kWebNavigationTypeLinkClicked;
  }
  ExtraData* GetExtraData() const override { return nullptr; }
  std::unique_ptr<ExtraData> CloneExtraData() override { return nullptr; }
  void SetExtraData(std::unique_ptr<ExtraData>) override {}
  void SetSubresourceFilter(blink::WebDocumentSubresourceFilter*) override {}
  void SetServiceWorkerNetworkProvider(
      std::unique_ptr<blink::WebServiceWorkerNetworkProvider>) override {}
  blink::WebServiceWorkerNetworkProvider* GetServiceWorkerNetworkProvider()
      override {
    return nullptr;
  }
  void BlockParser() override {}
  void ResumeParser() override {}
  bool HasBeenLoadedAsWebArchive() const override { return false; }
  blink::WebArchiveInfo GetArchiveInfo() const override {
    return blink::WebArchiveInfo();
  }
  bool LastNavigationHadTransientUserActivation() const override {
    return false;
  }
  void SetCodeCacheHost(
      blink::CrossVariantMojoRemote<blink::mojom::CodeCacheHostInterfaceBase>
          code_cache_host,
      blink::CrossVariantMojoRemote<blink::mojom::CodeCacheHostInterfaceBase>
          code_cache_host_for_background) override {}
  blink::WebString OriginCalculationDebugInfo() const override {
    return blink::WebString();
  }
  bool HasLoadedNonInitialEmptyDocument() const override { return false; }
  blink::WebDocumentSubresourceFilter* GetWebSubresourceFilter() override {
    return nullptr;
  }
};

struct TestWebLocalFrameCondition {
  bool is_frame_created_by_ad_script = false;
  bool is_ad_frame = false;
  bool is_local_frame = false;
  bool is_ad_script_in_stack = true;
  bool is_provisional = false;
  bool is_in_fenced_frame_tree = false; 
  bool has_elemhide_type_option = false;
  bool has_generichide_type_option = false;
  bool has_document_type_option = false;
};

struct TestRenderFrameCondition {
  bool is_main_frame = false;
  bool is_in_fenced_frame_tree = false;  
  bool has_parent_frame = true;
  bool has_local_frame = true;
  TestWebLocalFrameCondition local_frame_conditions[2];
};

class MockRenderFrame : public content::RenderFrame {
 public:
  MockRenderFrame(TestRenderFrameCondition condition) : condition_(condition) {
  }
  ~MockRenderFrame() {}

  void SetTestLocalFrame(MockWebLocalFrame* local_frame) {
    web_local_frame_ = local_frame;
  }

  RenderFrame* GetMainRenderFrame() { return this; }
  bool IsMainFrame() { return condition_.is_main_frame; }
  bool IsHidden() { return false; }
  bool IsPasting() { return false; }
  bool IsInFencedFrameTree() const override { return condition_.is_in_fenced_frame_tree; }

  blink::WebLocalFrame* GetWebFrame() override { 
      if (!condition_.has_local_frame) {
      return nullptr;
    }
    return reinterpret_cast<blink::WebLocalFrame*>(web_local_frame_); }
  const blink::WebLocalFrame* GetWebFrame() const override { 
    if (!condition_.has_local_frame) {
      return nullptr;
    }
    return reinterpret_cast<blink::WebLocalFrame*>(web_local_frame_); }
  content::RenderAccessibility* GetRenderAccessibility() override { return nullptr; }
  std::unique_ptr<content::AXTreeSnapshotter> CreateAXTreeSnapshotter(
      ui::AXMode) override {
    return nullptr;
  }
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  int GetRoutingID() override {
    return 0;
  }
#endif
  blink::WebView* GetWebView() override {
    return nullptr;
  }
  const blink::WebView* GetWebView() const override {
    return nullptr;
  }
  const blink::web_pref::WebPreferences& GetBlinkPreferences() override {}
  void ShowVirtualKeyboard() override {}
  blink::WebPlugin* CreatePlugin(
      const content::WebPluginInfo& info,
      const blink::WebPluginParams& params) override {
    return nullptr;
  }
  void ExecuteJavaScript(const std::u16string& javascript) override {}
  void BindLocalInterface(
      const std::string& interface_name,
      mojo::ScopedMessagePipeHandle interface_pipe) override {}
  const blink::BrowserInterfaceBrokerProxy& GetBrowserInterfaceBroker()
      override {}
  blink::AssociatedInterfaceRegistry* GetAssociatedInterfaceRegistry()
      override {
    return nullptr;
  }
  blink::AssociatedInterfaceProvider* GetRemoteAssociatedInterfaces() override {
    return nullptr;
  }
  void SetSelectedText(const std::u16string& selection_text,
                       size_t offset,
                       const gfx::Range& range) override {}
  void AddMessageToConsole(blink::mojom::ConsoleMessageLevel level,
                           const std::string& message) override {}
  void LoadHTMLStringForTesting(std::string_view html,
                                const GURL& base_url,
                                const std::string& text_encoding,
                                const GURL& unreachable_url,
                                bool replace_current_item) override {}
  bool IsRequestingNavigation() override {}
  scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner(
      blink::TaskType task_type) override {
    return nullptr;
  }
  content::BindingsPolicySet GetEnabledBindings() override {}
  void SetAccessibilityModeForTest(ui::AXMode new_mode) override {}
  const content::RenderFrameMediaPlaybackOptions& GetRenderFrameMediaPlaybackOptions()
      override {}
  void SetRenderFrameMediaPlaybackOptions(
      const content::RenderFrameMediaPlaybackOptions& opts) override {}
  void SetAllowsCrossBrowsingInstanceFrameLookup() override {}
  gfx::RectF ElementBoundsInWindow(const blink::WebElement& element) override {}
  [[nodiscard]] gfx::Rect ConvertViewportToWindow(
      const gfx::Rect& rect) override {}
  float GetDeviceScaleFactor() override {}
  blink::scheduler::WebAgentGroupScheduler& GetAgentGroupScheduler() override {}

#if BUILDFLAG(ARKWEB_PDF)
  void OnPdfScrollAtBottom(const std::string& url) override {}
  void OnPdfLoadEvent(int32_t result, const std::string& url) override {}
#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_ADBLOCK)
  bool GetGlobalAdblockEnabled() override {}
#endif
   bool OnMessageReceived(const IPC::Message& message) override {}
   bool Send(IPC::Message* msg) override {}
   gfx::Vector2dF GetOverScrollOffset() override {}

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  void OnDocumentEndReady() override {}
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  bool IsReaderModeEnabled() override { return false; }
  void ReportDistillableResult(const std::string& event_type,
                               const std::string& value) override {};
#endif

 private:
  TestRenderFrameCondition condition_;
  MockWebLocalFrame* web_local_frame_;
};

class MockWebLocalFrame : public blink::WebLocalFrame {
 protected:
 public:
  MockWebLocalFrame(std::string_view url, TestWebLocalFrameCondition condition,
    blink::mojom::TreeScopeType scope,
                    const blink::LocalFrameToken& frame_token)
      : blink::WebLocalFrame(scope, frame_token),  url_(url), condition_(condition) {}
  ~MockWebLocalFrame() override = default;

  const blink::WebDocument InitDoc() const {
    blink::WebDocument mock_document;
    blink::WebURL test_url(url_);
    mock_document.InitiatePreview(test_url);
    return mock_document;
  }
  void SetTestParentFrame(MockWebLocalFrame* parent_frame) {
    parent_frame_ = parent_frame;
  }

  blink::WebDocument GetDocument() const override {
    return InitDoc();
  }

  blink::WebDocumentLoader* GetDocumentLoader() const override {
    return mock_loader_;
  }
  bool IsProvisional() const override { return condition_.is_provisional; }
  bool IsWebLocalFrame() const override {
    return condition_.is_local_frame;
  }
  bool IsAdFrame() const override {
    return condition_.is_ad_frame;
  }
  bool IsFrameCreatedByAdScript() override {
    return condition_.is_frame_created_by_ad_script;
  }
  blink::WebLocalFrame* ToWebLocalFrame() override {
    if (condition_.is_local_frame) {
      return this;
    }
    return nullptr;
  }
  blink::WebSecurityOrigin GetSecurityOrigin() const { return security_origin_; }
  MockWebSecurityOrigin* GetMockSecurityOrigin() { return &security_origin_; }
  void SetAdEvidence(const blink::FrameAdEvidence&) override{}
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void DidSubresourceFiltered() override {}
  void SetHasElemHideTypeOption(bool has_elemhide_type_option) override {
    condition_.has_elemhide_type_option = has_elemhide_type_option;
  }
  bool GetHasElemHideTypeOption() const override {
    return condition_.has_elemhide_type_option;
  }
  void SetHasDocumentTypeOption(bool has_document_type_option) override {
    condition_.has_document_type_option = has_document_type_option;
  }
  bool GetHasDocumentTypeOption() const override {
    return condition_.has_document_type_option;
  }
  void SetHasGenericHideTypeOption(bool has_generichide_type_option) override {
    condition_.has_generichide_type_option = has_generichide_type_option;
  }
  bool GetHasGenericHideTypeOption() const override {
    return condition_.has_generichide_type_option;
  }
  bool GetAdBlockEnabled() override {}
#endif
  const blink::WebLocalFrame* ToWebLocalFrame() const override {
    if (condition_.is_local_frame) {
      return this;
    }
    return nullptr;
  }
  bool IsWebRemoteFrame() const override {
    return false;
  }
  blink::WebRemoteFrame* ToWebRemoteFrame() override {
    return nullptr;
  }
  const blink::WebRemoteFrame* ToWebRemoteFrame() const override {
    return nullptr;
  }
  blink::WebView* View() const override {
    return nullptr;
  }
  bool HasSelection() const override {
    return false;
  }
  blink::WebRange SelectionRange() const override {
    return blink::WebRange();
  }
  blink::WebRange MarkedRange() const override {
    return blink::WebRange();
  }
  blink::WebPrintPageDescription GetPageDescription(
      uint32_t page_index) override {
    return blink::WebPrintPageDescription();
  }
  blink::WebLocalFrame* CreateLocalChild(
      blink::mojom::TreeScopeType,
      blink::WebLocalFrameClient*,
      blink::InterfaceRegistry*,
      const blink::LocalFrameToken& frame_token) override {}
  blink::WebLocalFrameClient* Client() const override {
    return nullptr;
  }
  void SetAutofillClient(blink::WebAutofillClient*) override {}
  blink::WebAutofillClient* AutofillClient() override {
    return nullptr;
  }
  void SetContentCaptureClient(blink::WebContentCaptureClient*) override {}
  blink::WebContentCaptureClient* ContentCaptureClient() const override {
    return nullptr;
  }
  blink::BrowserInterfaceBrokerProxy& GetBrowserInterfaceBroker() override {}
  blink::WebString AssignedName() const override {
    return blink::WebString();
  }
  void SetName(const blink::WebString&) override {}
  ui::AXTreeID GetAXTreeID() const override {}
  void SetNotRestoredReasons(
      const blink::mojom::BackForwardCacheNotRestoredReasonsPtr&) override {}
  void SetLCPPHint(
      const blink::mojom::LCPCriticalPathPredictorNavigationTimeHintPtr&)
      override {}
  bool IsFeatureEnabled(
      const blink::mojom::PermissionsPolicyFeature&) const override {
    return false;
  }
  blink::WebLocalFrame* LocalRoot() override {
    return nullptr;
  }
  blink::WebFrameWidget* FrameWidget() const override {
    return nullptr;
  }
  blink::WebFrameWidget* InitializeFrameWidget(
      blink::CrossVariantMojoAssociatedRemote<
          blink::mojom::FrameWidgetHostInterfaceBase>,
      blink::CrossVariantMojoAssociatedReceiver<
          blink::mojom::FrameWidgetInterfaceBase>,
      blink::CrossVariantMojoAssociatedRemote<
          blink::mojom::WidgetHostInterfaceBase>,
      blink::CrossVariantMojoAssociatedReceiver<
          blink::mojom::WidgetInterfaceBase> widget,
      const viz::FrameSinkId& frame_sink_id,
      bool is_for_nested_main_frame = false,
      bool is_for_scalable_page = true,
      bool hidden = false) {
    return nullptr;
  }
  blink::WebFrame* FindFrameByName(const blink::WebString& name) override {
    return nullptr;
  }
  void SetEmbeddingToken(
      const base::UnguessableToken& embedding_token) override {}
  const std::optional<base::UnguessableToken>& GetEmbeddingToken()
      const override {}
  bool IsInFencedFrameTree() const override { return condition_.is_in_fenced_frame_tree; }
  void SendPings(const blink::WebURL& destination_url) override {}
  void StartReload(blink::WebFrameLoadType) override {}
  void EnableViewSourceMode(bool) override {}
  bool IsViewSourceModeEnabled() const override {}
  void SetReferrerForRequest(blink::WebURLRequest&,
                             const blink::WebURL&) override {}
  void DownloadURL(
      const blink::WebURLRequest& request,
      network::mojom::RedirectMode,
      blink::CrossVariantMojoRemote<blink::mojom::BlobURLTokenInterfaceBase>)
      override {}
  blink::WebFrame* GetProvisionalOwnerFrame() override {}
  bool IsNavigationScheduledWithin(base::TimeDelta interval) const override {}
  void BlinkFeatureUsageReport(blink::mojom::WebFeature feature) override {}
  void ExecuteScript(const blink::WebScriptSource&) override {}
  void ExecuteScriptInIsolatedWorld(int32_t world_id,
                                    const blink::WebScriptSource&,
                                    blink::BackForwardCacheAware) override {}
  [[nodiscard]] v8::Local<v8::Value> ExecuteScriptInIsolatedWorldAndReturnValue(
      int32_t world_id,
      const blink::WebScriptSource&,
      blink::BackForwardCacheAware) override {}
  void ClearIsolatedWorldCSPForTesting(int32_t world_id) override {}
  v8::Local<v8::Value> ExecuteScriptAndReturnValue(
      const blink::WebScriptSource&) override {}
  v8::MaybeLocal<v8::Value> ExecuteMethodAndReturnValue(
      v8::Local<v8::Function>,
      v8::Local<v8::Value>,
      int argc,
      v8::Local<v8::Value> argv[]) override {}
  v8::MaybeLocal<v8::Value> CallFunctionEvenIfScriptDisabled(
      v8::Local<v8::Function>,
      v8::Local<v8::Value>,
      int argc,
      v8::Local<v8::Value> argv[]) override {}
  v8::Local<v8::Context> MainWorldScriptContext() const override {}
  int32_t GetScriptContextWorldId(
      v8::Local<v8::Context> script_context) const override {}
  v8::Local<v8::Context> GetScriptContextFromWorldId(
      v8::Isolate* isolate,
      int world_id) const override {}
  v8::Local<v8::Object> GlobalProxy(v8::Isolate* isolate) const override {}
  void RequestExecuteV8Function(v8::Local<v8::Context>,
                                v8::Local<v8::Function>,
                                v8::Local<v8::Value> receiver,
                                int argc,
                                v8::Local<v8::Value> argv[],
                                blink::WebScriptExecutionCallback) override {}
  void RequestExecuteScript(int32_t world_id,
                            base::span<const blink::WebScriptSource> sources,
                            blink::mojom::UserActivationOption,
                            blink::mojom::EvaluationTiming,
                            blink::mojom::LoadEventBlockingOption,
                            blink::WebScriptExecutionCallback,
                            blink::BackForwardCacheAware,
                            blink::mojom::WantResultOption,
                            blink::mojom::PromiseResultOption) override {}
  bool IsInspectorConnected() override {}
  void Alert(const blink::WebString& message) override {}
  bool Confirm(const blink::WebString& message) override {}
  blink::WebString Prompt(const blink::WebString& message,
                          const blink::WebString& default_value) override {}
  void GenerateInterventionReport(const blink::WebString& message_id,
                                  const blink::WebString& message) override {}
  void UnmarkText() override {}
  bool HasMarkedText() const override {
    return false;
  }
  bool FirstRectForCharacterRange(uint32_t location,
                                  uint32_t length,
                                  gfx::Rect&) const override {
    return false;
  }
  bool ExecuteCommand(const blink::WebString&) override {
    return false;
  }
  bool ExecuteCommand(const blink::WebString&,
                      const blink::WebString& value) override {
    return false;
  }
  bool IsCommandEnabled(const blink::WebString&) const override {
    return false;
  }
  bool SelectionTextDirection(base::i18n::TextDirection& start,
                              base::i18n::TextDirection& end) const override {
    return false;
  }
  bool IsSelectionAnchorFirst() const override {
    return false;
  }
  void SetTextDirectionForTesting(
      base::i18n::TextDirection direction) override {}
  void CenterSelection() override {}
  blink::WebString SelectionAsText() const override {
    return blink::WebString();
  }
  blink::WebString SelectionAsMarkup() const override {
    return blink::WebString();
  }
  void TextSelectionChanged(const blink::WebString& selection_text,
                            uint32_t offset,
                            const gfx::Range& range) override {}
  void SelectRange(const gfx::Point& base, const gfx::Point& extent) override {}
#if BUILDFLAG(ARKWEB_MENU)
  void SelectRangeV2(const gfx::Point& position, bool is_base) override {}
#endif
  void SelectRange(const blink::WebRange&,
                   HandleVisibilityBehavior,
                   blink::mojom::SelectionMenuBehavior,
                   SelectionSetFocusBehavior) override {}
  blink::WebString RangeAsText(const blink::WebRange&) override {
    return blink::WebString();
  }
  void MoveRangeSelection(const gfx::Point& base,
                          const gfx::Point& extent,
                          blink::WebFrame::TextGranularity) override {}
  void MoveCaretSelection(const gfx::Point&) override {}
  bool SetEditableSelectionOffsets(int start, int end) override {
    return false;
  }
  bool AddImeTextSpansToExistingText(
      const blink::WebVector<ui::ImeTextSpan>& ime_text_spans,
      unsigned text_start,
      unsigned text_end) override {}
  bool ClearImeTextSpansByType(ui::ImeTextSpan::Type type,
                               unsigned text_start,
                               unsigned text_end) override {
    return false;
  }
  bool SetCompositionFromExistingText(
      int composition_start,
      int composition_end,
      const blink::WebVector<ui::ImeTextSpan>& ime_text_spans) override {
    return false;
  }
  void ExtendSelectionAndDelete(int before, int after) override {}
  void ExtendSelectionAndReplace(
      int before,
      int after,
      const blink::WebString& replacement_text) override {}
  void MoveRangeSelectionExtent(const gfx::Point&) override {}
  void ReplaceSelection(const blink::WebString&) override {}
  void DeleteSurroundingText(int before, int after) override {}
  void DeleteSurroundingTextInCodePoints(int before, int after) override {}
  void SetTextCheckClient(blink::WebTextCheckClient*) override {}
  void SetSpellCheckPanelHostClient(
      blink::WebSpellCheckPanelHostClient*) override {}
  blink::WebSpellCheckPanelHostClient* SpellCheckPanelHostClient()
      const override {
    return nullptr;
  }
  void ReplaceMisspelledRange(const blink::WebString&) override {}
  void RemoveSpellingMarkers() override {}
  void RemoveSpellingMarkersUnderWords(
      const blink::WebVector<blink::WebString>& words) override {}
  blink::WebContentSettingsClient* GetContentSettingsClient() const override {
    return nullptr;
  }
  void SetContentSettingsClient(blink::WebContentSettingsClient*) override {}
  void ReloadImage(const blink::WebNode&) override {}
  bool IsAllowedToDownload() const override {
    return false;
  }
  bool IsCrossOriginToOutermostMainFrame() const override {
    return false;
  }
  bool FindForTesting(int identifier,
                      const blink::WebString& search_text,
                      bool match_case,
                      bool forward,
                      bool new_session,
                      bool force,
                      bool wrap_within_frame,
                      bool async) override {
    return false;
  }
  void SetTickmarks(const blink::WebElement& target,
                    const blink::WebVector<gfx::Rect>& tickmarks) override {}
  blink::WebNode ContextMenuImageNode() const override {
    return blink::WebNode();
  }
  blink::WebNode ContextMenuNode() const override {
    return blink::WebNode();
  }
  void CopyImageAtForTesting(const gfx::Point&) override {}
  void ShowContextMenuFromExternal(
      const blink::UntrustworthyContextMenuParams& params,
      blink::CrossVariantMojoAssociatedRemote<
          blink::mojom::ContextMenuClientInterfaceBase> context_menu_client)
      override {}
  void UsageCountChromeLoadTimes(const blink::WebString& metric) override {}
  void UsageCountChromeCSI(const blink::WebString& metric) override {}
  bool DispatchedPagehideAndStillHidden() const override {
    return false;
  }
  blink::FrameScheduler* Scheduler() const override {
    return nullptr;
  }
  blink::scheduler::WebAgentGroupScheduler* GetAgentGroupScheduler()
      const override {
    return nullptr;
  }
  scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner(
      blink::TaskType) override {
    return nullptr;
  }
  blink::WebInputMethodController* GetInputMethodController() override {
    return nullptr;
  }
  std::unique_ptr<blink::WebAssociatedURLLoader> CreateAssociatedURLLoader(
      const blink::WebAssociatedURLLoaderOptions&) override {
    return nullptr;
  }

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  virtual std::unique_ptr<blink::WebAssociatedURLLoader> CreateVideoURLLoader(
      const blink::WebAssociatedURLLoaderOptions&) override {
    return nullptr;
  }
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

  void DeprecatedStopLoading() override {}
  gfx::PointF GetScrollOffset() const override {
    return gfx::PointF(0, 0);
  }
  void SetScrollOffset(const gfx::PointF&) override {}
  gfx::Size DocumentSize() const override {}
  bool HasVisibleContent() const override {
    return false;
  }
  gfx::Rect VisibleContentRect() const override {
    return gfx::Rect();
  }
  void DispatchBeforePrintEvent(
      base::WeakPtr<blink::WebPrintClient> print_client) override {}
  blink::WebPlugin* GetPluginToPrint(
      const blink::WebNode& constrain_to_node) override {
    return nullptr;
  }
  uint32_t PrintBegin(const blink::WebPrintParams& print_params,
                      const blink::WebNode& constrain_to_node) override {
    return 0;
  }
  bool WillPrintSoon() override {
    return false;
  }
  void PrintPage(uint32_t page_index, cc::PaintCanvas*) override {}
  void PrintEnd() override {}
  void DispatchAfterPrintEvent() override {}
  bool GetPrintPresetOptionsForPlugin(const blink::WebNode&,
                                      blink::WebPrintPresetOptions*) override {
    return false;
  }
  bool CapturePaintPreview(const gfx::Rect& bounds,
                           cc::PaintCanvas* canvas,
                           bool include_linked_destinations,
                           bool skip_accelerated_content) override {
    return false;
  }
  bool ShouldSuppressKeyboardForFocusedElement() override {
    return false;
  }
  blink::WebPerformanceMetricsForReporting PerformanceMetricsForReporting()
      const override {
    return blink::WebPerformanceMetricsForReporting();
  }
  blink::WebPerformanceMetricsForNestedContexts
  PerformanceMetricsForNestedContexts() const override {
    return blink::WebPerformanceMetricsForNestedContexts();
  }
  void NotifyUserActivation(
      blink::mojom::UserActivationNotificationType notification_type) override {
  }
  bool HasStickyUserActivation() override {
    return false;
  }
  bool HasTransientUserActivation() override {
    return false;
  }
  bool ConsumeTransientUserActivation(
      blink::UserActivationUpdateSource update_source) override {
    return false;
  }
  bool LastActivationWasRestricted() const override {
    return false;
  }
#if BUILDFLAG(IS_WIN)
  WebFontFamilyNames GetWebFontFamilyNames() const override {
    return WebFontFamilyNames();
  }
#endif
  gfx::Size SpoolSizeInPixelsForTesting(
      const blink::WebVector<uint32_t>& pages) override {
    return gfx::Size();
  }
  gfx::Size SpoolSizeInPixelsForTesting(uint32_t page_count) override {
    return gfx::Size();
  }
  void PrintPagesForTesting(
      cc::PaintCanvas*,
      const gfx::Size& spool_size_in_pixels,
      const blink::WebVector<uint32_t>* pages = nullptr) override {}
  gfx::Rect GetSelectionBoundsRectForTesting() const override {
    return gfx::Rect(0, 0);
  }
  gfx::Point GetPositionInViewportForTesting() const override {
    return gfx::Point(0, 0);
  }
  void WasHidden() override {}
  void WasShown() override {}
  void SetAllowsCrossBrowsingInstanceFrameLookup() override {}
  void SetTargetToCurrentHistoryItem(const blink::WebString& target) override {}
  void UpdateCurrentHistoryItem() override {}
  blink::PageState CurrentHistoryItemToPageState() override {
    return blink::PageState();
  }
  blink::WebHistoryItem GetCurrentHistoryItem() const override {
    return blink::WebHistoryItem();
  }
  void ClearActiveFindMatchForTesting() override {}
  void SetLocalStorageArea(
      blink::CrossVariantMojoRemote<blink::mojom::StorageAreaInterfaceBase>
          local_storage_area) override {}
  void SetSessionStorageArea(
      blink::CrossVariantMojoRemote<blink::mojom::StorageAreaInterfaceBase>
          session_storage_area) override {}
  void AddHitTestOnTouchStartCallback(
      base::RepeatingCallback<void(const blink::WebHitTestResult&)> callback)
      override {}
  void BlockParserForTesting() {}
  void ResumeParserForTesting() {}
  void FlushInputForTesting(base::OnceClosure) {}
  bool AllowStorageAccessSyncAndNotify(
      blink::WebContentSettingsClient::StorageType storage_type) override {
    return false;
  }
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void SelectClosetWordAndShowSelectionMenu() override {}
#endif
#if BUILDFLAG(ARKWEB_AI)
  void OnDataDetectorSelectText() override {}
#endif
  void AddMessageToConsoleImpl(const blink::WebConsoleMessage&,
                               bool discard_duplicates) override {}
  void AddInspectorIssueImpl(blink::mojom::InspectorIssueCode code) override {}
  void AddGenericIssueImpl(blink::mojom::GenericIssueErrorType error_type,
                           int violating_node_id) override {}
  void AddGenericIssueImpl(blink::mojom::GenericIssueErrorType error_type,
                           int violating_node_id,
                           const blink::WebString&) override {}
  void CreateFrameWidgetInternal(
      base::PassKey<WebLocalFrame> pass_key,
      blink::CrossVariantMojoAssociatedRemote<
          blink::mojom::FrameWidgetHostInterfaceBase>,
      blink::CrossVariantMojoAssociatedReceiver<
          blink::mojom::FrameWidgetInterfaceBase>,
      blink::CrossVariantMojoAssociatedRemote<
          blink::mojom::WidgetHostInterfaceBase>,
      blink::CrossVariantMojoAssociatedReceiver<
          blink::mojom::WidgetInterfaceBase> widget,
      const viz::FrameSinkId& frame_sink_id,
      bool is_for_nested_main_frame,
      bool is_for_scalable_page,
      bool hidden) override {}
        const std::optional<blink::FrameAdEvidence>& AdEvidence() override {}
  bool IsAdScriptInStack() const {
    return condition_.is_ad_script_in_stack;
  }
 private:
  GURL url_;
  TestWebLocalFrameCondition condition_;
  const blink::WebDocument mock_document_;
  MockWebSecurityOrigin security_origin_;
  MockWebLocalFrame* parent_frame_ { nullptr };
  MockWebDocumentLoader *mock_loader_ { nullptr };
};

class MockWebDocumentSubresourceFilter
    : public blink::WebDocumentSubresourceFilter {
 public:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  bool HasDocumentTypeOption(const blink::WebURL&, const url::Origin&) {}
  bool HasElemHideTypeOption(const blink::WebURL&, const url::Origin&) {}
  bool HasGenericHideTypeOption(const blink::WebURL&, const url::Origin&) {}
#endif
  LoadPolicy GetLoadPolicy(const blink::WebURL& resource_url,
                           network::mojom::RequestDestination) override {
    return LoadPolicy::kAllow;
  }
  LoadPolicy GetLoadPolicyForWebSocketConnect(const blink::WebURL&) override {
    return LoadPolicy::kAllow;
  }
  LoadPolicy GetLoadPolicyForWebTransportConnect(
      const blink::WebURL&) override {
    return LoadPolicy::kAllow;
  }
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void ClearStatistics() override {}
  std::unique_ptr<std::string> GetElementHidingSelectors(
      const blink::WebURL& document_url,
      bool need_common_selectors) override {
    return nullptr;
  }
  void DidMatchCssRule(const blink::WebURL& document_url,
                       const std::string& dom_path,
                       //  unsigned rule_line_num = 0,
                       bool is_for_report = false) override {}
  void SetDidFinishLoad(bool did_load_finished) override {}
  bool GetDidFinishLoad() override {
    return false;
  }
  std::unique_ptr<std::vector<std::string>> GetUserDomPathSelectors(
      const blink::WebURL& document_url,
      bool need_generic_selectors) override {
    return nullptr;
  }
  void set_activation_state(bool enabled) override {}
#endif
  void ReportDisallowedLoad() override {}
  bool ShouldLogToConsole() override {
    return false;
  }
};

class MockWebWorkerFetchContext : public blink::WebWorkerFetchContext {
 public:
  MOCK_METHOD(void,
              SetUserSubresourceFilterBuilder,
              (std::unique_ptr<blink::WebDocumentSubresourceFilter::Builder>),
              (override));
  void SetTerminateSyncLoadEvent(base::WaitableEvent*) override {}
  void InitializeOnWorkerThread(blink::AcceptLanguagesWatcher*) override {}
  blink::URLLoaderFactory* GetURLLoaderFactory() override { return nullptr; }
  std::unique_ptr<blink::URLLoaderFactory> WrapURLLoaderFactory(
      blink::CrossVariantMojoRemote<
          network::mojom::URLLoaderFactoryInterfaceBase> url_loader_factory)
      override {
    return nullptr;
  }
  void FinalizeRequest(blink::WebURLRequest&) override {}
  // void DidStartRequest(const blink::WebURLRequest&) override {}
  // void DidReceiveResponse(const blink::WebURLRequest&,
  //                        const blink::WebURLResponse&) override {}
  net::SiteForCookies SiteForCookies() const {}
  std::optional<blink::WebSecurityOrigin> TopFrameOrigin() const {}
  blink::WebString GetAcceptLanguages() const override {
    return blink::WebString();
  }
  void SetIsOfflineMode(bool is_offline_mode) override {}
  blink::WebVector<std::unique_ptr<blink::URLLoaderThrottle>> CreateThrottles(
      const network::ResourceRequest& request) override {
    return blink::WebVector<std::unique_ptr<blink::URLLoaderThrottle>>();
  }
  blink::mojom::ControllerServiceWorkerMode GetControllerServiceWorkerMode()
      const override {
    return blink::mojom::ControllerServiceWorkerMode::kNoController;
  }
};

class MockUserSubresourceFilterHost : public mojom::UserSubresourceFilterHost {
 public:
  MOCK_METHOD(void, DidDisallowFirstUserSubresource, (), (override));
  MOCK_METHOD(void,
              UserSetDocumentLoadStatistics,
              (mojom::DocumentLoadStatisticsPtr),
              (override));
  MOCK_METHOD(void, FrameIsUserAd, (), (override));
  MOCK_METHOD(void, FrameWasCreatedByUserAdScript, (), (override));
  MOCK_METHOD(void, OnAdsViolationTriggered, (mojom::AdsViolation), ()); // (override));
  MOCK_METHOD(void,
              UserAdScriptDidCreateFencedFrame,
              (const blink::RemoteFrameToken&),
              (override));
#if BUILDFLAG(ARKWEB_ADBLOCK)
  MOCK_METHOD(void,
              UserSetStatisticsAfterDocumentLoad,
              (mojom::DocumentLoadStatisticsPtr),
              (override));
#endif
};

class MockUserUnverifiedRulesetDealer
    : public subresource_filter::UserUnverifiedRulesetDealer {
 public:
  MOCK_METHOD(bool, IsRulesetFileAvailable, (), ());
  MOCK_METHOD(scoped_refptr<const subresource_filter::MemoryMappedRuleset>,
              GetRuleset,
              (),
              (override));
  MOCK_METHOD(base::File, DuplicateRulesetFile, (), ());
};

class MockSubresourceFilterHost {
 public:
  MOCK_METHOD(void,
              SetStatisticsAfterDocumentLoad,
              (const mojom::DocumentLoadStatistics&),
              ());
};

class MockDocumentSubresourceFilter {
 public:
  MOCK_METHOD(const mojom::DocumentLoadStatistics&, statistics, (), (const));
};

class MockFilterForLastCreatedDocument {
 public:
  MockDocumentSubresourceFilter filter_;
  MOCK_METHOD(MockDocumentSubresourceFilter&, filter, (), ());
  MOCK_METHOD(void, ClearStatistics, (), ());
};

class MockKURL {
 public:
  MOCK_METHOD(bool, ProtocolIs, (const char*), (const));
};

class MockDocumentSubresourceFilterExt : public DocumentSubresourceFilterExt {
 public:
  MockDocumentSubresourceFilterExt(
      url::Origin document_origin,
      mojom::ActivationState activation_state,
      scoped_refptr<const MemoryMappedRuleset> ruleset,
      std::string_view uma_tag)
      : DocumentSubresourceFilterExt(document_origin,
                                     activation_state,
                                     ruleset,
                                     uma_tag) {}

#if BUILDFLAG(ARKWEB_ADBLOCK)
  MOCK_METHOD(void, ClearStatistics, (), ());
  MOCK_METHOD(void, SetDidFinishLoad, (bool), ());
  MOCK_METHOD(bool, GetDidFinishLoad, (), ());
  MOCK_METHOD(std::unique_ptr<std::string>,
              GetSelectors,
              (const GURL&, bool),
              (const));
  MOCK_METHOD(std::unique_ptr<
                  const std::vector<const url_pattern_index::flat::CssRule*>>,
              FindMatchingCssRule,
              (const GURL&, bool),
              (const));
  MOCK_METHOD(bool,
              HasGenericHideTypeOption,
              (const GURL&, const url::Origin&),
              (const));
  MOCK_METHOD(bool,
              HasElemHideTypeOption,
              (const GURL&, const url::Origin&),
              (const));
  MOCK_METHOD(bool,
              HasDocumentTypeOption,
              (const GURL&, const url::Origin&),
              (const));
  MOCK_METHOD(std::unique_ptr<
                  const std::vector<const url_pattern_index::flat::CssRule*>>,
              FindMatchingCssRule,
              (const GURL&),
              (const));
  MOCK_METHOD(void,
              DidMatchCssRule,
              (const GURL&, const std::string&, bool),
              ());
  MOCK_METHOD(std::unique_ptr<std::vector<std::string>>,
              GetUserDomPathSelectors,
              (const GURL&, bool),
              (const));
#endif
};

class MockActivationState : public mojom::ActivationState {
 public:
  MockActivationState() {}
};

class MockAssociatedInterfaceProvider
    : public blink::AssociatedInterfaceProvider {
 public:
  MockAssociatedInterfaceProvider()
      : blink::AssociatedInterfaceProvider(nullptr) {}
  ~MockAssociatedInterfaceProvider() = default;
  MOCK_METHOD(void,
              GetInterface,
              (const std::string& name,
               mojo::ScopedInterfaceEndpointHandle handle),
              ());
};

class TestWebDocumentSubresourceFilterImpl {
 public:
  constexpr static char kTestAlphaURLPathSuffix[] = "alpha";
  constexpr static char kTestAlphaURL[] = "https://example.com/alpha";
  TestWebDocumentSubresourceFilterImpl() {}

  void CreateTestRuleset(std::string_view suffix = kTestAlphaURLPathSuffix) {
    if (ruleset_) {
      return;
    }
    testing::TestRulesetPair test_ruleset_pair;
    ASSERT_NO_FATAL_FAILURE(
        test_ruleset_creator_.CreateRulesetToDisallowURLsWithPathSuffix(
            suffix, &test_ruleset_pair));
    ruleset_ = MemoryMappedRuleset::CreateAndInitialize(
        testing::TestRuleset::Open(test_ruleset_pair.indexed));
  }
  std::unique_ptr<WebDocumentSubresourceFilterImpl> CreateWebFilterImpl(
      const std::string_view& url = kTestAlphaURL) {
    base::OnceClosure first_disallowed_load_callback(
        base::BindOnce([]() { /* Do nothing */ }));
    return std::make_unique<WebDocumentSubresourceFilterImpl>(
        url::Origin::Create(GURL(url)), activation_state_, std::move(ruleset_),
        std::move(first_disallowed_load_callback));
  }

  std::unique_ptr<MockDocumentSubresourceFilterExt> CreateFilter(
      const std::string_view& url = kTestAlphaURL,
      const std::string_view& tag = "test") {
    std::make_unique<MockDocumentSubresourceFilterExt>(
        url::Origin::Create(GURL(url)), activation_state_, std::move(ruleset_),
        tag);
  }

  testing::TestRulesetCreator test_ruleset_creator_;
  scoped_refptr<const MemoryMappedRuleset> ruleset_;
  MockActivationState activation_state_;
};

class MockArkWebSubresourceFilterAgentExt
    : public ArkWebSubresourceFilterAgentExt {
 public:
  MockArkWebSubresourceFilterAgentExt(std::string_view url,
                                      const TestRenderFrameCondition& cond,
                                      content::RenderFrame* render_frame,
                                      UnverifiedRulesetDealer* ruleset_dealer)
      : ArkWebSubresourceFilterAgentExt(render_frame, ruleset_dealer) {}

  MockArkWebSubresourceFilterAgentExt(
      const MockArkWebSubresourceFilterAgentExt&) = delete;
  MockArkWebSubresourceFilterAgentExt& operator=(
      const MockArkWebSubresourceFilterAgentExt&) = delete;

  ~MockArkWebSubresourceFilterAgentExt() {}

 protected:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void SendStatisticsAfterDocumentLoad(
      const mojom::DocumentLoadStatistics& statistics) override {
  }
#endif
};

class MockUserSubresourceFilterAgent : public UserSubresourceFilterAgent {
 public:
  MockUserSubresourceFilterAgent(std::string_view url,
                                 const TestRenderFrameCondition& cond,
                                 content::RenderFrame* render_frame,
                                 UserUnverifiedRulesetDealer* ruleset_dealer)
      : UserSubresourceFilterAgent(render_frame, ruleset_dealer),
        url_(url),
        condition_(cond) {}

  MockUserSubresourceFilterAgent(const MockUserSubresourceFilterAgent&) =
      delete;
  MockUserSubresourceFilterAgent& operator=(
      const MockUserSubresourceFilterAgent&) = delete;

  ~MockUserSubresourceFilterAgent() override {}

  bool IsParentAdFrame() {
    return condition_.has_parent_frame &&
           condition_.local_frame_conditions[1].is_ad_frame;
  }
  blink::WebFrame* GetLocalFrame(content::RenderFrame* render_frame) {
    return nullptr;
  }
  void SetAdEvidence(const blink::FrameAdEvidence& ad_evidence) {
  }
  void SetUserSubresourceFilterForCurrentDocument(
      std::unique_ptr<blink::WebDocumentSubresourceFilter> filter) {}

  void SignalFirstSubresourceDisallowedForCurrentDocument() {}

  void SendDocumentLoadStatistics(
      const mojom::DocumentLoadStatistics& statistics) {}

  void SendFrameIsAd() {}

  void SendFrameWasCreatedByAdScript() {}
  const std::optional<blink::FrameAdEvidence>& AdEvidence() {}

 private:
  GURL url_;
  TestRenderFrameCondition condition_;
};

class SubresourceFilterAgentTester {
 public:
  SubresourceFilterAgentTester() = default;
  ~SubresourceFilterAgentTester() {
    parent_frame_.reset();
    web_frame_.reset();
    render_frame_.reset();
  }

  MockRenderFrame* GetRenderFrame() const { return render_frame_.get(); }
  MockWebLocalFrame* GetWebLocalFrame() { return web_frame_.get(); }
  MockWebLocalFrame* GetWebParentFrame() { return parent_frame_.get(); }

 protected:
  void CreateRenderFrame(const TestRenderFrameCondition& cond,
                                    std::string_view doc_url = "about:blank") {
    render_frame_ = std::make_unique<MockRenderFrame>(cond);
    EXPECT_NE(render_frame_, nullptr);
    web_frame_ = std::make_unique<MockWebLocalFrame>(
        doc_url, cond.local_frame_conditions[0], scope_, frame_token_);
    EXPECT_NE(web_frame_, nullptr);
    render_frame_->SetTestLocalFrame(web_frame_.get());
    // parent
    if (cond.has_parent_frame) {
      parent_frame_ = std::make_unique<MockWebLocalFrame>(
          doc_url, cond.local_frame_conditions[1], scope_, frame_token_);
      EXPECT_NE(parent_frame_, nullptr);
      web_frame_->SetTestParentFrame(parent_frame_.get());
    }
  }

  blink::WebDocument document_;
  std::unique_ptr<MockRenderFrame> render_frame_{nullptr};
  std::unique_ptr<MockWebLocalFrame> web_frame_{nullptr};
  std::unique_ptr<MockWebLocalFrame> parent_frame_{nullptr};
  blink::mojom::TreeScopeType scope_;
  blink::LocalFrameToken frame_token_;
};

class UserSubresourceFilterAgentTester : public SubresourceFilterAgentTester{
 public:
  UserSubresourceFilterAgentTester() = default;
  ~UserSubresourceFilterAgentTester() {
    if (agent_) {
      auto abserver = static_cast<content::RenderFrameObserver*>(agent_.get());
      abserver->render_frame_ = nullptr;
    }
    agent_.reset();
  }

  UserSubresourceFilterAgent* GetAgent() { return agent_.get(); }
  UserUnverifiedRulesetDealer *GetUserUnverifiedRulesetDealer() { return &user_ruleset_dealer_; }

  void CreateSubresourceFilterAgent(const TestRenderFrameCondition& cond,
                                    std::string_view doc_url = "about:blank") {
    if (!render_frame_) {
      CreateRenderFrame(cond, doc_url);
    }
    // agent
    agent_ =
        std::make_unique<::testing::StrictMock<MockUserSubresourceFilterAgent>>(
            doc_url, cond, nullptr, &user_ruleset_dealer_);
    EXPECT_NE(agent_, nullptr);
    auto abserver = static_cast<content::RenderFrameObserver*>(agent_.get());
    abserver->render_frame_ = render_frame_.get();
  }

  void SetTestUserRulesetToDisallowURLs(const std::string& suffix) {
    testing::TestRulesetPair test_ruleset_pair;
    ASSERT_NO_FATAL_FAILURE(
        test_ruleset_creator_.CreateRulesetToDisallowURLsWithPathSuffix(
            suffix, &test_ruleset_pair));
    user_ruleset_dealer_.SetRulesetFile(
        testing::TestRuleset::Open(test_ruleset_pair.indexed));
  }
 private:
  std::unique_ptr<UserSubresourceFilterAgent> agent_{nullptr};
  testing::TestRulesetCreator test_ruleset_creator_;
  UserUnverifiedRulesetDealer user_ruleset_dealer_;
};

class ArkWebSubresourceFilterAgentExtTester : public SubresourceFilterAgentTester {
 public:
  ArkWebSubresourceFilterAgentExtTester() = default;
  ~ArkWebSubresourceFilterAgentExtTester() {
    if (agent_) {
      auto abserver = static_cast<content::RenderFrameObserver*>(agent_.get());
      abserver->render_frame_ = nullptr;
    }
    agent_.reset();
  }
  ArkWebSubresourceFilterAgentExt* GetAgent() { return agent_.get(); }
  UnverifiedRulesetDealer *GetUnverifiedRulesetDealer() { return &ruleset_dealer_; }

 void SetTestRulesetToDisallowURLs(std::string_view suffix) {
    subresource_filter::testing::TestRulesetPair test_ruleset_pair;
    ASSERT_NO_FATAL_FAILURE(
        test_ruleset_creator_.CreateRulesetToDisallowURLsWithPathSuffix(
            suffix, &test_ruleset_pair));
    ruleset_dealer_.SetRulesetFile(
        subresource_filter::testing::TestRuleset::Open(
            test_ruleset_pair.indexed));
  }

  void CreateMockArkSubresourceFilterAgentExt(const TestRenderFrameCondition& cond,
                                    std::string_view doc_url = "about:blank") {
    if (!render_frame_) {
      CreateRenderFrame(cond, doc_url);
    }

    // agent
    agent_ =
        std::make_unique<::testing::StrictMock<MockArkWebSubresourceFilterAgentExt>>(doc_url, cond,
            nullptr, &ruleset_dealer_);
    EXPECT_NE(agent_, nullptr);
    auto abserver = static_cast<content::RenderFrameObserver*>(agent_.get());
    abserver->render_frame_ = render_frame_.get();
  }

 private:
  std::unique_ptr<ArkWebSubresourceFilterAgentExt> agent_{nullptr};
  testing::TestRulesetCreator test_ruleset_creator_;
  UnverifiedRulesetDealer ruleset_dealer_;
};
}  // namespace subresource_filter
#endif  // USER_SUBRESOURCE_FILTER_AGENT_TEST_H_