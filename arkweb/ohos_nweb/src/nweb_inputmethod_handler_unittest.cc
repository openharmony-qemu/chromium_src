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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "cef_delegate/nweb_inputmethod_client.h"
#include "condition_variable"
#define private public
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_base_ohos_api.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "cef_devtools_message_handler_delegate.h"
#include "cef_client.h"
#include "imf_adapter.h"
#include "nweb_inputmethod_handler.h"
#include "nweb_inputmethod_handler.cc"
#include "ohos_cef_ext/libcef/common/cef_open_devtools_ext_opt.h"
#include "ohos_nweb/include/nweb_errors.h"

using namespace base::ohos;

namespace OHOS::NWeb {

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
  MOCK_METHOD(void, SetFocus, (bool), (override));
  CefWindowHandle GetWindowHandle() { return 0; }
  CefWindowHandle GetOpenerWindowHandle() { return 0; }
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
  MOCK_METHOD(void, ImeFinishComposingText, (bool), (override));
  MOCK_METHOD(void, ImeCancelComposition, (), (override));
  MOCK_METHOD(void, ImeSetComposition, (
    const CefString&, const std::vector<CefCompositionUnderline>&,
    const CefRange&,const CefRange&), (override));
  void SendCaptureLostEvent() override {}
  void NotifyMoveOrResizeStarted() override {}
  int GetWindowlessFrameRate() override { return 0; }
  void SetWindowlessFrameRate(int frame_rate) override {}
  void ImeCommitText(const CefString& text,
                     const CefRange& replacement_range,
                     int relative_cursor_pos) override {}
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
  MOCK_METHOD(void, PostTaskToUIThread, (CefRefPtr<CefTask>), (override));
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
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetScrollable(bool enable, int scrollType) override {}
  void SetImeShow(bool visible) override {}
#endif
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
                              const std::vector<CefString>& excluded_dir_list) override {}
  void SetAutofillCallback(CefRefPtr<CefWebMessageReceiver> callback) override {}
  void FillAutofillData(CefRefPtr<CefValue> message) override {}
  void ScrollFocusedEditableNodeIntoView() override {}
  void ProcessAutofillCancel(const CefString& fillContent) override {}
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
  void EnableVideoAssistantAVCast(bool enable) override {}
  void ExecuteVideoAssistantFunction(const CefString& cmdId) override {}
  bool IsIframe() override { return false; }
  void ReloadFocusedFrame() override {}
  void StopScreenCapture(int32_t nweb_id, const CefString &session_id) override {}
  void SetScreenCapturePickerShow() override {}
  void DisableSessionReuse() override {}
  void RegisterScreenCaptureDelegateListener(
      CefRefPtr<CefScreenCaptureCallback> listener) override {}
  void CustomWebMediaPlayer(bool enable) override {}
  bool IsValid() override { return false; }
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
  void PrefetchPage(const OHOS::NWeb::PrefetchOptions& prefetch_options) override {}
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
                                bool allowOpaqueOrigin,
                                bool supportWildcard,
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
#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  int InsertBackForwardEntry(int index, const CefString& url) override { return 0; }
  int UpdateNavigationEntryUrl(int index, const CefString& url) override { return 0; }
  void ClearForwardList() override {}
#endif
  void ExtensionSetTabId(int tab_id) override {}
  uint32_t GetAcceleratedWidget(bool isPopup) { return 0; }
  void SetAdBlockEnabledForSite(bool is_adblock_enabled, int main_frame_tree_node_id) override {}
  void FindEx(const CefString &searchText, bool forward, bool matchCase, bool findNext, bool newSession) override {}
  MOCK_METHOD(void, SetFocusOnWeb, (), (override));
  MOCK_METHOD(void, UpdateSecurityLayer, (bool), (override));
  void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) override {}
  MOCK_METHOD(void, SetHasComposition, (bool), (override));
  MOCK_METHOD(void, AdvanceFocusForIME, (int), (override));
  MOCK_METHOD(void, AutoFillWithIMFEvent, (bool, bool, bool, const CefString&), (override));
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
  void SetEnableCustomVideoPlayer(bool flag) override {}
  void SetNativeInnerWeb(bool isInnerWeb) override {}
  void ScaleGestureChangeV2(int type, float scale, float originScale, float width, float height) override {}
  virtual std::string GetCurrentLanguage() override { return ""; }
  void MaximizeResize() override {}
  void PutWebMediaAVSessionEnabled(bool enable) override {}
  void SetEnableHalfFrameRate(bool enabled) override {}
  bool SetFocusByPosition(float x, float y) override { return false; }
  int ExtensionGetTabId() override {}
  bool GetHasComposition() override {}
  void SetMediaResumeFromBFCachePage(bool resume) override {}
#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  void NotifyScreenInfoChangedV2() override {}
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition) override {}
#endif
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
#if BUILDFLAG(IS_ARKWEB)
  void EnableAppLinking(bool enable) override {}
  bool IsAppLinkingEnabled() const override { return false; }
#endif
  void RunJavaScriptInFrames(const std::string& jsString, FrameInfos rootFrame,
                             bool recursive, IsolatedWorld world,
                             CefRefPtr<CefJavaScriptResultCallback> callback) override {}
#if BUILDFLAG(ARKWEB_NWEB_EX)
  void GetAllFrameInfos(CefRefPtr<CefFrameInfosCallback> callback) override {}
  void GetLastJavaScriptProxyCallingFrameInfo(
    CefRefPtr<CefLastJavaScriptProxyCallingFrameInfoCallback> callback) override {}
#endif
#if BUILDFLAG(ARKWEB_READER_MODE)
  void Distill(const std::string& guid, const DistillOptions& distill_options,
    CefRefPtr<CefDistillCallback> callback) override {}
  void AbortDistill() override {}
#endif // ARKWEB_READER_MODE
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void GetFocusedFrameInfo(int32_t& frame_id, CefString& frame_url) override {}
#endif  // ARKWEB_ARKWEB_EXTENSIONS

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
#endif  // BUILDFLAG(IS_OHOS)
};

class MockIMFAdapterFunctionKeyAdapter : public IMFAdapterFunctionKeyAdapter {
 public:
  MockIMFAdapterFunctionKeyAdapter() = default;
  ~MockIMFAdapterFunctionKeyAdapter() override = default;

  MOCK_METHOD(IMFAdapterEnterKeyType, GetEnterKeyType, (), (override));
};

class MockNWebInputMethodHandler : public NWebInputMethodHandler {
 public:
  MockNWebInputMethodHandler() = default;
  ~MockNWebInputMethodHandler() override = default;

  MOCK_METHOD(void, MoveCursor, (IMFAdapterDirection), ());
  MOCK_METHOD(void, WebBlurKeyboardHide, (), ());
  MOCK_METHOD(void, SetIMEStatus, (bool), ());
  MOCK_METHOD(void, SendEnterKeyEvent, (int32_t), ());
  MOCK_METHOD(void, DeleteBackward, (int32_t), ());
  MOCK_METHOD(void, DeleteForward, (int32_t), ());
  MOCK_METHOD(void, InsertText, (const std::u16string&), ());
  MOCK_METHOD(bool, AttachToSystemIME, (bool, int32_t), ());
  MOCK_METHOD(int32_t, UpdateCompositionInfo, (const std::u16string&, int32_t, int32_t), ());
};

class MockCefBrowser : public CefBrowser {
 public:
  MockCefBrowser(CefRefPtr<CefBrowserHost> host) : host_(host) {}
  MOCK_METHOD(CefRefPtr<ArkWebBrowserHostExt>, GetHost, (), (override));
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
  bool IsValid() override { return false; }
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
  CefRefPtr<CefFrame> GetFrameByIdentifier(const CefString& identifier) override { return nullptr; }
  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override {
    return nullptr;
  }
  size_t GetFrameCount() override { return 0; }
  void GetFrameIdentifiers(std::vector<CefString>& identifiers) override {}
  void GetFrameNames(std::vector<CefString>& names) override {}
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  int PrerenderPage(const CefString& url,
                    const CefString& additional_headers) override { return OHOS::NWeb::NWEB_OK; };
  void CancelAllPrerendering() override {};
#endif
  CefRefPtr<CefBrowserHost> host_;
};

class MockCefBrowserExt : public ArkWebBrowserExt {
 public:
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
  void ShowFreeCopyMenu() override {}
  bool ShouldShowFreeCopyMenu() override { return false; }
  void PasswordSuggestionSelected(int list_index) override {}
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) override {}
  void UpdateBrowserControlsHeight(int height, bool animate) override {}
  void PrefetchPage(const OHOS::NWeb::PrefetchOptions& prefetch_options) override {}
  void ReloadOriginalUrl() override {}
  bool CanStoreWebArchive() override { return false; }
  void SetBrowserUserAgentString(const CefString& user_agent) override {}
  bool ShouldShowLoadingUI() override { return false; }
  void SetForceEnableZoom(bool forceEnableZoom) override {}
  bool GetForceEnableZoom() override { return false; }
  int GetNWebId() override { return -1; }
  bool GetSavePasswordAutomatically() override { return false; }
  void SetSavePasswordAutomatically(bool enable) override {}
  void SaveOrUpdatePassword(bool is_update) override {}
  bool GetSavePassword() override { return false; }
  void SetSavePassword(bool enable) override {}
  int GetSecurityLevel() override { return -1; }
  void EnableSafeBrowsing(bool enable) override {}
  bool IsSafeBrowsingEnabled() override { return false; }
  void EnableIntelligentTrackingPrevention(bool enable) override {}
  bool IsIntelligentTrackingPreventionEnabled() override { return false; }
  bool IsAdsBlockEnabled() override { return false; }
  bool IsAdsBlockEnabledForCurPage() override { return false; }
  void EnableAdsBlock(bool enable) override {}
  int SetUrlTrustListWithErrMsg(const CefString& urlTrustList,
                                bool allowOpaqueOrigin,
                                bool supportWildcard,
                                CefString& detailErrMsg) override {
    return -1;
  }
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override {}
#endif  // BUILDFLAG(IS_OHOS)
};

class MockOnTextChangedListenerImpl : public IMFTextListenerAdapter {
 public:
  MockOnTextChangedListenerImpl() = default;
  ~MockOnTextChangedListenerImpl() = default;
  void InsertText(const std::u16string& text) override {}
  void DeleteForward(int32_t length) override {}
  void DeleteBackward(int32_t length) override {}
  void SendKeyEventFromInputMethod() override {}
  void SendKeyboardStatus(const IMFAdapterKeyboardStatus& status) override {}
  void SendFunctionKey(
      std::shared_ptr<IMFAdapterFunctionKeyAdapter> functionKey) override {}
  void SetKeyboardStatus(bool status) override {}
  void MoveCursor(const IMFAdapterDirection direction) override {}
  void HandleSetSelection(int32_t start, int32_t end) override {}
  void HandleExtendAction(int32_t action) override {}
  void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) override {}

  int32_t GetTextIndexAtCursor() override {
    return handler_->GetTextIndexAtCursor();
  }

  std::u16string GetLeftTextOfCursor(int32_t number) override {
    return handler_->GetLeftTextOfCursor(number);
  }

  std::u16string GetRightTextOfCursor(int32_t number) override {
    return handler_->GetRightTextOfCursor(number);
  }

  int32_t SetPreviewText(const std::u16string& text,
                         int32_t start,
                         int32_t end) override {
    return handler_->SetPreviewText(text, start, end);
  }

  void FinishTextPreview() override { return handler_->FinishTextPreview(); }

  void SetNeedUnderLine(bool is_need_underline) override {
    handler_->SetNeedUnderLine(is_need_underline);
  }

 private:
  NWebInputMethodHandler* handler_;
};

class MockIMFAdapterImpl : public IMFAdapter {
 public:
  MockIMFAdapterImpl() = default;
  ~MockIMFAdapterImpl() override = default;
  MOCK_METHOD(bool,
              Attach,
              (std::shared_ptr<IMFTextListenerAdapter> listener,
               bool isShowKeyboard),
              (override));
  MOCK_METHOD(bool,
              Attach,
              (std::shared_ptr<IMFTextListenerAdapter> listener,
               bool isShowKeyboard,
               const std::shared_ptr<IMFTextConfigAdapter> config,
               bool isResetListener),
              (override));
  MOCK_METHOD(bool,
              AttachWithRequestKeyboardReason,
              (std::shared_ptr<IMFTextListenerAdapter> listener,
               bool isShowKeyboard,
               const std::shared_ptr<IMFTextConfigAdapter> config,
               bool isResetListener,
               int32_t requestKeyboardReason),
              (override));
  MOCK_METHOD(void,
              ShowCurrentInput,
              (const IMFAdapterTextInputType& inputType),
              (override));
  MOCK_METHOD(void, HideTextInput, (), (override));
  MOCK_METHOD(void, Close, (), (override));
  MOCK_METHOD(void,
              OnCursorUpdate,
              (const std::shared_ptr<IMFCursorInfoAdapter> cursorInfo),
              (override));
  MOCK_METHOD(void,
              OnSelectionChange,
              (std::u16string text, int start, int end),
              (override));
};

class NWebInputMethodHandlerTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();

 private:
  CefRefPtr<NWebInputMethodHandler> inputmethod_handler_ = nullptr;
  MockNWebInputMethodHandler* handler_ = nullptr;
  std::unique_ptr<OnTextChangedListenerImpl> listener_ = nullptr;
};

void NWebInputMethodHandlerTest::SetUpTestCase(void) {}

void NWebInputMethodHandlerTest::TearDownTestCase(void) {}

void NWebInputMethodHandlerTest::SetUp(void) {
  inputmethod_handler_ = new NWebInputMethodHandler();
  ASSERT_NE(inputmethod_handler_, nullptr);
  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  ASSERT_NE(inputmethod_handler_->inputmethod_adapter_, nullptr);
  handler_ = new MockNWebInputMethodHandler();
  listener_ = std::make_unique<OnTextChangedListenerImpl>(handler_);
  ASSERT_NE(listener_, nullptr);
  testing::Mock::AllowLeak(handler_);
  testing::Mock::AllowLeak(listener_.get());
}

void NWebInputMethodHandlerTest::TearDown(void) {
  if (inputmethod_handler_) {
    inputmethod_handler_ = nullptr;
  }

  if (listener_) {
    listener_ = nullptr;
  }

  if (handler_) {
    handler_ = nullptr;
  }
}

TEST_F(NWebInputMethodHandlerTest, GetCursorInfo) {
  if (!inputmethod_handler_) {
  }
  inputmethod_handler_->focus_rect_.Set(10, 10, 10, 10);
  inputmethod_handler_->device_pixel_ratio_ = 2;
  inputmethod_handler_->offset_x_ = 100;
  inputmethod_handler_->offset_y_ = 100;

  std::shared_ptr<IMFCursorInfoAdapter> cursorInfo =
      inputmethod_handler_->GetCursorInfo();
  EXPECT_NE(cursorInfo, nullptr);

  EXPECT_EQ(cursorInfo->GetLeft(), 140.0);
  EXPECT_EQ(cursorInfo->GetTop(), 120.0);
  EXPECT_EQ(cursorInfo->GetWidth(), 20.0);
  EXPECT_EQ(cursorInfo->GetHeight(), 20.0);
}

TEST_F(NWebInputMethodHandlerTest, Attach_inputmethod_adapter__Null) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  ASSERT_NE(mock_browser, nullptr);
  auto browser = CefRefPtr<MockCefBrowser>(mock_browser.release());
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->composing_text_.size(), 0);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_EnterKeyTypeValid) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = (int32_t)CEF_TEXT_INPUT_ACTION_DEFAULT;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::UNSPECIFIED);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::NONE;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NONE);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::GO;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::SEARCH;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::SEARCH);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::SEND;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::SEND);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::NEXT;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEXT);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::DONE;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::DONE);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::PREVIOUS;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::PREVIOUS);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::NEW_LINE;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_01) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_02) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TEXT;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_03) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_PASSWORD;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::VISIBLE_PASSWORD);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_04) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_SEARCH;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::SEARCH);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_05) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_EMAIL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::EMAIL_ADDRESS);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_06) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NUMBER;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_07) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TELEPHONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::PHONE);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_08) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_URL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::URL);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_09) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_10) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE_TIME;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_11) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE_TIME_LOCAL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_12) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_MONTH;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_13) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TIME;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_14) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_WEEK;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_15) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TEXT_AREA;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_16) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_CONTENT_EDITABLE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_17) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE_TIME_FIELD;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_18) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TYPE_NULL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_19) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_MAX;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_20) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_NONE;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_inputmethod_listener__Null) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  ASSERT_NE(mock_browser, nullptr);
  auto browser = CefRefPtr<MockCefBrowser>(mock_browser.release());
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_01) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();
  inputmethod_handler_->isAttached_ = false;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  ASSERT_NE(mock_browser, nullptr);
  auto browser = CefRefPtr<MockCefBrowser>(mock_browser.release());
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_29) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_PASSWORD;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::VISIBLE_PASSWORD);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_02) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();
  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->focus_rect_status_ = false;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  ASSERT_NE(mock_browser, nullptr);
  auto browser = CefRefPtr<MockCefBrowser>(mock_browser.release());
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_03) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();
  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->focus_rect_status_ = false;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  ASSERT_NE(mock_browser, nullptr);
  auto browser = CefRefPtr<MockCefBrowser>(mock_browser.release());
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_28) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_MAX;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_04) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();
  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->focus_rect_status_ = true;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  ASSERT_NE(mock_browser, nullptr);
  auto browser = CefRefPtr<MockCefBrowser>(mock_browser.release());
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_27) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_SEARCH;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_05) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();

  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->focus_rect_status_ = true;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  ASSERT_NE(mock_browser, nullptr);
  auto browser = CefRefPtr<MockCefBrowser>(mock_browser.release());
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_False) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_CONTINUE;
  inputmethod_handler_->isNeedReattachOncontinue_ = false;
  inputmethod_handler_->is_editable_node_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);

  inputmethod_handler_->isNeedReattachOncontinue_ = false;
  inputmethod_handler_->is_editable_node_ = true;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_False2) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_CONTINUE;

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOncontinue_, false);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_False3) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_CONTINUE;

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOncontinue_, false);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_True) {
  uint32_t nwebId = 1;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_CONTINUE;

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOncontinue_, false);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
  EXPECT_EQ(inputmethod_handler_->lastAttachNWebId_, 0);
  EXPECT_EQ(inputmethod_handler_->lastInputMode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_False) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;
  inputmethod_handler_->isNeedReattachOnfocus_ = false;
  inputmethod_handler_->is_editable_node_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);

  inputmethod_handler_->isNeedReattachOnfocus_ = false;
  inputmethod_handler_->is_editable_node_ = true;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_False2) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, true);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_False3) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, true);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONDRAG) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_ONDRAG;
  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = false;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;
  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;
  result = inputmethod_handler_->Reattach(nwebId, type);
  inputmethod_handler_->is_editable_node_ = true;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, true);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, ShowTextInput) {
  inputmethod_handler_->ShowTextInput();
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_True) {
  uint32_t nwebId = 1;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, true);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
  EXPECT_EQ(inputmethod_handler_->lastAttachNWebId_, 0);
  EXPECT_EQ(inputmethod_handler_->lastInputMode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_21) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_TEXT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_22) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_TEL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::PHONE);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_23) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_URL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::URL);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_24) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_EMAIL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::EMAIL_ADDRESS);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_25) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_NUMERIC;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_isAttached_False) {
  inputmethod_handler_->isAttached_ = false;
  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  uint32_t nwebId = 1;
  inputmethod_handler_->lastAttachNWebId_ = 2;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_KERNEL;
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_isAttached_False2) {
  inputmethod_handler_->isAttached_ = false;
  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  uint32_t nwebId = 0;
  inputmethod_handler_->lastAttachNWebId_ = nwebId;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_isAttached_False3) {
  inputmethod_handler_->isAttached_ = false;
  uint32_t nwebId = 1;
  inputmethod_handler_->lastAttachNWebId_ = 2;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->lastCloseInputMethodTime_ =
      std::chrono::high_resolution_clock::now();
  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_inputmethod_adapter__Null) {
  inputmethod_handler_->isAttached_ = true;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  uint32_t nwebId = 1;
  inputmethod_handler_->lastAttachNWebId_ = nwebId;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_KERNEL;
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_Normal) {
  inputmethod_handler_->isAttached_ = true;
  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->lastCloseInputMethodTime_ =
      std::chrono::high_resolution_clock::now();
  inputmethod_handler_->lastAttachNWebId_ = 0;
  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_Normal02) {
  inputmethod_handler_->isAttached_ = true;
  uint32_t nwebId = 1;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->lastCloseInputMethodTime_ =
      std::chrono::high_resolution_clock::now();
  inputmethod_handler_->lastAttachNWebId_ = nwebId;
  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_Normal03) {
  inputmethod_handler_->isAttached_ = true;
  uint32_t nwebId = 2;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_KERNEL;
  inputmethod_handler_->lastCloseInputMethodTime_ =
      std::chrono::high_resolution_clock::now();
  inputmethod_handler_->lastAttachNWebId_ = 1;
  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_Normal04) {
  inputmethod_handler_->isAttached_ = true;
  uint32_t nwebId = 2;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->lastCloseInputMethodTime_ =
      std::chrono::high_resolution_clock::now();
  inputmethod_handler_->lastAttachNWebId_ = 1;
  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_26) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DECIMAL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInputForce_inputmethod_adapter_Null) {
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->HideTextInputForce();
}

TEST_F(NWebInputMethodHandlerTest, HideTextInputForce_Normal) {
  inputmethod_handler_->HideTextInputForce();
}

TEST_F(NWebInputMethodHandlerTest, OnTextSelectionChanged) {
  CefString selected_text = CefString("test");
  CefRange selected_range = CefRange(0, 3);
  inputmethod_handler_->OnTextSelectionChanged(nullptr, selected_text,
                                               selected_range);
  EXPECT_EQ(inputmethod_handler_->selected_text_.size(), 4);
}

TEST_F(NWebInputMethodHandlerTest, OnCursorUpdate) {
  CefRect rect;
  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->OnCursorUpdate(rect);
}

TEST_F(NWebInputMethodHandlerTest, OnSelectionChanged) {
  inputmethod_handler_->OnSelectionChanged(nullptr, CefString(""),
                                           CefRange(0, 1));
}

TEST_F(NWebInputMethodHandlerTest, SendEnterKeyEvent) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(
	::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->SendEnterKeyEvent(1);

  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->SendEnterKeyEvent(1);
}

TEST_F(NWebInputMethodHandlerTest, SendEnterKeyEventOnUI_001) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->SendEnterKeyEventOnUI(static_cast<int32_t>(IMFAdapterEnterKeyType::NEW_LINE));
  EXPECT_FALSE(inputmethod_handler_->input_flags_);
}

TEST_F(NWebInputMethodHandlerTest, SendEnterKeyEventOnUI_002) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(nullptr));
  EXPECT_CALL(*mockHost, AdvanceFocusForIME(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->input_flags_ |= CEF_TEXT_INPUT_FLAG_HAVE_NEXT_FOCUSABLE_ELEMENT;
  int32_t enterKeyType = static_cast<int32_t>(IMFAdapterEnterKeyType::NEXT);
  inputmethod_handler_->SendEnterKeyEventOnUI(enterKeyType);
}

TEST_F(NWebInputMethodHandlerTest, SendEnterKeyEventOnUI_003) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(
	::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, AdvanceFocusForIME(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->input_flags_ |= CEF_TEXT_INPUT_FLAG_HAVE_NEXT_FOCUSABLE_ELEMENT;
  int32_t enterKeyType = static_cast<int32_t>(IMFAdapterEnterKeyType::NEXT);
  inputmethod_handler_->SendEnterKeyEventOnUI(enterKeyType);

  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).Times(
	::testing::AtLeast(1)).WillRepeatedly(testing::Return(false));
  enterKeyType = static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED);
  inputmethod_handler_->SendEnterKeyEventOnUI(enterKeyType);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
}

TEST_F(NWebInputMethodHandlerTest, SendEnterKeyEventOnUI_004) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(
	::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  inputmethod_handler_->browser_ = mock_browser.release();
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).Times(
	::testing::AtLeast(1)).WillRepeatedly(testing::Return(true));
  int32_t enterKeyType = static_cast<int32_t>(IMFAdapterEnterKeyType::NEXT);
  inputmethod_handler_->input_flags_ |= CEF_TEXT_INPUT_FLAG_HAS_BEEN_PASSWORD;
  inputmethod_handler_->SendEnterKeyEventOnUI(enterKeyType);

  enterKeyType = static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED);
  inputmethod_handler_->SendEnterKeyEventOnUI(enterKeyType);

  inputmethod_handler_->input_flags_ |= CEF_TEXT_INPUT_FLAG_HAVE_PREVIOUS_FOCUSABLE_ELEMENT;
  enterKeyType = static_cast<int32_t>(IMFAdapterEnterKeyType::PREVIOUS);
  inputmethod_handler_->SendEnterKeyEventOnUI(enterKeyType);

  enterKeyType = static_cast<int32_t>(IMFAdapterEnterKeyType::NEXT);
  inputmethod_handler_->SendEnterKeyEventOnUI(enterKeyType);

  inputmethod_handler_->input_flags_ |= CEF_TEXT_INPUT_FLAG_HAS_BEEN_PASSWORD;
  enterKeyType = static_cast<int32_t>(IMFAdapterEnterKeyType::PREVIOUS);
  inputmethod_handler_->SendEnterKeyEventOnUI(enterKeyType);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
}

#if BUILDFLAG(ARKWEB_CLIPBOARD)
TEST_F(NWebInputMethodHandlerTest, GetSelectInfo) {
  inputmethod_handler_->GetSelectInfo();
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

TEST_F(NWebInputMethodHandlerTest, ResetTextSelectiondata) {
  inputmethod_handler_->is_need_notify_all_ = false;
  bool result = inputmethod_handler_->ResetTextSelectiondata();
  EXPECT_EQ(result, false);

  inputmethod_handler_->is_need_notify_all_ = true;
  result = inputmethod_handler_->ResetTextSelectiondata();
  EXPECT_EQ(result, true);
  EXPECT_EQ(inputmethod_handler_->is_need_notify_all_, false);
}

TEST_F(NWebInputMethodHandlerTest, IsCorrectParam) {
  inputmethod_handler_->whole_text_ = u"test text";
  int32_t selectBegin = 5;
  int32_t selectEnd = 0;
  EXPECT_FALSE(inputmethod_handler_->IsCorrectParam(-1, selectBegin, selectEnd));

  EXPECT_TRUE(inputmethod_handler_->IsCorrectParam(1, selectBegin, selectEnd));
  EXPECT_EQ(selectBegin, 0);
  EXPECT_EQ(selectEnd, 5);

  selectBegin = 0;
  selectEnd = 5;
  EXPECT_TRUE(inputmethod_handler_->IsCorrectParam(1, selectBegin, selectEnd));

  selectBegin = 0;
  selectEnd = 10;
  EXPECT_FALSE(inputmethod_handler_->IsCorrectParam(1, selectBegin, selectEnd));
}

TEST_F(NWebInputMethodHandlerTest, SetWindowIdForIME) {
  uint32_t testWindowId = 12345;
  inputmethod_handler_->SetWindowIdForIME(testWindowId);
  EXPECT_EQ(inputmethod_handler_->windowId_, testWindowId);
}

TEST_F(NWebInputMethodHandlerTest, GetAllTextInfo) {
  inputmethod_handler_->whole_text_ = u"Hello 你好! 123";
  std::string result = inputmethod_handler_->GetAllTextInfo();
  EXPECT_EQ(result, "Hello 你好! 123");
}

TEST_F(NWebInputMethodHandlerTest, GetSelectEndIndex) {
  inputmethod_handler_->selected_to_ = 10;
  int32_t result = inputmethod_handler_->GetSelectEndIndex();
  EXPECT_EQ(result, 10);
}

TEST_F(NWebInputMethodHandlerTest, GetSelectStartIndex) {
  inputmethod_handler_->selected_from_ = 5;
  int32_t result = inputmethod_handler_->GetSelectStartIndex();
  EXPECT_EQ(result, 5);
}

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
TEST_F(NWebInputMethodHandlerTest, AutoFillWithIMFEventOnUI) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(
	::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, AutoFillWithIMFEvent(testing::_, testing::_, testing::_, testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  EXPECT_TRUE(inputmethod_handler_->browser_);
  bool is_username = true;
  bool is_other_account = false;
  bool is_new_password = true;
  std::string content = "test_content";
  inputmethod_handler_->AutoFillWithIMFEventOnUI(is_username, is_other_account, is_new_password, content);

  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->AutoFillWithIMFEventOnUI(is_username, is_other_account, is_new_password, content);
}

TEST_F(NWebInputMethodHandlerTest, AutoFillWithIMFEvent) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(
	::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  EXPECT_TRUE(inputmethod_handler_->browser_);
  inputmethod_handler_->AutoFillWithIMFEvent(true, true, true, "test_content");
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->AutoFillWithIMFEvent(true, true, true, "test_content");
  EXPECT_TRUE(inputmethod_handler_->browser_ == nullptr);
}
#endif

TEST_F(NWebInputMethodHandlerTest, FinishTextPreview) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->FinishTextPreview();
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(
	::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  EXPECT_TRUE(inputmethod_handler_->browser_);

  inputmethod_handler_->FinishTextPreview();
  inputmethod_handler_->SetNeedUnderLine(true);
}

TEST_F(NWebInputMethodHandlerTest, UpdateCompositionInfo) {
  std::u16string text = u"";
  inputmethod_handler_->has_composition_ = false;
  int32_t start = 0;
  int32_t end = 4;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);

  text = u"";
  start = -1;
  end = -1;
  inputmethod_handler_->preview_text_cache_ = u"t";
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->composition_type_ = COMPOSITION_CANCEL;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->OK);

  text = u"test";
  inputmethod_handler_->has_composition_ = false;
  inputmethod_handler_->composition_type_ = COMPOSITION_CURRENT;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->OK);

  start = 1;
  end = 1;
  inputmethod_handler_->whole_text_ = u"testtest";
  inputmethod_handler_->composition_range_start_ = 0;
  inputmethod_handler_->composition_range_end_ = 2;
  inputmethod_handler_->composition_type_ = COMPOSITION_POSITION;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->OK);

  text = u"";
  start = 2;
  end = 2;
  inputmethod_handler_->preview_text_cache_ = u"t";
  inputmethod_handler_->whole_text_ = u"testtest";
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->composition_range_start_ = 3;
  inputmethod_handler_->composition_range_end_ = 1;
  inputmethod_handler_->composition_type_ = COMPOSITION_POSITION;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);

  text = u"";
  start = 2;
  end = 2;
  inputmethod_handler_->preview_text_cache_ = u"t";
  inputmethod_handler_->whole_text_ = u"testtest";
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->composition_range_start_ = 1;
  inputmethod_handler_->composition_range_end_ = 3;
  inputmethod_handler_->composition_type_ = COMPOSITION_POSITION;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->OK);

  text = u"";
  start = 1;
  end = 2;
  inputmethod_handler_->preview_text_cache_ = u"t";
  inputmethod_handler_->whole_text_ = u"testtest";
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->composition_range_start_ = 1;
  inputmethod_handler_->composition_range_end_ = 3;
  inputmethod_handler_->composition_type_ = COMPOSITION_DELETE;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->OK);

  text = u"test";
  start = 1;
  end = 2;
  inputmethod_handler_->preview_text_cache_ = u"t";
  inputmethod_handler_->whole_text_ = u"testtest";
  inputmethod_handler_->has_composition_ = false;
  inputmethod_handler_->composition_range_start_ = 2;
  inputmethod_handler_->composition_range_end_ = 3;
  inputmethod_handler_->composition_type_ = COMPOSITION_REPLACE;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->OK);

  text = u"";
  start = 1;
  end = 2;
  inputmethod_handler_->preview_text_cache_ = u"test";
  inputmethod_handler_->whole_text_ = u"testtest";
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->composition_range_start_ = 2;
  inputmethod_handler_->composition_range_end_ = 1;
  inputmethod_handler_->composition_type_ = COMPOSITION_REPLACE;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);

  text = u"";
  start = 1;
  end = 2;
  inputmethod_handler_->preview_text_cache_ = u"test";
  inputmethod_handler_->whole_text_ = u"testtest";
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->composition_range_start_ = 1;
  inputmethod_handler_->composition_range_end_ = 3;
  inputmethod_handler_->composition_type_ = COMPOSITION_REPLACE;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->OK);

  text = u"";
  start = 3;
  end = 1;
  inputmethod_handler_->preview_text_cache_ = u"test";
  inputmethod_handler_->whole_text_ = u"testtest";
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->composition_range_start_ = 1;
  inputmethod_handler_->composition_range_end_ = 3;
  inputmethod_handler_->composition_type_ = COMPOSITION_REPLACE;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);
}

TEST_F(NWebInputMethodHandlerTest, GetCompositionTypeAndCheckInput) {
  inputmethod_handler_->has_composition_ = false;
  inputmethod_handler_->composition_range_start_ = 0;
  inputmethod_handler_->composition_range_end_ = 0;
  inputmethod_handler_->whole_text_ = u"";
  inputmethod_handler_->preview_text_cache_ = u"";
  CompositionType type;
  int32_t result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"", -1, -1, type);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
  EXPECT_EQ(type, COMPOSITION_INVALID);

  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", -1, -1, type);
  EXPECT_EQ(result, inputmethod_handler_->OK);
  EXPECT_EQ(type, COMPOSITION_CURRENT);

  inputmethod_handler_->composition_range_end_ = 3;
  inputmethod_handler_->whole_text_ = u"test";
  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 0, 0, type);
  EXPECT_EQ(result, inputmethod_handler_->OK);
  EXPECT_EQ(type, COMPOSITION_POSITION);

  inputmethod_handler_->has_composition_ = true;
  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 0, 2, type);
  EXPECT_EQ(result, inputmethod_handler_->OK);
  EXPECT_EQ(type, COMPOSITION_REPLACE);

  inputmethod_handler_->preview_text_cache_ = u"a";
  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"", 0, 0, type);
  EXPECT_EQ(result, inputmethod_handler_->OK);
  EXPECT_EQ(type, COMPOSITION_POSITION);

  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 4, 5, type);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
  EXPECT_EQ(type, COMPOSITION_REPLACE);

  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 2, 1, type);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
  EXPECT_EQ(type, COMPOSITION_INVALID);

  inputmethod_handler_->composition_range_start_ = 1;
  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 0, 4, type);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
  EXPECT_EQ(type, COMPOSITION_REPLACE);
}

TEST_F(NWebInputMethodHandlerTest, GetRightTextOfCursor) {
  inputmethod_handler_->textCursorReady_ = 1;
  inputmethod_handler_->selected_from_ = 0;
  inputmethod_handler_->selected_to_ = 5;
  inputmethod_handler_->whole_text_ = u"Hello, World!";
  inputmethod_handler_->is_need_notify_all_ = true;
  std::u16string result = inputmethod_handler_->GetRightTextOfCursor(5);
  EXPECT_EQ(result, u"");

  result = inputmethod_handler_->GetRightTextOfCursor(-5);
  EXPECT_EQ(result, u"");

  inputmethod_handler_->textCursorReady_ = 1;
  result = inputmethod_handler_->GetRightTextOfCursor(5);
  EXPECT_EQ(result, u"");
}

TEST_F(NWebInputMethodHandlerTest, GetLeftTextOfCursor) {
  inputmethod_handler_->textCursorReady_ = 0;
  inputmethod_handler_->selected_from_ = 10;
  inputmethod_handler_->selected_to_ = 20;
  inputmethod_handler_->whole_text_ = u"Hello, World!";
  inputmethod_handler_->textCursorReady_ = 1;
  inputmethod_handler_->is_need_notify_all_ = true;
  EXPECT_EQ(inputmethod_handler_->GetLeftTextOfCursor(5), u"");

  inputmethod_handler_->textCursorReady_ = 1;
  EXPECT_EQ(inputmethod_handler_->GetLeftTextOfCursor(5), u"");

  EXPECT_EQ(inputmethod_handler_->GetLeftTextOfCursor(-5), u"");

  EXPECT_EQ(inputmethod_handler_->GetLeftTextOfCursor(10), u"");
}

TEST_F(NWebInputMethodHandlerTest, GetTextIndexAtCursor) {
  inputmethod_handler_->is_need_notify_all_ = false;
  inputmethod_handler_->textCursorReady_ = 0;
  EXPECT_EQ(inputmethod_handler_->GetTextIndexAtCursor(), inputmethod_handler_->selected_to_);

  inputmethod_handler_->is_need_notify_all_ = true;
  EXPECT_EQ(inputmethod_handler_->GetTextIndexAtCursor(), 0);
}

TEST_F(NWebInputMethodHandlerTest, HasComposition) {
  inputmethod_handler_->has_composition_ = false;
  EXPECT_EQ(inputmethod_handler_->HasComposition(), inputmethod_handler_->has_composition_);
}

TEST_F(NWebInputMethodHandlerTest, GetIsEditableNode) {
  inputmethod_handler_->is_editable_node_ = true;
  bool result = inputmethod_handler_->GetIsEditableNode();
  EXPECT_TRUE(result);
}

TEST_F(NWebInputMethodHandlerTest, OnEditableChanged) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  auto browser = mock_browser.release();
  inputmethod_handler_->OnEditableChanged(browser, true);
  EXPECT_TRUE(inputmethod_handler_->is_editable_node_);
}

TEST_F(NWebInputMethodHandlerTest, SetFocusStatus) {
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->focus_rect_.x = 10;
  inputmethod_handler_->focus_rect_.y = 20;
  inputmethod_handler_->focus_rect_.width = 30;
  inputmethod_handler_->focus_rect_.height = 40;
  inputmethod_handler_->device_pixel_ratio_ = 2.0;
  inputmethod_handler_->offset_x_ = 5;
  inputmethod_handler_->offset_y_ = 6;
  inputmethod_handler_->focus_rect_status_ = true;
  inputmethod_handler_->SetFocusStatus(true);
  EXPECT_TRUE(inputmethod_handler_->focus_status_);
  inputmethod_handler_->SetFocusStatus(true);
  EXPECT_TRUE(inputmethod_handler_->focus_status_);

  inputmethod_handler_->SetFocusStatus(false);
  inputmethod_handler_->focus_rect_status_ = false;
  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  inputmethod_handler_->SetFocusStatus(false);
}

TEST_F(NWebInputMethodHandlerTest, SetVirtualDeviceRatio) {
  const float expectedRatio = -1.5f;
  inputmethod_handler_->SetVirtualDeviceRatio(expectedRatio);
  EXPECT_EQ(expectedRatio, inputmethod_handler_->device_pixel_ratio_);
}

TEST_F(NWebInputMethodHandlerTest, SetScreenOffSet) {
  inputmethod_handler_->focus_rect_status_ = true;
  inputmethod_handler_->offset_x_ = 0.0;
  inputmethod_handler_->offset_y_ = 0.0;
  inputmethod_handler_->focus_rect_ = {0, 0, 100, 50};
  inputmethod_handler_->device_pixel_ratio_ = 1.0;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->SetScreenOffSet(10.0, 20.0);
  EXPECT_EQ(inputmethod_handler_->offset_x_, 10.0);
  EXPECT_EQ(inputmethod_handler_->offset_y_, 20.0);

  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->focus_rect_status_ = false;
  inputmethod_handler_->offset_x_ = 0.0;
  inputmethod_handler_->offset_y_ = 0.0;
  inputmethod_handler_->SetScreenOffSet(10.0, 20.0);
  EXPECT_EQ(inputmethod_handler_->offset_x_, 10.0);
  EXPECT_EQ(inputmethod_handler_->offset_y_, 20.0);

  inputmethod_handler_->focus_rect_status_ = true;
  inputmethod_handler_->SetScreenOffSet(10.0, 20.0);
  EXPECT_EQ(inputmethod_handler_->offset_x_, 10.0);
  EXPECT_EQ(inputmethod_handler_->offset_y_, 20.0);

  inputmethod_handler_->offset_x_ = 0.0;
  inputmethod_handler_->offset_y_ = 0.0;
  inputmethod_handler_->focus_rect_ = {0, 0, 100, 50};
  inputmethod_handler_->SetScreenOffSet(10.0, 20.0);
  EXPECT_EQ(inputmethod_handler_->offset_x_, 10.0);
  EXPECT_EQ(inputmethod_handler_->offset_y_, 20.0);
}

TEST_F(NWebInputMethodHandlerTest, MoveCursor) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, SendKeyEvent(testing::_)).Times(testing::AtLeast(1));
  inputmethod_handler_->browser_ = mock_browser.release();
  IMFAdapterDirection direction = IMFAdapterDirection::UP;
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 1);

  direction = IMFAdapterDirection::LEFT;
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 2);

  direction = IMFAdapterDirection::RIGHT;
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 3);

  direction = IMFAdapterDirection::DOWN;
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 4);

  direction = static_cast<IMFAdapterDirection>(4);
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 5);
}

TEST_F(NWebInputMethodHandlerTest, DeleteBackwardHandlerOnUI) {
  std::unique_ptr mock_host = std::make_unique<MockCefBrowserHost>();
  auto mock_host_raw = mock_host.get();
  std::unique_ptr browser = std::make_unique<MockCefBrowser>(mock_host_raw);
  inputmethod_handler_->selected_from_ = 0;
  inputmethod_handler_->text_cursor_length_ = 0;
  inputmethod_handler_->preview_text_cache_ = u"";
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->DeleteBackwardHandlerOnUI(1);

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, SendKeyEvent(testing::_)).Times(testing::AtLeast(1));
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->selected_from_ = 2;
  int32_t length = 3;
  inputmethod_handler_->DeleteBackwardHandlerOnUI(length);
  EXPECT_EQ(inputmethod_handler_->text_cursor_length_, 2);

  inputmethod_handler_->selected_from_ = 5;
  length = 3;
  inputmethod_handler_->DeleteBackwardHandlerOnUI(length);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 3);

  inputmethod_handler_->preview_text_cache_ = u"A";
  length = 1;
  inputmethod_handler_->DeleteBackwardHandlerOnUI(length);
  EXPECT_EQ(inputmethod_handler_->preview_text_cache_.length(), 0);
}

TEST_F(NWebInputMethodHandlerTest, DeleteForwardHandlerOnUI) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->DeleteForwardHandlerOnUI(1);
  EXPECT_EQ(inputmethod_handler_->text_cursor_length_, 0);

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, SendKeyEvent(testing::_)).Times(testing::AtLeast(1));
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->whole_text_ = u"test";
  inputmethod_handler_->selected_from_ = 2;
  inputmethod_handler_->DeleteForwardHandlerOnUI(3);
  EXPECT_EQ(inputmethod_handler_->text_cursor_length_, 2);
  EXPECT_FALSE(inputmethod_handler_->is_need_notify_all_);

  inputmethod_handler_->whole_text_ = u"test";
  inputmethod_handler_->selected_from_ = 0;
  inputmethod_handler_->DeleteForwardHandlerOnUI(2);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 2);

  inputmethod_handler_->whole_text_ = u"test";
  inputmethod_handler_->selected_from_ = 6;
  inputmethod_handler_->DeleteForwardHandlerOnUI(2);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 2);

  inputmethod_handler_->preview_text_cache_ = u"a";
  inputmethod_handler_->DeleteForwardHandlerOnUI(1);
  EXPECT_TRUE(inputmethod_handler_->preview_text_cache_.empty());

  inputmethod_handler_->preview_text_cache_ = u"abc";
  inputmethod_handler_->DeleteForwardHandlerOnUI(1);
}

TEST_F(NWebInputMethodHandlerTest, SetNeedUnderLineOnUI) {
  inputmethod_handler_->SetNeedUnderLineOnUI(true);
  EXPECT_TRUE(inputmethod_handler_->is_need_underline_);
}

TEST_F(NWebInputMethodHandlerTest, FinishPreviewTextOnUI) {
  inputmethod_handler_->preview_text_cache_ = u"a";
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->FinishPreviewTextOnUI();
  EXPECT_FALSE(inputmethod_handler_->preview_text_cache_.empty());

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, ImeFinishComposingText(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->FinishPreviewTextOnUI();
  EXPECT_TRUE(inputmethod_handler_->preview_text_cache_.empty());
}

TEST_F(NWebInputMethodHandlerTest, CancelPreviewHandlerOnUI) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->CancelPreviewHandlerOnUI();
  EXPECT_TRUE(inputmethod_handler_->preview_text_cache_.empty());

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, ImeCancelComposition()).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->CancelPreviewHandlerOnUI();
  EXPECT_TRUE(inputmethod_handler_->preview_text_cache_.empty());
}

TEST_F(NWebInputMethodHandlerTest, PreviewTextHandlerOnUI) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, ImeSetComposition(testing::_, testing::_, testing::_, testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->is_need_underline_ = true;
  inputmethod_handler_->composition_type_ = COMPOSITION_REPLACE;
  std::u16string text = u"test";
  int32_t start = 0;
  int32_t end = 4;
  inputmethod_handler_->PreviewTextHandlerOnUI(text, start, end);

  inputmethod_handler_->composition_type_ = COMPOSITION_CANCEL;
  inputmethod_handler_->PreviewTextHandlerOnUI(text, start, end);

  inputmethod_handler_->is_need_underline_ = false;
  inputmethod_handler_->composition_type_ = COMPOSITION_REPLACE;
  inputmethod_handler_->PreviewTextHandlerOnUI(text, start, end);

  inputmethod_handler_->composition_type_ = COMPOSITION_CANCEL;
  start = 1;
  end = 3;
  inputmethod_handler_->PreviewTextHandlerOnUI(text, start, end);
}

TEST_F(NWebInputMethodHandlerTest, ClearComposingStatus) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->preview_text_cache_ = u"test";
  inputmethod_handler_->composition_range_start_ = 1;
  inputmethod_handler_->composition_range_end_ = 4;
  inputmethod_handler_->ClearComposingStatus();
  EXPECT_FALSE(inputmethod_handler_->has_composition_);
  EXPECT_EQ(inputmethod_handler_->preview_text_cache_, std::u16string());
  EXPECT_EQ(inputmethod_handler_->composition_range_start_, 0);
  EXPECT_EQ(inputmethod_handler_->composition_range_end_, 0);

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, SetHasComposition(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->preview_text_cache_ = u"test";
  inputmethod_handler_->composition_range_start_ = 1;
  inputmethod_handler_->composition_range_end_ = 4;
  inputmethod_handler_->ClearComposingStatus();
  EXPECT_FALSE(inputmethod_handler_->has_composition_);
  EXPECT_EQ(inputmethod_handler_->preview_text_cache_, std::u16string());
  EXPECT_EQ(inputmethod_handler_->composition_range_start_, 0);
  EXPECT_EQ(inputmethod_handler_->composition_range_end_, 0);
}

TEST_F(NWebInputMethodHandlerTest, InsertTextHandlerOnUI) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, SendKeyEvent(testing::_)).Times(testing::AtLeast(1));
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->ime_text_composing_ = false;
  inputmethod_handler_->composing_text_.clear();
  inputmethod_handler_->textCursorReady_ = 0;
  inputmethod_handler_->selected_from_ = 0;
  const std::u16string empty_text = u"";
  inputmethod_handler_->InsertTextHandlerOnUI(empty_text);

  std::u16string text = u"A";
  inputmethod_handler_->InsertTextHandlerOnUI(text);

  text = u"test";
  inputmethod_handler_->InsertTextHandlerOnUI(text);
  inputmethod_handler_->ime_text_composing_ = false;
  inputmethod_handler_->InsertTextHandlerOnUI(text);
}

TEST_F(NWebInputMethodHandlerTest, WebBlurKeyboardHideOnUI) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, SetFocusOnWeb()).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->WebBlurKeyboardHideOnUI();
  EXPECT_TRUE(inputmethod_handler_->isManualCloseKeyboard_);
}

TEST_F(NWebInputMethodHandlerTest, SetIMEStatusOnUI) {
  bool enable_ime = true;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, ImeFinishComposingText(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->isManualCloseKeyboard_ = true;
  inputmethod_handler_->ime_text_composing_ = false;
  inputmethod_handler_->SetIMEStatusOnUI(enable_ime);
  EXPECT_FALSE(inputmethod_handler_->isManualCloseKeyboard_);
  EXPECT_TRUE(inputmethod_handler_->ime_shown_);

  bool disable_ime = false;
  inputmethod_handler_->ime_text_composing_ = false;
  inputmethod_handler_->composing_text_ = u"test";
  inputmethod_handler_->SetIMEStatusOnUI(disable_ime);
  EXPECT_FALSE(inputmethod_handler_->ime_text_composing_);
  EXPECT_FALSE(inputmethod_handler_->composing_text_.empty());
  EXPECT_FALSE(inputmethod_handler_->ime_shown_);

  inputmethod_handler_->ime_text_composing_ = true;
  inputmethod_handler_->composing_text_ = u"test";
  inputmethod_handler_->SetIMEStatusOnUI(enable_ime);
  EXPECT_TRUE(inputmethod_handler_->ime_text_composing_);
  EXPECT_FALSE(inputmethod_handler_->composing_text_.empty());
  EXPECT_TRUE(inputmethod_handler_->ime_shown_);
}

TEST_F(NWebInputMethodHandlerTest, DeleteForward) {
  int32_t test_length = 5;
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->DeleteForward(test_length);

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->DeleteForward(test_length);
}

TEST_F(NWebInputMethodHandlerTest, DeleteBackward) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->DeleteBackward(10);

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->DeleteBackward(10);
}

TEST_F(NWebInputMethodHandlerTest, InsertText) {
  std::u16string empty_text;
  testing::internal::CaptureStderr();
  inputmethod_handler_->InsertText(empty_text);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("insert text empty!"), std::string::npos);

  inputmethod_handler_->browser_ = nullptr;
  std::u16string text = u"test";
  inputmethod_handler_->InsertText(text);

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  text = u"test";
  inputmethod_handler_->InsertText(text);
}

TEST_F(NWebInputMethodHandlerTest, WebBlurKeyboardHide) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->WebBlurKeyboardHide();
  EXPECT_EQ(inputmethod_handler_->browser_, nullptr);

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->WebBlurKeyboardHide();
  EXPECT_NE(inputmethod_handler_->browser_->GetHost(), nullptr);
}


TEST_F(NWebInputMethodHandlerTest, SetIMEStatus) {
  inputmethod_handler_->browser_ = nullptr;
  bool status = true;
  inputmethod_handler_->SetIMEStatus(status);
  EXPECT_EQ(inputmethod_handler_->browser_, nullptr);

  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).Times(::testing::AtLeast(1)).WillRepeatedly(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->SetIMEStatus(status);
  EXPECT_NE(inputmethod_handler_->browser_->GetHost(), nullptr);
}

TEST_F(NWebInputMethodHandlerTest, OnUpdateTextInputStateCalled) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  auto browser = mock_browser.release();
  CefString text = u"";
  inputmethod_handler_->whole_text_ = u"";
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 0), CefRange(0, 0));

  inputmethod_handler_->whole_text_ = u"text";
  text = u"text";
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(1, 3), CefRange(0, 0));
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(1, 3), CefRange(0, 0));

  CefRange compositon_range = CefRange::InvalidRange();
  inputmethod_handler_->composition_range_start_ = compositon_range.from;
  inputmethod_handler_->composition_range_end_ = compositon_range.to;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), compositon_range);
  EXPECT_FALSE(inputmethod_handler_->has_composition_);
  EXPECT_EQ(inputmethod_handler_->composition_range_start_, 0);
  EXPECT_EQ(inputmethod_handler_->composition_range_end_, 0);
  EXPECT_EQ(inputmethod_handler_->preview_text_cache_, u"");

  inputmethod_handler_->composition_range_start_ = 1;
  inputmethod_handler_->composition_range_end_ = 3;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(1, 3));
  EXPECT_FALSE(inputmethod_handler_->has_composition_);
  EXPECT_EQ(inputmethod_handler_->composition_range_start_, 1);
  EXPECT_EQ(inputmethod_handler_->composition_range_end_, 3);
  EXPECT_EQ(inputmethod_handler_->preview_text_cache_, u"");

  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(1, 3));
  EXPECT_EQ(inputmethod_handler_->whole_text_, u"text");

  std::unique_lock<std::mutex> lock(inputmethod_handler_->textCursorMutex_);
  inputmethod_handler_->textCursorReady_ = 1;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(1, 3));
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 1);

  inputmethod_handler_->selected_from_ = 0;
  inputmethod_handler_->selected_to_ = 4;
  inputmethod_handler_->composition_range_start_ = 0;
  inputmethod_handler_->composition_range_end_ = 4;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(0, 4));

  inputmethod_handler_->composition_range_start_ = 0;
  inputmethod_handler_->composition_range_end_ = 5;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(0, 5));

  text = u"";
  inputmethod_handler_->whole_text_ = u"";
  inputmethod_handler_->composition_range_start_ = 0;
  inputmethod_handler_->composition_range_end_ = 4;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(0, 4));

  inputmethod_handler_->composition_range_start_ = 0;
  inputmethod_handler_->composition_range_end_ = 0;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(0, 0));
}

TEST_F(NWebInputMethodHandlerTest, IsTextInputStateChange) {
  inputmethod_handler_->whole_text_ = u"old text";
  std::u16string text = u"new text";
  bool result = inputmethod_handler_->IsTextInputStateChange(text, CefRange(0, 0), CefRange(0, 0));
  EXPECT_TRUE(result);

  inputmethod_handler_->whole_text_ = u"text";
  text = u"text";
  result = inputmethod_handler_->IsTextInputStateChange(text, CefRange(1, 3), CefRange(0, 0));
  EXPECT_TRUE(result);

  result = inputmethod_handler_->IsTextInputStateChange(text, CefRange(0, 0), CefRange(1, 3));
  EXPECT_TRUE(result);

  CefRange selected_range = CefRange(inputmethod_handler_->selected_from_,
    inputmethod_handler_->selected_to_);
  CefRange composition_range = CefRange(inputmethod_handler_->composition_range_start_,
    inputmethod_handler_->composition_range_end_);
  result = inputmethod_handler_->IsTextInputStateChange(text, selected_range, composition_range);
  EXPECT_FALSE(result);

  selected_range = CefRange(1, 3);
  result = inputmethod_handler_->IsTextInputStateChange(text, selected_range, composition_range);
  EXPECT_TRUE(result);

  inputmethod_handler_->whole_text_ = u"";
  text = u"";
  result = inputmethod_handler_->IsTextInputStateChange(text, CefRange(0, 0), CefRange(0, 0));
  EXPECT_FALSE(result);

  inputmethod_handler_->whole_text_ = u"text";
  text = u"text";
  selected_range.from = std::numeric_limits<uint32_t>::max();
  selected_range.to = std::numeric_limits<uint32_t>::max();
  composition_range.from = std::numeric_limits<uint32_t>::max();
  composition_range.to = std::numeric_limits<uint32_t>::max();
  result = inputmethod_handler_->IsTextInputStateChange(text, selected_range, composition_range);
  EXPECT_TRUE(result);
}

TEST_F(NWebInputMethodHandlerTest, OnImeCompositionRangeChanged) {
  CefRange selected_range = CefRange(0, 0);
  selected_range.from = std::numeric_limits<uint32_t>::max();
  selected_range.to = std::numeric_limits<uint32_t>::max();
  testing::internal::CaptureStderr();
    CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  auto browser = mock_browser.release();
  inputmethod_handler_->OnImeCompositionRangeChanged(browser, selected_range);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("NWebInputMethodHandler::OnImeCompositionRangeChanged"), std::string::npos);
}

TEST_F(NWebInputMethodHandlerTest, OnCursorUpdate_Test_001) {
  inputmethod_handler_->focus_status_ = false;
  CefRect rect;
  rect.x = 0;
  rect.y = 0;
  rect.width = 100;
  rect.height = 20;
  inputmethod_handler_->OnCursorUpdate(rect);
  EXPECT_EQ(inputmethod_handler_->focus_rect_.x, rect.x);
  EXPECT_EQ(inputmethod_handler_->focus_rect_.y, rect.y);
  EXPECT_EQ(inputmethod_handler_->focus_rect_.width, rect.width);
  EXPECT_EQ(inputmethod_handler_->focus_rect_.height, rect.height);
  EXPECT_TRUE(inputmethod_handler_->focus_rect_status_);

  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->OnCursorUpdate(rect);
  EXPECT_TRUE(inputmethod_handler_->focus_rect_status_);
}

TEST_F(NWebInputMethodHandlerTest, SetNeedReattach) {
  inputmethod_handler_->SetNeedReattach(NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOncontinue_);
  EXPECT_FALSE(inputmethod_handler_->isNeedReattachOnfocus_);

  inputmethod_handler_->SetNeedReattach(NWebInputMethodClient::HideTextinputType::FROM_ONBLUR);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOncontinue_);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOnfocus_);

  inputmethod_handler_->SetNeedReattach(NWebInputMethodClient::HideTextinputType::FROM_KERNEL);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOncontinue_);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOnfocus_);
}

TEST_F(NWebInputMethodHandlerTest, AttachTest) {
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->focus_rect_status_ = true;
  inputmethod_handler_->nweb_id_ = 123;
  NWebInputMethodClient::InputInfo inputInfo;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  auto browser = mock_browser.release();
  inputmethod_handler_->Attach(browser, inputInfo, true, 0, 0);
  EXPECT_FALSE(inputmethod_handler_->isAttached_);
  EXPECT_EQ(inputmethod_handler_->lastAttachNWebId_, 0);

  inputmethod_handler_->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->Attach(browser, inputInfo, true, 0, 0);
  EXPECT_FALSE(inputmethod_handler_->isAttached_);

  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->focus_rect_status_ = false;
  inputmethod_handler_->Attach(browser, inputInfo, true, 0, 0);
}

TEST_F(NWebInputMethodHandlerTest, TestAttach_001) {
  auto handler = new MockNWebInputMethodHandler();
  testing::Mock::AllowLeak(handler);
  CefRefPtr<CefBrowser> browser;
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = true;
  int32_t enterKeyType = 0;
  int32_t requestKeyboardReason = 0;
  handler->focus_status_ = true;
  handler->focus_rect_status_ = true;
  handler->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  handler->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType, requestKeyboardReason);
  EXPECT_FALSE(handler->isAttached_);
}

TEST_F(NWebInputMethodHandlerTest, TestAttach_002) {
  auto handler = new MockNWebInputMethodHandler();
  testing::Mock::AllowLeak(handler);
  CefRefPtr<CefBrowser> browser;
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = true;
  int32_t enterKeyType = 0;
  int32_t requestKeyboardReason = 0;
  handler->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType, requestKeyboardReason);
  EXPECT_FALSE(handler->isAttached_);
}

TEST_F(NWebInputMethodHandlerTest, TestAttach_003) {
  auto handler = new MockNWebInputMethodHandler();
  testing::Mock::AllowLeak(handler);
  handler->focus_status_ = true;
  handler->focus_rect_status_ = true;
  CefRefPtr<CefBrowser> browser;
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = true;
  int32_t enterKeyType = 0;
  int32_t requestKeyboardReason = 0;
  EXPECT_CALL(*handler, AttachToSystemIME(is_need_reset_listener, requestKeyboardReason))
      .WillOnce(testing::Return(true));
  handler->inputmethod_adapter_ = nullptr;
  handler->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType, requestKeyboardReason);
  EXPECT_FALSE(handler->isAttached_);
}

TEST_F(NWebInputMethodHandlerTest, TestAttach_004) {
  auto handler = new MockNWebInputMethodHandler();
  testing::Mock::AllowLeak(handler);
  CefRefPtr<CefBrowser> browser;
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = true;
  int32_t enterKeyType = 0;
  int32_t requestKeyboardReason = 0;
  EXPECT_CALL(*handler, AttachToSystemIME(is_need_reset_listener, requestKeyboardReason))
      .WillOnce(testing::Return(true));
  handler->focus_status_ = false;
  handler->focus_rect_status_ = true;
  handler->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  handler->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType, requestKeyboardReason);
  EXPECT_FALSE(handler->isAttached_);
}

TEST_F(NWebInputMethodHandlerTest, TestAttach_005) {
  auto handler = new MockNWebInputMethodHandler();
  testing::Mock::AllowLeak(handler);
  CefRefPtr<CefBrowser> browser;
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = true;
  int32_t enterKeyType = 0;
  int32_t requestKeyboardReason = 0;
  EXPECT_CALL(*handler, AttachToSystemIME(is_need_reset_listener, requestKeyboardReason))
      .WillOnce(testing::Return(true));
  handler->focus_status_ = true;
  handler->focus_rect_status_ = false;
  handler->inputmethod_adapter_ = std::make_unique<MockIMFAdapterImpl>();
  handler->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType, requestKeyboardReason);
  EXPECT_FALSE(handler->isAttached_);
}

TEST_F(NWebInputMethodHandlerTest, AttachToSystemIME) {
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  bool result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(result);

  inputmethod_handler_->focus_status_ = false;
  result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(result);
  EXPECT_FALSE(inputmethod_handler_->isNeedReattachOnfocus_);

  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->isAttachSuccess_ = false;
  result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(result);
  EXPECT_FALSE(inputmethod_handler_->isNeedReattachOnfocus_);

  inputmethod_handler_->inputmethod_listener_ =
    std::make_shared<MockOnTextChangedListenerImpl>();
  inputmethod_handler_->isAttachSuccess_ = true;
  result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(inputmethod_handler_->isFocusSwitchOnBlur_);

  result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(result);
  EXPECT_TRUE(inputmethod_handler_->fill_content_.empty());

  inputmethod_handler_->isAttachSuccess_ = false;
  result = inputmethod_handler_->AttachToSystemIME(true, 0);
}

TEST_F(NWebInputMethodHandlerTest, ComputeEditorInfo) {
  NWebInputMethodClient::InputInfo inputInfo;
  inputInfo.show_keyboard = true;
  inputInfo.input_flags = CEF_TEXT_INPUT_FLAG_HAS_BEEN_PASSWORD;
  inputInfo.node_id = 42;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_EMAIL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TEXT;
  int32_t customEnterKeyType = 1;
  inputmethod_handler_->ComputeEditorInfo(inputInfo, customEnterKeyType);
  EXPECT_FALSE(inputmethod_handler_->type_text_flag_multi_line_);
  EXPECT_TRUE(inputmethod_handler_->show_keyboard_);
  EXPECT_EQ(inputmethod_handler_->input_flags_, CEF_TEXT_INPUT_FLAG_HAS_BEEN_PASSWORD);
  EXPECT_EQ(inputmethod_handler_->input_node_id_, 42);
  EXPECT_FALSE(inputmethod_handler_->input_is_password_);

  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputmethod_handler_->ComputeEditorInfo(inputInfo, customEnterKeyType);

  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_PASSWORD;
  inputmethod_handler_->ComputeEditorInfo(inputInfo, customEnterKeyType);
  EXPECT_TRUE(inputmethod_handler_->input_is_password_);

  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TEXT;
  inputmethod_handler_->ComputeEditorInfo(inputInfo, customEnterKeyType);
  EXPECT_FALSE(inputmethod_handler_->input_is_password_);

  customEnterKeyType = 9;
  inputmethod_handler_->ComputeEditorInfo(inputInfo, customEnterKeyType);
}

TEST_F(NWebInputMethodHandlerTest, UpdateTextFieldStatus_001) {
  inputmethod_handler_->browser_ = nullptr;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(0);
  inputmethod_handler_->UpdateTextFieldStatus(true, true);
}

TEST_F(NWebInputMethodHandlerTest, UpdateTextFieldStatus_002) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->UpdateTextFieldStatus(true, true);
  EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, UpdateTextFieldStatus_003) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(nullptr));
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->UpdateTextFieldStatus(true, true);
  EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, UpdateTextFieldStatusHandlerOnUI_001) {
  inputmethod_handler_->browser_ = nullptr;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(0);
  inputmethod_handler_->UpdateTextFieldStatusHandlerOnUI(true, true);
}

TEST_F(NWebInputMethodHandlerTest, UpdateTextFieldStatusHandlerOnUI_002) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->UpdateTextFieldStatusHandlerOnUI(true, true);
  EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, UpdateTextFieldStatusHandlerOnUI_003) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(nullptr));
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->UpdateTextFieldStatusHandlerOnUI(true, true);
  EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, HandleSecurityLayerHandlerOnUI_001) {
  inputmethod_handler_->browser_ = nullptr;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(0);
  inputmethod_handler_->HandleSecurityLayerHandlerOnUI();
}

TEST_F(NWebInputMethodHandlerTest, HandleSecurityLayerHandlerOnUI_002) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->HandleSecurityLayerHandlerOnUI();
  EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, HandleSecurityLayerHandlerOnUI_003) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(nullptr));
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->HandleSecurityLayerHandlerOnUI();
  EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, HandleSecurityLayer_001) {
  inputmethod_handler_->browser_ = nullptr;
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(0);
  inputmethod_handler_->HandleSecurityLayer();
}

TEST_F(NWebInputMethodHandlerTest, HandleSecurityLayer_002) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->HandleSecurityLayer();
  EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, HandleSecurityLayer_003) {
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(nullptr));
  inputmethod_handler_->browser_ = mock_browser.release();
  inputmethod_handler_->HandleSecurityLayer();
  EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, TextInputActionToIMFAdapter) {
  NWebInputMethodClient::InputInfo inputInfo;
  inputInfo.input_action = CEF_TEXT_INPUT_ACTION_DEFAULT;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_SEARCH;
  IMFAdapterEnterKeyType result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::SEARCH);

  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_NONE;
  inputmethod_handler_->type_text_flag_multi_line_ = true;
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::NEW_LINE);

  inputmethod_handler_->type_text_flag_multi_line_ = false;
  inputInfo.input_flags = CEF_TEXT_INPUT_FLAG_HAVE_NEXT_FOCUSABLE_ELEMENT;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillOnce(testing::Return(false));
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillOnce(testing::Return(true));
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::GO);

  inputInfo.input_flags = CEF_TEXT_INPUT_FLAG_VERTICAL;
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::GO);

  inputInfo.input_action = CEF_TEXT_INPUT_ACTION_ENTER;
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::NEW_LINE);

  inputInfo.input_action = CEF_TEXT_INPUT_ACTION_DONE;
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::DONE);

  inputInfo.input_action = CEF_TEXT_INPUT_ACTION_GO;
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::GO);

  inputInfo.input_action = CEF_TEXT_INPUT_ACTION_NEXT;
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::NEXT);

  inputInfo.input_action = CEF_TEXT_INPUT_ACTION_PREVIOUS;
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::PREVIOUS);

  inputInfo.input_action = CEF_TEXT_INPUT_ACTION_SEARCH;
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::SEARCH);

  inputInfo.input_action = CEF_TEXT_INPUT_ACTION_SEND;
  result = inputmethod_handler_->TextInputActionToIMFAdapter(inputInfo);
  EXPECT_EQ(result, IMFAdapterEnterKeyType::SEND);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
}

TEST_F(NWebInputMethodHandlerTest, TextInputTypeToIMFAdapter) {
  cef_text_input_type_t input = CEF_TEXT_INPUT_TYPE_TEXT;
  IMFAdapterTextInputType result = inputmethod_handler_->TextInputTypeToIMFAdapter(input);
  EXPECT_EQ(IMFAdapterTextInputType::TEXT, result);

  input = CEF_TEXT_INPUT_TYPE_PASSWORD;
  result = inputmethod_handler_->TextInputTypeToIMFAdapter(input);
  EXPECT_EQ(IMFAdapterTextInputType::VISIBLE_PASSWORD, result);

  input = CEF_TEXT_INPUT_TYPE_EMAIL;
  result = inputmethod_handler_->TextInputTypeToIMFAdapter(input);
  EXPECT_EQ(IMFAdapterTextInputType::EMAIL_ADDRESS, result);

  input = CEF_TEXT_INPUT_TYPE_NUMBER;
  result = inputmethod_handler_->TextInputTypeToIMFAdapter(input);
  EXPECT_EQ(IMFAdapterTextInputType::NUMBER, result);

  input = CEF_TEXT_INPUT_TYPE_TELEPHONE;
  result = inputmethod_handler_->TextInputTypeToIMFAdapter(input);
  EXPECT_EQ(IMFAdapterTextInputType::PHONE, result);

  input = CEF_TEXT_INPUT_TYPE_URL;
  result = inputmethod_handler_->TextInputTypeToIMFAdapter(input);
  EXPECT_EQ(IMFAdapterTextInputType::URL, result);

  input = CEF_TEXT_INPUT_TYPE_TEXT_AREA;
  result = inputmethod_handler_->TextInputTypeToIMFAdapter(input);
  EXPECT_EQ(IMFAdapterTextInputType::TEXT, result);

  input = CEF_TEXT_INPUT_TYPE_CONTENT_EDITABLE;
  result = inputmethod_handler_->TextInputTypeToIMFAdapter(input);
  EXPECT_EQ(IMFAdapterTextInputType::TEXT, result);

  input = static_cast<cef_text_input_type_t>(-1);
  result = inputmethod_handler_->TextInputTypeToIMFAdapter(input);
  EXPECT_EQ(IMFAdapterTextInputType::TEXT, result);
}

TEST_F(NWebInputMethodHandlerTest, TextInputModeToIMFAdapter) {
  cef_text_input_mode_t mode = CEF_TEXT_INPUT_MODE_TEXT;
  IMFAdapterTextInputType result = inputmethod_handler_->TextInputModeToIMFAdapter(mode);
  EXPECT_EQ(IMFAdapterTextInputType::TEXT, result);
  EXPECT_FALSE(inputmethod_handler_->type_text_flag_multi_line_);

  mode = CEF_TEXT_INPUT_MODE_TEL;
  result = inputmethod_handler_->TextInputModeToIMFAdapter(mode);
  EXPECT_EQ(IMFAdapterTextInputType::PHONE, result);
  EXPECT_FALSE(inputmethod_handler_->type_text_flag_multi_line_);

  mode = CEF_TEXT_INPUT_MODE_URL;
  result = inputmethod_handler_->TextInputModeToIMFAdapter(mode);
  EXPECT_EQ(IMFAdapterTextInputType::URL, result);
  EXPECT_FALSE(inputmethod_handler_->type_text_flag_multi_line_);

  mode = CEF_TEXT_INPUT_MODE_EMAIL;
  result = inputmethod_handler_->TextInputModeToIMFAdapter(mode);
  EXPECT_EQ(IMFAdapterTextInputType::EMAIL_ADDRESS, result);
  EXPECT_FALSE(inputmethod_handler_->type_text_flag_multi_line_);

  mode = CEF_TEXT_INPUT_MODE_EMAIL;
  result = inputmethod_handler_->TextInputModeToIMFAdapter(mode);
  EXPECT_EQ(IMFAdapterTextInputType::EMAIL_ADDRESS, result);
  EXPECT_FALSE(inputmethod_handler_->type_text_flag_multi_line_);

  mode = CEF_TEXT_INPUT_MODE_NUMERIC;
  result = inputmethod_handler_->TextInputModeToIMFAdapter(mode);
  EXPECT_EQ(IMFAdapterTextInputType::NUMBER, result);
  EXPECT_FALSE(inputmethod_handler_->type_text_flag_multi_line_);

  mode = CEF_TEXT_INPUT_MODE_DECIMAL;
  result = inputmethod_handler_->TextInputModeToIMFAdapter(mode);
  EXPECT_EQ(IMFAdapterTextInputType::NUMBER, result);
  EXPECT_FALSE(inputmethod_handler_->type_text_flag_multi_line_);

  mode = CEF_TEXT_INPUT_MODE_SEARCH;
  result = inputmethod_handler_->TextInputModeToIMFAdapter(mode);
  EXPECT_EQ(IMFAdapterTextInputType::TEXT, result);
  EXPECT_TRUE(inputmethod_handler_->type_text_flag_multi_line_);

  mode = static_cast<cef_text_input_mode_t>(-1);
  result = inputmethod_handler_->TextInputModeToIMFAdapter(mode);
  EXPECT_EQ(IMFAdapterTextInputType::TEXT, result);
  EXPECT_TRUE(inputmethod_handler_->type_text_flag_multi_line_);
}

TEST_F(NWebInputMethodHandlerTest, TestInsertNormalText) {
  const std::u16string text = u"Hello, World!";
  EXPECT_CALL(*handler_, InsertText(text));
  listener_->InsertText(text);

  const std::u16string newline = u"\n";
  EXPECT_CALL(*handler_, InsertText(newline)).Times(0);
  listener_->InsertText(newline);

  const std::u16string textH = u"H";
  listener_->InsertText(textH);

  const std::u16string textspace = u" \n";
  listener_->InsertText(textspace);
}

TEST_F(NWebInputMethodHandlerTest, TestDeleteForward) {
  const int32_t length = 5;
  EXPECT_CALL(*handler_, DeleteForward(length));
  listener_->DeleteForward(length);
}

TEST_F(NWebInputMethodHandlerTest, TestDeleteBackward) {
  const int32_t length = 3;
  EXPECT_CALL(*handler_, DeleteBackward(length));
  listener_->DeleteBackward(length);
}

TEST_F(NWebInputMethodHandlerTest, TestSendKeyboardStatusShow) {
  const IMFAdapterKeyboardStatus status = IMFAdapterKeyboardStatus::SHOW;
  EXPECT_CALL(*handler_, SetIMEStatus(true));
  listener_->SendKeyboardStatus(status);
}

TEST_F(NWebInputMethodHandlerTest, TestSendKeyboardStatusHide) {
  const IMFAdapterKeyboardStatus status = IMFAdapterKeyboardStatus::HIDE;
  EXPECT_CALL(*handler_, SetIMEStatus(false));
  listener_->SendKeyboardStatus(status);
}

TEST_F(NWebInputMethodHandlerTest, TestSendKeyboardStatusNone) {
  const IMFAdapterKeyboardStatus status = IMFAdapterKeyboardStatus::NONE;
  EXPECT_CALL(*handler_, SetIMEStatus(false));
  listener_->SendKeyboardStatus(status);
}

TEST_F(NWebInputMethodHandlerTest, TestSendFunctionKeyEnter) {
  std::shared_ptr<MockIMFAdapterFunctionKeyAdapter>
	  functionKey = std::make_shared<MockIMFAdapterFunctionKeyAdapter>();
  handler_ = nullptr;
  listener_ = std::make_unique<OnTextChangedListenerImpl>(handler_);
  const IMFAdapterEnterKeyType enterKeyType = IMFAdapterEnterKeyType::NONE;
  listener_->SendFunctionKey(functionKey);
  functionKey  = nullptr;
  listener_->SendFunctionKey(functionKey);
  handler_ = new MockNWebInputMethodHandler();
  testing::Mock::AllowLeak(handler_);
  listener_ = std::make_unique<OnTextChangedListenerImpl>(handler_);
  EXPECT_CALL(*handler_, SendEnterKeyEvent(static_cast<int32_t>(enterKeyType)));
  listener_->SendFunctionKey(functionKey);
  functionKey = std::make_shared<MockIMFAdapterFunctionKeyAdapter>();
  listener_->SendFunctionKey(functionKey);
}

TEST_F(NWebInputMethodHandlerTest, TestSetKeyboardStatusFalse) {
  const bool status = false;
  EXPECT_CALL(*handler_, SetIMEStatus(status));
  listener_->SetKeyboardStatus(status);
}

TEST_F(NWebInputMethodHandlerTest, TestKeyboardUpperRightCornerHide) {
  EXPECT_CALL(*handler_, WebBlurKeyboardHide());
  listener_->KeyboardUpperRightCornerHide();
}

TEST_F(NWebInputMethodHandlerTest, TestMoveCursorForward) {
  const IMFAdapterDirection direction = IMFAdapterDirection::UP;
  EXPECT_CALL(*handler_, MoveCursor(direction));
  listener_->MoveCursor(direction);
}

TEST_F(NWebInputMethodHandlerTest, TestMoveCursorNone) {
  const IMFAdapterDirection direction = IMFAdapterDirection::NONE;
  EXPECT_CALL(*handler_, MoveCursor(direction)).Times(0);
  listener_->MoveCursor(direction);
  listener_->SendKeyEventFromInputMethod();
  std::string content = "test";
  listener_->AutoFillWithIMFEvent(true, true, true, content);
}

TEST_F(NWebInputMethodHandlerTest, NeedKeyboardShow) {
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isManualCloseKeyboard_ = false;
  inputmethod_handler_->cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_NONE;
  auto result = inputmethod_handler_->NeedKeyboardShow();
  EXPECT_FALSE(result);

  inputmethod_handler_->show_keyboard_ = true;
  inputmethod_handler_->isManualCloseKeyboard_ = false;
  inputmethod_handler_->cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_DEFAULT;
  result = inputmethod_handler_->NeedKeyboardShow();
  EXPECT_TRUE(result);

  inputmethod_handler_->show_keyboard_ = true;
  inputmethod_handler_->isManualCloseKeyboard_ = true;
  inputmethod_handler_->cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_NONE;
  result = inputmethod_handler_->NeedKeyboardShow();
  EXPECT_FALSE(result);

  inputmethod_handler_->show_keyboard_ = true;
  inputmethod_handler_->isManualCloseKeyboard_ = false;
  inputmethod_handler_->cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_NONE;
  result = inputmethod_handler_->NeedKeyboardShow();
  EXPECT_FALSE(result);
}

TEST_F(NWebInputMethodHandlerTest, IsKeyboardShow) {
  inputmethod_handler_->show_keyboard_ = true;
  inputmethod_handler_->isManualCloseKeyboard_ = false;
  inputmethod_handler_->cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputmethod_handler_->isAttachSuccess_ = false;
  auto result = inputmethod_handler_->IsKeyboardShow();
  EXPECT_FALSE(result);

  inputmethod_handler_->show_keyboard_ = true;
  inputmethod_handler_->isManualCloseKeyboard_ = false;
  inputmethod_handler_->cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputmethod_handler_->isAttachSuccess_ = true;
  result = inputmethod_handler_->IsKeyboardShow();
  EXPECT_TRUE(result);

  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isManualCloseKeyboard_ = true;
  inputmethod_handler_->cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_NONE;
  inputmethod_handler_->isAttachSuccess_ = false;
  result = inputmethod_handler_->IsKeyboardShow();
  EXPECT_FALSE(result);

  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isManualCloseKeyboard_ = false;
  inputmethod_handler_->cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_NONE;
  inputmethod_handler_->isAttachSuccess_ = true;
  result = inputmethod_handler_->IsKeyboardShow();
  EXPECT_FALSE(result);
}

TEST_F(NWebInputMethodHandlerTest, HandleExtendAction) {
  int32_t action = 0;
  inputmethod_handler_->browser_ = nullptr;
  listener_->HandleExtendAction(action);
  inputmethod_handler_->HandleExtendAction(action);
  inputmethod_handler_->HandleExtendActionOnUI(action);
  EXPECT_EQ(inputmethod_handler_->browser_, nullptr);

  auto mock_host = std::make_shared<MockCefBrowserHost>();
  auto mock_browser = std::make_shared<MockCefBrowser>(mock_host.get());
  ASSERT_NE(mock_host, nullptr);
  ASSERT_NE(mock_browser, nullptr);
  inputmethod_handler_->browser_ =
      CefRefPtr<MockCefBrowser>(mock_browser.get());

  EXPECT_CALL(*mock_browser, GetHost).WillOnce(testing::Return(nullptr));
  inputmethod_handler_->HandleExtendAction(action);
  EXPECT_CALL(*mock_browser, GetHost).WillOnce(testing::Return(nullptr));
  inputmethod_handler_->HandleExtendActionOnUI(action);

  CefRefPtr<ArkWebBrowserHostExt> arkweb_host(mock_host.get());
  EXPECT_CALL(*mock_browser, GetHost).WillRepeatedly(testing::Invoke([&]() {
    return arkweb_host;
  }));
  inputmethod_handler_->HandleExtendAction(action);
  inputmethod_handler_->HandleExtendActionOnUI(action);
  EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, TestSetPreviewText_001) {
  auto handler = new MockNWebInputMethodHandler();
  testing::Mock::AllowLeak(handler);
  std::u16string text = u"test";
  int32_t start = 0;
  int32_t end = 4;
  handler->composition_type_ = COMPOSITION_CANCEL;
  EXPECT_CALL(*handler, UpdateCompositionInfo(
	testing::_, testing::_, testing::_)).WillOnce(testing::Return(inputmethod_handler_->OK));
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  handler->browser_ = mock_browser.release();
  int32_t result = handler->SetPreviewText(text, start, end);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
}

TEST_F(NWebInputMethodHandlerTest, TestSetPreviewText_002) {
  auto handler = new MockNWebInputMethodHandler();
  testing::Mock::AllowLeak(handler);
  std::u16string text = u"test";
  int32_t start = 0;
  int32_t end = 4;
  handler->composition_type_ = COMPOSITION_CANCEL;
  EXPECT_CALL(*handler, UpdateCompositionInfo(
	testing::_, testing::_, testing::_)).WillOnce(testing::Return(inputmethod_handler_->ERROR));
  CefRefPtr<MockCefBrowserHost> mockHost = new MockCefBrowserHost();
  testing::Mock::AllowLeak(mockHost.get());
  std::unique_ptr<MockCefBrowser> mock_browser = std::make_unique<MockCefBrowser>(mockHost);
  testing::Mock::AllowLeak(mock_browser.get());
  EXPECT_CALL(*mock_browser, GetHost()).WillOnce(testing::Return(mockHost));
  EXPECT_CALL(*mockHost, PostTaskToUIThread(testing::_)).Times(1);
  handler->browser_ = mock_browser.release();
  int32_t result = handler->SetPreviewText(text, start, end);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
}

}  // namespace OHOS::NWeb
