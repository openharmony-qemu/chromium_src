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

#ifndef NWEB_DELEGATE_H
#define NWEB_DELEGATE_H

#include <condition_variable>
#include <set>
#include <string>
#include <unordered_map>
#include "capi/nweb_app_client_extension_callback.h"
#include "cef/include/cef_command_line.h"
#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#include "nweb_accessibility_node_info_impl.h"
#include "nweb_application.h"
#include "nweb_delegate_interface.h"
#include "nweb_display_listener.h"
#include "nweb_errors.h"
#include "nweb_event_handler.h"
#include "nweb_handler.h"
#include "nweb_handler_delegate.h"
#include "nweb_inputmethod_client.h"
#include "nweb_render_handler.h"

#if defined(OHOS_EX_DOWNLOAD)
#include <memory>
#include "capi/nweb_download_delegate_callback.h"
#endif  //  OHOS_EX_DOWNLOAD

namespace OHOS::NWeb {
class JavaScriptResultCallbackImpl;
class CefPdfValueCallbackImpl;
class NWebDelegate : public NWebDelegateInterface, public virtual CefRefCount {
 public:
  NWebDelegate(int argc, const char* argv[]);
  ~NWebDelegate();

  bool Init(bool is_enhance_surface,
            void* window,
            bool popup
#if defined(OHOS_EX_DOWNLOAD)
            ,
            uint32_t nweb_id
#endif
#if BUILDFLAG(IS_OHOS)
            ,
            bool incognito_mode
#endif
#if defined(OHOS_RENDER_PROCESS_SHARE)
            ,
            const std::string& shared_render_process_token
#endif

  );
  void OnWindowShow() override;
  void OnWindowHide() override;
  void OnOnlineRenderToForeground() override;
  bool IsReady() override;
  void OnDestroy(bool is_close_all) override;
  void RegisterWebAppClientExtensionListener(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener) override;
  void RegisterDownLoadListener(
      std::shared_ptr<NWebDownloadCallback> downloadListener) override;
  void RegisterAccessibilityEventListener(
      std::shared_ptr<NWebAccessibilityEventCallback>
          accessibility_event_listener) override;
  void RegisterReleaseSurfaceListener(
      std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener)
      override;

  void RegisterWebDownloadDelegateListener(
      std::shared_ptr<NWebDownloadDelegateCallback> downloadDelegateListener)
      override;
  void StartDownload(const char* url) override;
  void ResumeDownload(std::shared_ptr<NWebDownloadItem> web_download) override;
#ifdef OHOS_EX_DOWNLOAD
  NWebDownloadItemState GetDownloadItemState(long item_id) override;
#endif

  void RegisterNWebHandler(std::shared_ptr<NWebHandler> handler) override;
  void RegisterRenderCb(
      std::function<void(const char*)> render_update_cb) override;

  void SetInputMethodClient(CefRefPtr<NWebInputMethodClient> client) override;

  void Resize(uint32_t width,
              uint32_t height,
              bool isKeyboard = false) override;
#if defined(OHOS_INPUT_EVENTS)
  void ResizeVisibleViewport(uint32_t width, uint32_t height, bool isKeyboard = false) override;
#endif
  void OnTouchPress(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchRelease(int32_t id,
                      double x,
                      double y,
                      bool from_overlay) override;
  void OnTouchMove(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                   bool from_overlay = false) override;
  void OnTouchCancel() override;
  void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay) override;
  bool SendKeyEvent(int32_t keyCode, int32_t keyAction) override;
  void SendTouchpadFlingEvent(double x, double y, double vx, double vy) override;
  void SendMouseWheelEvent(double x,
                           double y,
                           double deltaX,
                           double deltaY) override;
  void SendMouseEvent(int x, int y, int button, int action, int count) override;
  void NotifyScreenInfoChanged(RotationType rotation,
                               DisplayOrientation orientation,
                               bool isWebinitialization = false) override;

  int Load(const std::string& url) override;
  bool IsNavigatebackwardAllowed() const override;
  bool IsNavigateForwardAllowed() const override;
  bool CanNavigateBackOrForward(int num_steps) const override;
  void NavigateBack() const override;
  void NavigateForward() const override;
  void NavigateBackOrForward(int32_t step) const override;
  void DeleteNavigateHistory() override;
  void ClearSslCache() override;
  void ClearClientAuthenticationCache() override;
  void Reload() const override;
  void ReloadOriginalUrl() const override;
  int Zoom(float zoomFactor) const override;
  int ZoomIn() const override;
  int ZoomOut() const override;
  void Stop() const override;
  void ExecuteJavaScript(const std::string& code) const override;
  void PutBackgroundColor(int color) const override;
  void InitialScale(float scale) const override;
  void OnPause() override;
  void OnContinue() override;
  void WebComponentsBlur() override;
  void OnOccluded() override;
  void OnUnoccluded() override;
  void SetEnableLowerFrameRate(bool enabled) override;
  std::shared_ptr<NWebPreference> GetPreference() const override;
  std::string Title() override;
  std::shared_ptr<HitTestResult> GetHitTestResult() const override;
  int PageLoadProgress() override;
  float Scale() override;
  int Load(const std::string& url,
           const std::map<std::string, std::string>& additionalHttpHeaders) override;
  int LoadWithDataAndBaseUrl(const std::string& baseUrl,
                             const std::string& data,
                             const std::string& mimeType,
                             const std::string& encoding,
                             const std::string& historyUrl) override;
  int LoadWithData(const std::string& data,
                   const std::string& mimeType,
                   const std::string& encoding) override;
  int ContentHeight() override;

  void RegisterNativeArkJSFunction(const char* objName,
      const std::vector<std::shared_ptr<NWebJsProxyCallback>> &callbacks) override;
  void RegisterNativeJSProxy(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<char*(std::vector<std::vector<uint8_t>>&,
                                      std::vector<size_t>&)>>&& callback,
      bool isAsync, const std::string& permission) override;
  void RegisterNativeJSProxyWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
          std::vector<std::vector<uint8_t>>&, std::vector<size_t>&)>>&& callback,
      bool isAsync, const std::string& permission) override;
  void UnRegisterNativeArkJSFunction(const char* objName) override;
  void RegisterNativeLoadStartCallback(
      std::function<void(void)>&& callback) override;
  void RegisterNativeLoadEndCallback(
      std::function<void(void)>&& callback) override;

#ifdef OHOS_ARKWEB_ADBLOCK
  void UpdateAdblockEasyListRules(
      long adBlockEasyListVersion) override;
#endif

  void RegisterArkJSfunction(const std::string& object_name,
                             const std::vector<std::string>& method_list,
                             const std::vector<std::string>& async_method_list,
                             const int32_t object_id,
                             const std::string& permission) const override;

  void UnregisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) const override;

  void JavaScriptOnDocumentStart(const ScriptItems& scriptItems) override;
  void JavaScriptOnDocumentEnd(const ScriptItems& scriptItems) override;

  bool Discard() override;
  bool Restore() override;

  void CallH5Function(
      int32_t routing_id,
      int32_t h5_object_id,
      const std::string& h5_method_name,
      const std::vector<std::shared_ptr<NWebValue>>& args) const override;

  void RegisterNWebJavaScriptCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback) override;
  bool OnFocus(const FocusReason& focusReason =
                   FocusReason::FOCUS_DEFAULT) const override;
  void OnBlur() const override;

  void RegisterFindListener(
      std::shared_ptr<NWebFindCallback> find_listener) override;
  void FindAllAsync(const std::string& search_string) const override;
  void ClearMatches() const override;
  void FindNext(const bool forward) const override;
  bool TerminateRenderProcess() override;
  std::string GetUrl() const override;
  const std::string GetOriginalUrl() override;
  bool GetFavicon(const void** data,
                  size_t& width,
                  size_t& height,
                  ImageColorType& colorType,
                  ImageAlphaType& alphaType) override;
  void PutNetworkAvailable(bool available) override;

  void SetWindowId(uint32_t window_id) override;

  void SuggestionSelected(int index) const override;

  bool SetZoomInFactor(float factor);
  bool SetZoomOutFactor(float factor);

  bool IsFileProtocol(const GURL& gurl);
  bool IsUrlFileExist(const GURL& gurl, const std::string& url);

  uint32_t create_pdf_value_callback_id_ = 0;
  std::unordered_map<uint32_t, CefRefPtr<CefPdfValueCallbackImpl>>
      create_pdf_value_callback_map_;
  void EraseCreatePDFCallbackImpl(uint32_t id) override;
  void ExecuteCreatePDFExt(
      std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
      std::shared_ptr<NWebArrayBufferValueCallback> callback) override;

  void ExecuteJavaScriptExt(
      const int fd,
      const size_t scriptLength,
      std::shared_ptr<NWebMessageValueCallback> callback,
      bool extention) override;

  int ScaleGestureChange(double scale, double centerX, double centerY) const override;

#if defined(OHOS_MSGPORT)
  uint32_t runJSCallbackId_ = 0;
  std::unordered_map<uint32_t, CefRefPtr<JavaScriptResultCallbackImpl>> runJSCallbackMap_;
  void EraseJavaScriptCallbackImpl(uint32_t id) override;
  void ExecuteJavaScript(
      const std::string& code,
      std::shared_ptr<NWebMessageValueCallback> callback,
      bool extention) override;
  std::vector<std::string> CreateWebMessagePorts() override;
  void PostWebMessage(const std::string& message,
                      const std::vector<std::string>& ports,
                      const std::string& targetUri) override;
  void ClosePort(const std::string& port_handle) override;
  void PostPortMessage(const std::string& port_handle,
                       std::shared_ptr<NWebMessage> data) override;
  void SetPortMessageCallback(
      const std::string& port_handle,
      std::shared_ptr<NWebMessageValueCallback> callback)
      override;
#endif  // defined(OHOS_MSGPORT)

  void SetNWebId(uint32_t nwebId) override;

  void StoreWebArchive(
      const std::string& base_name,
      bool auto_name,
      std::shared_ptr<NWebStringValueCallback> callback) const override;
  void SetBrowserUserAgentString(const std::string& user_agent) override;
  void SendDragEvent(const DelegateDragEvent& dragEvent) const override;
#ifdef OHOS_I18N
  void UpdateLocale(const std::string& language,
                    const std::string& region) override;
#endif  // #ifdef OHOS_I18N
  CefRefPtr<CefClient> GetCefClient() const override {
    return handler_delegate_;
  }
#ifdef OHOS_DRAG_DROP
  std::string GetAppTempDir() const override {
    return ohos_temp_dir_;
  }
  void InitAppTempDir();
  std::shared_ptr<NWebDragData> GetOrCreateDragData() override;
  void ClearDragData() const;
  std::string ohos_temp_dir_;
  bool DarkModeEnabled() override;
#endif  // OHOS_DRAG_DROP
  void GetImages(std::shared_ptr<NWebBoolValueCallback> callback) override;
  void RemoveCache(bool include_disk_files) override;

#ifdef OHOS_NAVIGATION
  std::shared_ptr<NWebHistoryList> GetHistoryList() override;
  std::vector<uint8_t> SerializeWebState() override;
  bool RestoreWebState(const std::vector<uint8_t>& state) override;
#endif

#if defined(OHOS_NWEB_EX)
  bool CanStoreWebArchive() const override;
  void UnRegisterWebAppClientExtensionListener() override;
#endif  // defined(OHOS_NWEB_EX)

#if defined(OHOS_MEDIA_MUTE_AUDIO)
  void SetAudioMuted(bool muted) override;
#endif  // defined(OHOS_MEDIA_MUTE_AUDIO)

#if defined(OHOS_MEDIA_POLICY)
  void SetAudioResumeInterval(int32_t resumeInterval) override;
  void SetAudioExclusive(bool audioExclusive) override;
  void CloseAllMediaPresentations() override;
  void StopAllMedia() override;
  void ResumeAllMedia() override;
  void PauseAllMedia() override;
  int GetMediaPlaybackState() override;
#endif  // defined(OHOS_MEDIA_POLICY)

#if defined(OHOS_NO_STATE_PREFETCH)
  void PrefetchPage(
      const std::string& url,
      const std::map<std::string, std::string>& additionalHttpHeaders) override;
#endif  // defined(OHOS_NO_STATE_PREFETCH)

#if defined(OHOS_INPUT_EVENTS)
  void SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard) override;
  bool ShouldVirtualKeyboardOverlay() override;
#endif

#if defined(OHOS_SECURITY_STATE)
  int GetSecurityLevel() override;
#endif

#ifdef BUILDFLAG(IS_OHOS)
bool IsSafeBrowsingEnabled() override;
void EnableSafeBrowsing(bool enable) override;
void PrecompileJavaScript(const std::string& url,
                          const std::string& script,
                          std::shared_ptr<CacheOptions>& cacheOptions,
                          std::shared_ptr<NWebMessageValueCallback> callback) override;
void UpdateNativeEmbedInfo(std::shared_ptr<NWebNativeEmbedDataInfo> info) override;
bool HitNativeArea(double x, double y);
void SetTransformHint(uint32_t rotation) override;
#endif

#ifdef OHOS_PAGE_UP_DOWN
  void PageUp(bool top) override;
  void PageDown(bool bottom) override;
#ifdef OHOS_GET_SCROLL_OFFSET
  void GetScrollOffset(float* offset_x, float* offset_y) override;
#endif
#endif  // #ifdef OHOS_PAGE_UP_DOWN

#if defined(OHOS_INPUT_EVENTS)
  void SetNWebDelegateInterface(
      std::shared_ptr<NWebDelegateInterface> client) override;
  void ScrollTo(float x, float y) override;
  void ScrollBy(float delta_x, float delta_y) override;
  void ScrollByRefScreen(float delta_x, float delta_y, float vx, float vy) override;
  void SlideScroll(float vx, float vy) override;
  bool WebSendKeyEvent(int32_t keyCode, int32_t keyAction,
                       const std::vector<int32_t>& pressedCodes) override;
  void WebSendMouseWheelEvent(double x,
                              double y,
                              double deltaX,
                              double deltaY,
                              const std::vector<int32_t>& pressedCodes) override;
  void WebSendTouchpadFlingEvent(double x,
                                 double y,
                                 double vx,
                                 double vy,
                                 const std::vector<int32_t>& pressedCodes) override;
  void ScrollToWithAnime(float x, float y, int32_t duration) override;
  void ScrollByWithAnime(float delta_x, float delta_y, int32_t duration) override;
#if defined(OHOS_GET_SCROLL_OFFSET)
  void GetOverScrollOffset(float* offset_x, float* offset_y) override;
#endif
  bool ScrollByWithResult(float delta_x, float delta_y) override;
  void WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) override;
#endif  // defined(OHOS_INPUT_EVENTS)

#ifdef OHOS_ARKWEB_ADBLOCK
  void EnableAdsBlock(bool enable) override;
  bool IsAdsBlockEnabled() override;
  bool IsAdsBlockEnabledForCurPage() override;
  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                int main_frame_tree_node_id) override;
#endif

#if defined(OHOS_PASSWORD_AUTOFILL)
  void ProcessAutofillCancel(const std::string& fillContent) override;
#endif

#ifdef OHOS_EX_PASSWORD
  void SetSavePasswordAutomatically(bool enable) override;
  bool GetSavePasswordAutomatically() override;
  void SetSavePassword(bool enable) override;
  bool GetSavePassword() override;
  void SaveOrUpdatePassword(bool is_udpate) override;
#endif
#if defined(OHOS_EX_PASSWORD) || (OHOS_DATALIST)
  void PasswordSuggestionSelected(int list_index) const override;
#endif
#if defined(OHOS_EX_FORCE_ZOOM)
  void SetForceEnableZoom(bool forceEnableZoom) override;
  bool GetForceEnableZoom() override;
#endif
#if defined(OHOS_EX_FREE_COPY)
  void SelectAndCopy() override;
  bool ShouldShowFreeCopy() override;
#endif
#if defined(OHOS_COMPOSITE_RENDER)
  void SetShouldFrameSubmissionBeforeDraw(bool should) override;
  void SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height) override;
  void SetDrawMode(int32_t mode) override;
  bool GetPendingSizeStatus() override;
  void SetFitContentMode(int32_t mode) override;
#endif  // defined(OHOS_COMPOSITE_RENDER)

#if defined(OHOS_MULTI_WINDOW)
  void NotifyPopupWindowResult(bool result) override;
#endif  // defined(OHOS_MULTI_WINDOW)
#ifdef OHOS_CA
  bool GetCertChainDerData(std::vector<std::string>& certChainData,
                           bool isSingleCert) override;
#endif

#ifdef OHOS_EX_BLANK_TARGET_POPUP_INTERCEPT
  void SetEnableBlankTargetPopupIntercept(bool enableBlankTargetPopup) override;
#endif

#ifdef OHOS_EX_TOPCONTROLS
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) const override;
  void UpdateBrowserControlsHeight(int height, bool animate) override;
#endif

#if defined(OHOS_PRINT)
  void SetToken(void* token) override;
  void* CreateWebPrintDocumentAdapter(const std::string& jobName) override;
  void SetPrintBackground(bool enable) override;
  bool GetPrintBackground() override;
#endif // defined(OHOS_PRINT)

#ifdef OHOS_SCREEN_ROTATION
  void SetVirtualPixelRatio(float ratio) override;
#endif // defined(OHOS_SCREEN_ROTATION)

#ifdef OHOS_POST_URL
  int PostUrl(const std::string& url, const std::vector<char>& postData) override;
#endif // defined(OHOS_POST_URL)
#ifdef OHOS_EX_GET_ZOOM_LEVEL
 void SetBrowserZoomLevel(double zoom_factor) override;
 double GetBrowserZoomLevel() override;
#endif
  void SetAccessibilityState(cef_state_t accessibility_state) override;
  void ExecuteAction(int64_t accessibilityId, uint32_t action) override;
  void ExecuteAction(int64_t accessibilityId, uint32_t action,
      const std::map<std::string, std::string>& actionArguments) override;
  std::shared_ptr<NWebAccessibilityNodeInfo>
  GetFocusedAccessibilityNodeInfo(int64_t accessibilityId,
                                  bool isAccessibilityFocus) override;
  std::shared_ptr<NWebAccessibilityNodeInfo>
  GetAccessibilityNodeInfoById(int64_t accessibilityId) override;
  bool GetAccessibilityVisible(int64_t accessibilityId) override;
  std::shared_ptr<NWebAccessibilityNodeInfo>
  GetAccessibilityNodeInfoByFocusMove(int64_t accessibilityId,
                                      int32_t direction) override;
  void RefreshAccessibilityManagerClickEvent() override;
  void SetAutofillCallback(std::shared_ptr<NWebMessageValueCallback> callback) override;
  void FillAutofillData(std::shared_ptr<NWebMessage> data) override;

#if defined(OHOS_WEBRTC)
  void StartCamera() override;
  void StopCamera() override;
  void CloseCamera() override;
#endif  // defined(OHOS_WEBRTC)

#if defined(OHOS_SCREEN_LOCK)
  void SetWakeLockCallback(int32_t windowId, const std::shared_ptr<NWebScreenLockCallback>& callback) override;
#endif

#if defined(OHOS_SECURE_JAVASCRIPT_PROXY)
  std::string GetLastJavascriptProxyCallingFrameUrl() override;
#endif

#if defined(OHOS_SOFTWARE_COMPOSITOR)
  void EnableWholeWebPageDrawing() override;

  bool WebPageSnapshot(const char* id,
                       PixelUnit type,
                       int width,
                       int height,
                       const WebSnapshotCallback callback) override;
#endif

#ifdef OHOS_ITP
  void EnableIntelligentTrackingPrevention(bool enable) override;
  bool IsIntelligentTrackingPreventionEnabled() const override;
#endif

#if defined(OHOS_CLIPBOARD)
  void SetIsRichText(bool is_rich_text) override;
  std::string GetSelectInfo() override;
#endif

#ifdef OHOS_DISPLAY_CUTOUT
  void OnSafeInsetsChange(int left, int top, int right, int bottom) override;
#endif

void NotifyForNextTouchEvent() override;

#ifdef OHOS_AI
  void OnTextSelected() override;
#endif

#ifdef OHOS_URL_TRUST_LIST
  int SetUrlTrustListWithErrMsg(
    const std::string& urlTrustList, std::string& detailErrMsg) override;
#endif

#ifdef OHOS_NETWORK_LOAD
  void SetPathAllowingUniversalAccess(
      const std::vector<std::string>& pathList) override;
#endif

#ifdef OHOS_MIXED_CONTENT
  void EnableMixedContentAutoUpgrades(bool enable) override;
  bool IsMixedContentAutoUpgradesEnabled() override;
#endif

#ifdef OHOS_BFCACHE
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override;
#endif

#ifdef OHOS_EX_REFRESH_IFRAME
  bool WebExtensionContextMenuIsIframe() override;
  void WebExtensionContextMenuReloadFocusedFrame() override;
#endif

   void SetPopupSurface(void* popupSurface) override;

 public:
  int argc_;
  const char** argv_;

 private:
  void RunMessageLoop();

  void InitializeCef(std::string url,
                     bool is_enhance_surface,
                     void* window,
                     bool popup
#if defined(OHOS_EX_DOWNLOAD)
                     ,
                     uint32_t nweb_id
#endif
#if defined(OHOS_INCOGNITO_MODE)
                     ,
                     bool incognito_mode
#endif
#if defined(OHOS_RENDER_PROCESS_SHARE)
                     ,
                     const std::string& shared_render_process_token
#endif

  );

  const CefRefPtr<CefBrowser> GetBrowser() const;
  void RequestVisitedHistory();
  bool HasBackgroundColorWithInit(int32_t& backgroundColor);
  void InitRichtextIdentifier();
#if defined(OHOS_API_INIT_WEB_ENGINE)
  void OnContextInitializeComplete(
      const std::string& url,
      void* windows
#if defined(OHOS_INCOGNITO_MODE)
      ,
      bool incognito_mode
#endif
#if defined(OHOS_RENDER_PROCESS_SHARE)
      ,
      const std::string& shared_render_process_token
#endif

  );
#endif  // defined(OHOS_API_INIT_WEB_ENGINE)
#if defined(OHOS_MSGPORT)
  void ConvertNWebMsgToCefValue(std::shared_ptr<NWebMessage> data,
                                CefRefPtr<CefValue> message);
#endif  // defined(OHOS_MSGPORT)
#ifdef OHOS_CA
  bool GetCertChainDerDataInner(CefRefPtr<CefX509Certificate> cert,
                                std::vector<std::string>& certChainData,
                                bool isSingleCert);
#endif
#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  void RegisterOnCreateNativeMediaPlayerListener(
      std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) override;
#endif // OHOS_CUSTOM_VIDEO_PLAYER

  std::shared_ptr<NWebCustomKeyboardHandlerImpl> GetCustomKeyboardHandler() const override {
    if (render_handler_) {
      return render_handler_->GetCustomKeyboardHandler();
    }
    return nullptr;
  }

  bool IsCustomKeyboard() const override {
    if (render_handler_) {
      return render_handler_->IsCustomKeyboard();
    }
    return true;
  }

  void SendAccessibilityHoverEvent(int x, int y) override;

#if defined(OHOS_DISPATCH_BEFORE_UNLOAD)
  bool NeedToFireBeforeUnloadOrUnloadEvents() override;
  void DispatchBeforeUnload() override;
#endif // OHOS_DISPATCH_BEFORE_UNLOAD

 private:
  content::BrowserAccessibilityManagerOHOS* GetAccessibilityManager();
  void AddAccessibilityNodeInfoAttributes(
      std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
      const content::BrowserAccessibilityOHOS* node) const;
  void AddAccessibilityNodeInfoRect(
      std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
      const content::BrowserAccessibilityOHOS* node) const;
  void AddAccessibilityNodeInfoCollection(
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
    const content::BrowserAccessibilityOHOS* node) const;
  std::shared_ptr<NWebAccessibilityNodeInfo>
    PopulateAccessibilityNodeInfo(content::BrowserAccessibilityOHOS* node);
  void AddAccessibilityNodeInfoActions(
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
    const content::BrowserAccessibilityOHOS* node) const;
  float GetViewPointHeight() const;
  int32_t GetArgumentByKey(const std::map<std::string, std::string>& actionArguments,
    const std::string& checkKey) const;

  float zoom_in_factor_ = 1.25f;
  float zoom_out_factor_ = 0.8f;
  float default_virtual_pixel_ratio_ = 2.0;
  float intial_scale_ = 0;
  bool has_requested_visited_history = false;
  CefRefPtr<NWebApplication> nweb_app_ = nullptr;
  CefRefPtr<NWebHandlerDelegate> handler_delegate_ = nullptr;
  CefRefPtr<NWebRenderHandler> render_handler_ = nullptr;
  std::shared_ptr<NWebEventHandler> event_handler_ = nullptr;
  std::shared_ptr<NWebPreferenceDelegate> preference_delegate_ = nullptr;
  std::shared_ptr<NWebFindDelegate> find_delegate_ = nullptr;
  std::unique_ptr<OHOS::NWeb::DisplayManagerAdapter> display_manager_adapter_ =
      nullptr;
  std::shared_ptr<OHOS::NWeb::DisplayScreenListener> display_listener_ =
      nullptr;
  int32_t display_listener_id_;
  // Members only accessed on the main thread.
  bool hidden_ = false;
  bool occluded_ = false;
  bool is_popup_ready_ = false;
#if defined(OHOS_COMPOSITE_RENDER) || defined(OHOS_PAGE_UP_DOWN)
  uint32_t width_ = 0;
  uint32_t height_ = 0;
#endif  // defined(OHOS_COMPOSITE_RENDER)
#if defined(OHOS_INPUT_EVENTS)
  uint32_t visible_width_ = 0;
  uint32_t visible_height_ = 0;
#endif

  uint32_t nweb_id_;

#if BUILDFLAG(IS_OHOS)
  std::map<std::string, std::shared_ptr<NWebNativeEmbedDataInfo>> embedDataInfo_;
#endif

  bool is_enhance_surface_ = false;
#if defined(OHOS_INPUT_EVENTS)
  bool is_onPause_ = false;
#endif  // defined(OHOS_INPUT_EVENTS)
  bool accessibility_state_ = false;
  bool is_discarded_ = false;
  std::string richtext_data_str_ = "";
  // The number of fingers that trigger the down event
  int  pressing_num_ = 0;
  std::shared_ptr<NWebAccessibilityEventCallback>
      accessibility_event_listener_ = nullptr;
};
}  // namespace OHOS::NWeb
#endif
