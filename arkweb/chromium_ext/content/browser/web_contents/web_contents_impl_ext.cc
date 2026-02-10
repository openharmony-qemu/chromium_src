// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file

#include "arkweb/chromium_ext/content/browser/web_contents/web_contents_impl_ext.h"

#include "content/public/common/content_switches.h"

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

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#include "content/browser/media/video_assistant/video_assistant.h"
#include "content/public/browser/media_player_controller.h"
#include "content/public/browser/media_player_listener.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#endif  // ARKWEB_VIDEO_ASSISTANT

#include "base/trace_event/optional_trace_event.h"
#include "components/subresource_filter/content/browser/ohos_adblock_config.h"
#include "content/browser/browser_main_loop.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "content/browser/media/media_web_contents_observer.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/wake_lock/wake_lock_context_host.h"
#include "content/browser/web_contents/web_contents_view.h"
#include "content/public/browser/web_contents_delegate.h"

#if BUILDFLAG(ARKWEB_WEBRTC)
#include "arkweb/chromium_ext/media/audio/ohos/ohos_audio_input_stream.h"
#include "content/browser/renderer_host/media/audio_input_device_manager.h"
#include "media/audio/audio_manager.h"
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "base/strings/string_util.h"
#endif

namespace content {

// LCOV_EXCL_START
WebContentsImplExt::WebContentsImplExt(BrowserContext* browser_context)
    : WebContentsImpl(browser_context) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  video_assistant_ = std::make_unique<VideoAssistant>();
#endif  // ARKWEB_VIDEO_ASSISTANT
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  native_web_contents_observer_ =
      std::make_unique<NativeWebContentsObserver>(this);
#endif
}

WebContentsImplExt::~WebContentsImplExt() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  DelAllVideoSurfaces();
#endif  // ARKWEB_VIDEO_ASSISTANT
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  native_embed_rect_info_map_.clear();
#endif
}
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_WEBRTC)
void WebContentsImplExt::StartCamera(int nWebID) {
  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "media_stream_manager null";
#endif
    return;
  }

  auto videoCaptureManager = media_stream_manager->video_capture_manager();
  if (!videoCaptureManager) {
    LOG(ERROR) << "videoCaptureManager null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "videoCaptureManager null";
#endif
    return;
  }
  videoCaptureManager->AsVideoCaptureManagerExt()->StartCamera(nWebID);
}

void WebContentsImplExt::StopCamera(int nWebID) {
  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "media_stream_manager null";
#endif
    return;
  }

  auto videoCaptureManager = media_stream_manager->video_capture_manager();
  if (!videoCaptureManager) {
    LOG(ERROR) << "videoCaptureManager null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "videoCaptureManager null";
#endif
    return;
  }
  videoCaptureManager->AsVideoCaptureManagerExt()->StopCamera(nWebID);
}

void WebContentsImplExt::CloseCamera(int nWebID) {
  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "media_stream_manager null";
#endif
    return;
  }

  auto videoCaptureManager = media_stream_manager->video_capture_manager();
  if (!videoCaptureManager) {
    LOG(ERROR) << "videoCaptureManager null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "videoCaptureManager null";
#endif
    return;
  }
  videoCaptureManager->AsVideoCaptureManagerExt()->CloseCamera(nWebID);
}

#if BUILDFLAG(ARKWEB_WEBRTC)
void WebContentsImplExt::OnCameraCaptureStateChanged(int original_state,
                                              int new_state) {
  if (delegate_) {
    delegate_->OnCameraCaptureStateChanged(original_state, new_state);
  }
}
#endif

#if BUILDFLAG(ARKWEB_WEBRTC)
void WebContentsImplExt::ResumeMicrophone(int nWebID) {
  if (nWebID < 0) {
    LOG(ERROR) << "nWebID < 0.";
    return;
  }
  auto audio_manager = BrowserMainLoop::GetInstance()->audio_manager();
  if (!audio_manager) {
    LOG(ERROR) << "audio_manager null";
    return;
  }
  auto input_stream = audio_manager->GetInputStream();
  if (!input_stream.size()) {
    return;
  }
  for (auto it = input_stream.begin(); it != input_stream.end(); it++) {
    if (media::OHOSAudioInputStream::GetNWebId((*it)->GetAudioParameters()) ==
        nWebID) {
      (*it)->ResumeMicrophone();
    }
  }
}

void WebContentsImplExt::StopMicrophone(int nWebID) {
  if (nWebID < 0) {
    LOG(ERROR) << "nWebID < 0.";
    return;
  }

  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
    return;
  }

  if (media_stream_manager->AsMediaStreamManagerExt()) {
    media_stream_manager->AsMediaStreamManagerExt()->CloseAudioCapture(nWebID);
  }
}

void WebContentsImplExt::PauseMicrophone(int nWebID) {
  if (nWebID < 0) {
    LOG(ERROR) << "nWebID < 0.";
    return;
  }
  auto audio_manager = BrowserMainLoop::GetInstance()->audio_manager();
  if (!audio_manager) {
    LOG(ERROR) << "audio_manager null.";
    return;
  }
  auto input_stream = audio_manager->GetInputStream();
  if (!input_stream.size()) {
    LOG(ERROR) << "input_stream is empty.";
    return;
  }
  for (auto it = input_stream.begin(); it != input_stream.end(); it++) {
    if (media::OHOSAudioInputStream::GetNWebId((*it)->GetAudioParameters()) ==
        nWebID) {
      (*it)->PauseMicrophone();
    }
  }
}

void WebContentsImplExt::OnMicrophoneCaptureStateChanged(int original_state,
                                                         int new_state) {
  if (delegate_) {
    delegate_->OnMicrophoneCaptureStateChanged(original_state, new_state);
  }
}
#endif

// LCOV_EXCL_START
int WebContentsImplExt::GetNWebId() {
  return nWebID_;
}

void WebContentsImplExt::SetNWebId(int nWebID) {
  nWebID_ = nWebID;
}
// LCOV_EXCL_STOP
#endif  // defined(ARKWEB_WEBRTC)

// LCOV_EXCL_START
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

bool WebContentsImplExt::OnAudioStateChangedExt(bool is_currently_audible, bool is_ohos_currently_audible) {
  if (is_ohos_currently_audible != is_ohos_currently_audible_ 
      && is_currently_audible == is_currently_audible_) {
    is_ohos_currently_audible_ = is_ohos_currently_audible;
    observers_.NotifyObservers(&WebContentsObserver::OnAudioStateChanged,
                               is_ohos_currently_audible_);
    LOG(INFO) << "WebContentsImplExt::OnAudioStateChangedExt is_ohos_currently_audible: " << is_ohos_currently_audible_;
    return true;
  }
  return false;
}

void WebContentsImplExt::OnAudioStateChangedExtSetAudible(bool is_ohos_currently_audible) {
  is_ohos_currently_audible_ = is_ohos_currently_audible;
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
// LCOV_EXCL_STOP

#if BUILDFLAG(IS_ARKWEB)
void WebContentsImplExt::CreateWebMessagePorts(
    std::vector<blink::WebMessagePort>& ports) {
  std::pair<blink::WebMessagePort, blink::WebMessagePort> pipe =
      blink::WebMessagePort::CreatePair();
  ports.emplace_back(std::move(pipe.first));
  ports.emplace_back(std::move(pipe.second));
}

// LCOV_EXCL_START
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
// LCOV_EXCL_STOP
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
void WebContentsImplExt::OpenDateTimeChooser() {
  observers_.NotifyObservers(&WebContentsObserver::OpenDateTimeChooser);
}

void WebContentsImplExt::CloseDateTimeChooser() {
  observers_.NotifyObservers(&WebContentsObserver::CloseDateTimeChooser);
}
#endif  // ARKWEB_CSS_INPUT_TIME

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
void WebContentsImplExt::SetForceEnableZoom(bool forceEnableZoom) {
  if (force_enable_zoom_ != forceEnableZoom) {
    force_enable_zoom_ = forceEnableZoom;
    OnWebPreferencesChanged();
  }
}
#endif
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_ADBLOCK)
void WebContentsImplExt::EnableAdsBlock(bool enable) {
  LOG(INFO) << "enable adblock: " << enable;

  blink::RendererPreferences* prefs = GetMutableRendererPrefs();
  if (prefs != NULL && prefs->is_global_adblock_enabled != enable) {
    prefs->is_global_adblock_enabled = enable;
    SyncRendererPrefs();
  }
}

// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
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

#if BUILDFLAG(ARKWEB_AI)
void WebContentsImplExt::OnDataDetectorSelectText() {
  auto* input_handler = GetFocusedFrameWidgetInputHandler();
  if (!input_handler) {
    return;
  }
  input_handler->OnDataDetectorSelectText();
}
#endif  // BUILDFLAG(ARKWEB_AI)
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
void WebContentsImplExt::SetHtmlPlayEnabled(bool enabled) {
  observers_.NotifyObservers(&WebContentsObserver::SetHtmlPlayEnabled, enabled);
  is_enabled_HTML_play_ = enabled;
}

bool WebContentsImplExt::IsHtmlPlayEnabled() {
  return is_enabled_HTML_play_;
}
#endif
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_SAME_LAYER)
// LCOV_EXCL_START
void WebContentsImplExt::CreateNativeBridgeHostForRenderFrameHost(
    RenderFrameHostImpl* frame_host,
    mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeHost> receiver) {
  native_web_contents_observer()->BindNativeBridgeHost(
      frame_host->GetGlobalId(), std::move(receiver));
}
// LCOV_EXCL_STOP

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
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO) << "[NativeEmbed] OnNativeEmbedStatusUpdate "
                       << " state is " << (int)state << ", "
                       << native_embed_info << ", params:" << param_list;
#endif
  }
  if (delegate_) {
    delegate_->OnNativeEmbedStatusUpdate(native_embed_info, state);
  }
}

// LCOV_EXCL_START
void WebContentsImplExt::OnNativeEmbedFirstFramePaint(
    int32_t native_embed_id,
    const std::string& embed_id_attribute) {
  if (!delegate_) {
    LOG(WARNING) << "OnNativeEmbedFirstFramePaint failed, no delegate";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(WARNING) << "OnNativeEmbedFirstFramePaint failed, no delegate";
#endif
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

void WebContentsImplExt::OnNativeEmbedObjectParamChange(
    const NativeEmbedParamDataInfo& native_param_info) {
  if (delegate_) {
    delegate_->OnNativeEmbedObjectParamChange(native_param_info);
  }
}
// LCOV_EXCL_STOP
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

// LCOV_EXCL_START
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

bool WebContentsImplExt::IsQuickMenuShow() {
  if (render_view_host_delegate_view_) {
    render_view_host_delegate_view_->IsQuickMenuShow();
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_AI)
void WebContentsImplExt::CloseImageOverlaySelection() {
  if (render_view_host_delegate_view_) {
    render_view_host_delegate_view_->CloseImageOverlaySelection();
  }
}

void WebContentsImplExt::OnOverlayZoomChanged() {
  LOG(DEBUG) << "WebContentsImplExt::OnOverlayZoomChanged";
  if (render_view_host_delegate_view_) {
    render_view_host_delegate_view_->OnOverlayZoomChanged();
  }
}
#endif  // BUILDFLAG(ARKWEB_AI)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
int32_t WebContentsImplExt::ExtensionGetTabId() {
  if (delegate_) {
    return delegate_->ExtensionGetTabId();
  }
  return -1;
}
#endif // ARKWEB_ARKWEB_EXTENSIONS

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
  is_safe_browsing_config_ = true;
  if (is_safe_browsing_enabled_ != enable) {
    LOG(INFO) << "EnableSafeBrowsingDetection enable " << enable;
    is_safe_browsing_enabled_ = enable;
  }
  if (safe_browsing_strict_mode_ != strictMode) {
    LOG(INFO) << "EnableSafeBrowsingDetection strictMode " << strictMode;
    safe_browsing_strict_mode_ = strictMode;
  }
}

bool WebContentsImplExt::IsSafeBrowsingDetectionConfig() {
  return is_safe_browsing_config_;
}

bool WebContentsImplExt::IsSafeBrowsingDetectionStrict() {
  return safe_browsing_strict_mode_;
}

bool WebContentsImplExt::IsSafeBrowsingDetectionDisabled() {
  return !is_safe_browsing_enabled_;
}
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
void WebContentsImplExt::RequestEnterFullscreen(
    const MediaPlayerId& player_id) {
  media_web_contents_observer()->RequestEnterFullscreen(player_id);
}

void WebContentsImplExt::RequestExitFullscreen(const MediaPlayerId& player_id) {
  media_web_contents_observer()->RequestExitFullscreen(player_id);
}
// LCOV_EXCL_STOP
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
const std::string& WebContentsImplExt::SharedRenderProcessToken() {
  return shared_render_process_token_;
}
#endif

void WebContentsImplExt::SetDelegate(WebContentsDelegate* delegate) {
  WebContentsImpl::SetDelegate(delegate);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (delegate_) {
    video_assistant_ = delegate_->CreateVideoAssistant();
  }
  if (!video_assistant_) {
    video_assistant_ = std::make_unique<VideoAssistant>();
  }
#endif  // ARKWEB_VIDEO_ASSISTANT
}
// LCOV_EXCL_STOP

void WebContentsImplExt::MediaDestroyed(const MediaPlayerId& id) {
  WebContentsImpl::MediaDestroyed(id);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (video_assistant_) {
    video_assistant_->OnVideoDestroyed(id);
  }

  auto iter = surface_widget_map_.find(id);
  if (iter != surface_widget_map_.end()) {
    // destroy native window
    DelVideoSurface(iter->second);
    surface_widget_map_.erase(iter);
  }
#endif  // ARKWEB_VIDEO_ASSISTANT
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
// LCOV_EXCL_START
void WebContentsImplExt::EnableVideoAssistant(bool enable) {
  if (video_assistant_->Enabled() == enable) {
    return;
  }
  video_assistant_->EnableVideoAssistant(enable);
  OnWebPreferencesChanged();
}

void WebContentsImplExt::ExecuteVideoAssistantFunction(
    const std::string& cmdId) {
  video_assistant_->ExecuteVideoAssistantFunction(cmdId);
}

void WebContentsImplExt::OnShowToast(double duration,
                                     const std::string& toast) {
  if (!delegate_) {
    LOG(ERROR) << "delegate is nullptr when notify to show toast";
    return;
  }

  delegate_->OnShowToast(duration, toast);
}

void WebContentsImplExt::OnReportStatisticLog(const std::string& content) {
  if (!delegate_) {
    LOG(ERROR) << "delegate is nullptr when notify to report statistic log";
    return;
  }

  delegate_->OnReportStatisticLog(content);
}

void WebContentsImplExt::CustomWebMediaPlayer(bool enable) {
  LOG(INFO) << "WebContentsImplExt::CustomWebMediaPlayer enter. enable = "
            << enable;
  if (custom_media_player_enabled_ == enable) {
    return;
  }
  custom_media_player_enabled_ = enable;
  OnWebPreferencesChanged();
}

void WebContentsImplExt::PopluateVideoAssistantConfig(
    media::mojom::VideoAssistantConfigPtr& config) {
  if (delegate_) {
    delegate_->PopluateVideoAssistantConfig(
        GetLastCommittedURL().DeprecatedGetOriginAsURL().spec(), config);
    video_assistant_->UpdateVideoAssistantConfig(config);
  }
}

void WebContentsImplExt::OnVideoPlaying(
    media::mojom::VideoAttributesForVASTPtr video_attributes,
    const MediaPlayerId& id) {
  LOG(INFO) << "OhMedia, OnVideoPlaying media_player_id:" << id.delegate_id;
  media_player_id_ = id;
  video_assistant_->OnVideoPlaying(std::move(video_attributes), id);
}

void WebContentsImplExt::OnUpdateVideoAttributes(
    media::mojom::VideoAttributesForVASTPtr video_attributes,
    const MediaPlayerId& id) {
  video_assistant_->OnUpdateVideoAttributes(std::move(video_attributes), id);
}

void WebContentsImplExt::OnVideoDestroyed(const MediaPlayerId& id) {
  video_assistant_->OnVideoDestroyed(id);
}

std::unique_ptr<MediaPlayerListener>
WebContentsImplExt::OnFullScreenOverlayEnter(
    media::mojom::MediaInfoForVASTPtr media_info,
    const MediaPlayerId& media_player_id) {
  if (!delegate_) {
    return nullptr;
  }
  return delegate_->OnFullScreenOverlayEnter(std::move(media_info),
                                             media_player_id);
}
// LCOV_EXCL_STOP

void WebContentsImplExt::SetVideoSurface(const MediaPlayerId& id,
                                         int32_t surface_widget) {
  auto [iter, success] = surface_widget_map_.insert({id, surface_widget});
  if (success) {
    return;
  }

  // destroy old native window
  DelVideoSurface(iter->second);
  iter->second = surface_widget;
}

void WebContentsImplExt::DelVideoSurface(int32_t surface_id) {
  void* native_window =
      NWebNativeWindowTracker::Get()->GetNativeWindow(surface_id);
  NWebNativeWindowTracker::Get()->DestroyNativeWindow(surface_id);
  OHOS::NWeb::OhosAdapterHelperExt::GetWindowAdapterNdkInstance()
      .DestroyNativeWindow(native_window);

  content::GpuProcessHost* host = content::GpuProcessHost::Get();
  if ((host != nullptr) && (host->gpu_host() != nullptr)) {
    host->gpu_host()->DestroyNativeWindow(surface_id);
  }
}

// LCOV_EXCL_START
void WebContentsImplExt::DelAllVideoSurfaces() {
  for (auto iter = surface_widget_map_.begin();
       iter != surface_widget_map_.end();) {
    DelVideoSurface(iter->second);
    surface_widget_map_.erase(iter++);
  }
}

void WebContentsImplExt::DelVideoAssistant() {
  if (media_player_id_.has_value()) {
    LOG(INFO) << "OhMedia, DelVideoAssistant media_player_id:"
              << media_player_id_.value().delegate_id;
    video_assistant_->OnVideoDestroyed(media_player_id_.value());
  }
}

void WebContentsImplExt::ReportVideoDecoderName(
    const std::string& decoder_name) {
  video_assistant_->ReportVideoDecoderName(decoder_name);
}
// LCOV_EXCL_STOP
#endif  // ARKWEB_VIDEO_ASSISTANT

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_DATALIST)
void WebContentsImplExt::ShowAutofillPopup(
    const gfx::RectF& element_bounds,
    bool is_rtl,
    const std::vector<autofill::Suggestion>& suggestions,
    bool is_password_popup_type) {
  if (delegate_) {
    delegate_->OnShowAutofillPopup(element_bounds, is_rtl, suggestions,
                                   is_password_popup_type);
  }
}
void WebContentsImplExt::HideAutofillPopup() {
  // notify ui to dismiss hideAutofillPopup
  if (delegate_) {
    delegate_->OnHideAutofillPopup();
  }
}
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
std::shared_ptr<OHOS::NWeb::NWebVaultPlainTextCallback> WebContentsImplExt::GetVaultPlainTextCallback() {
  return vault_plain_text_callback_;
}
void WebContentsImplExt::SetVaultPlainTextCallback(
    std::shared_ptr<OHOS::NWeb::NWebVaultPlainTextCallback> callback) {
  vault_plain_text_callback_ = callback;
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void WebContentsImplExt::OnShareFile(const std::string& filePath,
                                     const std::string& utdTypeId) {
  if (delegate_) {
    delegate_->OnShareFile(filePath, utdTypeId);
  }
}
#endif
#if BUILDFLAG(ARKWEB_CLIPBOARD)
void WebContentsImplExt::CollapseAllFramesSelection() {
  for (FrameTreeNode* node : primary_frame_tree_.Nodes()) {
    RenderFrameHostImpl* rfh = node->current_frame_host();
    if (!rfh) {
      continue;
    }
    if (!rfh->IsRenderFrameLive()) {
      continue;
    }
    RenderWidgetHostImpl* render_widget_host = rfh->GetRenderWidgetHost();
    if (!render_widget_host) {
      continue;
    }
    auto* input_handler = render_widget_host->GetFrameWidgetInputHandler();
    if (!input_handler) {
      continue;
    }
    input_handler->CollapseSelection();
  }
}
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)
#if BUILDFLAG(ARKWEB_MENU)
void WebContentsImplExt::SelectRangeV2(const gfx::Point& position,
                                       bool is_base) {
  OPTIONAL_TRACE_EVENT0("content", "WebContentsImplExt::SelectRangeV2");
  auto* input_handler = GetFocusedFrameWidgetInputHandler();
  if (!input_handler) {
    return;
  }

  input_handler->SelectRangeV2(position, is_base);
}
#endif
#if BUILDFLAG(ARKWEB_USERAGENT)
void WebContentsImplExt::SetCustomUA(std::string custom_user_agent) {
  custom_user_agent_ = custom_user_agent;
}

std::string WebContentsImplExt::GetCustomUA() {
  return custom_user_agent_;
}

bool WebContentsImplExt::isSameUserAgent(
    const blink::UserAgentOverride& ua_override) {
  if (GetUserAgentOverride() == ua_override) {
    return true;
  }

  if (!ua_override.ua_string_override.empty() &&
      !net::HttpUtil::IsValidHeaderValue(ua_override.ua_string_override)) {
    return true;
  }
  if (GetUserAgentOverride().ua_string_override.empty()) {
    return true;
  }

  return false;
}

void WebContentsImplExt::SetUserAgentMetadata(
    const std::string& user_agent,
    const blink::UserAgentMetadata& metadata) {
  user_agent_for_metadata_map_[user_agent] = metadata;
}

const blink::UserAgentMetadata WebContentsImplExt::GetUserAgentMetadata(
    const std::string& user_agent) {
  auto it = user_agent_for_metadata_map_.find(user_agent);
  if (it != user_agent_for_metadata_map_.end()) {
    return it->second;
  }
#if !defined(COMPONENT_BUILD)
  if (user_agent.empty()) {
    return embedder_support::GetUserAgentMetadata(true);
  }
  return embedder_support::GetUserAgentMetadata(
      !base::Contains(user_agent, embedder_support::GetUserAgent()));
#else
  return blink::UserAgentMetadata();
#endif
}

#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
void WebContentsImplExt::OneShotMediaPlayerStopped() {
  observers_.NotifyObservers(&WebContentsObserver::OneShotMediaPlayerStopped);
}
#endif
#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
void WebContentsImplExt::OnBeforeUnloadFired(bool proceed) {
  if (delegate_) {
    delegate_->OnBeforeUnloadFired(proceed);
  }
}
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD
#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
void WebContentsImplExt::StopScreenCapture(int32_t nweb_id,
                                           const std::string& session_id) {
  if (!BrowserMainLoop::GetInstance()) {
    LOG(ERROR) << "BrowserMainLoop null";
    return;
  }
  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
    return;
  }
  media_stream_manager->AsMediaStreamManagerExt()->StopScreenCapture(
      nweb_id, session_id);
}

void WebContentsImplExt::SetScreenCapturePickerShow() {
  if (!BrowserMainLoop::GetInstance()) {
    LOG(ERROR) << "BrowserMainLoop null";
    return;
  }
  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
    return;
  }
  media_stream_manager->AsMediaStreamManagerExt()->SetScreenCapturePickerShow();
}

void WebContentsImplExt::DisableSessionReuse() {
  if (!BrowserMainLoop::GetInstance()) {
    LOG(ERROR) << "BrowserMainLoop null";
    return;
  }
  auto media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  if (!media_stream_manager) {
    LOG(ERROR) << "media_stream_manager null";
    return;
  }
  media_stream_manager->AsMediaStreamManagerExt()->DisableSessionReuse();
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)
// LCOV_EXCL_STOP

void WebContentsImplExt::EnterFullscreenMode(
    RenderFrameHostImpl* requesting_frame,
    const blink::mojom::FullscreenOptions& options) {
  WebContentsImpl::EnterFullscreenMode(requesting_frame, options);

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExTopControls)) {
    AsWebContentsImplExt()->controls_state_current_fullscreen_ =
        cc::BrowserControlsState::kBoth;
    if (auto* view = GetRenderWidgetHostView()) {
      int top_controls_offset =
          static_cast<RenderWidgetHostViewBase*>(view)->GetTopControlsOffset();
      AsWebContentsImplExt()->controls_state_current_fullscreen_ =
          top_controls_offset < 0 ? cc::BrowserControlsState::kHidden
                                  : cc::BrowserControlsState::kShown;
    }
    AsWebContentsImplExt()->controls_state_fullscreen_ =
        AsWebContentsImplExt()->browser_controls_state_;
    UpdateBrowserControlsState(cc::BrowserControlsState::kHidden,
                               cc::BrowserControlsState::kHidden, false,
                               std::nullopt);
  }
#endif
}

// LCOV_EXCL_START
void WebContentsImplExt::ExitFullscreenMode(bool will_cause_resize) {
  WebContentsImpl::ExitFullscreenMode(will_cause_resize);
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExTopControls)) {
    UpdateBrowserControlsState(
        AsWebContentsImplExt()->controls_state_fullscreen_,
        AsWebContentsImplExt()->controls_state_current_fullscreen_, false,
        std::nullopt);
  }
#endif
}
void WebContentsImplExt::RenderViewReady(RenderViewHost* rvh) {
  WebContentsImpl::RenderViewReady(rvh);
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExTopControls)) {
    UpdateBrowserControlsState(AsWebContentsImplExt()->browser_controls_state_,
                               cc::BrowserControlsState::kShown, false,
                               std::nullopt);
  }
#endif
}
void WebContentsImplExt::DidFinishNavigation(
    NavigationHandle* navigation_handle) {
  WebContentsImpl::DidFinishNavigation(navigation_handle);
#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  if (navigation_handle->IsInPrimaryMainFrame() && view_) {
    view_->DidStopRefresh();
  }
#endif
}
void WebContentsImplExt::RenderWidgetCreated(
    RenderWidgetHostImpl* render_widget_host) {
  WebContentsImpl::RenderWidgetCreated(render_widget_host);
  observers_.NotifyObservers(&WebContentsObserver::RenderWidgetCreated,
                             render_widget_host);
}
RenderViewHostImpl* WebContentsImplExt::GetRenderViewHost() {
#if BUILDFLAG(ARKWEB_BUGFIX_CRASH)
  if (GetRenderManager() && GetRenderManager()->current_frame_host()) {
    return GetRenderManager()->current_frame_host()->render_view_host();
  }

  LOG(WARNING) << "GetRenderViewHost is nullptr";
  return nullptr;
#else
  return WebContentsImpl::GetRenderViewHost();
#endif
}
void WebContentsImplExt::UpdateBrowserControlsState(
    cc::BrowserControlsState constraints,
    cc::BrowserControlsState current,
    bool animate,
    const std::optional<cc::BrowserControlsOffsetTagsInfo>& offset_tags_info) {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  AsWebContentsImplExt()->browser_controls_state_ = constraints;
#endif
  WebContentsImpl::UpdateBrowserControlsState(constraints, current, animate,
                                              offset_tags_info);
}

#if BUILDFLAG(ARKWEB_PIP)
MediaPlayerId WebContentsImplExt::GetMediaPlayerId(int delegate_id,
                                                   int child_id,
                                                   int frame_routing_id,
                                                   bool& status) {
  return media_web_contents_observer_->GetMediaPlayerId(
      delegate_id, child_id, frame_routing_id, status);
}

void WebContentsImplExt::OnPip(int status,
                               int delegate_id,
                               int child_id,
                               int frame_routing_id,
                               int width,
                               int height) {
  OPTIONAL_TRACE_EVENT0("content", "WebContentsImpl::OnPip");
  if (delegate_) {
    delegate_->OnPip(status, delegate_id, child_id, frame_routing_id, width,
                     height);
    if (status == PIP_STATE_ENTER || status == PIP_STATE_HLS_ENTER) {
      pip_status_ = true;
    }
  }
}

void WebContentsImplExt::OnPipEvent(int event) {
  LOG(INFO) << __func__ << " Pip event:" << event;
  if (delegate_) {
    if ((event == PIP_STATE_EXIT || event == PIP_STATE_HLS_EXIT) &&
        !pip_status_) {
      return;
    }
    delegate_->OnPipEvent(event);
    if ((event == PIP_STATE_EXIT || event == PIP_STATE_HLS_EXIT) &&
        pip_status_) {
      pip_status_ = false;
    }
  }
}

void WebContentsImplExt::SetUpdateSurface(bool state) {
  pip_update_surface_ = state;
}

bool WebContentsImplExt::IsUpdateSurface() {
  return pip_update_surface_;
}
#endif

#if BUILDFLAG(ARKWEB_PDF)
void WebContentsImplExt::OnPdfScrollAtBottom(const std::string& url) {
  // Ensure that PdfScrollAtBottom event propagate from innermost to outermost
  // WebContents.
  if (GetOuterWebContents() && GetOuterWebContents()->AsWebContentsImplExt()) {
    GetOuterWebContents()->AsWebContentsImplExt()->OnPdfScrollAtBottom(url);
    return;
  }
  if (delegate_) {
    delegate_->OnPdfScrollAtBottom(url);
  }
}

void WebContentsImplExt::OnPdfLoadEvent(int32_t result,
                                        const std::string& url) {
  // Ensure that PdfLoad event propagate from innermost to outermost
  // WebContents.
  if (GetOuterWebContents() && GetOuterWebContents()->AsWebContentsImplExt()) {
    GetOuterWebContents()->AsWebContentsImplExt()->OnPdfLoadEvent(result, url);
    return;
  }
  if (delegate_) {
    delegate_->OnPdfLoadEvent(result, url);
  }
}

void WebContentsImplExt::ProcessForPdfType(NavigationHandle* navigation_handle) {
  if (!navigation_handle) {
    LOG(ERROR) << "navigation_handle is null";
    return;
  }

  std::string mime_type = GetContentsMimeType();
  RenderFrameHostImpl* rfh = GetPrimaryMainFrame();
  if (!rfh) {
    LOG(ERROR) << "rfh is null";
    return;
  }
  bool is_pdf = base::EqualsCaseInsensitiveASCII(mime_type, "application/pdf");
  LOG_IF(INFO, is_pdf) << "content is pdf.";
  rfh->SetIsPDF(is_pdf);
}
#endif  // BUILDFLAG(ARKWEB_PDF)
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_BFCACHE)
void WebContentsImplExt::SetMediaResumeFromBFCachePage(bool resume) {
  LOG(INFO)
      << "WebContentsImplExt SetMediaResumeFromBFCachePage enter resume = "
      << resume;
  if (media_resume_from_bfcache_page_ == resume) {
    return;
  }
  LOG(INFO) << "WebContentsImplExt SetMediaResumeFromBFCachePage";
  media_resume_from_bfcache_page_ = resume;
  OnWebPreferencesChanged();
}
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_READER_MODE)
void WebContentsImplExt::OnIsPageDistillable(int page_type,
                                             const std::string& distillable_page_url,
                                             const std::string& title) {
  if (delegate_) {
    delegate_->OnIsPageDistillable(page_type, distillable_page_url, title);
  }
}

bool WebContentsImplExt::IsDistillerPageWebContents() {
  return SharedRenderProcessToken() == "0xAAAAAA";
}
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_BGTASK)
void WebContentsImplExt::OnBrowserForeground() {
  LOG(INFO) << "WebContentsImplExt::OnBrowserForeground";
  observers_.NotifyObservers(&WebContentsObserver::OnBrowserForeground);
}

void WebContentsImplExt::OnBrowserBackground() {
  LOG(INFO) << "WebContentsImplExt::OnBrowserBackground";
  observers_.NotifyObservers(&WebContentsObserver::OnBrowserBackground);
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
void WebContentsImplExt::DetectBlankScreen(const std::string& url) {
  RenderFrameHostImpl* render_frame_host = GetPrimaryMainFrame();
  if (!render_frame_host) {
    return;
  }
  render_frame_host->DetectBlankScreen(url);
}

void WebContentsImplExt::SetBlankScreenDetectionConfig(
    bool enable,
    const std::vector<double>& detectionTiming,
    const std::vector<int32_t>& detectionMethods,
    int32_t contentfulNodesCountThreshold) {
  RenderFrameHostImpl* render_frame_host = GetPrimaryMainFrame();
  if (!render_frame_host) {
    return;
  }
  render_frame_host->SetBlankScreenDetectionConfig(
      enable, detectionTiming, detectionMethods, contentfulNodesCountThreshold);
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
bool WebContentsImplExt::OnStartBackgroundTask(int32_t type,
                                               const std::string& message) {
  LOG(INFO) << "WebContentsImplExt::OnStartBackgroundTask, type=" << type
            << ", message=" << message.c_str();
  if (delegate_) {
    return delegate_->OnStartBackgroundTask(type, message);
  }
  return true;
}
#endif  // ARKWEB_PERFORMANCE_PERSISTENT_TASK

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void WebContentsImplExt::GetOverScrollOffset(float* offset_x, float* offset_y) {
  if (offset_x && offset_y) {
    *offset_x = over_scroll_offset_x_;
    *offset_y = over_scroll_offset_y_;
  }
}

void WebContentsImplExt::OnOverScrollOffsetChanged(float offset_x,
                                                   float offset_y) {
  over_scroll_offset_x_ = offset_x;
  over_scroll_offset_y_ = offset_y;
}
#endif


#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
std::string WebContentsImplExt::OnRewriteUrlForNavigation(const std::string& original_url,
                                                          const std::string& referrer,
                                                          int transition_type,
                                                          bool is_key_request) {
  if (delegate_) {
    return delegate_->OnRewriteUrlForNavigation(original_url, referrer, transition_type, is_key_request);
  } else {
    LOG(ERROR) << "WebContentsImplExt::OnRewriteUrlForNavigation delegate_ is nullptr";
    return "";
  }
}

std::string WebContentsImplExt::NotifyNavigationRewriteUrl(const std::string& original_url,
                                                           const std::string& referrer,
                                                           int transition_type,
                                                           bool is_key_request) {
  return OnRewriteUrlForNavigation(original_url, referrer, transition_type, is_key_request);
}
#endif

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
void WebContentsImplExt::OnDocumentEndReady(const FrameInfos& frameInfo) {
  if (delegate_) {
    delegate_->OnDocumentEndReady(frameInfo);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
void WebContentsImplExt::OnMediaCastEnter() {
  LOG(INFO) << "WebContentsImplExt::OnMediaCastEnter";
  if (delegate_) {
    delegate_->OnMediaCastEnter();
  }
}

void WebContentsImplExt::NotifyRemoteExitFullScreen() {
  LOG(INFO) << "WebContentsImplExt::NotifyRemoteExitFullScreen";
  if (media_web_contents_observer()) {
    media_web_contents_observer()->NotifyRemoteExitFullScreen();
  }  
}
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
void WebContentsImplExt::OnSafeBrowsingCheckDetail(int code,
                                                   int policy,
                                                   int threat) {
  if (delegate_) {
    delegate_->OnSafeBrowsingCheckDetail(code, policy, threat);
  }
}
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
void WebContentsImplExt::GetAllFrameInfos(
    std::map<std::string, std::string>& frameinfos) {
  LOG(DEBUG) << "WebContentsImplExt::GetAllFrameInfos.";
  //get all frameinfo
  ForEachFrameTree([&frameinfos](FrameTree& frame_tree) {
    for (FrameTreeNode* child : frame_tree.Nodes()) {
      std::string parent_string = "";
      std::string current_string = "";

      if (child->current_frame_host()) {
        auto currentId = child->current_frame_host()->GetGlobalId();
        current_string = std::to_string(currentId.child_id) + "_" + std::to_string(currentId.frame_routing_id);
      }
      if (child->parent()) {
        auto parentId = child->parent()->GetGlobalId();
        parent_string = std::to_string(parentId.child_id) + "_" + std::to_string(parentId.frame_routing_id);
      }

      LOG(DEBUG) << "WebContentsImplExt::GetAllFrameInfos current=" << current_string
                 << " parent=" << parent_string;
      frameinfos.insert(std::make_pair(current_string, parent_string));
    }
  });
}
#endif
}  // namespace content
