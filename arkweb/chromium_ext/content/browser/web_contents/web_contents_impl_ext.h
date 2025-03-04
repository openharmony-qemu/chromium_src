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
#endif  // BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)

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

class WebContentsImplExt : public WebContentsImpl {
 public:
  WebContentsImplExt(BrowserContext* browser_context);

  content::WebContentsImplExt* AsWebContentsImplExt() override { return this; };

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
  bool CloseImageOverlaySelection() override;
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
  bool is_safe_browsing_enabled_ = true;
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override;
  bool IsSafeBrowsingDetectionEnabled() override {
    return is_safe_browsing_enabled_;
  }
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
};
}  // namespace content

#endif
