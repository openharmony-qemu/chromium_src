// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/content/browser/web_contents/web_contents_impl_ext.h"

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "content/public/browser/custom_media_player_listener.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
#include "arkweb/chromium_ext/content/browser/display_cutout/display_cutout_host_ohos.h"
#include "arkweb/chromium_ext/content/browser/media/ohos/native_web_contents_observer.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "content/public/browser/message_port_provider.h"
#endif

#if BUILDFLAG(ARKWEB_I18N)
#include "base/ohos/locale_utils.h"
#include "ui/base/ui_base_switches.h"
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
#include "content/browser/renderer_host/render_process_host_impl.h"
#endif

#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
#include "content/browser/ohos/date_time_chooser_ohos.h"
#endif  // ARKWEB_CSS_INPUT_TIME

#include "base/trace_event/optional_trace_event.h"
#include "components/subresource_filter/content/browser/ohos_adblock_config.h"
#include "content/browser/browser_main_loop.h"
#include "content/browser/media/media_web_contents_observer.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/wake_lock/wake_lock_context_host.h"
#include "content/browser/web_contents/web_contents_view.h"
#include "content/public/browser/web_contents_delegate.h"

namespace content {

WebContentsImplExt::WebContentsImplExt(BrowserContext* browser_context)
    : WebContentsImpl(browser_context) {}

#if BUILDFLAG(ARKWEB_WEBRTC)
void WebContentsImplExt::StartCamera(int nWebID) {
  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
    return;
  }

  auto videoCaptureManager = media_stream_manager->video_capture_manager();
  if (!videoCaptureManager) {
    LOG(ERROR) << "videoCaptureManager null";
    return;
  }
  videoCaptureManager->StartCamera(nWebID);
}

void WebContentsImplExt::StopCamera(int nWebID) {
  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
    return;
  }

  auto videoCaptureManager = media_stream_manager->video_capture_manager();
  if (!videoCaptureManager) {
    LOG(ERROR) << "videoCaptureManager null";
    return;
  }
  videoCaptureManager->StopCamera(nWebID);
}

void WebContentsImplExt::CloseCamera(int nWebID) {
  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
    return;
  }

  auto videoCaptureManager = media_stream_manager->video_capture_manager();
  if (!videoCaptureManager) {
    LOG(ERROR) << "videoCaptureManager null";
    return;
  }
  videoCaptureManager->CloseCamera(nWebID);
}

int WebContentsImplExt::GetNWebId() {
  return nWebID_;
}

void WebContentsImplExt::SetNWebId(int nWebID) {
  nWebID_ = nWebID;
}
#endif  // defined(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
void WebContentsImplExt::AddMediaPlayerAudibleCount() {
  ++media_player_audible_count_;
}

void WebContentsImplExt::DelMediaPlayerAudibleCount() {
  if (media_player_audible_count_ > 0) {
    --media_player_audible_count_;
  }
}

bool WebContentsImplExt::GetMediaPlayerCurrentAudible() {
  return media_player_audible_count_ > 0;
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(IS_ARKWEB)
void WebContentsImplExt::CreateWebMessagePorts(
    std::vector<blink::WebMessagePort>& ports) {
  std::pair<blink::WebMessagePort, blink::WebMessagePort> pipe =
      blink::WebMessagePort::CreatePair();
  ports.emplace_back(std::move(pipe.first));
  ports.emplace_back(std::move(pipe.second));
}

void WebContentsImplExt::PostWebMessage(
    std::string& message,
    std::vector<blink::WebMessagePort>& ports,
    std::string& targetUri) {
  if (targetUri.compare("*") == 0) {
    targetUri = "";
  }
  // send ports to html5.
  MessagePortProvider::OhosPostMessageToFrame(
      GetPrimaryPage(), std::u16string(), base::UTF8ToUTF16(targetUri),
      base::UTF8ToUTF16(message), ports);
}
#endif

#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
void WebContentsImplExt::OpenDateTimeChooser() {
  observers_.NotifyObservers(&WebContentsObserver::OpenDateTimeChooser);
}

void WebContentsImplExt::CloseDateTimeChooser() {
  observers_.NotifyObservers(&WebContentsObserver::CloseDateTimeChooser);
}
#endif  // ARKWEB_CSS_INPUT_TIME

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
void WebContentsImplExt::SetForceEnableZoom(bool forceEnableZoom) {
  if (force_enable_zoom_ != forceEnableZoom) {
    force_enable_zoom_ = forceEnableZoom;
    OnWebPreferencesChanged();
  }
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void WebContentsImplExt::EnableAdsBlock(bool enable) {
  LOG(INFO) << "enable adblock: " << enable;

  blink::RendererPreferences* prefs = GetMutableRendererPrefs();
  if (prefs != NULL && prefs->is_global_adblock_enabled != enable) {
    prefs->is_global_adblock_enabled = enable;
    SyncRendererPrefs();
  }
}

bool WebContentsImplExt::IsAdsBlockEnabled() {
  blink::RendererPreferences* prefs = GetMutableRendererPrefs();
  if (prefs == NULL) {
    return false;
  }
  return prefs->is_global_adblock_enabled;
}

bool WebContentsImplExt::IsAdsBlockEnabledForCurPage() {
  return GetAdblockEnabledForSite();
}

void WebContentsImplExt::TrigAdBlockEnabledForSiteFromUi(
    const std::string& main_frame_url) {
  // add FrameTreeNode info
  RenderFrameHostImpl* rfh = GetPrimaryMainFrame();
  if (!rfh) {
    LOG(ERROR) << "[AdBlock] main render frame null";
    return;
  }

  int main_frame_tree_node_id = rfh->GetFrameTreeNodeId().value();
  if (!delegate_) {
    return;
  }

  if (!(delegate_->TrigAdBlockEnabledForSiteFromUi(main_frame_url,
                                                   main_frame_tree_node_id))) {
    LOG(INFO) << "[AdBlock] trigger adblock switch with rule list";
    bool enable =
        OHOS::adblock::AdBlockConfig::GetInstance()->IsAdblockEnabledForUrl(
            GURL(main_frame_url));
    SetAdBlockEnabledForSite(enable && IsAdsBlockEnabled(),
                             main_frame_tree_node_id);
  }
}

void WebContentsImplExt::OnAdsBlocked(
    const std::string& main_frame_url,
    const std::map<std::string, int32_t>& subresource_blocked,
    bool is_site_first_report) {
  LOG(DEBUG) << "[AdBlock] subresource_blocked.size():"
             << subresource_blocked.size();

  if (delegate_) {
    delegate_->OnAdsBlocked(main_frame_url, subresource_blocked,
                            is_site_first_report);
  }
}

void WebContentsImplExt::UpdateAdBlockEnabledToRender(
    bool site_adblock_enabled) {
  RenderFrameHostImpl* pending_main_frame =
      GetRenderManager()->speculative_frame_host();

  if (pending_main_frame) {
    pending_main_frame->UpdateAdBlockEnabledToRender(site_adblock_enabled);
  } else {
    RenderFrameHostImpl* rfh = GetPrimaryMainFrame();
    if (rfh) {
      rfh->UpdateAdBlockEnabledToRender(site_adblock_enabled);
    }
  }
}

bool WebContentsImplExt::GetAdblockEnabledForSite() {
  RenderFrameHost* rfh = GetPrimaryMainFrame();
  if (rfh == nullptr) {
    return false;
  }
  content::FrameTreeNode* node =
      static_cast<content::RenderFrameHostImpl*>(rfh)->frame_tree_node();
  if (node == nullptr) {
    return false;
  }
  return node->is_adblock_enabled();
}

void WebContentsImplExt::SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                                  int main_frame_tree_node_id) {
  content::RenderFrameHost* rfh = GetPrimaryMainFrame();
  if (!rfh) {
    LOG(ERROR) << "[AdBlock] SetAdBlockEnabledForSite main frame null";
    return;
  }
  content::FrameTreeNode* node =
      static_cast<content::RenderFrameHostImpl*>(rfh)->frame_tree_node();

  if (!node) {
    LOG(ERROR) << "[AdBlock] SetAdBlockEnabledForSite frame tree node null";
    return;
  }

  // check FrameTreeNode id
  if (node->frame_tree_node_id().value() != main_frame_tree_node_id) {
    LOG(INFO) << "[AdBlock] Not set adblock enabled enabled "
                 "switch owing to frame tree node id changed";
    return;
  }
  LOG(INFO) << "[Adblock] set adblock switch for site, frame tree node id:"
            << node->frame_tree_node_id()
            << "  adblock switch from UI: " << is_adblock_enabled;
  node->set_adblock_enabled(is_adblock_enabled);

  // send to render
  UpdateAdBlockEnabledToRender(is_adblock_enabled);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void WebContentsImplExt::NotifyContextMenuWillShow() {
  SetShowingContextMenu(false);
}

void WebContentsImplExt::ShowFreeCopyMenu() {
  auto* input_handler = GetFocusedFrameWidgetInputHandler();
  if (!input_handler) {
    return;
  }
  input_handler->ShowFreeCopyMenu();
}

#endif  // BUILDFLAG(ARKWEB_EXT_FREE_COPY)

RenderFrameHost* WebContentsImplExt::GetTargetFramesIncludingPending(
    int routing_id) {
  std::vector<RenderFrameHost*> frame_hosts;
  for (FrameTreeNode* node : primary_frame_tree_.Nodes()) {
    frame_hosts.push_back(node->current_frame_host());
    RenderFrameHostImpl* pending_frame_host =
        node->render_manager()->speculative_frame_host();
    if (pending_frame_host) {
      frame_hosts.push_back(pending_frame_host);
    }
  }

  for (RenderFrameHost* rfh : frame_hosts) {
    if (!rfh || !rfh->IsRenderFrameLive()) {
      continue;
    }
    if (routing_id == rfh->GetRoutingID()) {
      return rfh;
    }
  }

  return nullptr;
}

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
void WebContentsImplExt::SetHtmlPlayEnabled(bool enabled) {
  observers_.NotifyObservers(&WebContentsObserver::SetHtmlPlayEnabled, enabled);
  is_enabled_HTML_play_ = enabled;
}

bool WebContentsImplExt::IsHtmlPlayEnabled() {
  return is_enabled_HTML_play_;
}
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void WebContentsImplExt::CreateNativeBridgeHostForRenderFrameHost(
    RenderFrameHostImpl* frame_host,
    mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeHost> receiver) {
  native_web_contents_observer()->BindNativeBridgeHost(
      frame_host->GetGlobalId(), std::move(receiver));
}

void WebContentsImplExt::OnNativeEmbedStatusUpdate(
    const NativeEmbedInfo& native_embed_info,
    NativeEmbedInfo::TagState state) {
  bool print_log = true;
  if (native_embed_rect_info_map_.count(native_embed_info.embed_element_id)) {
    gfx::Rect history_rect =
        native_embed_rect_info_map_[native_embed_info.embed_element_id];
    if (history_rect.size() == native_embed_info.rect.size()) {
      print_log = false;
    }
    native_embed_rect_info_map_[native_embed_info.embed_element_id] =
        native_embed_info.rect;
  } else {
    native_embed_rect_info_map_.insert(std::make_pair(
        native_embed_info.embed_element_id, native_embed_info.rect));
  }
  if (print_log) {
    std::string param_list;
    for (auto& item : native_embed_info.params) {
      param_list += item.first + " ";
      param_list += item.second + ", ";
    }
    LOG(INFO) << "[NativeEmbed] OnNativeEmbedStatusUpdate " << " state is "
              << (int)state << ", " << native_embed_info
              << ", params: " << param_list;
  }
  if (delegate_) {
    delegate_->OnNativeEmbedStatusUpdate(native_embed_info, state);
  }
}

void WebContentsImplExt::OnNativeEmbedFirstFramePaint(
    int32_t native_embed_id,
    const std::string& embed_id_attribute) {
  if (!delegate_) {
    LOG(WARNING) << "OnNativeEmbedFirstFramePaint failed, no delegate";
    return;
  }
  delegate_->OnNativeEmbedFirstFramePaint(native_embed_id, embed_id_attribute);
}

void WebContentsImplExt::OnLayerRectVisibilityChange(
    const std::string& embed_id,
    bool visibility) {
  if (delegate_) {
    delegate_->OnLayerRectVisibilityChange(embed_id, visibility);
  }
}

void WebContentsImplExt::OnRenderFrameHostEnterBackForwardCache(
    const GlobalRenderFrameHostId& id) {
  if (native_web_contents_observer_) {
    native_web_contents_observer_->OnRenderFrameHostEnterBackForwardCache(id);
  }
}

void WebContentsImplExt::OnRenderFrameHostLeaveBackForwardCache(
    const GlobalRenderFrameHostId& id) {
  if (native_web_contents_observer_) {
    native_web_contents_observer_->OnRenderFrameHostLeaveBackForwardCache(id);
  }
}
#endif

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
void WebContentsImplExt::SetWakeLockHandler(int32_t windowId,
                                            const SetKeepScreenOn& handler) {
  if (!wake_lock_context_host_) {
    wake_lock_context_host_ = std::make_unique<WakeLockContextHost>(this);
  }
  wake_lock_context_host_->SetWakeLockHandler(windowId, std::move(handler));
}
#endif  // BUILDFLAG(ARKWEB_SCREEN_LOCK)

#if BUILDFLAG(ARKWEB_MENU)
void WebContentsImplExt::MouseSelectMenuShow(bool show) {
  if (render_view_host_delegate_view_) {
    render_view_host_delegate_view_->MouseSelectMenuShow(show);
  }
}

void WebContentsImplExt::ChangeVisibilityOfQuickMenu() {
  if (render_view_host_delegate_view_) {
    render_view_host_delegate_view_->ChangeVisibilityOfQuickMenu();
  }
}
#endif

#if BUILDFLAG(ARKWEB_AI)
bool WebContentsImplExt::CloseImageOverlaySelection() {
  if (render_view_host_delegate_view_) {
    return render_view_host_delegate_view_->CloseImageOverlaySelection();
  }
  return false;
}
#endif  // BUILDFLAG(ARKWEB_AI)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void WebContentsImplExt::WebExtensionUpdateTabUrl(int32_t tab_id,
                                                  const GURL& url) {
  OPTIONAL_TRACE_EVENT2("content", "WebContentsImpl::WebExtensionUpdateTabUrl",
                        "tab_id", tab_id, "url", url);
  if (delegate_) {
    delegate_->WebExtensionUpdateTabUrl(tab_id, url);
  }
}

int32_t WebContentsImplExt::ExtensionGetTabId() const {
  if (delegate_) {
    return delegate_->ExtensionGetTabId();
  }
  return -1;
}
#endif

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
void WebContentsImplExt::OnFormEditingStateChanged(uint64_t form_id,
                                                   bool did_submit) {
  observers_.NotifyObservers(&WebContentsObserver::OnFormEditingStateChanged,
                             !did_submit, form_id);
}
#endif

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
void WebContentsImplExt::MediaPlayerGone(
    const WebContentsObserver::MediaPlayerInfo& media_info,
    const MediaPlayerId& id) {
  observers_.NotifyObservers(&WebContentsObserver::MediaPlayerGone, media_info,
                             id);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
void WebContentsImplExt::UpdateBrowserControlsHeight(int height, bool animate) {
  if (view_) {
    view_->UpdateBrowserControlsHeight(height, animate);
  }
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void WebContentsImplExt::ClearContextMenu() {
  if (delegate_) {
    delegate_->ClearContextMenu();
  }
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
void WebContentsImplExt::EnableSafeBrowsingDetection(bool enable,
                                                     bool strictMode) {
  if (is_safe_browsing_enabled_ != enable) {
    LOG(INFO) << "EnableSafeBrowsingDetection enable " << enable;
    is_safe_browsing_enabled_ = enable;
  }
  if (safe_browsing_strict_mode_ != strictMode) {
    LOG(INFO) << "EnableSafeBrowsingDetection strictMode " << strictMode;
    safe_browsing_strict_mode_ = strictMode;
  }
}
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
std::unique_ptr<CustomMediaPlayer> WebContentsImplExt::CreateCustomMediaPlayer(
    std::unique_ptr<CustomMediaPlayerListener> listener,
    const MediaInfo& media_info) {
  if (delegate_) {
    return delegate_->CreateCustomMediaPlayer(std::move(listener), media_info);
  } else {
    LOG(WARNING) << "CreateCustomMediaPlayer failed, no delegate_";

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(WARNING) << "CreateCustomMediaPlayer failed, no delegate_";
#endif  // ARKWEB_LOGGER_REPORT
  }
  return nullptr;
}

void WebContentsImplExt::AddCustomMediaPlayer(const MediaPlayerId& player_id,
                                              CustomMediaPlayer* player) {
  players_[player_id] = player;
}

void WebContentsImplExt::RemoveCustomMediaPlayer(const MediaPlayerId& player_id,
                                                 CustomMediaPlayer* player) {
  auto iter = players_.find(player_id);
  if (iter == players_.end()) {
    LOG(WARNING) << "RemoveCustomMediaPlayer failed";

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(WARNING) << "RemoveCustomMediaPlayer failed";
#endif  // ARKWEB_LOGGER_REPORT

    return;
  }
  DCHECK(iter->second == player);
  players_.erase(iter);
}

void WebContentsImplExt::UpdateLayerRect(const MediaPlayerId& player_id,
                                         const gfx::Rect& rect) {
  auto iter = players_.find(player_id);
  if (iter == players_.end()) {
    return;
  }
  iter->second->UpdateLayerRect(rect.x(), rect.y(), rect.width(),
                                rect.height());
}

void WebContentsImplExt::FullScreenChanged(const MediaPlayerId& player_id,
                                           bool is_fullscreen) {
  DVLOG(1) << "FullScreenChanged(" << is_fullscreen << ")";
  auto iter = players_.find(player_id);
  if (iter == players_.end()) {
    LOG(WARNING) << "FullScreenChanged failed, no player found";

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(WARNING) << "FullScreenChanged failed, no player found";
#endif  // ARKWEB_LOGGER_REPORT

    return;
  }
  if (is_fullscreen) {
    iter->second->EnterFullscreen();
  } else {
    iter->second->ExitFullscreen();
  }
}

void WebContentsImplExt::RequestEnterFullscreen(
    const MediaPlayerId& player_id) {
  media_web_contents_observer()->RequestEnterFullscreen(player_id);
}

void WebContentsImplExt::RequestExitFullscreen(const MediaPlayerId& player_id) {
  media_web_contents_observer()->RequestExitFullscreen(player_id);
}
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
const std::string& WebContentsImplExt::SharedRenderProcessToken() {
  return shared_render_process_token_;
}
#endif

}  // namespace content
