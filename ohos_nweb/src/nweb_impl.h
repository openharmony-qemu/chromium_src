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

#ifndef NWEB_IMPL_H
#define NWEB_IMPL_H

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <vector>
#include <EGL/eglplatform.h>
#include "capi/nweb_app_client_extension_callback.h"
#include "capi/nweb_download_delegate_callback.h"
#include "nweb.h"
#include "nweb_download_callback.h"
#include "nweb_errors.h"
#include "nweb_input_handler.h"
#include "nweb_inputmethod_handler.h"
#include "nweb_output_handler.h"
#if defined(OHOS_EX_SCREEN_CAPTURE)
#include "capi/nweb_screencapture_delegate_callback.h"
#endif  // defined(OHOS_EX_SCREEN_CAPTURE)

#if defined(OHOS_VIDEO_ASSISTANT)
#include "capi/nweb_statistic_callback.h"
#endif  // defined(OHOS_VIDEO_ASSISTANT)

struct OpenDevToolsParam;

namespace OHOS::NWeb {
class NWebImpl : public NWeb {
 public:
  explicit NWebImpl(uint32_t id);
  ~NWebImpl() override;

  bool Init(std::shared_ptr<NWebCreateInfo> create_info);
  void OnDestroy() override;

  /* event interface */
  void Resize(uint32_t width, uint32_t height, bool isKeyboard = false) override;
  void ResizeVisibleViewport(uint32_t width, uint32_t height, bool isKeyboard) override;
  void OnTouchPress(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchRelease(int32_t id,
                      double x,
                      double y,
                      bool from_overlay) override;
  void OnTouchMove(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                   bool fromOverlay = false) override;
  void OnTouchCancel() override;
  void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay) override;
  void OnNavigateBack() override;
  bool SendKeyEvent(int32_t keyCode, int32_t keyAction) override;
  void SendTouchpadFlingEvent(double x, double y, double vx, double vy) override;
  void SendMouseWheelEvent(double x,
                           double y,
                           double deltaX,
                           double deltaY) override;
  void SendMouseEvent(int x, int y, int button, int action, int count) override;
  void FillAutofillData(std::shared_ptr<NWebMessage> data) override;
  void OnAutofillCancel(const std::string& fillContent) override;

  // public api
  int Load(const std::string& url) override;
  bool IsNavigatebackwardAllowed() override;
  bool IsNavigateForwardAllowed() override;
  bool CanNavigateBackOrForward(int numSteps) override;
  void NavigateBack() override;
  void NavigateForward() override;
  void NavigateBackOrForward(int step) override;
  void DeleteNavigateHistory() override;
  void ClearSslCache() override;
  void ClearClientAuthenticationCache() override;
  void Reload() override;
  int Zoom(float zoomFactor) override;
  int ZoomIn() override;
  int ZoomOut() override;
  void Stop() override;
  void ExecuteJavaScript(const std::string& code) override;
  void PutBackgroundColor(int color) override;
  void InitialScale(float scale) override;
  void PutOptimizeParserBudgetEnabled(bool enable) override;
  void OnPause() override;
  void OnContinue() override;
  void WebComponentsBlur() override;
  void OnOccluded() override;
  void OnUnoccluded() override;
  void SetEnableLowerFrameRate(bool enabled) override;
  std::shared_ptr<NWebPreference> GetPreference() override;
  void PutDownloadCallback(
      std::shared_ptr<NWebDownloadCallback> downloadListener) override;
  void PutReleaseSurfaceCallback(std::shared_ptr<NWebReleaseSurfaceCallback>
                                     releaseSurfaceListener) override;
  void SetNWebHandler(std::shared_ptr<NWebHandler> handler) override;
  std::string Title() override;
  uint32_t GetWebId() override;
  std::shared_ptr<HitTestResult> GetHitTestResult() override;
  int PageLoadProgress() override;
  int ContentHeight() override;
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

  void RegisterNativeArkJSFunction(
      const char* objName,
      const std::vector<std::shared_ptr<NWebJsProxyCallback>> &callbacks) override;
  using NativeJSProxyCallbackFunc = std::function<char*(std::vector<std::vector<uint8_t>>&, std::vector<size_t>&)>;
  using NativeJSProxyCallbackFuncWithResult = std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
      std::vector<std::vector<uint8_t>>&, std::vector<size_t>&)>;
  void RegisterNativeArkJSFunction(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<NativeJSProxyCallbackFunc>&& callback,
      bool isAsync,
      const std::string& permission);
  void RegisterNativeArkJSFunctionWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<NativeJSProxyCallbackFuncWithResult>&& callback,
      bool isAsync,
      const std::string& permission);
  void UnRegisterNativeArkJSFunction(const char* objName) override;
  void RegisterNativeValideCallback(const char* webName, const NativeArkWebOnValidCallback callback) override;
  void RegisterNativeDestroyCallback(const char* webName, const NativeArkWebOnDestroyCallback callback) override;
  void RegisterNativeDestroyCallback(std::function<void(void)>&& callback);
  void RegisterNativeLoadStartCallback(std::function<void(void)>&& callback);
  void RegisterNativeLoadEndCallback(std::function<void(void)>&& callback);

  void RegisterArkJSfunction(const std::string& object_name,
                             const std::vector<std::string>& method_list,
                             const int32_t object_id) override;
  void RegisterArkJSfunction(const std::string& object_name,
                             const std::vector<std::string>& method_list,
                             const std::vector<std::string>& async_method_list,
                             const int32_t object_id) override;
  void RegisterArkJSfunction(const std::string& object_name,
                             const std::vector<std::string>& method_list,
                             const std::vector<std::string>& async_method_list,
                             const int32_t object_id,
                             const std::string& permission) override;
  void UnregisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) override;
  void CallH5Function(
      int32_t routing_id,
      int32_t h5_object_id,
      const std::string& h5_method_name,
      const std::vector<std::shared_ptr<NWebValue>>& args) override;
  void SetNWebJavaScriptResultCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback) override;
  void OnFocus(const FocusReason& focusReason =
                   FocusReason::FOCUS_DEFAULT) override;
  void OnBlur(const BlurReason& blurReason) override;
  void StoreWebArchive(
      const std::string& base_name,
      bool auto_name,
      std::shared_ptr<NWebStringValueCallback> callback) override;

  void PutFindCallback(std::shared_ptr<NWebFindCallback> findListener) override;
  void FindAllAsync(const std::string& search_string) override;
  void ClearMatches() override;
  void FindNext(const bool forward) override;
  const std::string GetOriginalUrl() override;
  bool GetFavicon(const void** data,
                  size_t& width,
                  size_t& height,
                  ImageColorType& colorType,
                  ImageAlphaType& alphaType) override;
  void PutNetworkAvailable(bool available) override;
  void SendDragEvent(std::shared_ptr<NWebDragEvent> dragEvent) override;
  void UpdateLocale(const std::string& language,
                    const std::string& region) override;

  void HasImages(std::shared_ptr<NWebBoolValueCallback> callback) override;
  void RemoveCache(bool include_disk_files) override;
  void PutAccessibilityEventCallback(
      std::shared_ptr<NWebAccessibilityEventCallback>
          accessibilityEventListener) override;
  void PutAccessibilityIdGenerator(
      const AccessibilityIdGenerateFunc accessibilityIdGenerator) override;
  void ExecuteAction(int64_t accessibilityId, uint32_t action) override;
  std::shared_ptr<NWebAccessibilityNodeInfo> GetFocusedAccessibilityNodeInfo(
      int64_t accessibilityId,
      bool isAccessibilityFocus) override;
  std::shared_ptr<NWebAccessibilityNodeInfo> GetAccessibilityNodeInfoById(
      int64_t accessibilityId) override;
  bool GetAccessibilityVisible(int64_t accessibilityId) override;
  std::shared_ptr<NWebAccessibilityNodeInfo> GetAccessibilityNodeInfoByFocusMove(
      int64_t accessibilityId,
      int32_t direction) override;
  void SetAccessibilityState(bool state) override;
  void SuggestionSelected(int index) override;
  void PutSpanstringConvertHtmlCallback(std::shared_ptr<NWebSpanstringConvertHtmlCallback> callback) override;
#ifdef OHOS_SCREEN_LOCK
  void RegisterScreenLockFunction(int32_t windowId,
                                  std::shared_ptr<NWebScreenLockCallback> callback) override;
  void UnRegisterScreenLockFunction(int32_t windowId) override;
#endif  // #ifdef OHOS_SCREEN_LOCK

#ifdef OHOS_ARKWEB_ADBLOCK
  static void AddAdsBlockDisallowList(
      const std::vector<std::string>& domain_suffixes);
  static void AddAdsBlockAllowList(
      const std::vector<std::string>& domainSuffixes);
  static void SetAdsBlockRules(const std::string& rulesFiles,
                               const bool replace);
  static void RemoveAdsBlockDisallowedList(
      const std::vector<std::string>& domainSuffixes);
  static void RemoveAdsBlockAllowedList(
      const std::vector<std::string>& domainSuffixes);
  static void ClearAdsBlockDisallowedList();
  static void ClearAdsBlockAllowedList();
  void EnableAdsBlock(bool enable) override;
  bool IsAdsBlockEnabled() override;
  bool IsAdsBlockEnabledForCurPage() override;
  static bool IsAnyNWebAdblockEnabled();
  void UpdateAdblockEasyListRules(long adBlockEasyListVersion);
  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                int main_frame_tree_node_id);
#endif

#if defined(OHOS_EX_PASSWORD)
  void SetSavePasswordAutomatically(bool enable) const;
  bool GetSavePasswordAutomatically() const;
  void SetSavePassword(bool enable) const;
  bool GetSavePassword() const;
  void SaveOrUpdatePassword(bool is_update);
#endif
#if defined(OHOS_EX_PASSWORD) || (OHOS_DATALIST)
  void PasswordSuggestionSelected(int list_index) const;
#endif
#if defined(OHOS_COMPOSITE_RENDER)
  void SetShouldFrameSubmissionBeforeDraw(bool should) override;
  void SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height) override;
  void SetDrawMode(int32_t mode) override;
  bool GetPendingSizeStatus() override;
  void SetFitContentMode(int32_t mode) override;
#endif  // defined(OHOS_COMPOSITE_RENDER)

  void ExecuteJavaScriptExt(
      const int fd,
      const size_t scriptLength,
      std::shared_ptr<NWebMessageValueCallback> callback,
      bool extention) override;
  void ExecuteCreatePDFExt(
      std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
      std::shared_ptr<NWebArrayBufferValueCallback> callback) override;


#if defined(OHOS_MSGPORT)
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
      std::shared_ptr<NWebMessageValueCallback> callback) override;
#endif  // defined(OHOS_MSGPORT)
#ifdef OHOS_EX_BLANK_TARGET_POPUP_INTERCEPT
  void SetEnableBlankTargetPopupIntercept(bool enableBlankTargetPopup) const;
#endif

  void SetAutofillCallback(std::shared_ptr<NWebMessageValueCallback> callback) override;
  std::shared_ptr<NWebHistoryList> GetHistoryList() override;
  std::vector<uint8_t> SerializeWebState() override;
  bool RestoreWebState(const std::vector<uint8_t>& state) override;
#ifdef OHOS_PAGE_UP_DOWN
  void PageUp(bool top) override;
  void PageDown(bool bottom) override;
#ifdef OHOS_GET_SCROLL_OFFSET
  void GetScrollOffset(float* offset_x, float* offset_y) override;
#endif
#endif  // #endif OHOS_PAGE_UP_DOWN

#if defined(OHOS_INPUT_EVENTS)
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
  bool SendKeyboardEvent(const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent) override;
  bool ScrollByWithResult (float delta_x, float delta_y) override;
  void WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) override;
#endif  // defined(OHOS_INPUT_EVENTS)

  bool GetCertChainDerData(std::vector<std::string>& certChainData,
                           bool isSingleCert) override;
  void SetScreenOffSet(double x, double y) override;
  void SetAudioMuted(bool muted) override;
  void SetAudioResumeInterval(int32_t resumeInterval) override;
  void CloseAllMediaPresentations() override;
  void StopAllMedia() override;
  void ResumeAllMedia() override;
  void PauseAllMedia() override;
  int GetMediaPlaybackState() override;
  void SetAudioExclusive(bool audioExclusive) override;
  void NotifyMemoryLevel(int32_t level) override;
  void OnWebviewHide() override;
  void OnWebviewShow() override;
  void StartCamera() override;
  void StopCamera() override;
  void CloseCamera() override;

#if defined(OHOS_EX_SCREEN_CAPTURE)
  void StopScreenCapture(int32_t nweb_id, const char* session_id);
  void PutWebScreenCaptureDelegateCallback(
      std::shared_ptr<NWebScreenCaptureDelegateCallback> callback);
#endif // defined(OHOS_EX_SCREEN_CAPTURE)

  void OnRenderToBackground() override;
  void OnRenderToForeground() override;

  void OnOnlineRenderToForeground() override;

  void NotifyForNextTouchEvent() override;

  bool IsActivePolicyDisable() override;

  static void DisableWebActivePolicy();

  void SetDelayDurationForBackgroundTabFreezing(int64_t delay);

#ifdef OHOS_DRAG_DROP
  std::shared_ptr<NWebDragData> GetOrCreateDragData() override;
#endif // #ifdef OHOS_DRAG_DROP

#ifdef BUILDFLAG(IS_OHOS)
  bool TerminateRenderProcess() override;
#endif

#if BUILDFLAG(IS_OHOS)
  void SetWindowId(uint32_t window_id) override;
  void SetToken(void* token) override;
  void* CreateWebPrintDocumentAdapter(const std::string& jobName) override;
  void SetNestedScrollMode(const NestedScrollMode& nestedScrollMode) override;
  int GetSecurityLevel() override;
  void SetPrintBackground(bool enable) override;
  bool GetPrintBackground() override;
  bool IsSafeBrowsingEnabled() override;
  void EnableSafeBrowsing(bool enable) override;
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) const;
  void PrecompileJavaScript(const std::string& url,
                            const std::string& script,
                            std::shared_ptr<CacheOptions>& cacheOptions,
                            std::shared_ptr<NWebMessageValueCallback> callback) override;
  void InjectOfflineResource(const std::string& url,
                             const std::string& origin,
                             const std::vector<uint8_t>& resource,
                             const std::map<std::string, std::string>& responseHeaders,
                             const int type) override;
#endif

  std::string GetLastJavascriptProxyCallingFrameUrl() override;
  void DragResize(uint32_t width,
                  uint32_t height,
                  uint32_t pre_height,
                  uint32_t pre_width) override;

#if defined(OHOS_INPUT_EVENTS)
  void SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard) override;
  bool ShouldVirtualKeyboardOverlay() override;
#endif

#if defined(OHOS_NO_STATE_PREFETCH)
  void PrefetchPage(
      const std::string& url,
      const std::map<std::string, std::string>& additionalHttpHeaders) override;
#endif  // defined(OHOS_NO_STATE_PREFETCH)

  int PostUrl(const std::string& url, const std::vector<char>& postData) override;
  void JavaScriptOnDocumentStart(const ScriptItems& scriptItems) override;
  void JavaScriptOnDocumentEnd(const ScriptItems& scriptItems) override;
  void JavaScriptOnDocumentStartByOrder(const ScriptItems& scriptItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
  void JavaScriptOnDocumentEndByOrder(const ScriptItems& scriptItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
  // For NWebEx
  static NWebImpl* FromID(int32_t nweb_id);
  static std::shared_ptr<NWebImpl> GetNWebSharedPtr(int32_t nweb_id);
  std::string GetUrl() override;

  CefRefPtr<CefClient> GetCefClient() const {
    return nweb_delegate_ ? nweb_delegate_->GetCefClient() : nullptr;
  }
  void AddNWebToMap(uint32_t id, std::shared_ptr<NWebImpl>& nweb);
#if defined (OHOS_NWEB_EX)
  bool CanStoreWebArchive() const;
  static const std::vector<std::string>& GetCommandLineArgsForNWebEx();
  static void InitBrowserServiceApi(std::vector<std::string>& browser_args);
  static bool GetBrowserServiceApiEnabled();

  void PutWebAppClientExtensionCallback(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener);
  void RemoveWebAppClientExtensionCallback();

  void ReloadOriginalUrl() const;
  void SetBrowserUserAgentString(const std::string& user_agent);
  void OpenDevtools(std::unique_ptr<OpenDevToolsParam> param);
  void CloseDevtools();
#endif  // defined(OHOS_NWEB_EX)

#if defined(OHOS_VIDEO_ASSISTANT)
  void EnableVideoAssistant(bool enable);
  void ExecuteVideoAssistantFunction(const std::string& cmd_id);
  static void OnReportStatisticLog(const std::string& content);
  static void SetOnReportStatisticLogCallback(OnReportStatisticLogFunc func);
#endif  // defined(OHOS_VIDEO_ASSISTANT)

#ifdef OHOS_EX_NETWORK_CONNECTION
  static void SetConnectTimeout(int32_t seconds);
#endif

#ifdef OHOS_EX_UA
  static void UpdateCloudUAConfig(const std::string& file_path,
                                  const std::string& version);
  static void UpdateUAListConfig(const std::string& ua_name,
                                 const std::string& ua_string);
  static void SetUAForHosts(const std::string& ua_name,
                            const std::vector<std::string>& hosts);
  static std::string GetUANameConfig(const std::string& host);
  static void SetBrowserUA(const std::string& ua_name);
#endif  // OHOS_EX_UA

#if defined(OHOS_VIDEO_ASSISTANT)
  static void UpdateBrowserEngineConfig(const std::string& file_path, const std::string& version);
#endif

#if defined(OHOS_EX_FORCE_ZOOM)
  void SetForceEnableZoom(bool forceEnableZoom) const;
  bool GetForceEnableZoom() const;
#endif  // OHOS_EX_FORCE_ZOOM

  void PutWebDownloadDelegateCallback(
      std::shared_ptr<NWebDownloadDelegateCallback>);
  void StartDownload(const char* url);
  void ResumeDownload(std::shared_ptr<NWebDownloadItem>);
  static void ResumeDownloadStatic(std::shared_ptr<NWebDownloadItem> download_item);
  static void SetFileRenameOption(const int file_rename_option);
#ifdef OHOS_EX_DOWNLOAD
  NWebDownloadItemState GetDownloadItemState(long item_id);
  static NWebDownloadItemState GetDownloadItemStateByGuid(const std::string& guid);
#endif

  bool Discard() override;
  bool Restore() override;

  bool NeedSoftKeyboard() override;

  static std::shared_ptr<NWeb> GetNWeb(int32_t nweb_id);
  static std::shared_ptr<NWeb> CreateNWeb(std::shared_ptr<NWebCreateInfo> create_info);
  static void SetWebTag(int32_t nweb_id, const char* web_tag);
  static void InitializeWebEngine(std::shared_ptr<NWebEngineInitArgs> init_args);
  static void PrepareForPageLoad(const std::string &url, bool preconnectable, int32_t num_sockets);
  static void PauseAllTimers();
  static void ResumeAllTimers();
  static void PrefetchResource(const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
                               const std::map<std::string, std::string>& additional_http_headers,
                               const std::string& cache_key,
                               const uint32_t& cache_valid_time);
  static void ClearPrefetchedResource(const std::vector<std::string>& cache_key_list);

#if defined(OHOS_COOKIE)
  static bool InitializeICUStatic(std::shared_ptr<NWebEngineInitArgs> init_args);
#endif // defined(OHOS_COOKIE)

#if defined(OHOS_MULTI_WINDOW)
  void NotifyPopupWindowResult(bool result) override {
    nweb_delegate_->NotifyPopupWindowResult(result);
  }
#endif  // defined(OHOS_MULTI_WINDOW)

#if defined(OHOS_EX_FREE_COPY)
  void SelectAndCopy() const;
  bool ShouldShowFreeCopy() const;
#endif  // defined(OHOS_EX_FREE_COPY)

#if defined(OHOS_EX_TOPCONTROLS)
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) const;
  void UpdateBrowserControlsHeight(int height, bool animate);
#endif

#ifdef OHOS_EX_REFRESH_IFRAME
  bool WebExtensionContextMenuIsIframe();
  void WebExtensionContextMenuReloadFocusedFrame();
#endif

#ifdef OHOS_EX_GET_ZOOM_LEVEL
  static void SetDefaultBrowserZoomLevel(double zoom_factor);
  void SetBrowserZoomLevel(double zoom_factor) const;
  double GetBrowserZoomLevel() const;
#endif

#ifdef OHOS_CRASHPAD
  static void SetDefaultCrashpadLogPath(const std::string& crashpad_log_path);
  static const std::string GetDefaultCrashpadLogPath();
#endif

#if defined(OHOS_INCOGNITO_MODE)
  bool IsIncognitoMode() override {
    return incognito_mode_;
  }
#endif

#if defined(OHOS_SCHEME_HANDLER)
  void SetWebTag(const std::string& web_tag) { web_tag_ = web_tag; }
  std::string GetWebTag() { return web_tag_; }
#endif

#ifdef OHOS_ITP
  void EnableIntelligentTrackingPrevention(bool enable) override;
  bool IsIntelligentTrackingPreventionEnabled() const override;
  static bool IsAnyNWebIntelligentTrackingPreventionEnabled();
#endif

#if defined(OHOS_CLIPBOARD)
  std::string GetSelectInfo() override;
#endif
  void OnCreateNativeMediaPlayer(
      std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) override;

  static void AddIntelligentTrackingPreventionBypassingList(
      const std::vector<std::string>& hosts);
  static void RemoveIntelligentTrackingPreventionBypassingList(
      const std::vector<std::string>& hosts);
  static void ClearIntelligentTrackingPreventionBypassingList();
  static std::string GetDefaultUserAgent();
  static void WarmupServiceWorker(const std::string &url);

  int ScaleGestureChange(double scale, double centerX, double centerY) override;
  int ScaleGestureChangeV2(int type, double scale, double originScale, double centerX, double centerY) override;
  static void SetHostIP(const std::string &hostName, const std::string &address, int32_t aliveTime);
  static void ClearHostIP(const std::string &hostName);

#ifdef OHOS_RENDER_PROCESS_MODE
  static void SetRenderProcessMode(RenderProcessMode mode);
  static RenderProcessMode GetRenderProcessMode();
#endif

#ifdef OHOS_DISPLAY_CUTOUT
  void OnSafeInsetsChange(int left, int top, int right, int bottom) override;
#endif

#ifdef OHOS_AI
  void OnTextSelected() override;
#endif
  static base::Lock nweb_map_lock_;

#if defined(OHOS_SOFTWARE_COMPOSITOR)
  static void EnableWholeWebPageDrawing();

  bool WebPageSnapshot(const char* id,
                       PixelUnit type,
                       int width,
                       int height,
                       const WebSnapshotCallback callback) override;
#endif
  int SetUrlTrustList(const std::string& urlTrustList) override;
  int SetUrlTrustListWithErrMsg(
    const std::string& urlTrustList, std::string& detailErrMsg) override;

#ifdef OHOS_NETWORK_LOAD
  void SetPathAllowingUniversalAccess(
    const std::vector<std::string>& pathList,
    const std::vector<std::string>& moduleName,
    std::string& errorPath) override;
#endif
  void PerformAction(int64_t accessibilityId, uint32_t action,
      const std::map<std::string, std::string>& actionArguments) override;
  void SendAccessibilityHoverEvent(int32_t x, int32_t y) override;

  static void TrimMemoryByPressureLevel(int32_t memoryLevel);

#ifdef OHOS_BFCACHE
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override;
#endif

#ifdef OHOS_MIXED_CONTENT
  void EnableMixedContentAutoUpgrades(bool enable);
  bool IsMixedContentAutoUpgradesEnabled();
#endif

 void SetPopupSurface(void* popupSurface) override;
 void SetTransformHint(uint32_t rotation) override;

#ifdef BUILDFLAG(IS_OHOS)
  void OnConfigurationUpdated(
      std::shared_ptr<NWebSystemConfiguration> configuration) override;
#endif
#if defined(OHOS_DISPATCH_BEFORE_UNLOAD)
 bool NeedToFireBeforeUnloadOrUnloadEvents();
 void DispatchBeforeUnload();
#endif // OHOS_DISPATCH_BEFORE_UNLOAD

 private:
  void ProcessInitArgs(std::shared_ptr<NWebEngineInitArgs> init_args);
  void InitWebEngineArgs(std::shared_ptr<NWebEngineInitArgs> init_args);
  bool InitWebEngine(std::shared_ptr<NWebCreateInfo> create_info);
  bool SetVirtualDeviceRatio();
#ifdef OHOS_I18N
  void UpdateAcceptLanguageInternal();
#endif
#if defined(OHOS_WEBRTC)
  void StopCameraSession() const;
  void RestartCameraSession() const;
#endif // defined(OHOS_WEBRTC)

  std::function<void(const char*)> validCallback_ = nullptr;
  std::function<void(const char*)> destroyCallback_ = nullptr;
  std::function<void(void)> nativeDestroyCallback_ = nullptr;
  std::string webName_;
  base::Lock state_lock_;

  uint32_t nweb_id_ = 0;
  int32_t draw_mode_ = 0;
  std::shared_ptr<NWebHandler> nweb_handle_ = nullptr;
  std::shared_ptr<NWebOutputHandler> output_handler_ = nullptr;
  std::shared_ptr<NWebInputHandler> input_handler_ = nullptr;
  CefRefPtr<NWebInputMethodHandler> inputmethod_handler_ = nullptr;

  std::shared_ptr<NWebDelegateInterface> nweb_delegate_ = nullptr;
  std::list<std::string> web_engine_args_;
  float device_pixel_ratio_ = 0.f;
  bool is_enhance_surface_ = false;
  bool is_richtext_value_ = false;

  bool incognito_mode_ = false;
  EGLNativeWindowType window_;
#if defined(OHOS_SCHEME_HANDLER)
  std::string web_tag_{""};
#endif

#if defined(OHOS_VIDEO_ASSISTANT)
  static OnReportStatisticLogFunc on_report_statistic_log_callback_;
#endif  // defined(OHOS_VIDEO_ASSISTANT)
  static bool disableWebActivePolicy_;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_IMPL_H
