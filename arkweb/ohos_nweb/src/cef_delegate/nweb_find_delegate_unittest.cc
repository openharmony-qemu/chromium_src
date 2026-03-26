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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "arkweb/build/features/features.h"
#include "arkweb/ohos_nweb/src/capi/nweb_prefetch_options.h"
#include "cef/include/cef_base.h"
#include "cef/include/cef_browser.h"
#include "cef/ohos_cef_ext/include/arkweb_browser_ext.h"
#include "include/cef_client.h"
#include "include/cef_devtools_message_handler_delegate.h"
#include "nweb.h"
#include "nweb_delegate.h"
#include "nweb_delegate_adapter.h"
#include "nweb_delegate_interface.h"
#include "nweb_errors.h"
#include "nweb_find_callback.h"
#include "nweb_input_delegate.h"
#include "ohos_cef_ext/libcef/common/cef_open_devtools_ext_opt.h"
#include "ui/events/keycodes/keyboard_code_conversion_x.h"
#include "ui/events/keycodes/keysym_to_unicode.h"

#define private public
#include "nweb_find_delegate.h"

using namespace testing;

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

  bool HasView() override { return false; }

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

  MOCK_METHOD(void, StopFinding, (bool), (override));

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

  int PrerenderPage(const CefString& url, const CefString& additional_headers) {
    return OHOS::NWeb::NWEB_OK;
  };

  void CancelAllPrerendering(){};

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
                                 const std::vector<std::pair<CefString, CefString>>& script_regex_rules,
                                 bool is_transfer_finished) override {}

  void RemoveJavaScriptOnDocumentStart() override {}

  void JavaScriptOnDocumentEnd(const CefString& script,
                               const std::vector<CefString>& script_rules,
                               const std::vector<std::pair<CefString, CefString>>& script_regex_rules,
                               bool is_transfer_finished) override {}

  void RemoveJavaScriptOnDocumentEnd() override {}

  void OnDataDetectorSelectText() override {}
  std::string GetDataDetectorSelectText() override { return std::string(); }
  void SetBypassVsyncCondition(int32_t condition) override {}

  void JavaScriptOnHeadReady(const CefString& script,
                             const std::vector<CefString>& script_rules,
                             const std::vector<std::pair<CefString, CefString>>& script_regex_rules,
                             bool is_transfer_finished) override {}

  void RemoveJavaScriptOnHeadReady() override {}

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

  void GetScrollOffset(float* offset_x, float* offset_y) override {}

  void GetOverScrollOffset(float* offset_x, float* offset_y) override {}

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

  void ShowDevToolsWith(CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
                        CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate,
                        const CefPoint& inspect_element_at) override {}

  void ShowDevToolsWithByPb(CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
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

  cef_runtime_style_t GetRuntimeStyle() override {
    return CEF_RUNTIME_STYLE_DEFAULT;
  }

  void EnableVideoAssistant(bool enable) override { return false; }

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
  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override {
    return nullptr;
  }
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
  void ShowFreeCopyMenu() override {}
  bool ShouldShowFreeCopyMenu() override { return false; }
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override {}
#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  int InsertBackForwardEntry(int index, const CefString& url) override {
    return 0;
  }
  int UpdateNavigationEntryUrl(int index, const CefString& url) override {
    return 0;
  }
  void ClearForwardList() override {}
#endif
  void ExtensionSetTabId(int tab_id) override {}
  int ExtensionGetTabId() override { return 0; }
  uint32_t GetAcceleratedWidget(bool isPopup) { return 0; }
  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                int main_frame_tree_node_id) override {}
  MOCK_METHOD(void,
              FindEx,
              (const CefString&, bool, bool, bool, bool),
              (override));
  void SetFocusOnWeb() override {}
  void UpdateSecurityLayer(bool isNeedSecurityLayer) override {}
  void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) override {}
  CefString GetCustomUserAgent() override { return CefString(); }
  void GetLastHitData(int& type, CefString& extra_data) override {}
  std::string GetSelectedTextFromContextParam() override { return ""; }
  bool JudgeTextInputState() override { return true; }
  void SetNeedsReload(bool needs_reload) override {}
  void SetOptimizeParserBudgetEnabled(bool enable) override {}
  void OnDestroyImageAnalyzerOverlay() override {}
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

  void SetPipNativeWindow(int delegate_id,
                          int child_id,
                          int frame_routing_id,
                          cef_native_window_t window) override {}
  void SendPipEvent(int delegate_id,
                    int child_id,
                    int frame_routing_id,
                    int event) override {}

  CefRefPtr<CefFrame> GetFrameByIdentifier(
      const CefString& identifier) override {
    return nullptr;
  }

#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override {}
  void OnBrowserBackground() override {}
#endif

  void RunJavaScriptInFrames(
      const std::string& jsString,
      FrameInfos rootFrame,
      bool recursive,
      IsolatedWorld world,
      CefRefPtr<CefJavaScriptResultCallback> callback) override {}
#if BUILDFLAG(ARKWEB_NWEB_EX)
  void GetAllFrameInfos(CefRefPtr<CefFrameInfosCallback> callback) override {}
  void GetLastJavaScriptProxyCallingFrameInfo(
      CefRefPtr<CefLastJavaScriptProxyCallingFrameInfoCallback> callback) override {}
#endif
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
  
#if BUILDFLAG(ARKWEB_READER_MODE)
  void Distill(uint64_t request_id, const DistillOptions& distill_options,
    CefRefPtr<CefDistillCallback> callback) override {}
  void AbortDistill() override {}
#endif
  void EnableHttpsUpgrades(bool enable) override {}
#endif

#if BUILDFLAG(ARKWEB_EXT_RECEIVE_RESPONSE)
  int32_t GetLastCommittedEntryPageTransition() override { return 0; }
#endif

#if BUILDFLAG(ARKWEB_UNITTESTS)
  void SetMediaResumeFromBFCachePage(bool resume) override {}
  void SetHasComposition(bool has_composition) override {}
  bool GetHasComposition() override { return false; }
  void SetEnableCustomVideoPlayer(bool flag) override {}
  void EnableAppLinking(bool enable) override {}
  bool IsAppLinkingEnabled() const override { return false; }
#endif // ARKWEB_UNITTESTS
};

class NWebFindDelegateTest : public ::testing::Test {
 public:
  void SetUp() override {}
  void TearDown() override {}
};

class MockNWebFindCallback : public NWebFindCallback {
 public:
  MockNWebFindCallback() = default;
  virtual ~MockNWebFindCallback() = default;

  MOCK_METHOD(void,
              OnFindResultReceived,
              (const int, const int, const bool),
              (override));
};

class MockCefBrowser : public CefBrowser {
 public:
  MockCefBrowser(CefRefPtr<CefBrowserHost> host) : host_(host) {}
  MOCK_METHOD(bool, IsValid, (), (override));
  MOCK_METHOD(CefRefPtr<ArkWebBrowserHostExt>, GetHost, (), (override));
  MOCK_METHOD(bool, CanGoBack, (), (override));
  MOCK_METHOD(void, GoBack, (), (override));
  MOCK_METHOD(bool, CanGoForward, (), (override));
  MOCK_METHOD(void, GoForward, (), (override));
  MOCK_METHOD(bool, IsLoading, (), (override));
  MOCK_METHOD(void, Reload, (), (override));
  MOCK_METHOD(void, ReloadIgnoreCache, (), (override));
  MOCK_METHOD(void, StopLoad, (), (override));
  MOCK_METHOD(int, GetIdentifier, (), (override));
  MOCK_METHOD(bool, IsSame, (CefRefPtr<CefBrowser>), (override));
  MOCK_METHOD(bool, IsPopup, (), (override));
  MOCK_METHOD(bool, HasDocument, (), (override));
  MOCK_METHOD(CefRefPtr<CefFrame>, GetMainFrame, (), (override));
  MOCK_METHOD(CefRefPtr<CefFrame>, GetFocusedFrame, (), (override));
  MOCK_METHOD(CefRefPtr<CefFrame>,
              GetFrameByIdentifier,
              (const CefString&),
              (override));
  MOCK_METHOD(CefRefPtr<CefFrame>,
              GetFrameByName,
              (const CefString&),
              (override));
  MOCK_METHOD(size_t, GetFrameCount, (), (override));
  MOCK_METHOD(void, GetFrameIdentifiers, (std::vector<CefString>&), (override));
  MOCK_METHOD(void, GetFrameNames, (std::vector<CefString>&), (override));
  MOCK_METHOD(bool, NeedToFireBeforeUnloadOrUnloadEvents, (), (override));
  MOCK_METHOD(void, DispatchBeforeUnload, (), (override));
  MOCK_METHOD(int,
              PrerenderPage,
              (const CefString&, const CefString&),
              (override));
  MOCK_METHOD(void, CancelAllPrerendering, (), (override));
  MOCK_METHOD(void, AddRef, (), (const, override));
  MOCK_METHOD(bool, Release, (), (const, override));
  MOCK_METHOD(bool, HasOneRef, (), (const, override));
  MOCK_METHOD(bool, HasAtLeastOneRef, (), (const, override));

 private:
  CefRefPtr<CefBrowserHost> host_;
};

TEST_F(NWebFindDelegateTest, SetListenerTest) {
  std::unique_ptr find = std::make_unique<NWebFindDelegate>();
  ASSERT_NE(find, nullptr);
  std::shared_ptr callback = std::make_shared<MockNWebFindCallback>();
  ASSERT_NE(callback, nullptr);
  find->SetListener(callback);
  EXPECT_NE(find->listener_, nullptr);
}

TEST_F(NWebFindDelegateTest, FindAllAsyncAndMaybeHandleEmptySearchTest) {
  std::unique_ptr find = std::make_unique<NWebFindDelegate>();
  ASSERT_NE(find, nullptr);
  std::unique_ptr mock_host = std::make_unique<MockCefBrowserHost>();
  auto mock_host_raw = mock_host.get();
  std::unique_ptr browser = std::make_unique<MockCefBrowser>(mock_host_raw);
  ASSERT_NE(browser, nullptr);
  ASSERT_NE(mock_host, nullptr);
  const std::u16string abc = u"abc";
  EXPECT_TRUE(!abc.empty());
  const std::u16string null = u"";
  EXPECT_TRUE(null.empty());
  find->FindAllAsync(nullptr, abc);
  EXPECT_FALSE(find->async_find_started_);
  EXPECT_CALL(*browser, GetHost()).WillRepeatedly(Return(mock_host_raw));
  find->FindAllAsync(browser.get(), abc);
  EXPECT_TRUE(find->async_find_started_);
  auto result = find->MaybeHandleEmptySearch(browser.get(), null);
  EXPECT_TRUE(result);
  Mock::VerifyAndClearExpectations(browser.get());
}

TEST_F(NWebFindDelegateTest, HandleFindReplyAndNotifyResultsTest) {
  std::unique_ptr find = std::make_unique<NWebFindDelegate>();
  ASSERT_NE(find, nullptr);
  std::shared_ptr callback = std::make_shared<MockNWebFindCallback>();
  ASSERT_NE(callback, nullptr);
  find->HandleFindReply(1, 1, 1, true);
  EXPECT_TRUE(!find->async_find_started_);
  EXPECT_EQ(find->last_match_count_, -1);
  find->async_find_started_ = true;
  find->HandleFindReply(1, 1, 1, true);
  EXPECT_EQ(find->last_match_count_, 1);
  EXPECT_EQ(find->last_active_ordinal_, 1);
  find->last_match_count_ = 2;
  find->last_active_ordinal_ = -1;
  find->NotifyResults(-1, -1, false);
  EXPECT_EQ(find->last_match_count_, 2);
  find->last_match_count_ = -1;
  find->last_active_ordinal_ = -1;
  find->async_find_started_ = true;
  find->NotifyResults(1, 1, false);
  EXPECT_EQ(find->last_match_count_, 1);
  find->SetListener(callback);
  find->NotifyResults(1, 1, true);
  EXPECT_NE(find->listener_, nullptr);
}

TEST_F(NWebFindDelegateTest, FindNextTest) {
  std::unique_ptr find = std::make_unique<NWebFindDelegate>();
  ASSERT_NE(find, nullptr);
  std::unique_ptr mock_host = std::make_unique<MockCefBrowserHost>();
  auto mock_host_raw = mock_host.get();
  std::unique_ptr browser = std::make_unique<MockCefBrowser>(mock_host_raw);
  ASSERT_NE(browser, nullptr);
  ASSERT_NE(mock_host, nullptr);
  find->FindNext(nullptr, true);
  EXPECT_FALSE(find->async_find_started_);
  const std::u16string abc = u"abc";
  find->FindNext(browser.get(), true);
  find->async_find_started_ = true;
  find->last_search_string_ = u"";
  EXPECT_CALL(*browser, GetHost()).WillRepeatedly(Return(mock_host_raw));
  find->FindNext(browser.get(), true);
  EXPECT_TRUE(find->last_search_string_.empty());
  find->last_search_string_ = abc;
  EXPECT_CALL(*browser, GetHost()).WillRepeatedly(Return(mock_host_raw));
  find->FindNext(browser.get(), true);
  Mock::VerifyAndClearExpectations(mock_host_raw);
  Mock::VerifyAndClearExpectations(browser.get());
}

TEST_F(NWebFindDelegateTest, ClearMatchesTest) {
  std::unique_ptr find = std::make_unique<NWebFindDelegate>();
  ASSERT_NE(find, nullptr);
  std::unique_ptr mock_host = std::make_unique<MockCefBrowserHost>();
  auto mock_host_raw = mock_host.get();
  std::unique_ptr browser = std::make_unique<MockCefBrowser>(mock_host_raw);
  ASSERT_NE(browser, nullptr);
  ASSERT_NE(mock_host, nullptr);
  find->async_find_started_ = true;
  find->ClearMatches(nullptr);
  EXPECT_TRUE(find->async_find_started_);
  EXPECT_CALL(*browser, GetHost()).WillRepeatedly(Return(mock_host_raw));
  find->ClearMatches(browser.get());
  EXPECT_FALSE(find->async_find_started_);
  Mock::VerifyAndClearExpectations(browser.get());
}

TEST_F(NWebFindDelegateTest, StartNewSessionTest) {
  std::unique_ptr find = std::make_unique<NWebFindDelegate>();
  ASSERT_NE(find, nullptr);
  EXPECT_TRUE(find->last_search_string_.empty());
  const std::u16string abc = u"abc";
  find->StartNewSession(abc);
  EXPECT_EQ(find->last_search_string_, abc);
}

TEST_F(NWebFindDelegateTest, NotifyResultsTest) {
  std::unique_ptr find_delegate = std::make_unique<NWebFindDelegate>();
  int test_match_count = 2;
  int test_active_ordinal = 3;
  bool test_finished = true;
  find_delegate->NotifyResults(test_active_ordinal, test_match_count,
                               test_finished);
}
}  // namespace OHOS::NWeb
