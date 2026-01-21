/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "nweb_render_handler.h"
#include "include/cef_devtools_message_handler_delegate.h"
#include "cef/include/cef_drag_data.h"
#include "include/cef_base.h"
#include "include/cef_image.h"
#include "include/cef_stream.h"
#include "ohos_nweb/include/nweb_drag_data.h"
#include "ohos_nweb/include/nweb_errors.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

namespace OHOS::NWeb {
namespace {
std::shared_ptr<NWebRenderHandler> g_nweb_render_handler = nullptr;
}

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

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  void SetBlankScreenDetectionConfig( 
      bool enable,
      const std::vector<double>& detectionTiming,
      const std::vector<int32_t>& detectionMethods,
      int32_t contentfulNodesCountThreshold) override {}
#endif

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetOverScrollOffsetValue(float* offset_x, float* offset_y) override {}
#endif

  MOCK_METHOD(void, SetFocus, (bool), (override));

  CefWindowHandle GetWindowHandle() { return 0; }
  CefWindowHandle GetOpenerWindowHandle() { return 0; }

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

  void OnWindowShow() override {}

  void OnWindowHide() override {}

  void OnOnlineRenderToForeground() override {}

  MOCK_METHOD(void,
              SendTouchEventList,
              (const std::vector<CefTouchEvent>&),
              (override));

  void NotifyScreenInfoChanged() override {}

  void Invalidate(PaintElementType type) override {}

  void SendExternalBeginFrame() override {}

  MOCK_METHOD(void, SendKeyEvent, (const CefKeyEvent& event), (override));

  MOCK_METHOD(void,
              SendMouseClickEvent,
              (const CefMouseEvent&, cef_mouse_button_type_t, bool, int),
              (override));

  MOCK_METHOD(void,
              SendMouseMoveEvent,
              (const CefMouseEvent&, bool),
              (override));

  MOCK_METHOD(void,
              SendMouseWheelEvent,
              (const CefMouseEvent&, int, int),
              (override));

  MOCK_METHOD(void, SendTouchEvent, (const CefTouchEvent&), (override));

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

  void GetImageForContextNode(int command_id) override {}

  void GetImageFromCache(const CefString& url, int command_id) override {}

  void GetImageFromCacheEx(const CefString& url, int command_id) override {}

  void ExitFullScreen() override {}

  void UpdateLocale(const CefString& locale) override {}

  CefString GetOriginalUrl() override { return CefString(); }

  void PutNetworkAvailable(bool available) override {}

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  int PrerenderPage(const CefString& url,
                    const CefString& additional_headers) { return OHOS::NWeb::NWEB_OK; };

  void CancelAllPrerendering() {};
#endif

  void RemoveCache(bool include_disk_files) override {}

  void PostTaskToUIThread(CefRefPtr<CefTask> task) override {}

  void SetVirtualPixelRatio(float ratio) override {}

  float GetVirtualPixelRatio() override { return 0.0f; }

  void SetWebPreferences(const CefBrowserSettings& browser_settings) override {}

  void PutUserAgent(const CefString& ua, bool from_app) override {}

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

  void PostPortMessage(const CefString &port_handle, CefRefPtr<CefValue> message) override {}

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

  void JavaScriptOnDocumentStart(
      const CefString& script,
      const std::vector<CefString>& script_rules,
      const std::vector<std::pair<CefString, CefString>>& script_regex_rules,
      bool is_transfer_finished) override {}

  void RemoveJavaScriptOnDocumentStart() override {}

  void JavaScriptOnDocumentEnd(
      const CefString& script,
      const std::vector<CefString>& script_rules,
      const std::vector<std::pair<CefString, CefString>>& script_regex_rules,
      bool is_transfer_finished) override {}

  void RemoveJavaScriptOnDocumentEnd() override {}

#if BUILDFLAG(ARKWEB_UNITTESTS)
  void OnDataDetectorSelectText() override {}
  std::string GetDataDetectorSelectText() override { return std::string(); }
  void SetBypassVsyncCondition(int32_t condition) override {}
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  void JavaScriptOnHeadReady(
      const CefString& script,
      const std::vector<CefString>& script_rules,
      const std::vector<std::pair<CefString, CefString>>& script_regex_rules,
      bool is_transfer_finished) override {}

  void RemoveJavaScriptOnHeadReady() override {}
#endif

  void SetDrawRect(int x, int y, int width, int height) override {}

  void SetDrawMode(int mode) override {}

  void CreateWebPrintDocumentAdapter(const CefString& jobName,
                                     void** webPrintDocumentAdapter) override {}

  void CreateWebPrintDocumentAdapterV2(const CefString& jobName,
                                       void** adapter) override {}

  void SetOverscrollMode(int mode) override {}

  bool Discard() override { return false; }

  bool Restore() override { return false; }

  void SetBrowserZoomLevel(double zoomFactor) override {}

  int GetTopControlsOffset() override { return 0; }

  MOCK_METHOD0(GetShrinkViewportHeight, int());

  void OnEyeDropperResult(bool success, uint32_t color) override {}

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

  CefRefPtr<CefDownloadItem> GetDownloadItem(uint32_t item_id) override { return nullptr; }

  bool NeedsReload() override { return false; }

  bool TerminateRenderProcess() override { return false; }

  void RegisterNativeJSProxy(const CefString& object_name,
                             const std::vector<CefString>& method_list,
                             const int32_t object_id,
                             bool is_async,
                             const CefString& permission) override {}

  MOCK_METHOD(void,
              SendTouchpadFlingEvent,
              (const CefMouseEvent&, double, double),
              (override));
  
  MOCK_METHOD(void,
              SendCancelFlingEvent, (const CefMouseEvent&),
              (override));

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

  MOCK_METHOD(void, NotifyForNextTouchEvent, (), (override));

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

  void ShowDevToolsWith(
      CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
      CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate,
      const CefPoint& inspect_element_at) override {}

  void ShowDevToolsWithByPb(
      CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
      CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate,
      const CefPoint& inspect_element_at,
      const CefOpenDevToolsExtOpt& ext_opt) override {}

  bool IsFullscreen() override { return false; }

  void ExitFullscreen(bool will_cause_resize) override {}

  bool CanExecuteChromeCommand(int command_id) override { return false; }

  void ExecuteChromeCommand(
      int command_id,
      cef_window_open_disposition_t disposition) override {}

  bool IsRenderProcessUnresponsive() override { return false; }

  cef_runtime_style_t GetRuntimeStyle() override { return CEF_RUNTIME_STYLE_DEFAULT; }

  void EnableVideoAssistant(bool enable) override { return false; }

  void ExecuteVideoAssistantFunction(const CefString& cmdId) override {}

  bool IsIframe() override { return false; }

  void ReloadFocusedFrame() override {}

  void StopScreenCapture(int32_t nweb_id, const CefString &session_id) override {}

  void SetScreenCapturePickerShow() override {}

  void DisableSessionReuse() override {}

  void RegisterScreenCaptureDelegateListener(
      CefRefPtr<CefScreenCaptureCallback> listener) override {}

  void CustomWebMediaPlayer(bool enable) override {}

  void SetMediaResumeFromBFCachePage(bool resume) override {}

  bool IsValid() override { return false; }

  MOCK_METHOD(CefRefPtr<ArkWebBrowserHostExt>, GetHost, (), (override));
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
  CefRefPtr<CefBrowserPermissionRequestDelegate> GetPermissionRequestDelegate() override { return nullptr; }
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
  void PrefetchPage(const OHOS::NWeb::PrefetchOptions& prefetch_options) override {
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
                                CefString& detailErrMsg) override {
    return 0;
  }
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override {}
  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override { return nullptr; }
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
  void ShowFreeCopyMenu() override {}
  bool ShouldShowFreeCopyMenu() override { return false; }
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override {}
  void ExtensionSetTabId(int tab_id) override {}
  int ExtensionGetTabId() override { return 0; }
  uint32_t GetAcceleratedWidget(bool isPopup) { return 0; }
  void SetAdBlockEnabledForSite(bool is_adblock_enabled, int main_frame_tree_node_id) override {}
  void FindEx(const CefString &searchText, bool forward, bool matchCase, bool findNext, bool newSession) override {}
  void SetFocusOnWeb() override {}
  void UpdateSecurityLayer(bool isNeedSecurityLayer) override {}
  void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) override {}
  void SetHasComposition(bool has_composition) override {}
  bool GetHasComposition() override {}
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
  void SetEnableCustomVideoPlayer(bool flag) override {}
  void ScaleGestureChangeV2(int type, float scale, float originScale, float width, float height) override {}
  virtual std::string GetCurrentLanguage() override { return ""; }
  void MaximizeResize() override {}
  void PutWebMediaAVSessionEnabled(bool enable) override {}
  void SetEnableHalfFrameRate(bool enabled) override {}
  bool SetFocusByPosition(float x, float y) override { return false; }
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
      const CefString& identifier) override {  return nullptr; }
#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override {}
  void OnBrowserBackground() override {}
#endif
  void RunJavaScriptInFrames(const std::string& jsString, FrameInfos rootFrame,
                             bool recursive, IsolatedWorld world,
                             CefRefPtr<CefJavaScriptResultCallback> callback) override {}
#if BUILDFLAG(IS_ARKWEB)
  void EnableAppLinking(bool enable) override {}
  bool IsAppLinkingEnabled() const override { return false; }
#endif
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void GetFocusedFrameInfo(int32_t& frame_id, CefString& frame_url) override {}
#endif  // ARKWEB_ARKWEB_EXTENSIONS
#if BUILDFLAG(ARKWEB_READER_MODE)
  void Distill(uint64_t request_id, const DistillOptions& distill_options,
    CefRefPtr<CefDistillCallback> callback) override {}
  void AbortDistill() override {}
#endif  // BUILDFLAG(ARKWEB_READER_MODE)
#endif  // BUILDFLAG(IS_OHOS)
#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  void LoadUrlWithParams(const std::string& url,
                         const LoadUrlType& load_type,
                         const std::string& refer,
                         const std::string& headers,
                         const std::string& post_data,
                         const bool& allow_https_upgrade,
                         int32_t transition_type) override {}
  void EnableHttpsUpgrades(bool enable) override {}
#endif
#if BUILDFLAG(ARKWEB_UNITTESTS)
  void SetImeShow(bool visible) override {}
#endif // ARKWEB_UNITTESTS
};

class MockCefBrowser : public ArkWebBrowserExt {
 public:
  bool IsValid() override { return false; }

  MockCefBrowser(CefRefPtr<CefBrowserHost> host) : host_(host) {}
  MOCK_METHOD(CefRefPtr<ArkWebBrowserHostExt>, GetHost, (), (override));
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
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
#if BUILDFLAG(IS_OHOS)
  bool CanGoBackOrForward(int num_steps) override { return false; }
  void GoBackOrForward(int num_steps) override {}
  void DeleteHistory() override {}
  void PasswordSuggestionSelected(int list_index) override {}
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) override {}
  void UpdateBrowserControlsHeight(int height, bool animate) override {}
  void PrefetchPage(const OHOS::NWeb::PrefetchOptions& prefetch_options) override {
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
                                CefString& detailErrMsg) override {
    return 0;
  }
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override {}

  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override { return nullptr; }
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
  void ShowFreeCopyMenu() override {}
  bool ShouldShowFreeCopyMenu() override { return false; }
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override {}
  void ExtensionSetTabId(int tab_id) override {}
  int ExtensionGetTabId() override { return 0; }
  uint32_t GetAcceleratedWidget(bool isPopup) { return 0; }
  void SetAdBlockEnabledForSite(bool is_adblock_enabled, int main_frame_tree_node_id) override {}
  CefRefPtr<CefFrame> GetFrameByIdentifier(
      const CefString& identifier) override {  return nullptr; }
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  int PrerenderPage(const CefString& url,
                    const CefString& additional_headers) override { return OHOS::NWeb::NWEB_OK; };
  void CancelAllPrerendering() override {};
#endif
#endif  // BUILDFLAG(IS_OHOS)
 private:
  CefRefPtr<CefBrowserHost> host_;
};

class MockCefDragData : public CefDragData {
 public:
  MOCK_METHOD(CefRefPtr<CefDragData>, Clone, (), (override));
  MOCK_METHOD(bool, IsReadOnly, (), (override));
  MOCK_METHOD(bool, IsLink, (), (override));
  MOCK_METHOD(bool, IsFragment, (), (override));
  MOCK_METHOD(bool, IsFile, (), (override));
  MOCK_METHOD(CefString, GetLinkURL, (), (override));
  MOCK_METHOD(CefString, GetLinkTitle, (), (override));
  MOCK_METHOD(CefString, GetLinkMetadata, (), (override));
  MOCK_METHOD(CefString, GetFragmentText, (), (override));
  MOCK_METHOD(CefString, GetFragmentHtml, (), (override));
  MOCK_METHOD(CefString, GetFragmentBaseURL, (), (override));
  MOCK_METHOD(CefString, GetFileName, (), (override));
  MOCK_METHOD(size_t, GetFileContents, (CefRefPtr<CefStreamWriter>), (override));
  MOCK_METHOD(bool, GetFileNames, (std::vector<CefString>&), (override));
  MOCK_METHOD(bool, GetFilePaths, (std::vector<CefString>&), (override));
  MOCK_METHOD(void, SetLinkURL, (const CefString&), (override));
  MOCK_METHOD(void, SetLinkTitle, (const CefString&), (override));
  MOCK_METHOD(void, SetLinkMetadata, (const CefString&), (override));
  MOCK_METHOD(void, SetFragmentText, (const CefString&), (override));
  MOCK_METHOD(void, SetFragmentHtml, (const CefString&), (override));
  MOCK_METHOD(void, SetFragmentBaseURL, (const CefString&), (override));
  MOCK_METHOD(void, ResetFileContents, (), (override));
  MOCK_METHOD(void, AddFile, (const CefString&, const CefString&), (override));
  MOCK_METHOD(void, ClearFilenames, (), (override));
  MOCK_METHOD(CefRefPtr<CefImage>, GetImage, (), (override));
  MOCK_METHOD(CefPoint, GetImageHotspot, (), (override));
  MOCK_METHOD(bool, HasImage, (), (override));
  MOCK_METHOD(void, SetReadOnly, (bool), (override));
  MOCK_METHOD(bool, IsImageFileContents, (), (override));
  MOCK_METHOD(size_t, GetImageFileSize, (), (override));
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
};

class NWebRenderHandlerTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
  MockCefDragData* mock_drag_data = nullptr;
  MockCefBrowser* mock_browser = nullptr;
  MockCefBrowserHost* mock_host = nullptr;
};

void NWebRenderHandlerTest::SetUpTestCase(void) {}

void NWebRenderHandlerTest::TearDownTestCase(void) {}

void NWebRenderHandlerTest::SetUp() {
  g_nweb_render_handler = std::make_shared<NWebRenderHandler>();
  mock_drag_data = new MockCefDragData();
  mock_host = new MockCefBrowserHost();
  mock_browser = new MockCefBrowser(mock_host);
  ASSERT_NE(g_nweb_render_handler, nullptr);
}

void NWebRenderHandlerTest::TearDown(void) {
  g_nweb_render_handler = nullptr;
  delete mock_drag_data;
  delete mock_browser;
  delete mock_host;
}

#if BUILDFLAG(IS_OHOS)
TEST_F(NWebRenderHandlerTest, StartDragging_001) {
  CefRefPtr<CefBrowser> browser = nullptr;
  CefRefPtr<CefDragData> drag_data = nullptr;
  bool result = g_nweb_render_handler->StartDragging(
      browser, drag_data,
      CefRenderHandler::DragOperationsMask::DRAG_OPERATION_EVERY, 100, 100);
  ASSERT_FALSE(result);
}

TEST_F(NWebRenderHandlerTest, StartDragging_002) {
  CefRefPtr<CefBrowser> browser = nullptr;
  ON_CALL(*mock_drag_data, HasImage()).WillByDefault(::testing::Return(false));
  bool result = g_nweb_render_handler->StartDragging(
      browser, mock_drag_data,
      CefRenderHandler::DragOperationsMask::DRAG_OPERATION_EVERY, 100, 100);
  ASSERT_FALSE(result);
}

TEST_F(NWebRenderHandlerTest, StartDragging_003) {
  CefRefPtr<CefBrowser> browser = nullptr;
  ON_CALL(*mock_drag_data, HasImage()).WillByDefault(::testing::Return(false));
  bool result = g_nweb_render_handler->StartDragging(
      browser, mock_drag_data,
      CefRenderHandler::DragOperationsMask::DRAG_OPERATION_EVERY, 100, 100);
  ASSERT_FALSE(result);
}

TEST_F(NWebRenderHandlerTest, StartDragging_004) {
  CefRefPtr<CefBrowser> browser = nullptr;
  ON_CALL(*mock_drag_data, HasImage()).WillByDefault(::testing::Return(true));
  bool result = g_nweb_render_handler->StartDragging(
      browser, mock_drag_data,
      CefRenderHandler::DragOperationsMask::DRAG_OPERATION_EVERY, 100, 100);
  ASSERT_FALSE(result);
}

TEST_F(NWebRenderHandlerTest, StartDragging_005) {
  ON_CALL(*mock_drag_data, HasImage()).WillByDefault(::testing::Return(true));
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  bool result = g_nweb_render_handler->StartDragging(
      mock_browser, mock_drag_data,
      CefRenderHandler::DragOperationsMask::DRAG_OPERATION_EVERY, 100, 100);
  ASSERT_FALSE(result);
}

TEST_F(NWebRenderHandlerTest, StartDragging_006) {
  ON_CALL(*mock_drag_data, HasImage()).WillByDefault(::testing::Return(true));
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  bool result = g_nweb_render_handler->StartDragging(
      mock_browser, mock_drag_data,
      CefRenderHandler::DragOperationsMask::DRAG_OPERATION_EVERY, 100, 100);
  ASSERT_FALSE(result);
}

TEST_F(NWebRenderHandlerTest, OnNativeEmbedLifecycleChange) {
  CefRefPtr<CefBrowser> browser = nullptr;
  CefNativeEmbedData data_info;
  data_info.status = CREATE;
  data_info.surfaceId = "";
  data_info.embedId = "";
  data_info.info.id = "";
  data_info.info.x = 0;
  data_info.info.y = 0;
  data_info.info.width = 0;
  data_info.info.height = 0;
  data_info.info.type = "";
  data_info.info.src = "";
  data_info.info.url = "";
  data_info.info.tag = "";
  data_info.info.params = {{"key1", "value1"}, {"key2", "value2"}};
  g_nweb_render_handler->OnNativeEmbedLifecycleChange(browser, data_info);
}

TEST_F(NWebRenderHandlerTest, OnNativeEmbedGestureEvent) {
  cef_embed_touch_event_t touchEvent;

  g_nweb_render_handler->OnNativeEmbedGestureEvent(nullptr, touchEvent,
                                                   nullptr);
}
#endif
}  // namespace OHOS::NWeb
