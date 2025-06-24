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

namespace content {

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

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
void RenderFrameHostImpl::SendAccessibilityEvent(int64_t accessibilityId,
                                                 int32_t eventType,
                                                 const std::string& argument) {
  RenderWidgetHostViewBase* view = static_cast<RenderWidgetHostViewBase*>(
      render_view_host_->GetWidget()->GetView());
  if (view) {
    view->SendAccessibilityEvent(accessibilityId, eventType, argument);
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void RenderFrameHostImpl::NotifyContextMenuWillShow() {
  delegate_->NotifyContextMenuWillShow();
}
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
void RenderFrameHostImpl::GetCreateNewWindow(
    const GURL& target_url,
    WindowOpenDisposition disposition,
    bool allow_popup,
    GetCreateNewWindowCallback callback) {
  bool effective_transient_activation_state =
      allow_popup || frame_tree_node_->HasTransientUserActivation();
  GetContentClient()->browser()->CanCreateWindow(
      this, target_url, disposition, effective_transient_activation_state,
      std::move(callback));
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
#endif

#if BUILDFLAG(ARKWEB_AI)
void RenderFrameHostImpl::CloseImageOverlaySelection() {
  if (delegate_) {
    delegate_->CloseImageOverlaySelection();
  }
}
#endif  // BUILDFLAG(ARKWEB_AI)

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

}  // namespace content
