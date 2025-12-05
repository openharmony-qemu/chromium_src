// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_ALLOY_ALLOY_BROWSER_HOST_IMPL_EXT_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_ALLOY_ALLOY_BROWSER_HOST_IMPL_EXT_H_
#pragma once

#include "cef/libcef/browser/alloy/alloy_browser_host_impl.h"

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "chrome/browser/extensions/extension_keybinding_registry.h"
#endif
class AlloyBrowserHostImplExt : public AlloyBrowserHostImpl
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
                                , public extensions::ExtensionKeybindingRegistry::Delegate
#endif
{
public:
  AlloyBrowserHostImplExt(
      const CefBrowserSettings& settings,
      CefRefPtr<CefClient> client,
      content::WebContents* web_contents,
      scoped_refptr<CefBrowserInfo> browser_info,
      CefRefPtr<AlloyBrowserHostImpl> opener,
      CefRefPtr<CefRequestContextImpl> request_context,
      std::unique_ptr<CefBrowserPlatformDelegate> platform_delegate);

  CefRefPtr<AlloyBrowserHostImplExt> AsAlloyBrowserHostImplExt() override { return this; }

#if BUILDFLAG(ARKWEB_DEVTOOLS)
  void ShowDevToolsWith(
      CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
      CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate,
      const CefPoint& inspect_element_at) override;
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void WasOccluded(bool occluded) override;
  void SetEnableLowerFrameRate(bool enabled) override;
#endif

#if BUILDFLAG(IS_ARKWEB)
  void GetRootBrowserAccessibilityManager(void** manager) override;
  void SetWakeLockHandler(int32_t windowId, CefRefPtr<CefSetLockCallback> callback) override;
#endif

#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
  bool IsIframe() override;
  void ReloadFocusedFrame() override;
#endif

#if BUILDFLAG(ARKWEB_PDF)
  void OnPdfScrollAtBottom(const std::string& url) override;
  void OnPdfLoadEvent(int32_t result, const std::string& url) override;
#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void OnNativeEmbedStatusUpdate(
      const content::NativeEmbedInfo& native_embed_info,
      content::NativeEmbedInfo::TagState state) override;
  void OnNativeEmbedFirstFramePaint(
      int32_t native_embed_id,
      const std::string& embed_id_attribute) override;
  void OnLayerRectVisibilityChange(
    const std::string& embed_id,
    bool visibility) override;
  void SetEnableCustomVideoPlayer(bool flag) override;
  void OnNativeEmbedObjectParamChange(
      const content::NativeEmbedParamDataInfo& native_param_info) override;
#endif

#if BUILDFLAG(ARKWEB_FOCUS)
  void ActivateContents(content::WebContents* contents) override;
#endif  // BUILDFLAG(ARKWEB_FOCUS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetNativeEmbedMode(bool flag) override;
  void SetNativeInnerWeb(bool isInnerWeb) override;
#endif

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
  void OnFormEditingStateChanged(bool state, uint64_t form_id) override;
  void MediaStartedPlaying(const content::WebContentsObserver::MediaPlayerInfo& video_type,
                           const content::MediaPlayerId& id) override;
  void MediaStoppedPlaying(
    const content::WebContentsObserver::MediaPlayerInfo& video_type,
    const content::MediaPlayerId& id,
    content::WebContentsObserver::MediaStoppedReason reason) override;
  void MediaPlayerGone(
    const content::WebContentsObserver::MediaPlayerInfo& video_type,
    const content::MediaPlayerId& id) override;
#endif

#if BUILDFLAG(ARKWEB_PRINT)
  void SetToken(void* token) override;
  void CreateWebPrintDocumentAdapter(const CefString& jobName,
                                     void** webPrintDocumentAdapter) override;
  void CreateWebPrintDocumentAdapterV2(const CefString& jobName,
                                       void** adapter) override;
  void SetPrintBackground(bool enable) override;
  bool GetPrintBackground() override;
 #endif

#if BUILDFLAG(ARKWEB_DISCARD)
  bool Discard() override;
  bool Restore() override;
#endif


#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void OnShowToast(double duration, const std::string& toast) override;
  void OnShowVideoAssistant(const std::string& videoAssistantItems) override;
  void OnReportStatisticLog(const std::string& content) override;
#endif  // BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  // Shows the repost form confirmation dialog box.
  void ShowRepostFormWarningDialog(content::WebContents* source) override;
#endif // ARKWEB_NETWORK_BASE

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void OnAdsBlocked(const std::string& main_frame_url,
                    const std::map<std::string, int32_t>& subresource_blocked,
                    bool is_site_first_report) override;

  bool TrigAdBlockEnabledForSiteFromUi(const std::string& main_frame_url,
                                       int main_frame_tree_node_id) override;
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  bool WebExtensionCheck(
      const std::string functionName,
      content::BrowserContext*& browser_context,
      content::WebContents*& web_contents);

  void WebExtensionRegisterZoomObserver() override;

  void WebExtensionUnregisterZoomObserver() override;

  void WebExtensionTabUpdated(
      int tab_id,
      std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
      std::unique_ptr<NWebExtensionTab> tab) override;

  void ExtensionSetTabId(int tab_id) override;
  int ExtensionGetTabId() override;

  void WebExtensionTabRemoved(int tab_id,
    bool isWindowClosing, int windowId) override;

  void WebExtensionTabAttached(int tab_id,
    int new_position, int new_window_id) override;

  void WebExtensionTabDetached(int tab_id,
    const std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo) override;

  void WebExtensionTabHighlighted(NWebExtensionTabHighlightInfo& highlightInfo) override;

  void WebExtensionTabMoved(int tab_id, const std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) override;

  void WebExtensionTabReplaced(int32_t addedTabId, int32_t removedTabId) override;

  static void WebExtensionActionClicked(
      std::string extensionId,
      const NWebExtensionTab* tab);

  static void UpdatePinnedExtensionIds(
    content::BrowserContext* browser_context,
    std::string extensionId,
    bool isPinned);

  static void WebExtensionActionPinnedStateChanged(
      std::string extensionId, bool isPinned);

  static void WebExtensionActionShowPopup(int tabId, std::string extensionId);      

  content::BrowserContext* GetOriginalContext(content::BrowserContext* browser_context);

  void WebExtensionSetViewType(int32_t type) override;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void OnShareFile(const std::string& filePath, const std::string& utdTypeId) override;
  void NavigationStateChanged(content::WebContents* source,
                              content::InvalidateTypes changed_flags) override;

  bool ShouldAllowPartialParamMismatchOfPrerender2(
      content::NavigationHandle& navigation_handle) override;
  content::NavigationController::UserAgentOverrideOption
      ShouldOverrideUserAgentForPrerender2() override;
#endif

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  void OnBeforeUnloadFired(bool proceed) override;
#endif // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_DATALIST)
  void OnShowAutofillPopup(const gfx::RectF& element_bounds,
                           bool is_rtl,
                           const std::vector<autofill::Suggestion>& suggestions,
                           bool is_password_popup_type) override;
  void OnHideAutofillPopup() override;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void RequestPointerLock(content::WebContents* web_contents,
                          bool user_gesture,
                          bool last_unlocked_by_target) override;
  void LostPointerLock() override;
#endif


#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
  void OpenDateTimeChooser() override;
  void CloseDateTimeChooser() override;
#endif  // ARKWEB_CSS_INPUT_TIME


#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  std::unique_ptr<content::CustomMediaPlayer> CreateCustomMediaPlayer(
      std::unique_ptr<content::CustomMediaPlayerListener> listener,
      const content::MediaInfo& media_info) override;
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  void UpdateVSyncFrequency();
  void ResetVSyncFrequency();
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void SetTouchInsertHandleMenuShow(bool show) {
    web_contents()->SetTouchInsertHandleMenuShow(show);
  }

  bool GetTouchInsertHandleMenuShow() {
    return web_contents()->GetTouchInsertHandleMenuShow();
  }

  bool HandleContextMenu(content::RenderFrameHost& render_frame_host,
                         const content::ContextMenuParams& params) override;
#endif

#if BUILDFLAG(ARKWEB_HTML_SELECT)
  void ShowPopupMenu(
      mojo::PendingRemote<blink::mojom::PopupMenuClient> popup_client,
      const gfx::Rect& bounds,
      int item_height,
      double item_font_size,
      int selected_item,
      std::vector<blink::mojom::MenuItemPtr> menu_items,
      bool right_aligned,
      bool allow_multiple_selection);
#endif  // ARKWEB_HTML_SELECT

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  content::WebContents* GetWebContentsForExtension() override;
  void AcceleratorPressedUI(const ui::Accelerator& accelerator,
                            content::BrowserContext* browser_context);
  bool WebHandleKeyboardEvent(
    content::WebContents* source,
    const input::NativeWebKeyboardEvent& event);
#endif

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  void NotifyScreenInfoChangedV2() override;
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  void SetAudioResumeInterval(int resumeInterval) override;
  void SetAudioExclusive(bool audioExclusive) override;
  void SetAudioSessionType(int audioSessionType) override;
#endif // defined(OHOS_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  std::unique_ptr<content::MediaPlayerListener> OnFullScreenOverlayEnter(
      media::mojom::MediaInfoForVASTPtr media_info_ptr,
      const content::MediaPlayerId& media_player_id) override;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_READER_MODE)
  void OnIsPageDistillable(
      int page_type,
      const std::string& distillable_page_url,
      const std::string& title) override;
  bool IsForDistillerPage() override;
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  bool OnStartBackgroundTask(int32_t type, const std::string& message) override;
#endif  // ARKWEB_PERFORMANCE_PERSISTENT_TASK

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  std::string OnRewriteUrlForNavigation(const std::string& original_url, const std::string& referrer) override;
#endif

private:
  friend class AlloyBrowserHostImpl;
  friend class AlloyBrowserHostImplUtils;

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void WasKeyboardResized() override;
  void SetDrawRect(int x, int y, int width, int height) override;
  void SetDrawMode(int mode) override;
  bool GetPendingSizeStatus() override;
  void SetFitContentMode(int mode) override;
  void SetShouldFrameSubmissionBeforeDraw(bool should) override;
  std::string GetCurrentLanguage() override;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  std::unique_ptr<content::VideoAssistant> CreateVideoAssistant() override;
  void PopluateVideoAssistantConfig(
      const std::string& url,
      media::mojom::VideoAssistantConfigPtr& config) override;
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  bool has_video_playing_ = false;
  bool has_touch_event_ = false;
  bool set_lower_frame_rate_ = false;
  int video_stream_cnt_ = 0;
  static constexpr int WAIT_TOUCH_EVENT_DELAY_TIME = 3000/*ms*/;
#endif

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
  bool start_play_ = false;
#endif
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  int tab_id_ = -1;
#endif


#if BUILDFLAG(IS_OHOS)
  void OnWindowShow() override;
  void OnWindowHide() override;
  void OnOnlineRenderToForeground() override;
  void SetWindowId(int window_id, int nweb_id) override;
  void RenderViewReady() override;
#endif


#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void ReportWindowStatus(bool first_view_ready);
  bool is_hidden_ = false;
#endif

#if BUILDFLAG(IS_OHOS)
  void InactiveUnloadOldProcess(base::ProcessId pid);
  int nweb_id_ = -1;
  int window_id_ = -1;
  base::ProcessId last_pid_ = -1;
  void SetVisible(int32_t nweb_id, bool visible);
  static int32_t ltpo_strategy_;
#endif

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
  void OnDumpJavaScriptStackCallback(
      int pid,
      content::RendererIsUnresponsiveReason reason,
      const std::string& stack);
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  bool IsURLBlockedInIncognito(
      bool is_guest_view,
      const content::OpenURLParams& params);
#endif

};

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_ALLOY_ALLOY_BROWSER_HOST_IMPL_EXT_H_
