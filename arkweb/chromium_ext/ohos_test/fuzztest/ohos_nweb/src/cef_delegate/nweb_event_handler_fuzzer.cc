/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/cef_delegate/nweb_event_handler.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "cef/include/cef_devtools_message_handler_delegate.h"
#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

class MockCefBrowserHost : public ArkWebBrowserHostExt {
 public:
  bool CreateBrowser(const CefWindowInfo& windowInfo,
                     CefRefPtr<CefClient> client,
                     const CefString& url,
                     const CefBrowserSettings& settings,
                     CefRefPtr<CefDictionaryValue> extra_info,
                     CefRefPtr<CefRequestContext> request_context) {
    return false;
  }

  CefRefPtr<CefBrowser> CreateBrowserSync(
      const CefWindowInfo& windowInfo,
      CefRefPtr<CefClient> client,
      const CefString& url,
      const CefBrowserSettings& settings,
      CefRefPtr<CefDictionaryValue> extra_info,
      CefRefPtr<CefRequestContext> request_context) {
    return nullptr;
  }

  CefRefPtr<CefBrowser> GetBrowser() override { return nullptr; }

  void CloseBrowser(bool force_close) override {}

  void SetDisallowSandboxFileAccessFromFileUrl(bool disallow) override {}

  bool TryCloseBrowser() override { return false; }

  void SetFocus(bool) override {}

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  void SetBlankScreenDetectionConfig( 
      bool enable,
      const std::vector<double>& detectionTiming,
      const std::vector<int32_t>& detectionMethods,
      int32_t contentfulNodesCountThreshold) override {}
#endif

  CefWindowHandle GetWindowHandle() override { return 0; }
  CefWindowHandle GetOpenerWindowHandle() override { return 0; }

  bool HasView() override { return false; }

  CefRefPtr<CefClient> GetClient() override { return nullptr; }

  CefRefPtr<CefRequestContext> GetRequestContext() override { return nullptr; }

  double GetZoomLevel() override { return 0.0; }

  void SetZoomLevel(double zoomLevel) override {}

  void RunFileDialog(FileDialogMode mode,
                     const CefString& title,
                     const CefString& default_file_path,
                     const std::vector<CefString>& accept_filters,
                     CefRefPtr<CefRunFileDialogCallback> callback) override {}

  void StartDownload(const CefString& url) override {}

  void DownloadImage(const CefString& image_url,
                     bool is_favicon,
                     uint32_t max_image_size,
                     bool bypass_cache,
                     CefRefPtr<CefDownloadImageCallback> callback) override {}

  void Print() override {}

  void PrintToPDF(const CefString& path,
                  const CefPdfPrintSettings& settings,
                  CefRefPtr<CefPdfPrintCallback> callback) override {}

  void Find(const CefString& searchText,
            bool forward,
            bool matchCase,
            bool findNext) override {}

  void StopFinding(bool clearSelection) override {}

  void ShowDevTools(const CefWindowInfo& windowInfo,
                    CefRefPtr<CefClient> client,
                    const CefBrowserSettings& settings,
                    const CefPoint& inspect_element_at) override {}

  void CloseDevTools() override {}

  bool HasDevTools() override { return false; }

  bool SendDevToolsMessage(const void* message, size_t message_size) override {
    return false;
  }

  int ExecuteDevToolsMethod(int message_id,
                            const CefString& method,
                            CefRefPtr<CefDictionaryValue> params) override {
    return 0;
  }

  CefRefPtr<CefRegistration> AddDevToolsMessageObserver(
      CefRefPtr<CefDevToolsMessageObserver> observer) override {
    return nullptr;
  }

  void GetNavigationEntries(CefRefPtr<CefNavigationEntryVisitor> visitor,
                            bool current_only) override {}

  void ReplaceMisspelling(const CefString& word) override {}

  void AddWordToDictionary(const CefString& word) override {}

  bool IsWindowRenderingDisabled() override { return false; }

  void WasResized() override {}

  void WasHidden(bool hidden) override {}

  void WasOccluded(bool occluded) override {}

  void SetIsOfflineWebComponent() override {}

  void OnWindowShow() override {}

  void OnWindowHide() override {}

  void OnOnlineRenderToForeground() override {}

  void SendTouchEventList(const std::vector<CefTouchEvent>&) override {}

  void NotifyScreenInfoChanged() override {}

  void Invalidate(PaintElementType type) override {}

  void SendExternalBeginFrame() override {}

  void SendKeyEvent(const CefKeyEvent& event) override {}

  void SendMouseClickEvent(const CefMouseEvent&,
                           cef_mouse_button_type_t,
                           bool,
                           int) override {}

  void SendMouseMoveEvent(const CefMouseEvent&, bool) override {}

  void SendMouseWheelEvent(const CefMouseEvent&, int, int) override {}

  void SendTouchEvent(const CefTouchEvent&) override {}

  void SendCaptureLostEvent() override {}

  void NotifyMoveOrResizeStarted() override {}

  int GetWindowlessFrameRate() override { return 0; }

  void SetWindowlessFrameRate(int frame_rate) override {}

  void ImeSetComposition(const CefString& text,
                         const std::vector<CefCompositionUnderline>& underlines,
                         const CefRange& replacement_range,
                         const CefRange& selection_range) override {}

  void ImeCommitText(const CefString& text,
                     const CefRange& replacement_range,
                     int relative_cursor_pos) override {}

  void ImeFinishComposingText(bool keep_selection) override {}

  void ImeCancelComposition() override {}

  void DragTargetDragEnter(CefRefPtr<CefDragData> drag_data,
                           const CefMouseEvent& event,
                           DragOperationsMask allowed_ops) override {}

  void DragTargetDragOver(const CefMouseEvent& event,
                          DragOperationsMask allowed_ops) override {}

  void DragTargetDragLeave() override {}

  void DragTargetDrop(const CefMouseEvent& event) override {}

  void DragSourceEndedAt(int x, int y, DragOperationsMask op) override {}

  void DragSourceSystemDragEnded() override {}

  CefRefPtr<CefNavigationEntry> GetVisibleNavigationEntry() override {
    return nullptr;
  }

  void SetAccessibilityState(cef_state_t accessibility_state) override {}

  void SetAutoResizeEnabled(bool enabled,
                            const CefSize& min_size,
                            const CefSize& max_size) override {}

  void SetAudioMuted(bool mute) override {}

  bool IsAudioMuted() override { return false; }
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
#if BUILDFLAG(IS_OHOS)
  void GetRootBrowserAccessibilityManager(void** manager) override {}

  void ExecuteJavaScript(const std::string& code,
                         CefRefPtr<CefJavaScriptResultCallback> callback,
                         bool extention) override {}

  void ExecuteJavaScriptExt(const int fd,
                            const uint64_t scriptLength,
                            CefRefPtr<CefJavaScriptResultCallback> callback,
                            bool extention) override {}

  void SetNativeWindow(cef_native_window_t window) override {}

  void SetWebDebuggingAccess(bool isEnableDebug) override {}

  bool GetWebDebuggingAccess() override { return false; }

  void GetImageForContextNode(CefRefPtr<CefFrame> frame,
                              int command_id) override {}

  void GetImageFromCache(const CefString& url, int command_id) override {}

  void GetImageFromCacheEx(const CefString& url, int command_id) override {}

  void ExitFullScreen() override {}

  void UpdateLocale(const CefString& locale) override {}

  CefString GetOriginalUrl() override { return CefString(); }

  void PutNetworkAvailable(bool available) override {}

  void RemoveCache(bool include_disk_files) override {}

  void PostTaskToUIThread(CefRefPtr<CefTask> task) override {}

  void SetVirtualPixelRatio(float ratio) override {}

  float GetVirtualPixelRatio() override { return 0.0f; }

  void SetWebPreferences(const CefBrowserSettings& browser_settings) override {}

  void PutUserAgent(const CefString& ua) override {}

  CefString DefaultUserAgent() override { return CefString(); }

  void SetBackgroundColor(int color) override {}

  void UpdateAdblockEasyListRules(long adBlockEasyListVersion) override {}

  void RegisterArkJSfunction(const CefString& object_name,
                             const std::vector<CefString>& method_list,
                             const std::vector<CefString>& async_method_list,
                             const int32_t object_id,
                             const CefString& permission) override {}

  void UnregisterArkJSfunction(
      const CefString& object_name,
      const std::vector<CefString>& method_list) override {}

  void CallH5Function(int32_t routing_id,
                      int32_t h5_object_id,
                      const CefString& h5_method_name,
                      const std::vector<CefRefPtr<CefValue>>& args) override {}

  void StoreWebArchive(
      const CefString& base_name,
      bool auto_name,
      CefRefPtr<CefStoreWebArchiveResultCallback> callback) override {}

  void WasKeyboardResized() override {}

  void SetEnableLowerFrameRate(bool enabled) override {}
  /* ---------- ohos webview add end --------- */

  CefString Title() override { return CefString(); }

  void CreateWebMessagePorts(std::vector<CefString>& ports) override {}

  void PostWebMessage(CefString& message,
                      std::vector<CefString>& ports,
                      CefString& targetUri) override {}

  void ClosePort(const CefString& port_handle) override {}

  void DestroyAllWebMessagePorts() override {}

  void PostPortMessage(const CefString& port_handle,
                       CefRefPtr<CefValue> message) override {}

  void SetPortMessageCallback(
      const CefString& port_handle,
      CefRefPtr<CefWebMessageReceiver> callback) override {}

  void GetHitData(int& type, CefString& extra_data) override {}

  void SetInitialScale(float scale) override {}

  int PageLoadProgress() override { return 0; }

  float Scale() override { return 0.0f; }

  void LoadWithDataAndBaseUrl(const CefString& baseUrl,
                              const CefString& data,
                              const CefString& mimeType,
                              const CefString& encoding,
                              const CefString& historyUrl) override {}

  void LoadWithData(const CefString& data,
                    const CefString& mimeType,
                    const CefString& encoding) override {}

  void AddVisitedLinks(const std::vector<CefString>& urls) override {}

  void ResumeDownload(const CefString& url,
                      const CefString& full_path,
                      int64_t received_bytes,
                      int64_t total_bytes,
                      const CefString& etag,
                      const CefString& mime_type,
                      const CefString& last_modified,
                      const CefString& received_slices_string) override {}
  void SetAudioResumeInterval(int resumeInterval) override {}

  void SetAudioExclusive(bool audioExclusive) override {}

  void SetAudioSessionType(int audioSessionType) override {}

  void CloseMedia() override {}

  void StopMedia() override {}

  void ResumeMedia() override {}

  void PauseMedia() override {}

  int GetMediaPlaybackState() override { return 0; }

  void ScrollPageUpDown(bool is_up, bool is_half, float view_height) override {}

  CefRefPtr<CefBinaryValue> GetWebState() override { return nullptr; }

  bool RestoreWebState(const CefRefPtr<CefBinaryValue> state) override {
    return false;
  }

  void ScrollTo(float x, float y) override {}

  void ScrollBy(float delta_x, float delta_y) override {}

  void SlideScroll(float vx, float vy) override {}

  void SetFileAccess(bool falg) override {}

  void SetBlockNetwork(bool falg) override {}

  void SetCacheMode(int falg) override {}

  void SetShouldFrameSubmissionBeforeDraw(bool should) override {}

  void ZoomBy(float delta, float width, float height) override {}

  void SetWindowId(int window_id, int nweb_id) override {}

  void SetToken(void* token) override {}

  void SetVirtualKeyBoardArg(int32_t width,
                             int32_t height,
                             double keyboard) override {}

  bool ShouldVirtualKeyboardOverlay() override { return false; }

  void JavaScriptOnDocumentStart(const CefString& script,
                                 const std::vector<CefString>& script_rules,
                                 bool is_transfer_finished) override {}

  void RemoveJavaScriptOnDocumentStart() override {}

  void JavaScriptOnDocumentEnd(const CefString& script,
                               const std::vector<CefString>& script_rules,
                               bool is_transfer_finished) override {}

  void RemoveJavaScriptOnDocumentEnd() override {}

#if BUILDFLAG(ARKWEB_UNITTESTS)
  void OnDataDetectorSelectText() override {}
  std::string GetDataDetectorSelectText() override { return std::string(); }
  void SetBypassVsyncCondition(int32_t condition) override {}
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  void JavaScriptOnHeadReady(const CefString& script,
                             const std::vector<CefString>& script_rules,
                             bool is_transfer_finished) override {}

  void RemoveJavaScriptOnHeadReady() override {}
#endif

  void SetDrawRect(int x, int y, int width, int height) override {}

  void SetDrawMode(int mode) override {}

  void CreateWebPrintDocumentAdapter(const CefString& jobName,
                                     void** webPrintDocumentAdapter) override {}

  void SetOverscrollMode(int mode) override {}

  bool Discard() override { return false; }

  bool Restore() override { return false; }

  void SetBrowserZoomLevel(double zoomFactor) override {}

  int GetTopControlsOffset() override { return 0; }

  int GetShrinkViewportHeight() override { return 0; }

  void SetPrintBackground(bool enable) override {}

  bool GetPrintBackground() override { return false; }

  void SetScrollable(bool enable, int scrollType) override {}

  void StartCamera() override {}

  void StopCamera() override {}

  void CloseCamera() override {}

  CefString GetLastJavascriptProxyCallingFrameUrl() override {
    return CefString();
  }

  void SetNWebId(int nWebId) override {}

  bool GetPendingSizeStatus() override { return false; }

  void PrecompileJavaScript(
      const std::string& url,
      const std::string& script,
      CefRefPtr<CefCacheOptions> cacheOptions,
      CefRefPtr<CefPrecompileCallback> callback) override {}

  void SetWakeLockHandler(int32_t windowId,
                          CefRefPtr<CefSetLockCallback> callback) override {}

  CefRefPtr<CefDownloadItem> GetDownloadItem(uint32_t item_id) override {
    return nullptr;
  }

  bool NeedsReload() override { return false; }

  bool TerminateRenderProcess() override { return false; }

  void RegisterNativeJSProxy(const CefString& object_name,
                             const std::vector<CefString>& method_list,
                             const int32_t object_id,
                             bool is_async,
                             const CefString& permission) override {}

  void SendTouchpadFlingEvent(const CefMouseEvent&, double, double) override {}

  void SetFitContentMode(int mode) override {}

  void UpdateDrawRect() override {}

  void OnTextSelected(bool flag) override {}

  float GetPageScaleFactor() override { return 0.0f; }

  bool WebPageSnapshot(const char* id,
                       int width,
                       int height,
                       cef_web_snapshot_callback_t callback) override {
    return false;
  }

  void AdvanceFocusForIME(int focusType) override {}

  void ScrollToWithAnime(float x, float y, int32_t duration) override {}

  void ScrollByWithAnime(float delta_x,
                         float delta_y,
                         int32_t duration) override {}

#if defined(OHOS_GET_SCROLL_OFFSET)
  void GetScrollOffset(float* offset_x, float* offset_y) override {}

  void GetOverScrollOffset(float* offset_x, float* offset_y) override {}
#endif
#endif  // BUILDFLAG(IS_OHOS)

  void OnSafeInsetsChange(int left, int top, int right, int bottom) override {}

  void NotifyForNextTouchEvent() override {}

  void SetGrantFileAccessDirs(const std::vector<CefString>& dir_list,
                              const std::vector<CefString>& excluded_dir_list) override {
  }

  void SetAutofillCallback(CefRefPtr<CefWebMessageReceiver> callback) override {
  }

  void FillAutofillData(CefRefPtr<CefValue> message) override {}

  void ScrollFocusedEditableNodeIntoView() override {}

  void ProcessAutofillCancel(const CefString& fillContent) override {}

  void AutoFillWithIMFEvent(bool is_username,
                            bool is_other_account,
                            bool is_new_password,
                            const CefString& content) override {}
  void CreateToPDF(const CefPdfPrintSettings& settings,
                   CefRefPtr<CefPdfValueCallback> callback) override {}

  void SetPopupWindow(cef_native_window_t window) override {}

  bool IsReadyToBeClosed() override { return false; }

  int GetOpenerIdentifier() override { return 0; }

  bool CanZoom(cef_zoom_command_t command) override { return true; }

  void Zoom(cef_zoom_command_t command) override {}

  double GetDefaultZoomLevel() override { return 0.0f; }

  void ShowDevToolsWith(CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
                        CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate,
                        const CefPoint& inspect_element_at) override {}

  bool IsFullscreen() override { return false; }

  void ExitFullscreen(bool will_cause_resize) override {}

  bool CanExecuteChromeCommand(int command_id) override { return false; }

  void ExecuteChromeCommand(
      int command_id,
      cef_window_open_disposition_t disposition) override {}

  bool IsRenderProcessUnresponsive() override { return false; }

  cef_runtime_style_t GetRuntimeStyle() override {
    return CEF_RUNTIME_STYLE_DEFAULT;
  }

  void EnableVideoAssistant(bool enable) override { return false; }

  void EnableVideoAssistantAVCast(bool enable) override {}

  void ExecuteVideoAssistantFunction(const CefString& cmdId) override {}

  bool IsIframe() override { return false; }

  void ReloadFocusedFrame() override {}

  void StopScreenCapture(int32_t nweb_id,
                         const CefString& session_id) override {}

  void SetScreenCapturePickerShow() override {}

  void DisableSessionReuse() override {}

  void RegisterScreenCaptureDelegateListener(
      CefRefPtr<CefScreenCaptureCallback> listener) override {}

  void CustomWebMediaPlayer(bool enable) override {}

  bool IsValid() override { return false; }

  CefRefPtr<ArkWebBrowserHostExt> GetHost() override { return this; }
  bool CanGoBack() override { return false; }
  void GoBack() override {}
  bool CanGoForward() override { return false; }
  void GoForward() override {}
  bool IsLoading() override { return false; }
  void Reload() override {}
  void ReloadIgnoreCache() override {}
  void StopLoad() override {}
  int GetIdentifier() override { return 0; }
  bool IsSame(CefRefPtr<CefBrowser> that) override { return false; }
  bool IsPopup() override { return false; }
  bool HasDocument() override { return false; }
  CefRefPtr<CefFrame> GetMainFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFocusedFrame() override { return nullptr; }
  size_t GetFrameCount() override { return 0; }
  void GetFrameIdentifiers(std::vector<CefString>& identifiers) override {}
  void GetFrameNames(std::vector<CefString>& names) override {}
  CefRefPtr<CefBrowserPermissionRequestDelegate> GetPermissionRequestDelegate()
      override {
    return nullptr;
  }
  CefRefPtr<CefGeolocationAcess> GetGeolocationPermissions() override {
    return nullptr;
  }
#if BUILDFLAG(IS_OHOS)
  bool CanGoBackOrForward(int num_steps) override { return false; }
  void GoBackOrForward(int num_steps) override {}
  void DeleteHistory() override {}
  void PasswordSuggestionSelected(int list_index) override {}
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) override {}
  void UpdateBrowserControlsHeight(int height, bool animate) override {}
  void PrefetchPage(CefString& url, CefString& additionalHttpHeaders) override {
  }
  void ReloadOriginalUrl() override {}
  bool CanStoreWebArchive() override { return false; }
  void SetBrowserUserAgentString(const CefString& user_agent) override {}
  bool ShouldShowLoadingUI() override { return false; }
  void SetForceEnableZoom(bool forceEnableZoom) override {}
  bool GetForceEnableZoom() override { return false; }
  int GetNWebId() override { return 0; }
  bool GetSavePasswordAutomatically() override { return false; }
  void SetSavePasswordAutomatically(bool enable) override {}
  void SaveOrUpdatePassword(bool is_update) override {}
  bool GetSavePassword() override { return false; }
  void SetSavePassword(bool enable) override {}
  int GetSecurityLevel() override { return 0; }
  void EnableSafeBrowsing(bool enable) override {}
  bool IsSafeBrowsingEnabled() override { return false; }
  void EnableIntelligentTrackingPrevention(bool enable) override {}
  bool IsIntelligentTrackingPreventionEnabled() override { return false; }
  bool IsAdsBlockEnabled() override { return false; }
  bool IsAdsBlockEnabledForCurPage() override { return false; }
  void EnableAdsBlock(bool enable) override {}
  int SetUrlTrustListWithErrMsg(const CefString& urlTrustList,
      bool allowOpaqueOrigin, bool supportWildcard, CefString& detailErrMsg) override {
    return 0;
  }
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override {}
  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override {
    return nullptr;
  }
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
  void ShowFreeCopyMenu() override {}
  bool ShouldShowFreeCopyMenu() override { return false; }
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override {}
  int InsertBackForwardEntry(int index, const CefString& url) override {
    return 0;
  }
  int UpdateNavigationEntryUrl(int index, const CefString& url) override {
    return 0;
  }
  void ClearForwardList() override {}
  void ExtensionSetTabId(int tab_id) override {}
  int ExtensionGetTabId() const override { return 0; }
  uint32_t GetAcceleratedWidget(bool isPopup) override { return 0; }
  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                int main_frame_tree_node_id) override {}
  void FindEx(const CefString& searchText,
              bool forward,
              bool matchCase,
              bool findNext,
              bool newSession) override {}
  void SetFocusOnWeb() override {}
  void UpdateSecurityLayer(bool isNeedSecurityLayer) override {}
  CefString GetCustomUserAgent() override { return CefString(); }
  void GetLastHitData(int& type, CefString& extra_data) override {}
  std::string GetSelectedTextFromContextParam() override { return ""; }
  bool JudgeTextInputState() override { return true; }
  void SetNeedsReload(bool needs_reload) override {}
  void SetOptimizeParserBudgetEnabled(bool enable) override {}
  void OnDestroyImageAnalyzerOverlay() override {}
  void GetScrollOffset(float* offset_x, float* offset_y) override {}
  void GetOverScrollOffset(float* offset_x, float* offset_y) override {}
  void OnFoldStatusChanged(uint32_t foldStatus) override {}
  void SetNativeEmbedMode(bool flag) override {}
  void SetNativeInnerWeb(bool isInnerWeb) override {}
  void ScaleGestureChangeV2(int type,
                            float scale,
                            float originScale,
                            float width,
                            float height) override {}
  virtual std::string GetCurrentLanguage() override { return ""; }
  void MaximizeResize() override {}
  void PutWebMediaAVSessionEnabled(bool enable) override {}
  void SetEnableHalfFrameRate(bool enabled) override {}
  bool SetFocusByPosition(float x, float y) override { return false; }
  bool IsElementExist(CefString& xPath) override { return false; }
#if BUILDFLAG(ARKWEB_PIP)
  void SetPipNativeWindow(int delegate_id,
                          int child_id,
                          int frame_routing_id,
                          cef_native_window_t window) override {}
  void SendPipEvent(int delegate_id,
                    int child_id,
                    int frame_routing_id,
                    int event) override {}
#endif
  CefRefPtr<CefFrame> GetFrameByIdentifier(
      const CefString& identifier) override {
    return nullptr;
  }
#if BUILDFLAG(ARKWEB_NWEB_EX)
  void RunJavaScriptInFrames(
      const std::string& jsString,
      FrameInfos rootFrame,
      bool recursive,
      IsolatedWorld world,
      CefRefPtr<CefJavaScriptResultCallback> callback) override {}
void GetAllFrameInfos(CefRefPtr<CefFrameInfosCallback> callback) override {}
void GetLastJavaScriptProxyCallingFrameInfo(
    CefRefPtr<CefLastJavaScriptProxyCallingFrameInfoCallback> callback) override {}
#endif
#endif  // BUILDFLAG(IS_OHOS)
  int PrerenderPage(const CefString& url,
                    const CefString& additional_headers) override {
    return 0;
  }
  void CancelAllPrerendering() override {}
  void CreateWebPrintDocumentAdapterV2(const CefString& jobName,
                                       void** adapter) override {}
  void OnBrowserForeground() override {}
  void OnBrowserBackground() override {}
#if BUILDFLAG(ARKWEB_READER_MODE)
  void Distill(const std::string& guid, const DistillOptions& distill_options,
    CefRefPtr<CefDistillCallback> callback) override {}
  void AbortDistill() override {}
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_SAVE_PAGE)
  bool SavePage(int type,
                CefString& filePath,
                CefRefPtr<CefSavePageResultCallback> callback) override {
    return false;
  }
#endif // ARKWEB_SAVE_PAGE

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void GetFocusedFrameInfo(int32_t& frame_id, CefString& frame_url) override {}
#endif  // ARKWEB_ARKWEB_EXTENSIONS
#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  int LoadUrlWithParams(const std::string& url,
                        const LoadUrlType load_type,
                        const std::string& refer,
                        const std::string& headers,
                        const std::string& post_data,
                        const bool allow_https_upgrade,
                        int32_t transition_type) override {
    return 0;
  }
  void EnableHttpsUpgrades(bool enable) override {}
#endif
#if BUILDFLAG(ARKWEB_EXT_RECEIVE_RESPONSE)
  int32_t GetLastCommittedEntryPageTransition() override { return 0; }
#endif
};

class MockCefBrowser : public CefBrowser {
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
  bool IsValid() override { return false; }
  CefRefPtr<ArkWebBrowserHostExt> GetHost() override {
    if (!host_) {
      host_ = new MockCefBrowserHost();
    }
    return host_;
  }
  bool CanGoBack() override { return false; }
  void GoBack() override {}
  bool CanGoForward() override { return false; }
  void GoForward() override {}
  bool IsLoading() override { return false; }
  void Reload() override {}
  void ReloadIgnoreCache() override {}
  void StopLoad() override {}
  int GetIdentifier() override { return -1; }
  bool IsSame(CefRefPtr<CefBrowser> that) override { return false; }
  bool IsPopup() override { return false; }
  bool HasDocument() override { return false; }
  CefRefPtr<CefFrame> GetMainFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFocusedFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFrameByIdentifier(
      const CefString& identifier) override {
    return nullptr;
  }
  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override {
    return nullptr;
  }
  size_t GetFrameCount() override { return 0; }
  void GetFrameIdentifiers(std::vector<CefString>& identifiers) override {}
  void GetFrameNames(std::vector<CefString>& names) override {}
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
  int PrerenderPage(const CefString& url,
                    const CefString& additional_headers) override {
    return 0;
  }
  void CancelAllPrerendering() override {}

  CefRefPtr<ArkWebBrowserHostExt> host_ = nullptr;
};

class MockNWebTouchPointInfo : public NWebTouchPointInfo {
 public:
  MockNWebTouchPointInfo(int32_t id, double x, double y)
      : id_(id), x_(x), y_(y) {}
  int32_t GetId() override { return id_; }
  double GetX() override { return x_; }
  double GetY() override { return y_; }

 private:
  int32_t id_;
  double x_;
  double y_;
};

class MockNWebKeyboardEvent : public NWebKeyboardEvent {
 public:
  MockNWebKeyboardEvent(int32_t keyCode,
                        int32_t action,
                        int32_t unicode,
                        bool enableCapsLock,
                        const std::vector<int32_t>& pressKeyCodes)
      : keyCode_(keyCode),
        action_(action),
        unicode_(unicode),
        enableCapsLock_(enableCapsLock),
        pressKeyCodes_(pressKeyCodes) {}
  int32_t GetKeyCode() override { return keyCode_; }
  int32_t GetAction() override { return action_; }
  int32_t GetUnicode() override { return unicode_; }
  bool IsEnableCapsLock() override { return enableCapsLock_; }
  std::vector<int32_t> GetPressKeyCodes() override { return pressKeyCodes_; }

 private:
  int32_t keyCode_;
  int32_t action_;
  int32_t unicode_;
  bool enableCapsLock_;
  std::vector<int32_t> pressKeyCodes_;
};

class MockNWebMouseEvent : public NWebMouseEvent {
 public:
  MockNWebMouseEvent(int32_t x,
                     int32_t y,
                     int32_t button,
                     int32_t action,
                     int32_t clickNum,
                     const std::vector<int32_t>& pressKeyCodes,
                     int32_t rawX,
                     int32_t rawY)
      : x_(x),
        y_(y),
        button_(button),
        action_(action),
        clickNum_(clickNum),
        pressKeyCodes_(pressKeyCodes),
        rawX_(rawX),
        rawY_(rawY) {}
  int32_t GetX() override { return x_; }
  int32_t GetY() override { return y_; }
  int32_t GetButton() override { return button_; }
  int32_t GetAction() override { return action_; }
  int32_t GetClickNum() override { return clickNum_; }
  std::vector<int32_t> GetPressKeyCodes() override { return pressKeyCodes_; }
  int32_t GetRawX() override { return rawX_; }
  int32_t GetRawY() override { return rawY_; }

 private:
  int32_t x_;
  int32_t y_;
  int32_t button_;
  int32_t action_;
  int32_t clickNum_;
  std::vector<int32_t> pressKeyCodes_;
  int32_t rawX_;
  int32_t rawY_;
};

void NWebEventHandlerFuzzTest(FuzzedDataProvider* fdp) {
  int32_t id = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  double x = fdp->ConsumeFloatingPoint<double>();
  double y = fdp->ConsumeFloatingPoint<double>();
  double vx = fdp->ConsumeFloatingPoint<double>();
  double vy = fdp->ConsumeFloatingPoint<double>();
  double deltaX = fdp->ConsumeFloatingPoint<double>();
  double deltaY = fdp->ConsumeFloatingPoint<double>();
  float virtual_pixel_ratio = fdp->ConsumeFloatingPoint<float>();
  bool from_overlay = fdp->ConsumeBool();
  int32_t keyCode = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t keyAction = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t unicode = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t modifiers = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t source = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  bool enableCapsLock = fdp->ConsumeBool();
  std::vector<int32_t> pressedCodes;
  pressedCodes.push_back(fdp->ConsumeIntegralInRange<int32_t>(0, 100));
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  std::shared_ptr<NWebTouchPointInfo> touch_point_info =
      std::make_shared<MockNWebTouchPointInfo>(id, x, y);
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
  touch_point_infos.push_back(touch_point_info);
  CefKeyEvent keyEvent;
  std::shared_ptr<NWebKeyboardEvent> keyboardEvent =
      std::make_shared<MockNWebKeyboardEvent>(keyCode, keyAction, unicode,
                                              enableCapsLock, pressedCodes);
  std::shared_ptr<NWebEventHandler> impl = NWebEventHandler::Create();

  impl->OnTouchPress(id, x, y, from_overlay);

  impl->OnTouchMove(id, x, y, from_overlay);

  impl->OnTouchMove(touch_point_infos, from_overlay, virtual_pixel_ratio);

  impl->OnTouchRelease(id, x, y, from_overlay);

  impl->OnTouchCancelById(id, x, y, from_overlay);

  impl->SendKeyEventFromAce(keyCode, keyAction);

  impl->WebSendKeyEventFromAce(keyCode, keyAction, pressedCodes);

  impl->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);

  impl->SendMouseWheelEvent(x, y, deltaX, deltaY);

  impl->SendKeyboardEvent(keyboardEvent);

  impl->WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source);

  impl->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);

  impl->SendTouchpadFlingEvent(x, y, vx, vy);

  impl->SetBrowser(browser);

  impl->OnTouchPress(id, x, y, from_overlay);

  impl->OnTouchMove(id, x, y, from_overlay);

  impl->OnTouchMove(touch_point_infos, from_overlay, virtual_pixel_ratio);

  impl->OnTouchRelease(id, x, y, from_overlay);

  impl->OnTouchCancelById(id, x, y, from_overlay);

  impl->SendKeyEventFromAce(keyCode, keyAction);

  impl->WebSendKeyEventFromAce(keyCode, keyAction, pressedCodes);

  impl->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);

  impl->SendMouseWheelEvent(x, y, deltaX, deltaY);

  impl->SendKeyboardEvent(keyboardEvent);

  impl->WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source);

  impl->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);

  impl->SendTouchpadFlingEvent(x, y, vx, vy);
}

void NWebEventHandlerFuzzTwoTest(FuzzedDataProvider* fdp) {
  int32_t x = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t y = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t button = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t action = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t clickNum = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  std::vector<int32_t> pressedCodes;
  pressedCodes.push_back(fdp->ConsumeIntegralInRange<int32_t>(0, 100));
  int32_t rawX = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t rawY = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  std::shared_ptr<NWebMouseEvent> mouseEvent =
      std::make_shared<MockNWebMouseEvent>(x, y, button, action, clickNum,
                                           pressedCodes, rawX, rawY);
  std::shared_ptr<NWebEventHandler> impl = NWebEventHandler::Create();

  impl->SendMouseEvent(x, y, button, action, clickNum);

  impl->OnTouchCancel();

  impl->NotifyForNextTouchEvent();

  impl->SetBrowser(browser);

  impl->SendMouseEvent(x, y, button, action, clickNum);

  impl->OnTouchCancel();

  impl->NotifyForNextTouchEvent();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NWebEventHandlerFuzzTest(&fdp);

  NWebEventHandlerFuzzTwoTest(&fdp);

  return 0;
}
