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

#include "nweb_preference_delegate.h"

#include <gmock/gmock.h>

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "build/build_config.h"
#include "cef/include/cef_client.h"
#include "cef/include/cef_display_handler.h"
#include "cef/include/cef_command_line.h"
#include "cef/libcef/browser/browser_contents_delegate.h"
#include "cef/libcef/browser/browser_info.h"
#include "cef/include/cef_devtools_message_handler_delegate.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#include "gtest/gtest.h"
#include "ohos_cef_ext/libcef/common/cef_open_devtools_ext_opt.h"
#include "nweb.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#define DEFAULT_FONT_SIZE 16
#define DEFAULT_FIXED_FONT_SIZE 33
#define MINIMUM_FONT_SIZE 8
#define MINIMUM_LOGICAL_FONT_SIZE 8
#define SETTINGS_STRING_SET(src, target) \
  cef_string_set(src.str, src.length, &target, true)

using namespace testing;
using namespace OHOS::NWeb;

class MockPreferenceCefBrowserHost : public ArkWebBrowserHostExt {
 public:

  bool test_cef_browser_host = false;
  void PutUserAgent(const CefString& ua, bool from_app) override {
    test_cef_browser_host = from_app;
  }

  bool CreateBrowser(const CefWindowInfo& windowInfo,
                     CefRefPtr<CefClient> client,
                     const CefString& url,
                     const CefBrowserSettings& settings,
                     CefRefPtr<CefDictionaryValue> extra_info,
                     CefRefPtr<CefRequestContext> request_context) {
    return false;
  }
  void OnSafeBrowsingDetectionResult(int code,
                                   int policy,
                                   const std::string& mappingType,
                                   const std::string& url) {
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

  void SetFocus(bool) override {}

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
                              int command_id) {}

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

#if BUILDFLAG(ARKWEB_UNITTESTS)
  void OnDataDetectorSelectText() override {}
  std::string GetDataDetectorSelectText() override { return std::string(); }
  void SetBypassVsyncCondition(int32_t condition) override {}
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  void JavaScriptOnHeadReady(const CefString& script,
                             const std::vector<CefString>& script_rules,
                             const std::vector<std::pair<CefString, CefString>>& script_regex_rules,
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

  void OnEyeDropperResult(bool success, uint32_t color) override {}

  void SetPrintBackground(bool enable) override {}

  bool GetPrintBackground() override { return false; }

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetScrollable(bool enable, int scrollType) override {}

  void SetImeShow(bool visible) override {}
#endif

#if BUILDFLAG(IS_ARKWEB)
  void EnableAppLinking(bool enable) override {}

  bool IsAppLinkingEnabled() const override {}
#endif

  void RunJavaScriptInFrames(const std::string& jsString, FrameInfos rootFrame,
                                     bool recursive, IsolatedWorld world,
                                     CefRefPtr<CefJavaScriptResultCallback> callback) override {}

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void GetFocusedFrameInfo(int32_t& frame_id, CefString& frame_url) override {}
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
  void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) override {}
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
  void SetEnableCustomVideoPlayer(bool flag) { }
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
#endif
#endif  // BUILDFLAG(IS_OHOS)
  int PrerenderPage(const CefString& url,
                    const CefString& additional_headers) override {
    return 0;
  }
  void CancelAllPrerendering() override {}
  void CreateWebPrintDocumentAdapterV2(const CefString& jobName,
                                       void** adapter) override {}
#if BUILDFLAG(ARKWEB_BGTASK)                                       
  void OnBrowserForeground() override {}
  void OnBrowserBackground() override {}
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  void Distill(const std::string& guid, const DistillOptions& distill_options,
    CefRefPtr<CefDistillCallback> callback) override {}
  void AbortDistill() override {}
#endif // ARKWEB_READER_MODE

  bool GetHasComposition() {}
  void SetHasComposition(bool has_composition)  {}
  void GetImageForContextNode(int command_id) override {}
  void SetMediaResumeFromBFCachePage(bool resume)  {}
};

class MockPreferenceCefBrowser : public CefBrowser, public CefBrowserHost {
 public:

  CefRefPtr<ArkWebBrowserHostExt> host_ = nullptr;

  static CefRefPtr<MockPreferenceCefBrowser> Create();

  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
  bool IsIframe() override { return false; }

  bool IsValid() override {return false;}
  CefRefPtr<ArkWebBrowserExt> AsArkWebBrowser() { return nullptr; }
  CefRefPtr<CefBrowserHostBase> AsCefBrowserHostBase() {return nullptr;}

  void SetMediaResumeFromBFCachePage(bool resume) {}

  void SetHost(CefRefPtr<ArkWebBrowserHostExt> host) {
    this->host_ = host;
  }

  CefRefPtr<ArkWebBrowserHostExt> GetHost() {
    return host_;
  }
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetOverScrollOffsetValue(float* offset_x, float* offset_y) override {}
#endif
  bool CanGoBack() override {return false;}
  void GoBack() override {}
  bool CanGoForward() override {return false;}
  void GoForward() override {}
  bool IsLoading() override {return false;}
  void Reload() override {}
  void ReloadIgnoreCache() override {}
  void StopLoad() override {}
  int GetIdentifier() override {return -1;}
  bool IsSame(CefRefPtr<CefBrowser> that) override {return false;}
  bool IsPopup() override {return false;}
  bool HasDocument() override {return false;}
  CefRefPtr<CefFrame> GetMainFrame() override {return nullptr;}
  CefRefPtr<CefFrame> GetFocusedFrame() override {return nullptr;}
  CefRefPtr<CefFrame> GetFrameByIdentifier(const CefString& identifier) override {return nullptr;}
  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override {return nullptr;}
  size_t GetFrameCount() override {return 0;}
  void GetFrameIdentifiers(std::vector<CefString>& identifiers) override {}
  void GetFrameNames(std::vector<CefString>& names) override {}
  bool NeedToFireBeforeUnloadOrUnloadEvents() override {return false;}
  void DispatchBeforeUnload() override {}
  CefRefPtr<CefBrowser> GetBrowser() override {return nullptr;}
  void CloseBrowser(bool force_close) override {}
  bool TryCloseBrowser() override {return false;}
  bool IsReadyToBeClosed() override {return false;}
  void SetFocus(bool focus) override {}
  CefWindowHandle GetWindowHandle() override {return 0;}
  CefWindowHandle GetOpenerWindowHandle() override {return 0;}
  int GetOpenerIdentifier() override {return -1;}
  bool HasView() override {return false;}
  CefRefPtr<CefClient> GetClient() override {return nullptr;}
  CefRefPtr<CefRequestContext> GetRequestContext() override {return nullptr;}
  bool CanZoom(cef_zoom_command_t command) override {return false;}
  void Zoom(cef_zoom_command_t command) override {}
  double GetDefaultZoomLevel() override {return 0.0;}
  double GetZoomLevel() override {return 0.0;}
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
  #if BUILDFLAG(ARKWEB_DEVTOOLS)
  void ShowDevToolsWith(
  CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
  CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate,
  const CefPoint& inspect_element_at) override {}
  void ShowDevToolsWithByPb(
  CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
  CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate,
  const CefPoint& inspect_element_at,
  const CefOpenDevToolsExtOpt& ext_opt) override {}
  #endif // BUILDFLAG(ARKWEB_DEVTOOLS)
  void CloseDevTools() override {}
  bool HasDevTools() override {return false;}
  bool SendDevToolsMessage(const void* message,
  size_t message_size) override {return false;}
  int ExecuteDevToolsMethod(int message_id,
  const CefString& method,
  CefRefPtr<CefDictionaryValue> params) override {return -1;}
  CefRefPtr<CefRegistration> AddDevToolsMessageObserver(
  CefRefPtr<CefDevToolsMessageObserver> observer) override {return nullptr;}
  void GetNavigationEntries(
  CefRefPtr<CefNavigationEntryVisitor> visitor,
  bool current_only) override {}
  void ReplaceMisspelling(const CefString& word) override {}
  void AddWordToDictionary(const CefString& word) override {}
  bool IsWindowRenderingDisabled() override {return false;}
  void WasResized() override {}
  void WasHidden(bool hidden) override {}
  void NotifyScreenInfoChanged() override {}
  void Invalidate(PaintElementType type) override {}
  void SendExternalBeginFrame() override {}
  void SendKeyEvent(const CefKeyEvent& event) override {}
  void SendMouseClickEvent(const CefMouseEvent& event,
  MouseButtonType type,
  bool mouseUp,
  int clickCount) override {}
  void SendMouseMoveEvent(const CefMouseEvent& event,
  bool mouseLeave) override {}
  void SendMouseWheelEvent(const CefMouseEvent& event,
  int deltaX,
  int deltaY) override {}
  void SendTouchEvent(const CefTouchEvent& event) override {}
  void SendCaptureLostEvent() override {}
  void NotifyMoveOrResizeStarted() override {}
  int GetWindowlessFrameRate() override {return -1;}
  void SetWindowlessFrameRate(int frame_rate) override {}
  void ImeSetComposition(
  const CefString& text,
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
  CefRefPtr<CefNavigationEntry> GetVisibleNavigationEntry() override {return nullptr;}
  void SetAccessibilityState(cef_state_t accessibility_state) override {}
  void SetAutoResizeEnabled(bool enabled,
  const CefSize& min_size,
  const CefSize& max_size) override {}
  void SetAudioMuted(bool mute) override {}
  bool IsAudioMuted() override {return false;}
  bool IsFullscreen() override {return false;}
  void ExitFullscreen(bool will_cause_resize) override {}
  bool CanExecuteChromeCommand(int command_id) override {return false;}
  void ExecuteChromeCommand(
  int command_id,
  cef_window_open_disposition_t disposition) override {}
  bool IsRenderProcessUnresponsive() override {return false;}
  cef_runtime_style_t GetRuntimeStyle() override {return CEF_RUNTIME_STYLE_CHROME;}
  #if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  void SetPopupWindow(cef_native_window_t window) override {}
  #endif
  #if BUILDFLAG(ARKWEB_PDF)
  void CreateToPDF(const CefPdfPrintSettings& settings,
                           CefRefPtr<CefPdfValueCallback> callback) override {}
  #endif
  void EnableVideoAssistant(bool enable) override {}
  void ExecuteVideoAssistantFunction(const CefString& cmdId) override {}
  #if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
  void ReloadFocusedFrame() override {}
  #endif
  void StopScreenCapture(int32_t nweb_id, const CefString& session_id) override {}
  void SetScreenCapturePickerShow() override {}
  void DisableSessionReuse() override {}
  void RegisterScreenCaptureDelegateListener(CefRefPtr<CefScreenCaptureCallback> listener) override {}
  void CustomWebMediaPlayer(bool enable) override {}

  void UpdateBrowserSettings(const CefBrowserSettings& browser_settings);
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
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  int PrerenderPage(const CefString& url, const CefString& additional_headers) override { return 0; };
  void CancelAllPrerendering() override { };
#endif
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  void SetBlankScreenDetectionConfig(
      bool enable,
      const std::vector<double>& detectionTiming,
      const std::vector<int32_t>& detectionMethods,
      int32_t contentfulNodesCountThreshold) override {}
#endif
  CefBrowserSettings settings_;
  std::unique_ptr<CefBrowserContentsDelegate> contents_delegate_;
};

CefRefPtr<MockPreferenceCefBrowser> MockPreferenceCefBrowser::Create() {
  auto browser = new MockPreferenceCefBrowser();
  return browser;
}

void MockPreferenceCefBrowser::UpdateBrowserSettings(
    const CefBrowserSettings& browser_settings) {
  SETTINGS_STRING_SET(browser_settings.standard_font_family,
                      settings_.standard_font_family);
  SETTINGS_STRING_SET(browser_settings.fixed_font_family,
                      settings_.fixed_font_family);
  SETTINGS_STRING_SET(browser_settings.serif_font_family,
                      settings_.serif_font_family);
  SETTINGS_STRING_SET(browser_settings.sans_serif_font_family,
                      settings_.sans_serif_font_family);
  SETTINGS_STRING_SET(browser_settings.cursive_font_family,
                      settings_.cursive_font_family);
  SETTINGS_STRING_SET(browser_settings.fantasy_font_family,
                      settings_.fantasy_font_family);

  settings_.default_font_size = browser_settings.default_font_size;
  settings_.default_fixed_font_size = browser_settings.default_fixed_font_size;
  settings_.minimum_font_size = browser_settings.minimum_font_size;
  settings_.minimum_logical_font_size =
      browser_settings.minimum_logical_font_size;
  SETTINGS_STRING_SET(browser_settings.default_encoding,
                      settings_.default_encoding);
  settings_.javascript = browser_settings.javascript;
  settings_.image_loading = browser_settings.image_loading;
  settings_.local_storage = browser_settings.local_storage;
  settings_.databases = browser_settings.databases;

  settings_.force_dark_mode_enabled = browser_settings.force_dark_mode_enabled;
#if BUILDFLAG(ARKWEB_DARKMODE)
  settings_.dark_prefer_color_scheme_enabled =
      browser_settings.dark_prefer_color_scheme_enabled;
#endif
  settings_.javascript_can_open_windows_automatically =
      browser_settings.javascript_can_open_windows_automatically;
  settings_.loads_images_automatically =
      browser_settings.loads_images_automatically;
  settings_.text_size_percent = browser_settings.text_size_percent;
  settings_.allow_running_insecure_content =
      browser_settings.allow_running_insecure_content;
  settings_.strict_mixed_content_checking =
      browser_settings.strict_mixed_content_checking;
  settings_.allow_mixed_content_upgrades =
      browser_settings.allow_mixed_content_upgrades;
  settings_.geolocation_enabled = browser_settings.geolocation_enabled;
  settings_.supports_double_tap_zoom =
      browser_settings.supports_double_tap_zoom;
  settings_.supports_multi_touch_zoom =
      browser_settings.supports_multi_touch_zoom;
  settings_.initialize_at_minimum_page_scale =
      browser_settings.initialize_at_minimum_page_scale;
  settings_.viewport_meta_enabled = browser_settings.viewport_meta_enabled;
  settings_.user_gesture_required = browser_settings.user_gesture_required;
  settings_.pinch_smooth_mode = browser_settings.pinch_smooth_mode;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  settings_.hide_vertical_scrollbars =
      browser_settings.hide_vertical_scrollbars;
  settings_.hide_horizontal_scrollbars =
      browser_settings.hide_horizontal_scrollbars;
  settings_.scroll_enabled = browser_settings.scroll_enabled;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(IS_OHOS)
  settings_.native_embed_mode_enabled =
      browser_settings.native_embed_mode_enabled;
  SETTINGS_STRING_SET(browser_settings.embed_tag, settings_.embed_tag);
  SETTINGS_STRING_SET(browser_settings.embed_tag_type,
                      settings_.embed_tag_type);
  settings_.draw_mode = browser_settings.draw_mode;
  settings_.text_autosizing_enabled = browser_settings.text_autosizing_enabled;
#endif  // BUILDFLAG(IS_OHOS)
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  settings_.scrollbar_color = browser_settings.scrollbar_color;
#endif  // ARKWEB_SCROLLBAR

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  settings_.contextmenu_customization_enabled =
      browser_settings.contextmenu_customization_enabled;
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  settings_.copy_option = browser_settings.copy_option;
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  settings_.custom_video_player_enable =
      browser_settings.custom_video_player_enable;
  settings_.custom_video_player_overlay =
      browser_settings.custom_video_player_overlay;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  settings_.record_whole_document = browser_settings.record_whole_document;
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  settings_.supports_multiple_windows =
      browser_settings.supports_multiple_windows;
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  settings_.universal_access_from_file_urls =
      browser_settings.universal_access_from_file_urls;
#endif
  settings_.zoom_control_access = browser_settings.zoom_control_access;
}

std::string GetSharedRenderProcessToken(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) {
  return init_args ? init_args->GetSharedRenderProcessToken() : "";
}

void NWebTestSetBrowserSettings(CefBrowserSettings& browser_settings) {
  CefString str = CefString("StandardFontFamilyName");
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.standard_font_family), true);

  str = CefString("FixedFontFamilyName");
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.fixed_font_family), true);

  str = CefString("SerifFontFamilyName");
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.serif_font_family), true);

  str = CefString("SansSerifFontFamilyName");
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.sans_serif_font_family), true);

  str = CefString("CursiveFontFamilyName");
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.cursive_font_family), true);

  str = CefString("FantasyFontFamilyName");
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.fantasy_font_family), true);

  browser_settings.default_font_size = DEFAULT_FONT_SIZE;
  browser_settings.default_fixed_font_size = DEFAULT_FIXED_FONT_SIZE;
  browser_settings.minimum_font_size = MINIMUM_FONT_SIZE;
  browser_settings.minimum_logical_font_size = MINIMUM_LOGICAL_FONT_SIZE;
  browser_settings.initialize_at_minimum_page_scale = STATE_DISABLED;

  str = CefString("DefaultTextEncodingFormat");
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.default_encoding), true);

  browser_settings.javascript = STATE_DISABLED;
  browser_settings.loads_images_automatically = STATE_DISABLED;
  browser_settings.image_loading = STATE_DISABLED;

  browser_settings.local_storage = STATE_DISABLED;
  browser_settings.databases = STATE_DISABLED;

  browser_settings.universal_access_from_file_urls = STATE_DISABLED;
#if BUILDFLAG(ARKWEB_DARKMODE)
  browser_settings.force_dark_mode_enabled = STATE_DISABLED;
  browser_settings.dark_prefer_color_scheme_enabled = STATE_DISABLED;
#endif
  browser_settings.javascript_can_open_windows_automatically = false;
  browser_settings.text_size_percent = 100;

  browser_settings.allow_running_insecure_content = STATE_DISABLED;
  browser_settings.strict_mixed_content_checking = STATE_DISABLED;
  browser_settings.allow_mixed_content_upgrades = STATE_DISABLED;
  browser_settings.geolocation_enabled = false;
  browser_settings.supports_double_tap_zoom = false;
  browser_settings.supports_multi_touch_zoom = false;
  browser_settings.user_gesture_required = false;
  browser_settings.pinch_smooth_mode = false;
  browser_settings.hide_horizontal_scrollbars = STATE_DISABLED;
  browser_settings.hide_vertical_scrollbars = STATE_DISABLED;
  browser_settings.scroll_enabled = false;

#if BUILDFLAG(IS_OHOS)
  browser_settings.native_embed_mode_enabled = STATE_DISABLED;
  str = CefString("embed_tag");
  cef_string_set(str.c_str(), str.length(), &(browser_settings.embed_tag),
                 true);
  str = CefString("embed_tag_type");
  cef_string_set(str.c_str(), str.length(), &(browser_settings.embed_tag_type),
                 true);
  browser_settings.draw_mode = 0;
  browser_settings.text_autosizing_enabled = STATE_DISABLED;
#endif  // BUILDFLAG(IS_OHOS)
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  browser_settings.copy_option = 0;
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  browser_settings.scrollbar_color = 0;
#endif  // ARKWEB_SCROLLBAR
#if BUILDFLAG(ARKWEB_VIEWPORT)
  browser_settings.viewport_meta_enabled = false;
#endif  // BUILDFLAG(ARKWEB_VIEWPORT)

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  browser_settings.background_color = 0xffffffff;
#endif  // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  browser_settings.contextmenu_customization_enabled = false;
  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  if (command_line->HasSwitch(::switches::kEnableNwebExFreeCopy)) {
    browser_settings.contextmenu_customization_enabled = true;
  }
#endif  // ARKWEB_EXT_FREE_COPY
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  std::tuple<bool, bool> native_video_player_config{false, false};
  browser_settings.custom_video_player_enable =
      std::get<0>(native_video_player_config);
  browser_settings.custom_video_player_overlay =
      std::get<1>(native_video_player_config);
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  browser_settings.supports_multiple_windows = false;
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  browser_settings.record_whole_document = false;
#endif  // ARKWEB_SOFTWARE_COMPOSITOR
  browser_settings.zoom_control_access = true;
}

bool NWebTestVarifyBrowserSettings(CefBrowserSettings& browser_settings,
                                   NWebPreferenceDelegate* pWeb) {
  bool result = true;
  result &= (browser_settings.default_font_size == 16);
  result &= (browser_settings.default_font_size == DEFAULT_FONT_SIZE);
  result &=
      (browser_settings.default_fixed_font_size == DEFAULT_FIXED_FONT_SIZE);
  result &= (browser_settings.minimum_font_size == MINIMUM_FONT_SIZE);
  result &=
      (browser_settings.minimum_logical_font_size == MINIMUM_LOGICAL_FONT_SIZE);
  return result;
}

TEST(NWebPreferenceDelegateTest, WebPreferencesChanged_BrowserNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();

  CefRefPtr<CefBrowser> browser = nullptr;
  preference_delegate->SetBrowser(browser);
  preference_delegate->WebPreferencesChanged();
}

TEST(NWebPreferenceDelegateTest, SetNativeEmbedMode) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  preference_delegate->SetNativeEmbedMode(true);
}

TEST(NWebPreferenceDelegateTest, SetBrowserSettingsToNetHelpers) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->SetBrowserSettingsToNetHelpers();
}

TEST(NWebPreferenceDelegateTest, PutMultiWindowAccess) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutMultiWindowAccess(true);
    EXPECT_TRUE(preference_delegate->IsMultiWindowAccess());
}

TEST(NWebPreferenceDelegateTest, PutEnableContentAccess) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutEnableContentAccess(false);
    EXPECT_FALSE(preference_delegate->IsMultiWindowAccess());
}

TEST(NWebPreferenceDelegateTest, PutEnableRawFileAccess_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutEnableRawFileAccess(false);
    EXPECT_FALSE(preference_delegate->EnableRawFileAccess());
}

TEST(NWebPreferenceDelegateTest, PutEnableRawFileAccessFromFileURLs) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutEnableRawFileAccessFromFileURLs(true);
    EXPECT_TRUE(preference_delegate->EnableRawFileAccessFromFileURLs());
}

TEST(NWebPreferenceDelegateTest, PutEnableUniversalAccessFromFileURLs) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutEnableUniversalAccessFromFileURLs(true);
    EXPECT_TRUE(preference_delegate->EnableUniversalAccessFromFileURLs());
}

TEST(NWebPreferenceDelegateTest, PutLoadImageFromNetworkDisabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutLoadImageFromNetworkDisabled(true);
    EXPECT_TRUE(preference_delegate->IsLoadImageFromNetworkDisabled());
}

TEST(NWebPreferenceDelegateTest, PutCursiveFontFamilyName) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutCursiveFontFamilyName("cursive");
    EXPECT_EQ(preference_delegate->CursiveFontFamilyName(), "cursive");
}

TEST(NWebPreferenceDelegateTest, PutDatabaseAllowed) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutDatabaseAllowed(true);
    EXPECT_TRUE(preference_delegate->IsDataBaseEnabled());
}

TEST(NWebPreferenceDelegateTest, PutDefaultFixedFontSize) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutDefaultFixedFontSize(24);
    EXPECT_EQ(preference_delegate->DefaultFixedFontSize(), 24);
}

TEST(NWebPreferenceDelegateTest, PutDefaultFontSize) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutDefaultFontSize(24);
    EXPECT_EQ(preference_delegate->DefaultFontSize(), 24);
}

TEST(NWebPreferenceDelegateTest, PutDefaultTextEncodingFormat) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutDefaultTextEncodingFormat("UTF-8");
    EXPECT_EQ(preference_delegate->DefaultTextEncodingFormat(), "UTF-8");
}

TEST(NWebPreferenceDelegateTest, PutDomStorageEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutDomStorageEnabled(true);
    EXPECT_TRUE(preference_delegate->IsDomStorageEnabled());
}

TEST(NWebPreferenceDelegateTest, PutFantasyFontFamilyName) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutFantasyFontFamilyName("fansasy");
    EXPECT_EQ(preference_delegate->FantasyFontFamilyName(), "fansasy");
}

TEST(NWebPreferenceDelegateTest, PutFixedFontFamilyName) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutFixedFontFamilyName("monospace");
    EXPECT_EQ(preference_delegate->FixedFontFamilyName(), "monospace");
}

TEST(NWebPreferenceDelegateTest, SetBorderRadiusFromWeb) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->SetBorderRadiusFromWeb(0.0, 0.0, 0.0, 0.0);
}

TEST(NWebPreferenceDelegateTest, SetTouchHandleExistState) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->SetTouchHandleExistState(true);
}

TEST(NWebPreferenceDelegateTest, SetViewportScaleState) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->SetViewportScaleState(true);
}

TEST(NWebPreferenceDelegateTest, PutForceDarkModeEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutForceDarkModeEnabled(0);
    EXPECT_FALSE(preference_delegate->ForceDarkModeEnabled());
}

TEST(NWebPreferenceDelegateTest, PutDarkSchemeEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutDarkSchemeEnabled(0);
    EXPECT_FALSE(preference_delegate->DarkSchemeEnabled());
}

TEST(NWebPreferenceDelegateTest, PutIsCreateWindowsByJavaScriptAllowed) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutIsCreateWindowsByJavaScriptAllowed(true);
    EXPECT_TRUE(preference_delegate->IsCreateWindowsByJavaScriptAllowed());
}

TEST(NWebPreferenceDelegateTest, PutJavaScriptEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutJavaScriptEnabled(true);
    EXPECT_TRUE(preference_delegate->IsJavaScriptAllowed());
}

TEST(NWebPreferenceDelegateTest, PutImageLoadingAllowed) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutImageLoadingAllowed(true);
    EXPECT_TRUE(preference_delegate->IsImageLoadingAllowed());
}

TEST(NWebPreferenceDelegateTest, PutFontSizeLowerLimit) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutFontSizeLowerLimit(24);
    EXPECT_EQ(preference_delegate->FontSizeLowerLimit(), 24);
}

TEST(NWebPreferenceDelegateTest, PutLogicalFontSizeLowerLimit) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutLogicalFontSizeLowerLimit(24);
    EXPECT_EQ(preference_delegate->LogicalFontSizeLowerLimit(), 24);
}

TEST(NWebPreferenceDelegateTest, PutLoadWithOverviewMode) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutLoadWithOverviewMode(true);
    EXPECT_TRUE(preference_delegate->IsLoadWithOverviewMode());
}

TEST(NWebPreferenceDelegateTest, PutSansSerifFontFamilyName) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutSansSerifFontFamilyName("sans-serif");
    EXPECT_EQ(preference_delegate->SansSerifFontFamilyName(), "sans-serif");
}

TEST(NWebPreferenceDelegateTest, PutSerifFontFamilyName) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutSerifFontFamilyName("serif");
    EXPECT_EQ(preference_delegate->SerifFontFamilyName(), "serif");
}

TEST(NWebPreferenceDelegateTest, PutStandardFontFamilyName) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutStandardFontFamilyName("sans-serif");
    EXPECT_EQ(preference_delegate->StandardFontFamilyName(), "sans-serif");
}

TEST(NWebPreferenceDelegateTest, PutZoomingForTextFactor) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutZoomingForTextFactor(99);
    EXPECT_EQ(preference_delegate->ZoomingForTextFactor(), 99);
}

TEST(NWebPreferenceDelegateTest, PutGeolocationAllowed) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutGeolocationAllowed(true);
    EXPECT_TRUE(preference_delegate->GeolocationAllowed());
}

TEST(NWebPreferenceDelegateTest, PutAccessModeForSecureOriginLoadFromInsecure) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutAccessModeForSecureOriginLoadFromInsecure(OHOS::NWeb::NWebPreference::AccessMode::ALWAYS_ALLOW);
    EXPECT_EQ(preference_delegate->AccessModeForSecureOriginLoadFromInsecure(), OHOS::NWeb::NWebPreference::AccessMode::ALWAYS_ALLOW);
}

TEST(NWebPreferenceDelegateTest, PutZoomingFunctionEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutZoomingFunctionEnabled(false);
    EXPECT_FALSE(preference_delegate->ZoomingfunctionEnabled());
}

TEST(NWebPreferenceDelegateTest, PutBlockNetwork_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutBlockNetwork(true);
    EXPECT_TRUE(preference_delegate->IsNetworkBlocked());
    preference_delegate->PutBlockNetwork(false);
    EXPECT_FALSE(preference_delegate->IsNetworkBlocked());
}

TEST(NWebPreferenceDelegateTest, PutCacheMode_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutCacheMode(OHOS::NWeb::NWebPreference::CacheModeFlag::USE_DEFAULT);
    EXPECT_EQ(preference_delegate->CacheMode(), OHOS::NWeb::NWebPreference::CacheModeFlag::USE_DEFAULT);
}

TEST(NWebPreferenceDelegateTest, PutWebDebuggingAccess_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutWebDebuggingAccess(true);
}

TEST(NWebPreferenceDelegateTest, PutMediaPlayGestureAccess) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutMediaPlayGestureAccess(true);
    EXPECT_TRUE(preference_delegate->GetMediaPlayGestureAccess());
}

TEST(NWebPreferenceDelegateTest, PutPinchSmoothMode) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutPinchSmoothMode(true);
    EXPECT_TRUE(preference_delegate->GetPinchSmoothMode());
}

TEST(NWebPreferenceDelegateTest, DefaultUserAgent) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    EXPECT_EQ(preference_delegate->DefaultUserAgent(), "");
}

TEST(NWebPreferenceDelegateTest, UserAgent) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    EXPECT_EQ(preference_delegate->UserAgent(), "");
}

TEST(NWebPreferenceDelegateTest, MixedContentAutoupgradesAllowed001) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->EnableMixedContentAutoUpgrades(true);
    EXPECT_TRUE(preference_delegate->IsMixedContentAutoUpgradesEnabled());
}

TEST(NWebPreferenceDelegateTest, MixedContentAutoupgradesAllowed002) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    EXPECT_FALSE(preference_delegate->MixedContentAutoupgradesAllowed());
}

TEST(NWebPreferenceDelegateTest, PutHasInternetPermission) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutHasInternetPermission(true);

}

TEST(NWebPreferenceDelegateTest, IsWebDebuggingAccess) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->IsWebDebuggingAccess();
}

TEST(NWebPreferenceDelegateTest, PutHorizontalScrollBarAccess) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutHorizontalScrollBarAccess(true);
    EXPECT_TRUE(preference_delegate->IsHorizontalScrollBarAccess());

}

TEST(NWebPreferenceDelegateTest, PutVerticalScrollBarAccess) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutVerticalScrollBarAccess(true);
    EXPECT_TRUE(preference_delegate->IsVerticalScrollBarAccess());
}

TEST(NWebPreferenceDelegateTest, PutOverscrollMode_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutOverscrollMode(6);
    EXPECT_EQ(preference_delegate->GetOverscrollMode(), 6);
}

TEST(NWebPreferenceDelegateTest, SetBlurOnKeyboardHideMode) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->SetBlurOnKeyboardHideMode(24);
    EXPECT_EQ(preference_delegate->GetBlurEnable(), 24);
}

TEST(NWebPreferenceDelegateTest, SetScrollable_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetScrollable(true);
    EXPECT_TRUE(preference_delegate->GetScrollable());
}

TEST(NWebPreferenceDelegateTest, SetScrollable2_UNKNOWN_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetScrollable(true, -1);
    EXPECT_TRUE(preference_delegate->GetScrollable());
}

TEST(NWebPreferenceDelegateTest, SetScrollable2_EVENT_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
        CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetScrollable(true, 0);
    EXPECT_TRUE(preference_delegate->GetScrollable());
}

TEST(NWebPreferenceDelegateTest, SetNativeEmbedMode_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetNativeEmbedMode(true);
    EXPECT_TRUE(preference_delegate->GetNativeEmbedMode());
    preference_delegate->SetNativeEmbedMode(false);
    EXPECT_FALSE(preference_delegate->GetNativeEmbedMode());
}

TEST(NWebPreferenceDelegateTest, RegisterNativeEmbedRule) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->RegisterNativeEmbedRule("embed", "native/");
}

TEST(NWebPreferenceDelegateTest, SetViewportEnable) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetViewportEnable(true);
    EXPECT_TRUE(preference_delegate->GetViewportEnable());
}

TEST(NWebPreferenceDelegateTest, GetScrollBarColor) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->GetScrollBarColor();
}

TEST(NWebPreferenceDelegateTest, SetBackgroundColor) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetBackgroundColor(2);
    EXPECT_EQ(preference_delegate->GetBackgroundColor(), 2);
}

TEST(NWebPreferenceDelegateTest, PutScrollBarColor) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutScrollBarColor(2);
    EXPECT_EQ(preference_delegate->GetScrollBarColor(), 2);
}

TEST(NWebPreferenceDelegateTest, SetDrawMode) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetDrawMode(2);
    EXPECT_EQ(preference_delegate->GetDrawMode(), 2);
}

TEST(NWebPreferenceDelegateTest, PutTextAutosizingEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutTextAutosizingEnabled(true);
    EXPECT_TRUE(preference_delegate->IsTextAutosizingEnabled());
    preference_delegate->PutTextAutosizingEnabled(false);
    EXPECT_FALSE(preference_delegate->IsTextAutosizingEnabled());
}

TEST(NWebPreferenceDelegateTest, SetFitContent) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetFitContent(true);
    EXPECT_TRUE(preference_delegate->IsFitContent());
}

TEST(NWebPreferenceDelegateTest, SetFontWeightScale) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetFontWeightScale(1.0);
    EXPECT_EQ(preference_delegate->GetFontWeightScale(), 1.0);
}

TEST(NWebPreferenceDelegateTest, PutCopyOptionMode) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutCopyOptionMode(OHOS::NWeb::NWebPreference::CopyOptionMode::CROSS_DEVICE);
    EXPECT_EQ(preference_delegate->GetCopyOptionMode(), OHOS::NWeb::NWebPreference::CopyOptionMode::CROSS_DEVICE);
}

TEST(NWebPreferenceDelegateTest, SetGestureFocusMode) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetGestureFocusMode(1);
    EXPECT_EQ(preference_delegate->GetGestureFocusMode(), 1);
}

TEST(NWebPreferenceDelegateTest, SetNativeVideoPlayerConfig) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetNativeVideoPlayerConfig(true, true);
}

TEST(NWebPreferenceDelegateTest, PutOverlayScrollbarEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutOverlayScrollbarEnabled(true);
}

TEST(NWebPreferenceDelegateTest, PutAudioExclusive) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutAudioExclusive(true);
    EXPECT_TRUE(preference_delegate->GetAudioExclusive());
}

TEST(NWebPreferenceDelegateTest, PutAudioResumeInterval) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutAudioResumeInterval(2);
    EXPECT_EQ(preference_delegate->GetAudioResumeInterval(), 2);
}

TEST(NWebPreferenceDelegateTest, PutAudioSessionType) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutAudioSessionType(2);
    EXPECT_EQ(preference_delegate->GetAudioSessionType(), 2);
}

TEST(NWebPreferenceDelegateTest, PutJavaScriptOnDocumentStart) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    ScriptItems script_items = {
      {"testKey", {"testVal1", "testVal2"}}
    };
    preference_delegate->PutJavaScriptOnDocumentStart(script_items);
    EXPECT_EQ(preference_delegate->GetJavaScriptOnDocumentStart(), script_items);
}

TEST(NWebPreferenceDelegateTest, PutJavaScriptOnDocumentStartByOrder) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    ScriptItems script_items = {
      {"testKey", {"testVal1", "testVal2"}}
    };
    ScriptRegexItems script_regex_items = {
        {"testKey", {{"key1", "value1"}, {"key2", "value2"}}}
    };
    ScriptItemsByOrder script_items_start_by_order_ = {"testVal1", "testVal2"};
    preference_delegate->PutJavaScriptOnDocumentStartByOrder(
        script_items, script_regex_items, script_items_start_by_order_);
    EXPECT_EQ(preference_delegate->GetJavaScriptOnDocumentStart(), script_items);
    EXPECT_EQ(preference_delegate->GetJavaScriptRegexItemsOnDocumentStart(), script_regex_items);
    EXPECT_EQ(preference_delegate->GetJavaScriptOnDocumentStartByOrder(), script_items_start_by_order_);
}

TEST(NWebPreferenceDelegateTest, PutJavaScriptOnDocumentEnd) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    ScriptItems script_items = {
      {"testKey", {"testVal1", "testVal2"}}
    };
    ScriptItemsByOrder script_items_start_by_order_ = {"testVal1", "testVal2"};
    preference_delegate->PutJavaScriptOnDocumentEnd(script_items);
    EXPECT_EQ(preference_delegate->GetJavaScriptOnDocumentEnd(), script_items);
}

TEST(NWebPreferenceDelegateTest, PutJavaScriptOnDocumentEndByOrder) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    ScriptItems script_items = {
      {"testKey", {"testVal1", "testVal2"}}
    };
    ScriptRegexItems script_regex_items = {
        {"testKey", {{"key1", "value1"}, {"key2", "value2"}}}
    };
    ScriptItemsByOrder script_items_start_by_order_ = {"testVal1", "testVal2"};
    preference_delegate->PutJavaScriptOnDocumentEndByOrder(
        script_items, script_regex_items, script_items_start_by_order_);
    EXPECT_EQ(preference_delegate->GetJavaScriptOnDocumentEnd(), script_items);
    EXPECT_EQ(preference_delegate->GetJavaScriptRegexItemsOnDocumentEnd(), script_regex_items);
    EXPECT_EQ(preference_delegate->GetJavaScriptOnDocumentEndByOrder(), script_items_start_by_order_);
}

TEST(NWebPreferenceDelegateTest, PutJavaScriptOnHeadReady) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    ScriptItems script_items = {
      {"testKey", {"testVal1", "testVal2"}}
    };
    preference_delegate->PutJavaScriptOnHeadReady(script_items);
    EXPECT_EQ(preference_delegate->GetJavaScriptOnHeadReady(), script_items);
}

TEST(NWebPreferenceDelegateTest, PutJavaScriptOnHeadReadyByOrder) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    ScriptItems script_items = {
      {"testKey", {"testVal1", "testVal2"}}
    };
    ScriptRegexItems script_regex_items = {
        {"testKey", {{"key1", "value1"}, {"key2", "value2"}}}
    };
    ScriptItemsByOrder script_items_start_by_order_ = {"testVal1", "testVal2"};
    preference_delegate->PutJavaScriptOnHeadReadyByOrder(
        script_items, script_regex_items, script_items_start_by_order_);
    EXPECT_EQ(preference_delegate->GetJavaScriptOnHeadReady(), script_items);
    EXPECT_EQ(preference_delegate->GetJavaScriptRegexItemsOnHeadReady(), script_regex_items);
    EXPECT_EQ(preference_delegate->GetJavaScriptOnHeadReadyByOrder(), script_items_start_by_order_);
}

TEST(NWebPreferenceDelegateTest, SetWholePageDrawing) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetWholePageDrawing();
    EXPECT_TRUE(preference_delegate->GetWholeWebPageDrawing());
}

TEST(NWebPreferenceDelegateTest, SetSurfaceId) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetSurfaceId("test");
    EXPECT_EQ(preference_delegate->GetSurfaceId(), "test");
}

TEST(NWebPreferenceDelegateTest, EnableMixedContentAutoUpgrades) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->EnableMixedContentAutoUpgrades(true);
    EXPECT_TRUE(preference_delegate->IsMixedContentAutoUpgradesEnabled());
}

TEST(NWebPreferenceDelegateTest, SetUsageScenario) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetUsageScenario(1);
    EXPECT_EQ(preference_delegate->GetUsageScenario(), 1);
    preference_delegate->SetUsageScenario(2);
    EXPECT_EQ(preference_delegate->GetUsageScenario(), 2);
}

TEST(NWebPreferenceDelegateTest, PutBackForwardCacheOptions) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutBackForwardCacheOptions(1, 2);
    EXPECT_EQ(preference_delegate->GetCacheSize(), 1);
    EXPECT_EQ(preference_delegate->GetTimeToLive(), 2);
}

TEST(NWebPreferenceDelegateTest, SetDelayDurationForBackgroundTabFreezing) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetDelayDurationForBackgroundTabFreezing(-1);
    EXPECT_EQ(preference_delegate->GetDelayDurationForBackgroundTabFreezing(), -1);
        preference_delegate->SetDelayDurationForBackgroundTabFreezing(1);
    EXPECT_EQ(preference_delegate->GetDelayDurationForBackgroundTabFreezing(), 1);
}

TEST(NWebPreferenceDelegateTest, SetAutofillCallback) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->SetAutofillCallback(nullptr);
    EXPECT_EQ(preference_delegate->GetAutofillCallback(), nullptr);
}

TEST(NWebPreferenceDelegateTest, PutWebMediaAVSessionEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutWebMediaAVSessionEnabled(false);
}

TEST(NWebPreferenceDelegateTest, GetCastEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    EXPECT_FALSE(preference_delegate->GetCastEnabled());
}

TEST(NWebPreferenceDelegateTest, PutErrorPageEnabled) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutErrorPageEnabled(true);
    EXPECT_TRUE(preference_delegate->ErrorPageEnabled());
}

TEST(NWebPreferenceDelegateTest, GetPreferenceHash) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->GetPreferenceHash();
}

TEST(NWebPreferenceDelegateTest, ConvertCacheMode) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<CefBrowser> browser = nullptr;
  preference_delegate->SetBrowser(browser);
  preference_delegate->
        PutCacheMode(NWebPreference::CacheModeFlag::USE_CACHE_ELSE_NETWORK);
  preference_delegate->SetBrowserSettingsToNetHelpers();
  EXPECT_EQ(net_service::NetHelpers::cache_mode, 4);

  preference_delegate->
      PutCacheMode(NWebPreference::CacheModeFlag::USE_NO_CACHE);
  preference_delegate->SetBrowserSettingsToNetHelpers();
  EXPECT_EQ(net_service::NetHelpers::cache_mode,
    NWebPreference::CacheModeFlag::USE_NO_CACHE);

  preference_delegate->
      PutCacheMode(NWebPreference::CacheModeFlag::USE_CACHE_ONLY);
  preference_delegate->SetBrowserSettingsToNetHelpers();
  EXPECT_EQ(net_service::NetHelpers::cache_mode, 12);
}

TEST(NWebPreferenceDelegateTest, OnDestroy) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<CefBrowser> browser = nullptr;
  preference_delegate->SetBrowser(browser);
  preference_delegate->OnDestroy();
}

TEST(NWebPreferenceDelegateTest, WebPreferencesChanged_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->WebPreferencesChanged();
}

TEST(NWebPreferenceDelegateTest, PutEnableRawFileAccess_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->PutEnableRawFileAccess(true);
}

TEST(NWebPreferenceDelegateTest, PutUserAgent_BrowserNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = nullptr;
  preference_delegate->SetBrowser(browser);
  preference_delegate->PutUserAgent("");
}

TEST(NWebPreferenceDelegateTest, PutUserAgent_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->PutUserAgent("test");
}

TEST(NWebPreferenceDelegateTest, PutBlockNetwork_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->PutBlockNetwork(true);
  EXPECT_EQ(preference_delegate->ZoomingForTextFactor(), 100);
}

TEST(NWebPreferenceDelegateTest, PutCacheMode_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->PutCacheMode(NWebPreference::CacheModeFlag::USE_DEFAULT);
}

TEST(NWebPreferenceDelegateTest, DefaultUserAgent_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  EXPECT_EQ(preference_delegate->DefaultUserAgent(), "");
}

TEST(NWebPreferenceDelegateTest, RunningInsecureContentAllowed) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  EXPECT_FALSE(preference_delegate->RunningInsecureContentAllowed());
}

TEST(NWebPreferenceDelegateTest, UseStricMixedContentCheckingAllowed) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  EXPECT_TRUE(preference_delegate->UseStricMixedContentCheckingAllowed());
}

TEST(NWebPreferenceDelegateTest, IsWebDebuggingAccess_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  EXPECT_FALSE(preference_delegate->IsWebDebuggingAccess());
}

TEST(NWebPreferenceDelegateTest, PutOverscrollMode_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->PutOverscrollMode(1);
}

TEST(NWebPreferenceDelegateTest, SetScrollable_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetScrollable(true);
}

TEST(NWebPreferenceDelegateTest, SetScrollable2_EVENT_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->SetScrollable(true, 0);
  EXPECT_TRUE(preference_delegate->GetScrollable());
}

TEST(NWebPreferenceDelegateTest, SetNativeEmbedMode_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->SetNativeEmbedMode(true);
}

TEST(NWebPreferenceDelegateTest, SetIntrinsicSizeEnable) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->SetIntrinsicSizeEnable(true);
  EXPECT_TRUE(preference_delegate->GetIntrinsicSizeEnable());
}

TEST(NWebPreferenceDelegateTest, SetCssDisplayChangeEnabled) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->SetCssDisplayChangeEnabled(true);
  EXPECT_TRUE(preference_delegate->GetCssDisplayChangeEnabled());
}

TEST(NWebPreferenceDelegateTest, PutWebMediaAVSessionEnabled_BrowserNotNull) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  CefRefPtr<MockPreferenceCefBrowser> browser = new MockPreferenceCefBrowser();
  browser->SetHost(new MockPreferenceCefBrowserHost());
  preference_delegate->SetBrowser(browser);
  preference_delegate->PutWebMediaAVSessionEnabled(true);
}

TEST(NWebPreferenceDelegateTest, PutZoomControlAccess) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    preference_delegate->PutZoomControlAccess(false);
    EXPECT_FALSE(preference_delegate->IsZoomControlAccess());
}

TEST(NWebPreferenceDelegateTest, SetAgentNeedHighlight_BrowserNull) {
    auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
    CefRefPtr<CefBrowser> browser = nullptr;
    preference_delegate->SetBrowser(browser);
    preference_delegate->PutAgentNeedHighlight(true);
    EXPECT_TRUE(preference_delegate->GetAgentNeedHighlight());
    preference_delegate->PutAgentNeedHighlight(false);
    EXPECT_FALSE(preference_delegate->GetAgentNeedHighlight());
}