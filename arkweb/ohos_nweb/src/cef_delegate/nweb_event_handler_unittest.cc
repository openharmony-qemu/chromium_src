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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_devtools_message_handler_delegate.h"
#include "nweb_input_delegate.h"
#include "ohos_cef_ext/libcef/common/cef_open_devtools_ext_opt.h"
#include "ui/events/keycodes/keyboard_code_conversion_x.h"
#include "ui/events/keycodes/keysym_to_unicode.h"

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "ohos_nweb/include/nweb_errors.h"
#endif

#define private public
#include "nweb_event_handler.h"

using namespace testing;
namespace OHOS::NWeb {
class MockMMIAdapter : public MMIAdapter {
 public:
  MOCK_METHOD(char*, KeyCodeToString, (int32_t), (override));
  MOCK_METHOD(int32_t,
              RegisterMMIInputListener,
              (std::shared_ptr<MMIInputListenerAdapter>),
              (override));

  MOCK_METHOD(void, UnregisterMMIInputListener, (int32_t), (override));
  MOCK_METHOD(int32_t,
              RegisterDevListener,
              (std::string, std::shared_ptr<MMIListenerAdapter>),
              (override));

  MOCK_METHOD(int32_t, UnregisterDevListener, (std::string), (override));
  MOCK_METHOD(int32_t, GetKeyboardType, (int32_t, int32_t&), (override));
  MOCK_METHOD(int32_t, GetDeviceIds, (std::vector<int32_t>&), (override));
  MOCK_METHOD(int32_t,
              GetDeviceInfo,
              (int32_t, std::shared_ptr<MMIDeviceInfoAdapter>),
              (override));
  MOCK_METHOD(int32_t,
              GetMaxTouchPoints,
              (),
              (override));
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
      const CefString& identifier) override {  return nullptr; }
#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override {}
  void OnBrowserBackground() override {}
#endif
  void RunJavaScriptInFrames(const std::string& jsString, FrameInfos rootFrame,
                             bool recursive, IsolatedWorld world,
                             CefRefPtr<CefJavaScriptResultCallback> callback) override {}
#if BUILDFLAG(ARKWEB_NWEB_EX)
 void GetAllFrameInfos(CefRefPtr<CefFrameInfosCallback> callback) override {}
 void GetLastJavaScriptProxyCallingFrameInfo(
    CefRefPtr<CefLastJavaScriptProxyCallingFrameInfoCallback> callback) override {}
#endif
#if BUILDFLAG(IS_ARKWEB)
  void EnableAppLinking(bool enable) override {}
  bool IsAppLinkingEnabled() const override { return false; }
#endif
#if BUILDFLAG(ARKWEB_SAVE_PAGE)
  bool SavePage(int32_t type,
                CefString& filePath,
                CefRefPtr<CefSavePageResultCallback> callback) override {
    return false;
  }
#endif // ARKWEB_SAVE_PAGE
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void GetFocusedFrameInfo(int32_t& frame_id, CefString& frame_url) override {}
#endif  // ARKWEB_ARKWEB_EXTENSIONS
#if BUILDFLAG(ARKWEB_READER_MODE)
  void EnableReaderMode(bool enabled) override {}
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

#if BUILDFLAG(ARKWEB_EXT_RECEIVE_RESPONSE)
  int32_t GetLastCommittedEntryPageTransition() override { return 0; }
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

class NWebEventHandlerTest : public ::testing::Test {
 protected:
  void SetUp();
  void TearDown();

  std::shared_ptr<NWebEventHandler> handler = nullptr;
  MockCefBrowser* mock_browser = nullptr;
  MockCefBrowserHost* mock_host = nullptr;
};

void NWebEventHandlerTest::SetUp() {
  handler = std::make_shared<NWebEventHandler>();
  mock_host = new MockCefBrowserHost();
  mock_browser = new MockCefBrowser(mock_host);

  handler->SetBrowser(mock_browser);
}

void NWebEventHandlerTest::TearDown() {
  handler->OnDestroy();
  handler.reset();
  delete mock_browser;
  delete mock_host;
}

TEST_F(NWebEventHandlerTest, Create_TEST001) {
  auto event_handler = handler->Create();
  EXPECT_NE(event_handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnDestroy_TEST001) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  handler->mmi_adapter_ = std::make_unique<MockMMIAdapter>();
  handler->mmi_id_ = 1;
#endif
  handler->OnDestroy();
  EXPECT_EQ(handler->browser_, nullptr);
}

TEST_F(NWebEventHandlerTest, OnDestroy_TEST002) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  handler->mmi_adapter_ = nullptr;
  handler->mmi_id_ = 1;
#endif
  handler->OnDestroy();
  EXPECT_EQ(handler->browser_, nullptr);
}

TEST_F(NWebEventHandlerTest, OnDestroy_TEST003) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  handler->mmi_adapter_ = std::make_unique<MockMMIAdapter>();
  handler->mmi_id_ = -1;
#endif
  handler->OnDestroy();
  EXPECT_EQ(handler->browser_, nullptr);
}

TEST_F(NWebEventHandlerTest, OnDestroy_TEST004) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  handler->mmi_id_ = -1;
  handler->mmi_adapter_ = nullptr;
#endif
  handler->OnDestroy();
  EXPECT_EQ(handler->browser_, nullptr);
}

TEST_F(NWebEventHandlerTest, SetBrowser_TEST001) {
  EXPECT_EQ(handler->browser_, mock_browser);
  handler->SetBrowser(nullptr);
  EXPECT_EQ(handler->browser_, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchPress_TEST001) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(1);

  handler->OnTouchPress(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchPress_TEST002) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchPress(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchPress_TEST003) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(0);

  handler->OnTouchPress(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchPress_TEST004) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchPress(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchMove_TEST001) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(1);

  handler->OnTouchMove(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchMove_TEST002) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchMove(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchMove_TEST003) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(0);

  handler->OnTouchMove(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchMove_TEST004) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchMove(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchMove_TEST005) {
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos = {
      std::make_shared<MockNWebTouchPointInfo>(1, 100.0, 200.0),
  };
  bool from_overlay = false;
  float virtual_pixel_ratio = 2.0f;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEventList(::testing::_));

  handler->OnTouchMove(touch_point_infos, from_overlay, virtual_pixel_ratio);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchMove_TEST006) {
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos = {
      std::make_shared<MockNWebTouchPointInfo>(1, 100.0, 200.0)};
  bool from_overlay = false;
  float virtual_pixel_ratio = 2.0f;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEventList(::testing::_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchMove(touch_point_infos, from_overlay, virtual_pixel_ratio);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchMove_TEST007) {
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos = {
      std::make_shared<MockNWebTouchPointInfo>(1, 100.0, 200.0)};
  bool from_overlay = false;
  float virtual_pixel_ratio = 2.0f;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEventList(::testing::_)).Times(0);

  handler->OnTouchMove(touch_point_infos, from_overlay, virtual_pixel_ratio);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchMove_TEST008) {
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos = {};
  bool from_overlay = false;
  float virtual_pixel_ratio = 2.0f;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEventList(::testing::_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchMove(touch_point_infos, from_overlay, virtual_pixel_ratio);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchRelease_TEST001) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(1);

  handler->OnTouchPress(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchRelease_TEST002) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchPress(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchRelease_TEST003) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(0);

  handler->OnTouchPress(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchRelease_TEST004) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchPress(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchCancel_TEST001) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_));

  handler->OnTouchCancel();
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchCancel_TEST002) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchCancel();
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchCancel_TEST003) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);

  handler->OnTouchCancel();
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchCancel_TEST004) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchCancel();
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchCancelById_TEST001) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_));

  handler->OnTouchCancelById(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchCancelById_TEST002) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchCancelById(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchCancelById_TEST003) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);

  handler->OnTouchCancelById(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, OnTouchCancelById_TEST004) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);
  handler->SetBrowser(nullptr);

  handler->OnTouchCancelById(1, 100.0, 200.0, false);
  ASSERT_NE(handler, nullptr);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebEventHandlerTest, SendKeyEventFromMMI_TEST001) {
  handler->SetIsFocus(true);
  handler->SendKeyEventFromMMI(65, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendKeyEventFromMMI_TEST002) {
  handler->SetIsFocus(false);
  handler->SendKeyEventFromMMI(65, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendKeyEventFromMMI_TEST003) {
  handler->SetIsFocus(true);
  handler->SendKeyEventFromMMI(-1, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendKeyEventFromMMI_TEST004) {
  handler->SetIsFocus(false);
  handler->SendKeyEventFromMMI(-1, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendKeyEventFromAce_TEST001) {
  handler->mmi_id_ = 1;
  handler->SendKeyEventFromAce(65, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendKeyEventFromAce_TEST002) {
  handler->mmi_id_ = -1;
  handler->SendKeyEventFromAce(65, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendKeyEventFromAce_TEST003) {
  handler->mmi_id_ = 1;
  handler->SendKeyEventFromAce(-1, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendKeyEventFromAce_TEST004) {
  handler->mmi_id_ = -1;
  handler->SendKeyEventFromAce(-1, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendKeyEventFromAce_TEST001) {
  std::vector<int32_t> pressedCodes = {};

  handler->mmi_id_ = 1;
  handler->WebSendKeyEventFromAce(65, 1, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendKeyEventFromAce_TEST002) {
  std::vector<int32_t> pressedCodes = {};

  handler->mmi_id_ = -1;
  handler->WebSendKeyEventFromAce(65, 1, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendKeyEventFromAce_TEST003) {
  std::vector<int32_t> pressedCodes = {};

  handler->mmi_id_ = 1;
  handler->WebSendKeyEventFromAce(-1, 1, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendKeyEventFromAce_TEST004) {
  std::vector<int32_t> pressedCodes = {};

  handler->mmi_id_ = -1;
  handler->WebSendKeyEventFromAce(-1, 1, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendKeyEvent_TEST001) {
  std::vector<int32_t> pressedCodes = {};

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  bool result = handler->WebSendKeyEvent(65, 0, pressedCodes);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, WebSendKeyEvent_TEST002) {
  std::vector<int32_t> pressedCodes = {};

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendKeyEvent(::testing::_)).Times(0);

  bool result = handler->WebSendKeyEvent(-1, 0, pressedCodes);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, WebSendKeyEvent_TEST003) {
  std::vector<int32_t> pressedCodes = {};

  handler->SetBrowser(nullptr);

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendKeyEvent(::testing::_)).Times(0);

  bool result = handler->WebSendKeyEvent(65, 0, pressedCodes);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, WebSendKeyEvent_TEST004) {
  std::vector<int32_t> pressedCodes = {};

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);

  bool result = handler->WebSendKeyEvent(65, 0, pressedCodes);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, WebSendKeyEvent_TEST005) {
  std::vector<int32_t> pressedCodes = {};

  handler->SetBrowser(nullptr);

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);

  bool result = handler->WebSendKeyEvent(65, 0, pressedCodes);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, WebSendMouseWheelEvent_TEST001) {
  std::vector<int32_t> pressedCodes = {EVENTFLAG_SHIFT_DOWN};
  handler->mmi_id_ = 1;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendMouseWheelEvent(::testing::_, ::testing::_, ::testing::_));

  handler->WebSendMouseWheelEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendMouseWheelEvent_TEST002) {
  std::vector<int32_t> pressedCodes = {EVENTFLAG_SHIFT_DOWN};

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  handler->SetBrowser(nullptr);

  handler->WebSendMouseWheelEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendMouseWheelEvent_TEST003) {
  std::vector<int32_t> pressedCodes = {EVENTFLAG_SHIFT_DOWN};

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));

  handler->WebSendMouseWheelEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendMouseWheelEvent_TEST004) {
  std::vector<int32_t> pressedCodes = {EVENTFLAG_SHIFT_DOWN};

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  handler->SetBrowser(nullptr);

  handler->WebSendMouseWheelEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendMouseWheelEvent_TEST005) {
  std::vector<int32_t> pressedCodes = {};
  handler->mmi_id_ = 1;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendMouseWheelEvent(::testing::_, ::testing::_, ::testing::_));

  handler->WebSendMouseWheelEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendMouseWheelEvent_TEST006) {
  std::vector<int32_t> pressedCodes = {EVENTFLAG_SHIFT_DOWN};
  handler->mmi_id_ = -1;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendMouseWheelEvent(::testing::_, ::testing::_, ::testing::_));

  handler->WebSendMouseWheelEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendMouseWheelEvent_TEST007) {
  std::vector<int32_t> pressedCodes = {};
  handler->mmi_id_ = -1;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendMouseWheelEvent(::testing::_, ::testing::_, ::testing::_));

  handler->WebSendMouseWheelEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendTouchpadFlingEvent_TEST001) {
  std::vector<int32_t> pressedCodes = {1, 2, 3};

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendTouchpadFlingEvent(::testing::_, ::testing::_, ::testing::_));

  handler->WebSendTouchpadFlingEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendTouchpadFlingEvent_TEST002) {
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  handler->SetBrowser(nullptr);

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host,
              SendTouchpadFlingEvent(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  handler->WebSendTouchpadFlingEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendTouchpadFlingEvent_TEST003) {
  std::vector<int32_t> pressedCodes = {1, 2, 3};

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host,
              SendTouchpadFlingEvent(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  handler->WebSendTouchpadFlingEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendTouchpadFlingEvent_TEST004) {
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  handler->SetBrowser(nullptr);

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendTouchpadFlingEvent(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  handler->WebSendTouchpadFlingEvent(100.0, 200.0, 10.0, 20.0, pressedCodes);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendCancelFlingEvent_TEST001) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendCancelFlingEvent(::testing::_));

  handler->WebSendCancelFlingEvent();
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendCancelFlingEvent_TEST002) {
  handler->SetBrowser(nullptr);

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host,
              SendCancelFlingEvent(::testing::_))
      .Times(0);

  handler->WebSendCancelFlingEvent();
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendCancelFlingEvent_TEST003) {

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host,
              SendCancelFlingEvent(::testing::_))
      .Times(0);

  handler->WebSendCancelFlingEvent();
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, WebSendCancelFlingEvent_TEST004) {
  handler->SetBrowser(nullptr);

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendCancelFlingEvent(::testing::_))
      .Times(0);

  handler->WebSendCancelFlingEvent();
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendTouchpadFlingEvent_TEST001) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendTouchpadFlingEvent(::testing::_, ::testing::_, ::testing::_));

  handler->SendTouchpadFlingEvent(100.0, 200.0, 10.0, 20.0);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendTouchpadFlingEvent_TEST002) {
  handler->SetBrowser(nullptr);

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host,
              SendTouchpadFlingEvent(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  handler->SendTouchpadFlingEvent(100.0, 200.0, 10.0, 20.0);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendTouchpadFlingEvent_TEST003) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host,
              SendTouchpadFlingEvent(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  handler->SendTouchpadFlingEvent(100.0, 200.0, 10.0, 20.0);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendTouchpadFlingEvent_TEST004) {
  handler->SetBrowser(nullptr);

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendTouchpadFlingEvent(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  handler->SendTouchpadFlingEvent(100.0, 200.0, 10.0, 20.0);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseWheelEvent_TEST001) {
  handler->mmi_id_ = 1;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendMouseWheelEvent(::testing::_, ::testing::_, ::testing::_));

  handler->SendMouseWheelEvent(100.0, 200.0, 10.0, 20.0);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseWheelEvent_TEST002) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  handler->SetBrowser(nullptr);

  handler->SendMouseWheelEvent(100.0, 200.0, 10.0, 20.0);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseWheelEvent_TEST003) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));

  handler->SendMouseWheelEvent(100.0, 200.0, 10.0, 20.0);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseWheelEvent_TEST004) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  handler->SetBrowser(nullptr);

  handler->SendMouseWheelEvent(100.0, 200.0, 10.0, 20.0);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseWheelEvent_TEST005) {
  handler->mmi_id_ = -1;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host,
              SendMouseWheelEvent(::testing::_, ::testing::_, ::testing::_));

  handler->SendMouseWheelEvent(100.0, 200.0, 10.0, 20.0);
  ASSERT_NE(handler, nullptr);
}
#endif  // defined(OHOS_INPUT

TEST_F(NWebEventHandlerTest, SendKeyEvent_TEST001) {
  int32_t keyCode = 65;
  int32_t keyAction = 0;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  bool result = handler->SendKeyEvent(keyCode, keyAction);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, SendKeyEvent_TEST002) {
  int32_t keyCode = -1;
  int32_t keyAction = 0;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  bool result = handler->SendKeyEvent(keyCode, keyAction);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, SendKeyEvent_TEST003) {
  int32_t keyCode = 65;
  int32_t keyAction = 0;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendKeyEvent(::testing::_)).Times(0);
  handler->SetBrowser(nullptr);

  bool result = handler->SendKeyEvent(keyCode, keyAction);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, SendKeyEvent_TEST004) {
  int32_t keyCode = 65;
  int32_t keyAction = 0;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);

  bool result = handler->SendKeyEvent(keyCode, keyAction);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, SendKeyEvent_TEST005) {
  int32_t keyCode = 65;
  int32_t keyAction = 0;

  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendTouchEvent(::testing::_)).Times(0);
  handler->SetBrowser(nullptr);

  bool result = handler->SendKeyEvent(keyCode, keyAction);
  EXPECT_FALSE(result);
}

TEST_F(NWebEventHandlerTest, SendMouseEvent_TEST001) {
  handler->SetBrowser(nullptr);
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendMouseClickEvent(::testing::_, MBT_LEFT, false, 1))
      .Times(0);

  handler->SendMouseEvent(10, 20, 2, 1, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseEvent_TEST002) {
  handler->SetBrowser(nullptr);
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendMouseClickEvent(::testing::_, MBT_LEFT, false, 1))
      .Times(0);

  handler->SendMouseEvent(10, 20, 2, 1, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseEvent_TEST003) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(nullptr));
  EXPECT_CALL(*mock_host, SendMouseClickEvent(::testing::_, MBT_LEFT, false, 1))
      .Times(0);

  handler->SendMouseEvent(10, 20, 2, 1, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseEvent_TEST004) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  handler->SendMouseEvent(10, 20, 2, 1, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseEvent_TEST005) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  handler->SendMouseEvent(10, 20, 2, 1, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, SendMouseEvent_TEST006) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));
  EXPECT_CALL(*mock_host, SendMouseMoveEvent(::testing::_, false)).Times(1);
  handler->SendMouseEvent(10, 20, 0, 3, 1);
  ASSERT_NE(handler, nullptr);
}

TEST_F(NWebEventHandlerTest, NotifyForNextTouchEvent_TEST001) {
  ON_CALL(*mock_browser, GetHost()).WillByDefault(::testing::Return(mock_host));

  handler->NotifyForNextTouchEvent();
  ASSERT_NE(handler, nullptr);
}
}  // namespace OHOS::NWeb
