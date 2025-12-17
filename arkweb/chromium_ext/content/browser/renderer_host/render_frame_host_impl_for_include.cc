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

#include "arkweb/build/features/features.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "mojo/public/cpp/system/platform_handle.h"
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
#include "arkweb/chromium_ext/content/browser/renderer_host/navigation_request_utils.h"
#endif

#if BUILDFLAG(ARKWEB_DFX_TRACING)
#include "arkweb/chromium_ext/content/renderer/ark_web_render_frame_impl.h"
#endif

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
#include "net/http/http_status_code.h"
#endif

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
#include "arkweb/chromium_ext/content/browser/web_contents/web_contents_impl_ext.h"
#endif

namespace content {

// ExecuteJavascriptInFrames need create new worldId, this is the min value;
const int32_t kCreateIsolatedWorldIdMin = 10000;

#if BUILDFLAG(IS_ARKWEB)
void RenderFrameHostImpl::ExecuteJavaScriptExt(
    const int fd,
    const uint64_t scriptLength,
    JavaScriptResultCallback callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  CHECK(CanExecuteJavaScript());
  AssertFrameWasCommitted();

  const bool wants_result = !callback.is_null();
  MojoPlatformHandle platform_handle;
  platform_handle.struct_size = sizeof(platform_handle);
  platform_handle.type = MOJO_PLATFORM_HANDLE_TYPE_FILE_DESCRIPTOR;
  platform_handle.value = static_cast<uint64_t>(fd);
  MojoHandle handle;
  MojoWrapPlatformHandle(&platform_handle, nullptr, &handle);
  GetAssociatedLocalFrame()->JavaScriptExecuteRequestExt(
      mojo::ScopedHandle(mojo::Handle(handle)), scriptLength, wants_result,
      std::move(callback));
}
#endif

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void RenderFrameHostImpl::OnOverScrollOffsetChanged(float offset_x,
                                                    float offset_y) {
  WebContentsImplExt* web_contents =
      static_cast<WebContentsImplExt*>(WebContents::FromRenderFrameHost(this));
  if (web_contents) {
    web_contents->OnOverScrollOffsetChanged(offset_x, offset_y);
  }
}
#endif

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
void RenderFrameHostImpl::SendAccessibilityEvent(int64_t accessibilityId,
                                                 int32_t eventType,
                                                 const std::string& argument) {
  if (!render_view_host_ || !render_view_host_->GetWidget()) {
    LOG(ERROR) << "RenderFrameHostImpl::SendAccessibilityEvent accessibilityId " << accessibilityId
               << ", cannot get view from current.";
    return;
  }
  RenderWidgetHostViewBase* view = static_cast<RenderWidgetHostViewBase*>(render_view_host_->GetWidget()->GetView());
  if (view) {
    view->SendAccessibilityEvent(accessibilityId, eventType, argument);
    return;
  }
  // on tablet-type devices, view in current frame may be null
  RenderFrameHostImpl* parent = GetParent();
  uint8_t depth = 0;
  constexpr uint8_t kMaxIframeDepth = 5;
  while (parent && depth < kMaxIframeDepth) {
    depth++;
    if (!parent->render_view_host_ || !parent->render_view_host_->GetWidget()) {
      parent = parent->GetParent();
      continue;
    }
    view = static_cast<RenderWidgetHostViewBase*>(parent->render_view_host_->GetWidget()->GetView());
    if (view) {
      LOG(DEBUG) << "RenderFrameHostImpl::SendAccessibilityEvent accessibilityId " << accessibilityId
                 << ", successfully get view from parent, recursive count " << static_cast<int32_t>(depth);
      view->SendAccessibilityEvent(accessibilityId, eventType, argument);
      return;
    }
    parent = parent->GetParent();
  }
  LOG(ERROR) << "RenderFrameHostImpl::SendAccessibilityEvent accessibilityId " << accessibilityId
             << ", cannot get view from current or parent.";
}
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void RenderFrameHostImpl::NotifyContextMenuWillShow() {
  delegate_->NotifyContextMenuWillShow();
}
#endif
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
void RenderFrameHostImpl::GetCreateNewWindow(
    const GURL& target_url,
    WindowOpenDisposition disposition,
    bool allow_popup,
    blink::mojom::WindowFeaturesPtr features,
    GetCreateNewWindowCallback callback) {
  if (delegate_ && delegate_->IsActiveFileChooser()) {
    // Do not allow opening a new window or tab while a file select is active
    // file chooser to avoid user confusion over which tab triggered the file
    // chooser.
    AddMessageToConsole(blink::mojom::ConsoleMessageLevel::kWarning,
                        "window.open blocked due to active file chooser.");
    LOG(WARNING) << "The window.open blocked due to active file chooser.";
    std::move(callback).Run(mojom::CreateNewWindowStatus::kBlocked);
    return;
  }
  bool effective_transient_activation_state =
      allow_popup || frame_tree_node_->HasTransientUserActivation();
  GetContentClient()->browser()->CanCreateWindow(
      this, target_url, disposition, effective_transient_activation_state,
      features->bounds, std::move(callback));
}
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_PRECOMPILE)
void RenderFrameHostImpl::GenerateCodeCache(
    const std::string& url,
    const std::string& script,
    const std::shared_ptr<oh_code_cache::CacheOptions>& cacheOptions,
    CodeCacheCallback callback) {
  auto options = blink::mojom::CacheOptions::New();

  for (auto header : cacheOptions->response_headers_) {
    options->response_headers.insert(
        std::make_pair(header.first, header.second));
  }

  GetAssociatedLocalFrame()->GenerateCodeCache(url, script, std::move(options),
                                               std::move(callback));
}
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(IS_ARKWEB_EXT)
void RenderFrameHostImpl::GetImageFromCache(const std::string& url,
                                            ImageCacheCallback callback) {
  GetAssociatedLocalFrame()->GetImageFromCache(url, std::move(callback));
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void RenderFrameHostImpl::OnClearContextMenu() {
  if (IsInactiveAndDisallowActivation(
          DisallowActivationReasonId::kShowContextMenu)) {
    return;
  }
  delegate_->ClearContextMenu();
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_ADBLOCK)
void RenderFrameHostImpl::UpdateAdBlockEnabledToRender(
    bool site_adblock_enabled) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  FrameTreeNode* tree_node = frame_tree_node();
  if (!tree_node->IsMainFrame()) {
    return;
  }
  // Update the adblock site switch of local_frame_root and
  // activation_state.activation_level of subresource_filter_agent
  // in the rendering process only by the main frame.
  RenderFrameHostImpl* pending_frame_host =
      tree_node->render_manager()->speculative_frame_host();
  if (pending_frame_host && pending_frame_host->frame_) {
    LOG(INFO) << "[AdBlock] Speculative update adblock site switch:"
              << site_adblock_enabled;
    pending_frame_host->frame_->OnUpdateAdBlockEnabledToRender(
        site_adblock_enabled);
  }

  RenderFrameHostImpl* current_frame_host = tree_node->current_frame_host();
  if (current_frame_host && current_frame_host->frame_) {
    LOG(INFO) << "[AdBlock] Update adblock site switch:"
              << site_adblock_enabled;
    current_frame_host->frame_->OnUpdateAdBlockEnabledToRender(
        site_adblock_enabled);
  }
}
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
void RenderFrameHostImpl::AddNamedObject(const std::string& name,
                                         int32_t object_id,
                                         base::Value::List& async_method_list,
                                         bool need_update) {
  if (!frame_) {
    return;
  }
  frame_->AddNamedObject(name, object_id, std::move(async_method_list),
                         need_update);
}
#endif

#if BUILDFLAG(ARKWEB_MENU)
void RenderFrameHostImpl::MouseSelectMenuShow(bool show) {
  if (delegate_) {
    delegate_->MouseSelectMenuShow(show);
  }
}

void RenderFrameHostImpl::ChangeVisibilityOfQuickMenu() {
  if (delegate_) {
    delegate_->ChangeVisibilityOfQuickMenu();
  }
}

void RenderFrameHostImpl::HideQuickMenu() {
  if (delegate_ && delegate_->IsQuickMenuShow()) {
    delegate_->ChangeVisibilityOfQuickMenu();
  }
}
// LCOV_EXCL_STOP
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_AI)
void RenderFrameHostImpl::CloseImageOverlaySelection() {
  if (delegate_) {
    delegate_->CloseImageOverlaySelection();
  }
}
#endif  // BUILDFLAG(ARKWEB_AI)

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
bool RenderFrameHostImpl::IsJsDialogShowOrBeforeUnloadTimedOut() {
  DCHECK(IsInPrimaryMainFrame());
  if (!delegate_) {
    return false;
  }
  page_close_state_ = PageCloseState::kNotClosing;
  // If there is a JavaScript dialog up, don't bother sending the renderer the
  // close event because it is known unresponsive, waiting for the reply from
  // the dialog.
  return delegate_->IsJavaScriptDialogShowing() || BeforeUnloadTimedOut();
}
#endif // ARKWEB_DISATCH_BEFORE_UNLOAD
// LCOV_EXCL_STOP

void CommitNavigationExt(
    const std::string& effective_scheme,
    ContentBrowserClient::NonNetworkURLLoaderFactoryMap& non_network_factories,
    BrowserContext* browser_context) {
  if (effective_scheme == url::kResourcesScheme) {
    base::TaskPriority file_factory_priority =
        base::TaskPriority::USER_BLOCKING;
    non_network_factories.emplace(
        url::kResourcesScheme,
        FileURLLoaderFactory::Create(
            browser_context->GetPath(),
            browser_context->GetSharedCorsOriginAccessList(),
            file_factory_priority));
  }
}

bool RenderFrameHostImpl::GetWorldId(const std::string& worldName, int32_t* worldId) {
  if (worldName.empty()) {
    return false;
  }
  if (isolated_world_.empty()) {
    *worldId = kCreateIsolatedWorldIdMin + 1;
    isolated_world_.emplace(worldName, *worldId);
    return true;
  }
 
  auto it = isolated_world_.find(worldName);
  if (it != isolated_world_.end()) {
    *worldId = it->second;
    return true;
  }
 
  int32_t maxValue = INT_MIN;
  for (const auto& pair : isolated_world_) {
    if (pair.second > maxValue) {
      maxValue = pair.second;
    }
  }
  *worldId = maxValue + 1;
  isolated_world_.emplace(worldName, *worldId);
  return true;
}
 
void RenderFrameHostImpl::ExecuteJavaScriptInFrames(
    const std::u16string& javascript,
    bool recursive,
    const std::string& worldName,
    JavaScriptResultCallback callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  CHECK(CanExecuteJavaScript());
 
  const bool wants_result = !callback.is_null();
  int32_t worldId = 0;
  bool worldIdResult = GetWorldId(worldName, &worldId);
  if (worldIdResult) {
    GetAssociatedLocalFrame()->JavaScriptExecuteRequestInIsolatedWorld(
      javascript, wants_result, worldId, std::move(callback));
  } else {
    GetAssociatedLocalFrame()->JavaScriptExecuteRequest(javascript, wants_result,
                                                        std::move(callback));
  }
 
  if (!recursive) {
    return;
  }
 
  RenderFrameHostImpl* initialFrame = this;
  ForEachRenderFrameHost(
    [&javascript, &worldName, &initialFrame](RenderFrameHostImpl* rfh) {
    int32_t world_id = 0;
    if (rfh == initialFrame) {
      return;
    }
    rfh->AllowInjectingJavaScript();
    bool worldId_result = rfh->GetWorldId(worldName, &world_id); 
    if (worldId_result) {
      rfh->GetAssociatedLocalFrame()->JavaScriptExecuteRequestInIsolatedWorld(
        javascript, false, world_id, JavaScriptResultCallback {});
    } else {
      rfh->GetAssociatedLocalFrame()->JavaScriptExecuteRequest(javascript, false,
                                                               JavaScriptResultCallback {});
    }
  });
}

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
void RenderFrameHostImpl::CommitFailedNavigation(
    mojom::NavigationClient* navigation_client,
    NavigationRequest* navigation_request,
    blink::mojom::CommonNavigationParamsPtr common_params,
    blink::mojom::CommitNavigationParamsPtr commit_params,
    bool has_stale_copy_in_cache,
    int error_code,
    int extended_error_code,
    const net::ResolveErrorInfo& resolve_error_info,
    const std::optional<std::string>& error_page_content,
    std::unique_ptr<blink::PendingURLLoaderFactoryBundle> subresource_loaders,
    const blink::DocumentToken& document_token,
    blink::mojom::PolicyContainerPtr policy_container,
    mojom::AlternativeErrorPageOverrideInfoPtr alternative_error_page_info,
    mojom::NavigationClient::CommitFailedNavigationCallback callback) {
  std::string html = "";
  std::optional<std::string> override_error_page_content = std::nullopt;

  int real_error_code = error_code;
  std::string error_info = net::ErrorToShortString(error_code);
  if (error_code == net::ERR_HTTP_RESPONSE_CODE_FAILURE) {
    DCHECK_NE(commit_params->http_response_code, -1);
    real_error_code = commit_params->http_response_code;
    error_info = net::GetHttpReasonPhrase(
      static_cast<net::HttpStatusCode>(real_error_code));
  }

  FrameTreeNode* tree_node = frame_tree_node();

  if (GetOrCreateWebPreferences().error_page_enabled && tree_node->IsMainFrame()) {
    GetContentClient()->browser()->OverrideErrorPage(
      navigation_request->frame_tree_node()->frame_tree_node_id(),
      commit_params->is_browser_initiated,
      commit_params->original_url,
      commit_params->original_method,
      common_params->has_user_gesture,
      false,
      navigation_request->frame_tree_node()->IsOutermostMainFrame(),
      real_error_code,
      error_info,
      navigation_request->frame_tree_node()->frame_tree().is_prerendering(),
      ui::PageTransitionFromInt(common_params->transition),
      &html);
  }

  if (html.empty()) {
    override_error_page_content = error_page_content;
  } else {
    override_error_page_content.emplace(html);
    common_params->is_override_error_page = true;
  }

  navigation_client->CommitFailedNavigation(
      std::move(common_params), std::move(commit_params),
      has_stale_copy_in_cache, error_code, extended_error_code,
      resolve_error_info, override_error_page_content, std::move(subresource_loaders),
      document_token, std::move(policy_container),
      std::move(alternative_error_page_info), std::move(callback));
}
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_PDF)
void RenderFrameHostImpl::OnPdfScrollAtBottom(const std::string& url) {
  if (delegate_) {
    delegate_->OnPdfScrollAtBottom(url);
  }
}

void RenderFrameHostImpl::OnPdfLoadEvent(int32_t result, const std::string& url) {
  if (delegate_) {
    delegate_->OnPdfLoadEvent(result, url);
  }
}

void RenderFrameHostImpl::SetIsPDF(bool is_pdf) {
  auto* mojo_frame = GetMojomFrameInRenderer();
  if (mojo_frame) {
    mojo_frame->SetIsPDF(is_pdf);
  }
}
#endif  // BUILDFLAG(ARKWEB_PDF)
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
void RenderFrameHostImpl::OnDetectedBlankScreen(
    const std::string& url,
    int32_t blankScreenReason,
    int32_t detectedContentfulNodesCount) {
  RenderWidgetHostViewBase* view = static_cast<RenderWidgetHostViewBase*>(
      render_view_host_->GetWidget()->GetView());
  if (view && blank_screen_detection_enable_) {
    view->OnDetectedBlankScreen(url, blankScreenReason,
                                detectedContentfulNodesCount);
  }
}

void RenderFrameHostImpl::DetectBlankScreen(const std::string& url) {
  if (blank_screen_detection_enable_) {
    GetAssociatedLocalFrame()->DetectBlankScreen(
        url, blank_screen_detection_timing_, blank_screen_detection_methods_,
        blank_screen_threshold_);
  }
}

void RenderFrameHostImpl::SetBlankScreenDetectionConfig(
    bool enable,
    const std::vector<double>& detectionTiming,
    const std::vector<int32_t>& detectionMethods,
    int32_t contentfulNodesCountThreshold) {
  blank_screen_detection_enable_ = enable;
  blank_screen_detection_timing_ = detectionTiming;
  blank_screen_detection_methods_ = detectionMethods;
  blank_screen_threshold_ = contentfulNodesCountThreshold;
}
#endif

#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
void RenderFrameHostImpl::OnFirstScreenPaint(const std::string& url,
                                             int64_t navigationStartTime,
                                             int64_t firstScreenPaintTime) {
  RenderWidgetHostViewBase* view = static_cast<RenderWidgetHostViewBase*>(
      render_view_host_->GetWidget()->GetView());
  if (view) {
    view->OnFirstScreenPaint(url, navigationStartTime, firstScreenPaintTime);
  }
}
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
net::Error RenderFrameHostImpl::GetNetErrorCode() {
  return net_error_;
}
#endif

#if BUILDFLAG(ARKWEB_CONSOLE_LOGGING)
void RenderFrameHostImpl::DidAddMessageToConsoleV2(
    blink::mojom::ConsoleMessageLevel log_level,
    blink::mojom::ConsoleMessageSource log_source,
    const std::u16string& message,
    uint32_t line_no,
    const std::optional<std::u16string>& source_id,
    const std::optional<std::u16string>& untrusted_stack_trace) {
  std::u16string updated_source_id;
  if (source_id.has_value())
    updated_source_id = *source_id;
  if (delegate_->DidAddMessageToConsole(this, log_level, log_source, message, line_no,
                                        updated_source_id,
                                        untrusted_stack_trace)) {
    return;
  }

  // Pass through log severity only on builtin components pages to limit console
  // spew.
  const bool is_web_ui = HasWebUIScheme(GetMainFrame()->GetLastCommittedURL());
  if (is_web_ui) {
    DCHECK_EQ(GetMainFrame(), GetOutermostMainFrame())
        << "The mainframe and outermost mainframe should be the same in WebUI.";
  }
  const bool is_builtin_component =
      is_web_ui ||
      GetContentClient()->browser()->IsBuiltinComponent(
          GetProcess()->GetBrowserContext(), GetLastCommittedOrigin());
  const bool is_off_the_record =
      GetSiteInstance()->GetBrowserContext()->IsOffTheRecord();

  LogConsoleMessage(log_level, message, line_no, is_builtin_component,
                    is_off_the_record, updated_source_id);
}
#endif

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
void RenderFrameHostImpl::OnDocumentEndReady() {
  WebContentsImplExt* web_contents =
      static_cast<WebContentsImplExt*>(WebContents::FromRenderFrameHost(this));

  auto globalId = GetGlobalId();

  FrameInfos frameInfo;
  frameInfo.id = std::to_string(globalId.child_id) + "_" + std::to_string(globalId.frame_routing_id);
  if (content::RenderFrameHostImpl* parent = GetParent()) {
    auto parentGlobalId = parent->GetGlobalId();
    frameInfo.parentId = std::to_string(parentGlobalId.child_id) + "_" +
                         std::to_string(parentGlobalId.frame_routing_id);
  } else {
    frameInfo.parentId.clear();
  }

  LOG(DEBUG) << "RenderFrameHostImpl::OnDocumentEndReady frameInfo id:" << frameInfo.id
            << " ,parentId:" << frameInfo.parentId;

  if (web_contents) {
    web_contents->OnDocumentEndReady(frameInfo);
  }
}
#endif
}  // namespace content
