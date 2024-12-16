/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_DELEGATE_INTERFACE_H
#define NWEB_DELEGATE_INTERFACE_H

#include <functional>
#include <string>
#include "capi/nweb_app_client_extension_callback.h"
#include "capi/nweb_download_delegate_callback.h"
#include "cef/include/cef_client.h"
#include "cef_delegate/nweb_inputmethod_client.h"
#include "display_manager_adapter.h"
#include "nweb.h"
#include "nweb_accessibility_node_info.h"
#include "nweb_download_callback.h"
#include "nweb_find_callback.h"
#include "nweb_handler.h"
#include "nweb_preference.h"
#include "nweb_web_message.h"

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
#include "nweb_native_media_player.h"
#endif // OHOS_CUSTOM_VIDEO_PLAYER

#include "cef_delegate/nweb_custom_keyboard_handler_impl.h"

namespace OHOS::NWeb {
class NWebValue;

enum class DelegateDragAction {
  DRAG_START = 0,
  DRAG_ENTER,
  DRAG_LEAVE,
  DRAG_OVER,
  DRAG_DROP,
  DRAG_END,
  DRAG_CANCEL,
};

struct DelegateDragEvent {
  double x = 0.0;
  double y = 0.0;
  DelegateDragAction action = DelegateDragAction::DRAG_START;
};

#ifdef OHOS_NAVIGATION
using WebState = std::shared_ptr<std::vector<uint8_t>>;
#endif

class NWebDelegateInterface
    : public std::enable_shared_from_this<NWebDelegateInterface> {
 public:
   virtual ~NWebDelegateInterface() = default;
  virtual void OnWindowShow() = 0;
  virtual void OnWindowHide() = 0;
  virtual void OnOnlineRenderToForeground() = 0;
  virtual bool IsReady() = 0;
  virtual void OnDestroy(bool is_close_all) = 0;
  virtual void RegisterDownLoadListener(
      std::shared_ptr<NWebDownloadCallback> downloadListener) = 0;
  virtual void RegisterAccessibilityEventListener(
      std::shared_ptr<NWebAccessibilityEventCallback> accessibilityEventListener) = 0;
  virtual void RegisterReleaseSurfaceListener(
      std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) = 0;
  virtual void RegisterNWebHandler(std::shared_ptr<NWebHandler> handler) = 0;
  virtual void RegisterRenderCb(
      std::function<void(const char*)> render_update_cb) = 0;
  virtual void RegisterWebAppClientExtensionListener(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener) = 0;
  virtual void SetInputMethodClient(
      CefRefPtr<NWebInputMethodClient> client) = 0;
  virtual void RegisterWebDownloadDelegateListener(
      std::shared_ptr<NWebDownloadDelegateCallback>
          downloadDelegateListener) = 0;
  virtual void StartDownload(const char *url) = 0;
  virtual void
  ResumeDownload(std::shared_ptr<NWebDownloadItem> web_download) = 0;
  virtual void NotifyForNextTouchEvent() = 0;
  virtual void SetAutofillCallback(std::shared_ptr<NWebMessageValueCallback> callback) = 0;
  virtual void FillAutofillData(std::shared_ptr<NWebMessage> data) = 0;

#if defined(OHOS_INPUT_EVENTS)
  virtual void SetNWebDelegateInterface(
      std::shared_ptr<NWebDelegateInterface> client) = 0;
#endif  // defined(OHOS_INPUT_EVENTS)

#ifdef OHOS_ARKWEB_ADBLOCK
  virtual void EnableAdsBlock(bool enable) = 0;

  virtual bool IsAdsBlockEnabled() = 0;

  virtual bool IsAdsBlockEnabledForCurPage() = 0;

  virtual void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                        int main_frame_tree_node_id) = 0;
#endif

#if defined(OHOS_PASSWORD_AUTOFILL)
  virtual void ProcessAutofillCancel(const std::string& fillContent) = 0;
#endif

#ifdef OHOS_EX_PASSWORD
  virtual void SetSavePasswordAutomatically(bool enable) = 0;
  virtual bool GetSavePasswordAutomatically() = 0;
  virtual void SetSavePassword(bool enable) = 0;
  virtual bool GetSavePassword() = 0;
  virtual void SaveOrUpdatePassword(bool is_update) = 0;
#endif  // defined(OHOS_EX_PASSWORD)
#if defined(OHOS_EX_PASSWORD) || (OHOS_DATALIST)
  virtual void PasswordSuggestionSelected(int list_index) const = 0;
#endif
#if defined(OHOS_NWEB_EX)
  virtual bool CanStoreWebArchive() const = 0;
  virtual void UnRegisterWebAppClientExtensionListener() = 0;
#endif  // defined(OHOS_NWEB_EX)

#if defined(OHOS_EX_FREE_COPY)
  virtual void SelectAndCopy() = 0;
  virtual bool ShouldShowFreeCopy() = 0;
#endif  // OHOS_EX_FREE_COPY

#ifdef OHOS_EX_GET_ZOOM_LEVEL
  virtual void SetBrowserZoomLevel(double zoom_factor) = 0;
  virtual double GetBrowserZoomLevel() = 0;
#endif

  /* event interface */
  virtual void Resize(uint32_t width, uint32_t height, bool isKeyboard = false) = 0;
#if defined(OHOS_INPUT_EVENTS)
  virtual void ResizeVisibleViewport(uint32_t width, uint32_t height, bool isKeyboard = false) = 0;
#endif
  virtual void OnTouchPress(int32_t id,
                            double x,
                            double y,
                            bool from_overlay) = 0;
  virtual void OnTouchRelease(int32_t id,
                              double x,
                              double y,
                              bool from_overlay) = 0;
  virtual void OnTouchMove(int32_t id,
                           double x,
                           double y,
                           bool from_overlay) = 0;
  virtual void OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                           bool from_overlay) = 0;
  virtual void OnTouchCancel() = 0;
  virtual void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay) = 0;
  virtual bool SendKeyEvent(int32_t keyCode, int32_t keyAction) = 0;
  virtual void SendTouchpadFlingEvent(double x, double y, double vx, double vy) = 0;
  virtual void SendMouseWheelEvent(double x,
                                   double y,
                                   double deltaX,
                                   double deltaY) = 0;
  virtual void SendMouseEvent(int x,
                              int y,
                              int button,
                              int action,
                              int count) = 0;
  virtual void NotifyScreenInfoChanged(RotationType rotation,
                                       DisplayOrientation orientation,
                                       bool isWebinitialization = false) = 0;

  virtual int Load(const std::string& url) = 0;
  virtual bool IsNavigatebackwardAllowed() const = 0;
  virtual bool IsNavigateForwardAllowed() const = 0;
  virtual bool CanNavigateBackOrForward(int numSteps) const = 0;
  virtual void NavigateBack() const = 0;
  virtual void NavigateForward() const = 0;
  virtual void NavigateBackOrForward(int32_t step) const = 0;
  virtual void DeleteNavigateHistory() = 0;
  virtual void ClearSslCache() = 0;
  virtual void ClearClientAuthenticationCache() = 0;
  virtual void Reload() const = 0;
  virtual void ReloadOriginalUrl() const = 0;
  virtual int Zoom(float zoomFactor) const = 0;
  virtual int ZoomIn() const = 0;
  virtual int ZoomOut() const = 0;
  virtual void Stop() const = 0;
  virtual void ExecuteJavaScript(const std::string& code) const = 0;
  virtual void PutBackgroundColor(int color) const = 0;
  virtual void InitialScale(float scale) const = 0;
  virtual void OnPause() = 0;
  virtual void OnContinue() = 0;
  virtual void WebComponentsBlur() = 0;
  virtual void OnOccluded() = 0;
  virtual void OnUnoccluded() = 0;
  virtual void SetEnableLowerFrameRate(bool enabled) = 0;
  virtual std::shared_ptr<NWebPreference> GetPreference() const = 0;
  virtual std::string Title() = 0;
  virtual std::shared_ptr<HitTestResult> GetHitTestResult() const = 0;
  virtual int PageLoadProgress() = 0;
  virtual float Scale() = 0;
  virtual int Load(
      const std::string& url,
      const std::map<std::string, std::string>& additionalHttpHeaders) = 0;
  virtual int LoadWithDataAndBaseUrl(const std::string& baseUrl,
                                     const std::string& data,
                                     const std::string& mimeType,
                                     const std::string& encoding,
                                     const std::string& historyUrl) = 0;
  virtual int LoadWithData(const std::string& data,
                           const std::string& mimeType,
                           const std::string& encoding) = 0;
  virtual int ContentHeight() = 0;

  virtual void RegisterNativeArkJSFunction(
      const char* objName,
      const std::vector<std::shared_ptr<NWebJsProxyCallback>> &callbacks) = 0;
  virtual void RegisterNativeJSProxy(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<char*(std::vector<std::vector<uint8_t>>&,
                                      std::vector<size_t>&)>>&& callback,
      bool isAsync,
      const std::string& permission) = 0;
  virtual void RegisterNativeJSProxyWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
          std::vector<std::vector<uint8_t>>&, std::vector<size_t>&)>>&& callback,
      bool isAsync, const std::string& permission) = 0;
  virtual void UnRegisterNativeArkJSFunction(const char* objName) = 0;

#ifdef OHOS_ARKWEB_ADBLOCK
  virtual void UpdateAdblockEasyListRules(
      long adBlockEasyListVersion) = 0;
#endif

  virtual void RegisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list,
      const std::vector<std::string>& async_method_list,
      const int32_t object_id,
      const std::string& permission) const = 0;
  virtual void UnregisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) const = 0;
  virtual void RegisterNativeLoadStartCallback(std::function<void(void)>&& callback) = 0;
  virtual void RegisterNativeLoadEndCallback(std::function<void(void)>&& callback) = 0;
  virtual void JavaScriptOnDocumentStart(const ScriptItems& ScriptItems) = 0;
  virtual void JavaScriptOnDocumentEnd(const ScriptItems& ScriptItems) = 0;
  virtual void CallH5Function(
      int32_t routing_id,
      int32_t h5_object_id,
      const std::string& h5_method_name,
      const std::vector<std::shared_ptr<NWebValue>>& args) const = 0;
  virtual bool Discard() = 0;
  virtual bool Restore() = 0;
  virtual void RegisterNWebJavaScriptCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback) = 0;
  virtual bool OnFocus(const FocusReason& focusReason = FocusReason::FOCUS_DEFAULT) const = 0;
  virtual void OnBlur() const = 0;
  virtual void RegisterFindListener(
      std::shared_ptr<NWebFindCallback> find_listener) = 0;
  virtual void FindAllAsync(const std::string& str) const = 0;
  virtual void ClearMatches() const = 0;
  virtual void FindNext(const bool forward) const = 0;
  virtual void SuggestionSelected(int index) const = 0;

  virtual void ExecuteJavaScriptExt(
      const int fd,
      const size_t scriptLength,
      std::shared_ptr<NWebMessageValueCallback> callback,
      bool extention) = 0;
  virtual void EraseCreatePDFCallbackImpl(uint32_t id) = 0;
  virtual void ExecuteCreatePDFExt(
      std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
      std::shared_ptr<NWebArrayBufferValueCallback> callback) = 0;

#if defined(OHOS_MSGPORT)
  virtual void EraseJavaScriptCallbackImpl(uint32_t id) = 0;
  virtual void ExecuteJavaScript(
      const std::string& code,
      std::shared_ptr<NWebMessageValueCallback> callback,
      bool extention) = 0;
  virtual std::vector<std::string> CreateWebMessagePorts() = 0;
  virtual void PostWebMessage(const std::string& message,
                              const std::vector<std::string>& ports,
                              const std::string& targetUri) = 0;
  virtual void ClosePort(const std::string& portHandle) = 0;
  virtual void PostPortMessage(const std::string& portHandle,
                               std::shared_ptr<NWebMessage> data) = 0;
  virtual void SetPortMessageCallback(
      const std::string& portHandle,
      std::shared_ptr<NWebMessageValueCallback> callback) = 0;
#endif  // defined(OHOS_MSGPORT)

#ifdef OHOS_I18N
  virtual void UpdateLocale(const std::string& language,
                            const std::string& region) = 0;
#endif  // #ifdef OHOS_I18N

#ifdef OHOS_DRAG_DROP
  virtual std::shared_ptr<NWebDragData> GetOrCreateDragData() = 0;
  virtual std::string GetAppTempDir() const = 0;
  virtual bool DarkModeEnabled() = 0;
#endif // #ifdef OHOS_DRAG_DROP
  virtual void SetNWebId(uint32_t nwebId) = 0;

  virtual void StoreWebArchive(
      const std::string& base_name,
      bool auto_name,
      std::shared_ptr<NWebStringValueCallback> callback) const = 0;

  virtual void SetBrowserUserAgentString(const std::string& user_agent) = 0;

  virtual void SendDragEvent(const DelegateDragEvent& dragEvent) const = 0;
  virtual bool TerminateRenderProcess() = 0;
  virtual std::string GetUrl() const = 0;
  virtual const std::string GetOriginalUrl() = 0;
  virtual bool GetFavicon(const void** data,
                          size_t& width,
                          size_t& height,
                          ImageColorType& colorType,
                          ImageAlphaType& alphaType) = 0;
  virtual void PutNetworkAvailable(bool available) = 0;

  virtual CefRefPtr<CefClient> GetCefClient() const = 0;

  virtual void GetImages(std::shared_ptr<NWebBoolValueCallback> callback) = 0;
  virtual void RemoveCache(bool include_disk_files) = 0;

#ifdef OHOS_NAVIGATION
  virtual std::shared_ptr<NWebHistoryList> GetHistoryList() = 0;
  virtual std::vector<uint8_t> SerializeWebState() = 0;
  virtual bool RestoreWebState(const std::vector<uint8_t>& state) = 0;
#endif

#if defined(OHOS_MEDIA_MUTE_AUDIO)
  virtual void SetAudioMuted(bool muted) = 0;
#endif  // defined(OHOS_MEDIA_MUTE_AUDIO)

#if defined(OHOS_WEBRTC)
  virtual void StartCamera() = 0;
  virtual void StopCamera() = 0;
  virtual void CloseCamera() = 0;
#endif  // defined(OHOS_WEBRTC)

#ifdef OHOS_PAGE_UP_DOWN
  virtual void PageUp(bool top) = 0;
  virtual void PageDown(bool bottom) = 0;
#ifdef OHOS_GET_SCROLL_OFFSET
  virtual void GetScrollOffset(float* offset_x, float* offset_y) = 0;
#endif
#endif  // #ifdef OHOS_PAGE_UP_DOWN

#if defined(OHOS_INPUT_EVENTS)
  virtual void ScrollTo(float x, float y) = 0;
  virtual void ScrollBy(float delta_x, float delta_y) = 0;
  virtual void ScrollByRefScreen(float delta_x, float delta_y, float vx, float vy) = 0;
  virtual void SlideScroll(float vx, float vy) = 0;
  virtual bool WebSendKeyEvent(int32_t keyCode, int32_t keyAction,
                               const std::vector<int32_t>& pressedCodes) = 0;
  virtual void ScrollToWithAnime(float x, float y, int32_t duration) = 0;
  virtual void ScrollByWithAnime(float delta_x, float delta_y, int32_t duration) = 0;
  virtual bool ScrollByWithResult(float delta_x, float delta_y) = 0;
#if defined(OHOS_GET_SCROLL_OFFSET)
  virtual void GetOverScrollOffset(float* offset_x, float* offset_y) = 0;
#endif
  virtual void WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) = 0;
#endif  // defined(OHOS_INPUT_EVENTS)

#if defined(OHOS_EX_FORCE_ZOOM)
  virtual void SetForceEnableZoom(bool forceEnableZoom) = 0;
  virtual bool GetForceEnableZoom() = 0;
#endif

#if defined(OHOS_COMPOSITE_RENDER)
  virtual void SetShouldFrameSubmissionBeforeDraw(bool should) = 0;
  virtual void SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height) = 0;
  virtual void SetDrawMode(int32_t mode) = 0;
  virtual bool GetPendingSizeStatus() = 0;
  virtual void SetFitContentMode(int32_t mode) = 0;
#endif  // defined(OHOS_COMPOSITE_RENDER)

#if defined(OHOS_MEDIA_POLICY)
  virtual void SetAudioResumeInterval(int32_t resumeInterval) = 0;
  virtual void SetAudioExclusive(bool audioExclusive) = 0;
  virtual void CloseAllMediaPresentations() = 0;
  virtual void StopAllMedia() = 0;
  virtual void ResumeAllMedia() = 0;
  virtual void PauseAllMedia() = 0;
  virtual int GetMediaPlaybackState() = 0;
#endif // defined(OHOS_MEDIA_POLICY)

#if defined(OHOS_NO_STATE_PREFETCH)
  virtual void PrefetchPage(
      const std::string& url,
      const std::map<std::string, std::string>& additionalHttpHeaders) = 0;
#endif  // defined(OHOS_NO_STATE_PREFETCH)

#if defined(OHOS_MULTI_WINDOW)
  virtual void NotifyPopupWindowResult(bool result) = 0;
#endif  // defined(OHOS_MULTI_WINDOW)
#ifdef OHOS_CA
  virtual bool GetCertChainDerData(std::vector<std::string>& certChainData,
                                   bool isSingleCert) = 0;
#endif

  virtual void SetWindowId(uint32_t window_id);

#ifdef OHOS_EX_BLANK_TARGET_POPUP_INTERCEPT
  virtual void SetEnableBlankTargetPopupIntercept(
      bool enableBlankTargetPopup) = 0;
#endif

#if defined(OHOS_PRINT)
  virtual void SetToken(void* token) = 0;
  virtual void* CreateWebPrintDocumentAdapter(const std::string& jobName) = 0;
  virtual void SetPrintBackground(bool enable) = 0;
  virtual bool GetPrintBackground() = 0;
#endif // defined(OHOS_PRINT)

#ifdef OHOS_SCREEN_ROTATION
  virtual void SetVirtualPixelRatio(float ratio) = 0;
#endif // defined(OHOS_SCREEN_ROTATION)

#if BUILDFLAG(IS_OHOS)
  virtual void UpdateNativeEmbedInfo(std::shared_ptr<NWebNativeEmbedDataInfo> info) = 0;
  virtual void SetTransformHint(uint32_t rotation) = 0;
#endif

#ifdef OHOS_EX_TOPCONTROLS
  virtual void UpdateBrowserControlsState(int constraints,
                                          int current,
                                          bool animate) const = 0;
  virtual void UpdateBrowserControlsHeight(int height, bool animate) = 0;
#endif

#ifdef OHOS_POST_URL
  virtual int PostUrl(const std::string& url, const std::vector<char>& postData) = 0;
#endif

#if defined(OHOS_INPUT_EVENTS)
  virtual void SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard) = 0;
  virtual bool ShouldVirtualKeyboardOverlay() = 0;
  virtual void WebSendMouseWheelEvent(double x,
                                      double y,
                                      double deltaX,
                                      double deltaY,
                                      const std::vector<int32_t>& pressedCodes) = 0;
  virtual void WebSendTouchpadFlingEvent(double x,
                                         double y,
                                         double vx,
                                         double vy,
                                         const std::vector<int32_t>& pressedCodes) = 0;
#endif

#if BUILDFLAG(IS_OHOS)
  virtual bool IsSafeBrowsingEnabled() = 0;
  virtual void EnableSafeBrowsing(bool enable) = 0;
  virtual void PrecompileJavaScript(const std::string& url,
                                    const std::string& script,
                                    std::shared_ptr<CacheOptions>& cacheOptions,
                                    std::shared_ptr<NWebMessageValueCallback> callback) = 0;
#endif

#if defined(OHOS_SECURITY_STATE)
  virtual int GetSecurityLevel() = 0;
#endif

  virtual void SetAccessibilityState(cef_state_t accessibilityState) = 0;
  virtual void ExecuteAction(int64_t accessibilityId, uint32_t action) = 0;
  virtual void ExecuteAction(int64_t accessibilityId, uint32_t action,
      const std::map<std::string, std::string>& actionArguments) = 0;
  virtual std::shared_ptr<NWebAccessibilityNodeInfo>
  GetFocusedAccessibilityNodeInfo(int64_t accessibilityId,
                                  bool isAccessibilityFocus) = 0;
  virtual std::shared_ptr<NWebAccessibilityNodeInfo>
  GetAccessibilityNodeInfoById(int64_t accessibilityId) = 0;
  virtual bool GetAccessibilityVisible(int64_t accessibilityId) = 0;
  virtual std::shared_ptr<NWebAccessibilityNodeInfo>
  GetAccessibilityNodeInfoByFocusMove(int64_t accessibilityId,
                                      int32_t direction) = 0;

#if defined(OHOS_SECURE_JAVASCRIPT_PROXY)
  virtual std::string GetLastJavascriptProxyCallingFrameUrl() = 0;
#endif

#ifdef OHOS_ITP
  virtual void EnableIntelligentTrackingPrevention(bool enable) = 0;
  virtual bool IsIntelligentTrackingPreventionEnabled() const = 0;
#endif

  virtual int ScaleGestureChange(double scale, double centerX, double centerY) const = 0;

#if defined(OHOS_SCREEN_LOCK)
  virtual void SetWakeLockCallback(int32_t windowId, const std::shared_ptr<NWebScreenLockCallback>& callback) = 0;
#endif

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  virtual void RegisterOnCreateNativeMediaPlayerListener(
      std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) = 0;
#endif // OHOS_CUSTOM_VIDEO_PLAYER

#ifdef OHOS_EX_DOWNLOAD
  virtual NWebDownloadItemState GetDownloadItemState(long item_id) = 0;
#endif

#if defined(OHOS_CLIPBOARD)
  virtual void SetIsRichText(bool is_rich_text) = 0;
  virtual std::string GetSelectInfo() = 0;
#endif

#ifdef OHOS_DISPLAY_CUTOUT
  virtual void OnSafeInsetsChange(int left, int top, int right, int bottom) = 0;
#endif

#ifdef OHOS_AI
  virtual void OnTextSelected() = 0;
#endif

#if defined(OHOS_SOFTWARE_COMPOSITOR)
  virtual void EnableWholeWebPageDrawing() = 0;

  virtual bool WebPageSnapshot(const char* id,
                               PixelUnit type,
                               int width,
                               int height,
                               const WebSnapshotCallback callback) = 0;
#endif

#if OHOS_URL_TRUST_LIST
  virtual int SetUrlTrustListWithErrMsg(
    const std::string& urlTrustList, std::string& detailErrMsg) = 0;
#endif

#ifdef OHOS_NETWORK_LOAD
  virtual void SetPathAllowingUniversalAccess(
      const std::vector<std::string>& pathList) = 0;
#endif

#ifdef OHOS_BFCACHE
  virtual void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) = 0;
#endif

  virtual bool IsCustomKeyboard() const = 0;

  virtual std::shared_ptr<NWebCustomKeyboardHandlerImpl> GetCustomKeyboardHandler() const = 0;

  virtual void SendAccessibilityHoverEvent(int x, int y) = 0;
  virtual void RefreshAccessibilityManagerClickEvent() = 0;

#ifdef OHOS_MIXED_CONTENT
  virtual void EnableMixedContentAutoUpgrades(bool enable) = 0;
  virtual bool IsMixedContentAutoUpgradesEnabled() = 0;
#endif

  virtual void SetPopupSurface(void* popupSurface) = 0;

#ifdef OHOS_EX_REFRESH_IFRAME
  virtual bool WebExtensionContextMenuIsIframe() = 0;
  virtual void WebExtensionContextMenuReloadFocusedFrame() = 0;
#endif

#if defined(OHOS_DISPATCH_BEFORE_UNLOAD)
  virtual bool NeedToFireBeforeUnloadOrUnloadEvents() = 0;
  virtual void DispatchBeforeUnload() = 0;
#endif // OHOS_DISPATCH_BEFORE_UNLOAD
};
}  // namespace OHOS::NWeb

#endif  // nweb_delegate_INTERFACE_H
