/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_EXT_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_EXT_H_
#pragma once
 
#include "cef/libcef/browser/osr/browser_platform_delegate_osr.h"
#if BUILDFLAG(ARKWEB_DRAG_DROP)
#include "content/browser/web_contents/web_contents_impl.h"
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
#if BUILDFLAG(ARKWEB_PIP)
#include "base/timer/timer.h"
#endif  // BUILDFLAG(ARKWEB_PIP)

class CefBrowserPlatformDelegateOsrExt : public CefBrowserPlatformDelegateOsr
{
public:
  friend class CefBrowserPlatformDelegateOsr;
  friend class CefBrowserPlatformDelegateOsrUtils;
  CefBrowserPlatformDelegateOsrExt* AsCefBrowserPlatformDelegateOsrExt() override
  {
    return this;
  }
#if BUILDFLAG(ARKWEB_HTML_SELECT)
  void ShowPopupMenu(
      mojo::PendingRemote<blink::mojom::PopupMenuClient> popup_client,
      const gfx::Rect& bounds,
      double item_font_size,
      int selected_item,
      std::vector<blink::mojom::MenuItemPtr> menu_items,
      bool right_aligned,
      bool allow_multiple_selection) override;
#endif  // #if BUILDFLAG(ARKWEB_HTML_SELECT)
#if BUILDFLAG(ARKWEB_DATALIST)
  void OnShowAutofillPopup(const gfx::RectF& element_bounds,
                           bool is_rtl,
                           const std::vector<autofill::Suggestion>& suggestions,
                           bool is_password_popup_type) override;
  void OnHideAutofillPopup() override;
#endif
#if BUILDFLAG(ARKWEB_TOUCHPAD_FLING)
  void SendTouchpadFlingEvent(const CefMouseEvent& event,
                              double vx,
                              double vy) override;
#endif
#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  void NotifyScreenInfoChangedV2() override;
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void AdvanceFocusForIME(int focusType) override;
  void SendTouchEventList(
      const std::vector<CefTouchEvent>& event_list) override;
  void SetScrollable(bool enable) override;
  void ScrollFocusedEditableNodeIntoView() override;
  void ScaleGestureChangeV2(int type,
                            float scale,
                            float originScale,
                            float centerX,
                            float centerY) override;
  void ScrollBy(float delta_x, float delta_y) override;
  void UpdateSecurityLayer(bool isNeedSecurityLayer) override;
  void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) override;
  void SendMouseClickEvent(const CefMouseEvent& event,
                           CefBrowserHost::MouseButtonType type,
                           bool mouseUp,
                           int clickCount) override;
  void SendMouseMoveEvent(const CefMouseEvent& event, bool mouseLeave) override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition) override;
#endif
#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void WasOccluded(bool occluded) override;
#endif
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void WasKeyboardResized() override;
  void SetDrawMode(int mode) override;
  void SetFitContentMode(int mode) override;
  bool GetPendingSizeStatus() override;
  void SetDrawRect(int x, int y, int width, int height) override;
  void SetShouldFrameSubmissionBeforeDraw(bool should) override;
  std::string GetCurrentLanguage() override;
  int drawMode_ = 0;
  gfx::Rect drawRect_{0, 0, 0, 0};
  int fit_content_mode_ = 0;
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void OnNativeEmbedLifecycleChange(
      const ArkWebRenderHandlerExt::CefNativeEmbedData& info) override;
  void OnNativeEmbedFirstFramePaint(
      const content::NativeEmbedFirstPaintEvent& event) override;
  void SetNativeEmbedMode(bool flag) override;
  void OnNativeEmbedVisibilityChange(const std::string& embed_id,
                                     bool visibility) override;
  void SetNativeInnerWeb(bool isInnerWeb) override;
  void SetEnableCustomVideoPlayer(bool flag) override;
  void OnNativeEmbedObjectParamChange(
      const ArkWebRenderHandlerExt::CefNativeParamData& native_param_data) override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void OnTextSelected(bool flag) override;
  void OnDestroyImageAnalyzerOverlay() override;
  void OnFoldStatusChanged(uint32_t foldStatus) override;
  float GetPageScaleFactor() override;
  std::string GetDataDetectorSelectText() override;
  void OnDataDetectorSelectText() override;
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  void OnSafeInsetsChange(int left, int top, int right, int bottom) override;
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  int GetTopControlsOffset() override;
  int GetShrinkViewportHeight() override;
#endif
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  bool WebPageSnapshot(const char* id,
                       int width,
                       int height,
                       cef_web_snapshot_callback_t callback) override;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetVirtualKeyBoardArg(int32_t width,
                             int32_t height,
                             double keyboard) override;
  bool ShouldVirtualKeyboardOverlay() override;
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void MaximizeResize() override;
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  void OnBeforeUnloadFired(bool proceed) override;
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void OnShareFile(const std::string& file_path,
                   const std::string& utd_type_id) override;
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void OnAdsBlocked(const std::string& main_frame_url,
                    const std::map<std::string, int32_t>& subresource_blocked,
                    bool is_site_first_report) override;

  bool TrigAdBlockEnabledForSiteFromUi(const std::string& main_frame_url,
                                       int main_frame_tree_node_id) override;
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  bool GetCurRWH(content::WebContentsImpl* web_contents,
                 const gfx::PointF& client_pt,
                 gfx::PointF* transformed_pt);
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
#if BUILDFLAG(ARKWEB_PIP)
  void OnPip(int status,
             int delegate_id,
             int child_id,
             int frame_routing_id,
             int width,
             int height) override;
  void OnPipEvent(int event) override;
  void SetPipNativeWindow(int delegate_id,
                          int child_id,
                          int frame_routing_id,
                          cef_native_window_t window) override;
  void SendPipEvent(int delegate_id,
                    int child_id,
                    int frame_routing_id,
                    int event) override;
  void Pause();
  void PipExit(int delegate_id,
               int child_id,int frame_routing_id,
               content::MediaWebContentsObserver* observer,
               content::WebContentsImpl* web_contents_impl,
               content::MediaPlayerId& id);
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  CefRefPtr<CefDragData> GetDropData() override;
#endif // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#if BUILDFLAG(ARKWEB_PDF)
  void OnPdfScrollAtBottom(const std::string& url) override;
  void OnPdfLoadEvent(int32_t result, const std::string& url) override;
#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  bool OnStartBackgroundTask(int32_t type, const std::string& message) override;
#endif  // ARKWEB_PERFORMANCE_PERSISTENT_TASK

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  SkColor GetBackgroundColor() const override;
  void UpdateBackgroundColor(SkColor color) override;
#endif  // ARKWEB_BACKGROUND_COLOR

protected:
  // Platform-specific behaviors will be delegated to |native_delegate|.
  CefBrowserPlatformDelegateOsrExt(
      std::unique_ptr<CefBrowserPlatformDelegateNative> native_delegate,
      bool use_shared_texture,
      bool use_external_begin_frame);
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  int shrink_viewport_height_ = 0;
#endif
#if BUILDFLAG(ARKWEB_FOCUS)
  // We keep track of the view's focus-capturing logic, and if the view hasn't
  // been created yet, we temporarily store the focus-capturing event until
  // RenderViewCreated is created and then re-focus-capturing.
  bool is_view_focus_failed_ = false;
  bool focus_status_ = false;
#endif  // BUILDFLAG(ARKWEB_FOCUS)
#if BUILDFLAG(ARKWEB_AI)
  uint32_t fold_status_ = 0;
#endif
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  gfx::Insets safe_insets_;
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  bool native_embed_mode_ = false;
  bool isInnerWeb_ = false;
  bool custom_video_player_enable_ = false;
#endif
#if BUILDFLAG(ARKWEB_PIP)
  int delegate_id_ = 0;
  int child_id_ = 0;
  int frame_routing_id_ = 0;
  std::unique_ptr<base::OneShotTimer> pause_timer_;
  SEQUENCE_CHECKER(sequence_checker_);
#endif  // BUILDFLAG(ARKWEB_PIP)
#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  std::optional<SkColor> background_color_;
#endif  // ARKWEB_BACKGROUND_COLOR
};
 
#endif  // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_EXT_H_
