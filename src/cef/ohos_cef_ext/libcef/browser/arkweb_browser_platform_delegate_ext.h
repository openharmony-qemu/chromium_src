// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_BROWSER_PLATFORM_DELEGATE_EXT_H_
#define CEF_LIBCEF_BROWSER_BROWSER_PLATFORM_DELEGATE_EXT_H_

#include "cef/libcef/browser/browser_platform_delegate.h"

#if BUILDFLAG(ARKWEB_HTML_SELECT)
#include "third_party/blink/public/mojom/choosers/popup_menu.mojom.h"
#endif  // ARKWEB_HTML_SELECT

#if BUILDFLAG(IS_ARKWEB)
#include "ui/accessibility/platform/browser_accessibility_manager.h"
#endif

#if BUILDFLAG(ARKWEB_DATALIST)
#include "components/autofill/core/browser/suggestions/suggestion.h"
#include "ui/gfx/geometry/rect_f.h"
#endif

class CefBrowserPlatformDelegate;
// Provides platform-specific implementations of browser functionality. All
// methods are called on the browser process UI thread unless otherwise
// indicated.
class ArkWebCefBrowserPlatformDelegateExt : public CefBrowserPlatformDelegate {
 public:
  ArkWebCefBrowserPlatformDelegateExt* AsArkWebCefBrowserPlatformDelegateExt()
      override {
    return this;
  }

#if BUILDFLAG(ARKWEB_TOUCHPAD_FLING)
  virtual void SendTouchpadFlingEvent(const CefMouseEvent& event,
                                      double vx,
                                      double vy);
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  virtual void OnOnlineRenderToForeground();
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  // Send touch event list to the browser for a windowless browser.
  virtual void SendTouchEventList(const std::vector<CefTouchEvent>& event_list);
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  // Notify the browser that it was occluded. Only used with windowless
  // rendering.
  virtual void WasOccluded(bool occluded);
#endif

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  virtual void NotifyScreenInfoChangedV2();
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

#if BUILDFLAG(IS_ARKWEB)
  virtual ui::BrowserAccessibilityManager*
  GetRootBrowserAccessibilityManager() {
    return nullptr;
  }
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  virtual void WasKeyboardResized() {}
  virtual void SetDrawRect(int x, int y, int width, int height) {}
  virtual void SetDrawMode(int mode) {}
  virtual bool GetPendingSizeStatus() { return false; }
  virtual void SetFitContentMode(int mode) {}
  virtual void SetShouldFrameSubmissionBeforeDraw(bool should) {}
  virtual std::string GetCurrentLanguage() {}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual void OnNativeEmbedLifecycleChange(
      const ArkWebRenderHandlerExt::CefNativeEmbedData& info) {}
  virtual void OnNativeEmbedFirstFramePaint(
      const content::NativeEmbedFirstPaintEvent& event) {}
  virtual void SetNativeEmbedMode(bool flag) {}
  virtual void OnNativeEmbedVisibilityChange(const std::string& embed_id,
                                             bool visibility) {}
  virtual void SetNativeInnerWeb(bool isInnerWeb) {}
  virtual void SetEnableCustomVideoPlayer(bool flag) {}
  virtual void OnNativeEmbedObjectParamChange(
      const ArkWebRenderHandlerExt::CefNativeParamData& native_param_data) {}
#endif

#if BUILDFLAG(ARKWEB_AI)
  virtual void OnTextSelected(bool flag);
  virtual void OnDestroyImageAnalyzerOverlay();
  virtual void OnFoldStatusChanged(uint32_t foldStatus);
  virtual float GetPageScaleFactor();
  virtual std::string GetDataDetectorSelectText();
  virtual void OnDataDetectorSelectText();
#endif
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  virtual void OnSafeInsetsChange(int left, int top, int right, int bottom);
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void AdvanceFocusForIME(int focusType) {}
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  virtual bool WebPageSnapshot(const char* id,
                               int width,
                               int height,
                               cef_web_snapshot_callback_t callback);
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void SetVirtualKeyBoardArg(int32_t width,
                                     int32_t height,
                                     double keyboard) {}
  virtual bool ShouldVirtualKeyboardOverlay() { return false; }
  virtual void ScrollBy(float delta_x, float delta_y) {}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  virtual void SetBypassVsyncCondition(int32_t condition) {}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void SetScrollable(bool enable) {}
  virtual void UpdateSecurityLayer(bool isNeedSecurityLayer) {}
  virtual void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) {}
#endif
#if BUILDFLAG(ARKWEB_PRINT)
  virtual void SetToken(void* token) {}
  virtual void CreateWebPrintDocumentAdapter(const CefString& jobName,
                                             void** webPrintDocumentAdapter);
  virtual void CreateWebPrintDocumentAdapterV2(const CefString& jobName,
                                               void** adapter);
  virtual void SetPrintBackground(bool enable) {}
  virtual bool GetPrintBackground() { return false; }
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void ScrollFocusedEditableNodeIntoView();
  virtual void ScaleGestureChangeV2(int type,
                                    float scale,
                                    float originScale,
                                    float centerX,
                                    float centerY);
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  virtual int GetTopControlsOffset();
  virtual int GetShrinkViewportHeight();
#endif

#if BUILDFLAG(ARKWEB_HTML_SELECT)
  virtual void ShowPopupMenu(
      mojo::PendingRemote<blink::mojom::PopupMenuClient> popup_client,
      const gfx::Rect& bounds,
      double item_font_size,
      int selected_item,
      std::vector<blink::mojom::MenuItemPtr> menu_items,
      bool right_aligned,
      bool allow_multiple_selection);
#endif  // BUILDFLAG(ARKWEB_HTML_SELECT)

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  virtual void MaximizeResize() {}
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  virtual void OnBeforeUnloadFired(bool proceed);
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  virtual void OnShareFile(const std::string& filePath,
                           const std::string& utdTypeId);
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
  virtual void OnAdsBlocked(
      const std::string& main_frame_url,
      const std::map<std::string, int32_t>& subresource_blocked,
      bool is_site_first_report);

  virtual bool TrigAdBlockEnabledForSiteFromUi(
      const std::string& main_frame_url,
      int main_frame_tree_node_id);
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

#if BUILDFLAG(ARKWEB_DATALIST)
  virtual void OnShowAutofillPopup(
      const gfx::RectF& element_bounds,
      bool is_rtl,
      const std::vector<autofill::Suggestion>& suggestions,
      bool is_password_popup_type);
  virtual void OnHideAutofillPopup();
#endif  // BUILDFLAG(ARKWEB_DATALIST)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  virtual CefRefPtr<CefDragData> GetDropData();
#endif // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  virtual bool OnStartBackgroundTask(int32_t type, const std::string& message);
#endif  // ARKWEB_PERFORMANCE_PERSISTENT_TASK

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  virtual void UpdateBackgroundColor(SkColor color) {}
#endif  // ARKWEB_BACKGROUND_COLOR
};
#endif  // CEF_LIBCEF_BROWSER_BROWSER_PLATFORM_DELEGATE_EXT_H_
