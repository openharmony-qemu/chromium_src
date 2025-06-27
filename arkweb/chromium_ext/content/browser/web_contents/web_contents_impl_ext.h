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

  // WebContents ------------------------------------------------------
  void SetDelegate(WebContentsDelegate* delegate) override;
  void MediaDestroyed(const MediaPlayerId& id);

#if BUILDFLAG(ARKWEB_WEBRTC)
  void StartCamera(int nWebID) override;
  void StopCamera(int nWebID) override;
  void CloseCamera(int nWebID) override;
  int GetNWebId() override;
  void SetNWebId(int nWebID) override;
  int nWebID_ = 0;
#endif  // defined(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
  int media_player_audible_count_ = 0;
  bool is_ohos_currently_audible_ = false;
  void AddMediaPlayerAudibleCount();
  void DelMediaPlayerAudibleCount();
  bool GetMediaPlayerCurrentAudible();
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

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
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

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void NotifyContextMenuWillShow() override;
  void ShowFreeCopyMenu() override;
#endif

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
#endif

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
  void SetWakeLockHandler(int32_t windowId,
                          const SetKeepScreenOn& handler) override;
#endif  // BUILDFLAG(ARKWEB_SCREEN_LOCK)

#if BUILDFLAG(ARKWEB_MENU)
  void MouseSelectMenuShow(bool show) override;
  void ChangeVisibilityOfQuickMenu() override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void CloseImageOverlaySelection() override;
  void OnOverlayZoomChanged();
#endif  // BUILDFLAG(ARKWEB_AI)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void WebExtensionUpdateTabUrl(int32_t tab_id, const GURL& url) override;
  int32_t ExtensionGetTabId() const override;
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
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override;
  bool IsSafeBrowsingDetectionConfig() override;
  bool IsSafeBrowsingDetectionStrict() override;
  bool IsSafeBrowsingDetectionDisabled() override;
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  std::map<MediaPlayerId, CustomMediaPlayer*> players_;

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
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  bool is_selectable_;
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
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void WebExtensionUpdateTab(
      int32_t tab_id,
      const NWebExtensionTabUpdateProperties* update_properties) override;
#endif
#if BUILDFLAG(ARKWEB_MENU)
  void SelectRangeV2(const gfx::Point& position, bool is_base) override;
#endif
#if BUILDFLAG(ARKWEB_USERAGENT)
  void SetCustomUA(std::string custom_user_agent) override;
  std::string GetCustomUA() override;
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
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
friend class WebContentsImpl;
friend class WebContentsImplUtils;
private:
  std::unique_ptr<VideoAssistant> video_assistant_;
  bool custom_media_player_enabled_ = false;
  std::map<MediaPlayerId, int32_t> surface_widget_map_;
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
  void ReportVideoDecoderName(const std::string& decoder_name);
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override;
  void OnBrowserBackground() override;
#endif

private:
  std::string custom_user_agent_;
#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  bool safe_browsing_strict_mode_ = false;
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  std::unique_ptr<NativeWebContentsObserver> native_web_contents_observer_;
  std::map<std::string,gfx::Rect> native_embed_rect_info_map_;
#endif
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  bool touch_insert_handle_menu_show_ = false;
#endif
#if BUILDFLAG(ARKWEB_USERAGENT) || BUILDFLAG(ARKWEB_EXT_UA)
  std::string user_agent_{""};
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
};
}  // namespace content

#endif
