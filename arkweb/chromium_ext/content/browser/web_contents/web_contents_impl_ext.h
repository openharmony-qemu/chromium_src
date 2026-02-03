// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IMPL_EXT_H_
#define CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IMPL_EXT_H_

#include "base/allocator/partition_allocator/src/partition_alloc/partition_alloc_buildflags.h"
#include "content/browser/web_contents/web_contents_impl.h"

#if BUILDFLAG(ARKWEB_WEBRTC)
#include "content/browser/renderer_host/media/media_stream_manager.h"
#include "content/browser/renderer_host/media/video_capture_manager.h"
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "content/public/browser/custom_media_info.h"
#include "content/public/browser/custom_media_player.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "content/browser/media/video_assistant/video_assistant.h"
#include "content/public/browser/media_player_controller.h"
#include "content/public/browser/media_player_listener.h"
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "libcef/browser/alloy/alloy_browser_ua_config.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
#include "arkweb/ohos_nweb/src/capi/nweb_safe_browsing_detection_result_item.h"
#endif

namespace content {

class WebContentsImpl;

#if BUILDFLAG(ARKWEB_SAME_LAYER)
class NativeWebContentsObserver;
#endif

#if BUILDFLAG(ARKWEB_WEBRTC)
class MediaStreamManager;
class VideoCaptureManager;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
class CustomMediaPlayerListener;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
enum class RendererIsUnresponsiveReason;
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
class MediaPlayerListener;
class VideoAssistant;
#endif  // ARKWEB_VIDEO_ASSISTANT

class WebContentsImplExt : public WebContentsImpl {
 public:
  WebContentsImplExt(BrowserContext* browser_context);
  ~WebContentsImplExt() override;

  content::WebContentsImplExt* AsWebContentsImplExt() override { return this; }

  base::WeakPtr<content::WebContentsImplExt> AsWebContentsImplExtWeakThis() override {
    return weak_factory_.GetWeakPtr();
  }
  // WebContents ------------------------------------------------------
  void SetDelegate(WebContentsDelegate* delegate) override;
  void MediaDestroyed(const MediaPlayerId& id);

#if BUILDFLAG(ARKWEB_WEBRTC)
  void StartCamera(int nWebID) override;
  void StopCamera(int nWebID) override;
  void CloseCamera(int nWebID) override;
  int GetNWebId() override;
  void SetNWebId(int nWebID) override;
  void OnCameraCaptureStateChanged(int original_state, int new_state) override;
  void ResumeMicrophone(int nWebID) override;
  void StopMicrophone(int nWebID) override;
  void PauseMicrophone(int nWebID) override;
  void OnMicrophoneCaptureStateChanged(int original_state,
                                       int new_state) override;
  int nWebID_ = 0;
#endif  // defined(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
  int media_player_audible_count_ = 0;
  bool is_ohos_currently_audible_ = false;
  void AddMediaPlayerAudibleCount();
  void DelMediaPlayerAudibleCount();
  bool GetMediaPlayerCurrentAudible();
  bool OnAudioStateChangedExt(bool is_currently_audible, bool is_ohos_currently_audible);
  void OnAudioStateChangedExtSetAudible(bool is_ohos_currently_audible);
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(IS_ARKWEB)
  void CreateWebMessagePorts(
      std::vector<blink::WebMessagePort>& ports) override;
  void PostWebMessage(std::string& message,
                      std::vector<blink::WebMessagePort>& ports,
                      std::string& targetUri) override;
#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
  void OpenDateTimeChooser() override;
  void CloseDateTimeChooser() override;
#endif  // #if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)

#endif

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
  bool force_enable_zoom_ = false;
  void SetForceEnableZoom(bool forceEnableZoom) override;
  bool GetForceEnableZoom() override { return force_enable_zoom_; }
#endif  // ARKWEB_EXT_FORCE_ZOOM

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void SetTouchInsertHandleMenuShow(bool show) override {
    touch_insert_handle_menu_show_ = show;
  }
  bool GetTouchInsertHandleMenuShow() override {
    return touch_insert_handle_menu_show_;
  }
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void TrigAdBlockEnabledForSiteFromUi(
      const std::string& main_frame_url) override;
  void OnAdsBlocked(const std::string& main_frame_url,
                    const std::map<std::string, int32_t>& subresource_blocked,
                    bool is_site_first_report) override;

  void EnableAdsBlock(bool enable) override;

  bool IsAdsBlockEnabled() override;

  bool IsAdsBlockEnabledForCurPage() override;

  void UpdateAdBlockEnabledToRender(bool site_adblock_enabled) override;

  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                int main_frame_tree_node_id) override;

  bool GetAdblockEnabledForSite() override;
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
  void GetAllFrameInfos(std::map<std::string, std::string>& frameinfos);
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void NotifyContextMenuWillShow() override;
  void ShowFreeCopyMenu() override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void OnDataDetectorSelectText() override;
#endif  // BUILDFLAG(ARKWEB_AI)

  RenderFrameHost* GetTargetFramesIncludingPending(int routing_id);

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  bool is_enabled_HTML_play_ = true;
  void SetHtmlPlayEnabled(bool enabled) override;
  bool IsHtmlPlayEnabled() override;
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void CreateNativeBridgeHostForRenderFrameHost(
      RenderFrameHostImpl* frame_host,
      mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeHost> receiver)
      override;
  void OnNativeEmbedStatusUpdate(const NativeEmbedInfo& native_embed_info,
                                 NativeEmbedInfo::TagState state) override;
  void OnNativeEmbedFirstFramePaint(int32_t native_embed_id,
                                    const std::string& embed_id_attribute);
  void OnLayerRectVisibilityChange(const std::string& embed_id,
                                   bool visibility);
  void OnRenderFrameHostEnterBackForwardCache(
      const GlobalRenderFrameHostId& id) override;
  void OnRenderFrameHostLeaveBackForwardCache(
      const GlobalRenderFrameHostId& id) override;
  void OnNativeEmbedObjectParamChange(
      const NativeEmbedParamDataInfo& native_param_info) override;
#endif

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
  void SetWakeLockHandler(int32_t windowId,
                          const SetKeepScreenOn& handler) override;
#endif  // BUILDFLAG(ARKWEB_SCREEN_LOCK)

#if BUILDFLAG(ARKWEB_MENU)
  void MouseSelectMenuShow(bool show) override;
  void ChangeVisibilityOfQuickMenu() override;
  bool IsQuickMenuShow() override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void CloseImageOverlaySelection() override;
  void OnOverlayZoomChanged();
#endif  // BUILDFLAG(ARKWEB_AI)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  int32_t ExtensionGetTabId() override;
#endif

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
  void OnFormEditingStateChanged(uint64_t form_id, bool did_submit) override;
#endif

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
  void MediaPlayerGone(const WebContentsObserver::MediaPlayerInfo& media_info,
                       const MediaPlayerId& id);
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  void UpdateBrowserControlsHeight(int height, bool animate) override;
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void ClearContextMenu() override;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  bool is_safe_browsing_config_ = false;
  bool is_safe_browsing_enabled_ = true;
  GURL safe_browsing_check_url_;
  int safe_browsing_check_code_ = 0;
  int safe_browsing_check_threat_type_ = 0;
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override;
  bool IsSafeBrowsingDetectionConfig() override;
  bool IsSafeBrowsingDetectionStrict() override;
  bool IsSafeBrowsingDetectionDisabled() override;
  void SetSafeBrowsingCheckDetail(int code, int threat_type, const GURL& url);
  void GetSafeBrowsingCheckDetail(int& code, int& threat_type, GURL& url) const;
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  std::map<MediaPlayerId, raw_ptr<CustomMediaPlayer>> players_;

  std::unique_ptr<CustomMediaPlayer> CreateCustomMediaPlayer(
      std::unique_ptr<CustomMediaPlayerListener> listenter,
      const MediaInfo& media_info);

  void AddCustomMediaPlayer(const MediaPlayerId& player_id,
                            CustomMediaPlayer* player);
  void RemoveCustomMediaPlayer(const MediaPlayerId& player_id,
                               CustomMediaPlayer* player);
  void UpdateLayerRect(const MediaPlayerId& player_id, const gfx::Rect& rect);
  void FullScreenChanged(const MediaPlayerId& player_id, bool is_fullscreen);

  void RequestEnterFullscreen(const MediaPlayerId& player_id);
  void RequestExitFullscreen(const MediaPlayerId& player_id);
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  const std::string& SharedRenderProcessToken() override;
#endif

#if BUILDFLAG(ARKWEB_DATALIST)
  void ShowAutofillPopup(const gfx::RectF& element_bounds,
                         bool is_rtl,
                         const std::vector<autofill::Suggestion>& suggestions,
                         bool is_password_popup_type) override;
  void HideAutofillPopup() override;
#endif // BUILDFLAG(ARKWEB_DATALIST)

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  std::shared_ptr<OHOS::NWeb::NWebVaultPlainTextCallback> GetVaultPlainTextCallback() override;
  void SetVaultPlainTextCallback(
      std::shared_ptr<OHOS::NWeb::NWebVaultPlainTextCallback> callback) override;
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  bool is_selectable_ = false;
  void SetShouldShowFreeCopyMenu(bool is_selectable) {
    is_selectable_ = is_selectable;
  }
  bool ShouldShowFreeCopyMenu() override { return is_selectable_; }
#endif
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void OnShareFile(const std::string& filePath,
                   const std::string& utdTypeId) override;
#endif
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void CollapseAllFramesSelection() override;
#endif
#if BUILDFLAG(ARKWEB_MENU)
  void SelectRangeV2(const gfx::Point& position, bool is_base) override;
#endif
#if BUILDFLAG(ARKWEB_USERAGENT)
  void SetCustomUA(std::string custom_user_agent) override;
  std::string GetCustomUA() override;
  bool isSameUserAgent(const blink::UserAgentOverride& ua_override);
  void SetUserAgentMetadata(const std::string& user_agent,
                            const blink::UserAgentMetadata& metadata);
  const blink::UserAgentMetadata GetUserAgentMetadata(
      const std::string& user_agent);
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  void OneShotMediaPlayerStopped() override;
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  NativeWebContentsObserver* native_web_contents_observer() {
    return native_web_contents_observer_.get();
  }
#endif
#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  void OnBeforeUnloadFired(bool proceed) override;
#endif // ARKWEB_DISATCH_BEFORE_UNLOAD
#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  void StopScreenCapture(int32_t nweb_id, const std::string& session_id) override;
  void SetScreenCapturePickerShow() override;
  void DisableSessionReuse() override;
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)
  void EnterFullscreenMode(
      RenderFrameHostImpl* requesting_frame,
      const blink::mojom::FullscreenOptions& options) override;
  void ExitFullscreenMode(bool will_cause_resize) override;
  void RenderViewReady(RenderViewHost* render_view_host) override;
  void DidFinishNavigation(NavigationHandle* navigation_handle) override;
  void RenderWidgetCreated(RenderWidgetHostImpl* render_widget_host) override;
  RenderViewHostImpl* GetRenderViewHost() override;
  void UpdateBrowserControlsState(
    cc::BrowserControlsState constraints,
    cc::BrowserControlsState current,
    bool animate,
    const std::optional<cc::BrowserControlsOffsetTagsInfo>& offset_tags_info)
    override;
#if BUILDFLAG(ARKWEB_PIP)
  MediaPlayerId GetMediaPlayerId(int delegate_id,
                                 int child_id,
                                 int frame_routing_id,
                                 bool& status);
  void OnPip(int status,
             int delegate_id,
             int child_id,
             int frame_routing_id,
             int width,
             int height);
  void OnPipEvent(int event) override;
  void SetUpdateSurface(bool state);
  bool IsUpdateSurface();
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  bool OnStartBackgroundTask(int32_t type, const std::string& message) override;
#endif  // ARKWEB_PERFORMANCE_PERSISTENT_TASK

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  void SetBlankScreenDetectionConfig(
      bool enable,
      const std::vector<double>& detectionTiming,
      const std::vector<int32_t>& detectionMethods,
      int32_t contentfulNodesCountThreshold) override;
  void DetectBlankScreen(const std::string& url);
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  void OnIsPageDistillable(int page_type,
                           const std::string& distillable_page_url,
                           const std::string& title) override;
  bool IsDistillerPageWebContents();
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  std::string OnRewriteUrlForNavigation(const std::string& original_url,
                                        const std::string& referrer,
                                        int transition_type,
                                        bool is_key_request) override;
  std::string NotifyNavigationRewriteUrl(const std::string& original_url,
                                         const std::string& referrer,
                                         int transition_type,
                                         bool is_key_request) override;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  void OnMediaCastEnter();
  void NotifyRemoteExitFullScreen() override;
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
friend class WebContentsImpl;
friend class WebContentsImplUtils;
private:
  std::unique_ptr<VideoAssistant> video_assistant_;
  bool custom_media_player_enabled_ = false;
  std::map<MediaPlayerId, int32_t> surface_widget_map_;
  std::optional<MediaPlayerId> media_player_id_;
#if BUILDFLAG(ARKWEB_TEST)
  friend class WebContentsImplExtTest;
#endif  // ARKWEB_TEST
 public:
  void EnableVideoAssistant(bool enable) override;
  void ExecuteVideoAssistantFunction(const std::string& cmdId) override;
  void OnShowToast(double duration, const std::string& toast);
  void OnShowVideoAssistant(const std::string& videoAssistantItems);
  void OnReportStatisticLog(const std::string& content);
  void CustomWebMediaPlayer(bool enable) override;
  void PopluateVideoAssistantConfig(
      media::mojom::VideoAssistantConfigPtr& config);
  void OnVideoPlaying(media::mojom::VideoAttributesForVASTPtr video_attributes,
                      const MediaPlayerId& id);
  void OnUpdateVideoAttributes(
      media::mojom::VideoAttributesForVASTPtr video_attributes,
      const MediaPlayerId& id);
  void OnVideoDestroyed(const MediaPlayerId& id);
  std::unique_ptr<MediaPlayerListener> OnFullScreenOverlayEnter(
      media::mojom::MediaInfoForVASTPtr media_info,
      const MediaPlayerId& media_player_id);

  void SetVideoSurface(const MediaPlayerId& id, int32_t surface_widget);
  void DelVideoSurface(int32_t surface_id);
  void DelAllVideoSurfaces();
  void DelVideoAssistant();
  void ReportVideoDecoderName(const std::string& decoder_name);
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_FILE_UPLOAD)
  bool IsActiveFileChooser() override {
    return active_file_chooser_ != nullptr;
  }
  void SetFileChooserInActive() override { active_file_chooser_ = nullptr; }
#endif  // BUILDFLAG(ARKWEB_FILE_UPLOAD)

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void OnOverScrollOffsetChanged(float offset_x, float offset_y);
  void GetOverScrollOffset(float* offset_x, float* offset_y) override;
#endif
#if BUILDFLAG(ARKWEB_PDF)
  void OnPdfScrollAtBottom(const std::string& url) override;
  void OnPdfLoadEvent(int32_t result, const std::string& url) override;
  void ProcessForPdfType(NavigationHandle* navigation_handle);
#endif  // BUILDFLAG(ARKWEB_PDF)
#if BUILDFLAG(ARKWEB_BFCACHE)
  void SetMediaResumeFromBFCachePage(bool resume) override;
  bool media_resume_from_bfcache_page_ = true;
#endif // BUILDFLAG(ARKWEB_BFCACHE)
#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override;
  void OnBrowserBackground() override;
#endif

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  void OnDocumentEndReady(const FrameInfos& frameInfo) override;
#endif

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  void OnSafeBrowsingCheckDetail(int code, int policy, int threat) override;
#endif

private:
#if BUILDFLAG(ARKWEB_TEST)
  friend class WebContentsImplUtilsTest;
#endif
  std::string custom_user_agent_;
#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  std::shared_ptr<OHOS::NWeb::NWebVaultPlainTextCallback> vault_plain_text_callback_ = nullptr;
#endif
#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  bool safe_browsing_strict_mode_ = false;
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  float over_scroll_offset_x_ = 0.0f;
  float over_scroll_offset_y_ = 0.0f;
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  std::unique_ptr<NativeWebContentsObserver> native_web_contents_observer_;
  std::map<std::string,gfx::Rect> native_embed_rect_info_map_;
#endif
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  bool touch_insert_handle_menu_show_ = false;
#endif
#if BUILDFLAG(ARKWEB_USERAGENT) || BUILDFLAG(ARKWEB_EXT_UA)
  using UserAgentForMetadataMap =
      base::flat_map<std::string, blink::UserAgentMetadata>;
  std::string user_agent_{""};
  UserAgentForMetadataMap user_agent_for_metadata_map_;
#endif  // ARKWEB_EXT_UA
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  cc::BrowserControlsState browser_controls_state_ =
      cc::BrowserControlsState::kBoth;
  cc::BrowserControlsState controls_state_fullscreen_ =
      cc::BrowserControlsState::kBoth;
  cc::BrowserControlsState controls_state_current_fullscreen_ =
      cc::BrowserControlsState::kBoth;
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  std::string shared_render_process_token_;
#endif
#if BUILDFLAG(ARKWEB_PIP)
  bool pip_update_surface_ = false;
  bool pip_status_ = false;
#endif

#if BUILDFLAG(ARKWEB_TEST)
 public:
  void SetPrimaryMainFrame(RenderFrameHostImpl* impl) { test_impl_ = impl; }
  RenderFrameHostImpl* GetPrimaryMainFrame() override {
    if (test_impl_) {
      return test_impl_;
    }
    return WebContentsImpl::GetPrimaryMainFrame();
  }

  void SetTestFlag(bool flag) { flag_ = flag; }
  blink::RendererPreferences* GetMutableRendererPrefs() override {
    if (flag_) {
      return nullptr;
    }
    return WebContentsImpl::GetMutableRendererPrefs();
  }

  void SetRenderManagerForTesting(RenderFrameHostManager* manager) {
    test_manager_ = manager;
  }
  RenderFrameHostManager* GetRenderManager() override {
    if (test_manager_) {
      return test_manager_;
    }
    return WebContentsImpl::GetRenderManager();
  }

 private:
  RenderFrameHostImpl* test_impl_ = nullptr;
  RenderFrameHostManager* test_manager_ = nullptr;
  bool flag_ = false;
#endif  // ARKWEB_TEST
  base::WeakPtrFactory<WebContentsImplExt> weak_factory_{this};
};
}  // namespace content

#endif
