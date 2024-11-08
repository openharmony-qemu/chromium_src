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
#include "gtest/gtest.h"


#include "cef/include/cef_client.h"
#include "cef/include/cef_display_handler.h"
#include "cef/libcef/browser/browser_contents_delegate.h"
#include "cef/libcef/browser/browser_info.h"
#include "nweb.h"
#include "nweb_preference_delegate.h"
#include "ui/native_theme/native_theme_features.h"


const int kDefaultFontSize = 16;
const int kDefaultFixedFontSize = 33;
const int kMinimumFontSize = 8;
const int kMinimumLogicalFontSize = 8;

using namespace testing;
using namespace OHOS::NWeb;

class MockCefBrowser : public CefBrowser, public CefBrowserHost {
 public:
  static CefRefPtr<MockCefBrowser> Create();

  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
  bool IsValid() override { return false; }
  CefRefPtr<CefBrowserHost> GetHost() override;
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
  CefRefPtr<CefFrame> GetFrame(int64 identifier) override { return nullptr; }
  CefRefPtr<CefFrame> GetFrame(const CefString& name) override {
    return nullptr;
  }
  size_t GetFrameCount() override { return 0; }
  void GetFrameIdentifiers(std::vector<int64>& identifiers) override {}
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
  void SelectAndCopy() override {}
  bool ShouldShowFreeCopy() override { return false; }
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
  int GetNWebId() override { return -1; }
  void SetEnableBlankTargetPopupIntercept(
      bool enableBlankTargetPopup) override {}
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
                                CefString& detailErrMsg) override {
    return -1;
  }
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override {}
#endif  // BUILDFLAG(IS_OHOS)

  CefRefPtr<CefBrowser> GetBrowser() override;
  void CloseBrowser(bool force_close) override {}
  bool TryCloseBrowser() override { return true; }
  void SetFocus(bool focus) override {}
  CefWindowHandle GetWindowHandle() override { return 0; }
  CefWindowHandle GetOpenerWindowHandle() override { return 0; }
  bool HasView() override { return true; }
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
                     uint32 max_image_size,
                     bool bypass_cache,
                     CefRefPtr<CefDownloadImageCallback> callback) override {}
  void Print() override {}
  void PrintToPDF(const CefString& path,
                  const CefPdfPrintSettings& settings,
                  CefRefPtr<CefPdfPrintCallback> callback) override {}
  void Find(const CefString& searchText,
            bool forward,
            bool matchCase,
            bool findNext,
            bool newSession) override {}
  void StopFinding(bool clearSelection) override {}
  void ShowDevTools(const CefWindowInfo& windowInfo,
                    CefRefPtr<CefClient> client,
                    const CefBrowserSettings& settings,
                    const CefPoint& inspect_element_at) override {}
  void CloseDevTools() override {}
  bool HasDevTools() override { return true; }
  bool SendDevToolsMessage(const void* message, size_t message_size) override {
    return true;
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
  bool IsWindowRenderingDisabled() override { return true; }
  void WasResized() override {}
  void WasHidden(bool hidden) override {}
  void WasOccluded(bool occluded) override {}
  void OnWindowShow() override {}
  void OnWindowHide() override {}
  void OnOnlineRenderToForeground() override {}
  void SendTouchEventList(
      const std::vector<CefTouchEvent>& event_list) override {}
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
  CefRefPtr<CefExtension> GetExtension() override { return nullptr; }
  bool IsBackgroundHost() override { return true; }
  void SetAudioMuted(bool mute) override {}
  bool IsAudioMuted() override { return true; }
  void GetRootBrowserAccessibilityManager(void** manager) override {}
  void ExecuteJavaScript(const std::string& code,
                         CefRefPtr<CefJavaScriptResultCallback> callback,
                         bool extention) override {}
  void ExecuteJavaScriptExt(const int fd,
                            const uint64 scriptLength,
                            CefRefPtr<CefJavaScriptResultCallback> callback,
                            bool extention) override {}
  void SetNativeWindow(cef_native_window_t window) override {}
  void SetWebDebuggingAccess(bool isEnableDebug) override {}
  bool GetWebDebuggingAccess() override { return true; }
  void GetImageForContextNode() override {}
  void GetImageFromCache(const CefString& url) override {}
  void ExitFullScreen() override {}
  void UpdateLocale(const CefString& locale) override {}
  CefString GetOriginalUrl() override { return ""; }
  void PutNetworkAvailable(bool available) override {}
  void RemoveCache(bool include_disk_files) override {}
  void PostTaskToUIThread(CefRefPtr<CefTask> task) override {}
  void SetVirtualPixelRatio(float ratio) override {}
  float GetVirtualPixelRatio() override { return 0.0; }
  void SetWebPreferences(const CefBrowserSettings& browser_settings) override;
  void PutUserAgent(const CefString& ua) override {}
  CefString DefaultUserAgent() override { return ""; }
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
  CefString Title() override { return ""; }
  void CreateWebMessagePorts(std::vector<CefString>& ports) override {}
  void PostWebMessage(CefString& message,
                      std::vector<CefString>& ports,
                      CefString& targetUri) override {}
  void ClosePort(CefString& port_handle) override {}
  void DestroyAllWebMessagePorts() override {}
  void PostPortMessage(const CefString& port_handle,
                       CefRefPtr<CefValue> message) override {}
  void SetPortMessageCallback(
      const CefString& port_handle,
      CefRefPtr<CefWebMessageReceiver> callback) override {}
  void GetHitData(int& type, CefString& extra_data) override {}
  void SetInitialScale(float scale) override {}
  int PageLoadProgress() override { return 0; }
  float Scale() override { return 0.0; }
  void LoadWithDataAndBaseUrl(const CefString& baseUrl,
                              const CefString& data,
                              const CefString& mimeType,
                              const CefString& encoding,
                              const CefString& historyUrl) override {}
  void LoadWithData(const CefString& data,
                    const CefString& mimeType,
                    const CefString& encoding) override {}
  void AddVisitedLinks(const std::vector<CefString>& urls) {}
  void ResumeDownload(const CefString& url,
                      const CefString& full_path,
                      int64 received_bytes,
                      int64 total_bytes,
                      const CefString& etag,
                      const CefString& mime_type,
                      const CefString& last_modified,
                      const CefString& received_slices_string) override {}
  void SetAudioResumeInterval(int resumeInterval) override {}
  void SetAudioExclusive(bool audioExclusive) override {}
  void CloseMedia() override {}
  void StopMedia() override {}
  void ResumeMedia() override {}
  void PauseMedia() override {}
  int GetMediaPlaybackState() override { return 0; }
  void ScrollPageUpDown(bool is_up, bool is_half, float view_height) override {}
  CefRefPtr<CefBinaryValue> GetWebState() override { return nullptr; }
  bool RestoreWebState(const CefRefPtr<CefBinaryValue> state) override {
    return true;
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
  bool ShouldVirtualKeyboardOverlay() override { return true; }
  void JavaScriptOnDocumentStart(
      const CefString& script,
      const std::vector<CefString>& script_rules) override {}
  void RemoveJavaScriptOnDocumentStart() override {}
  void JavaScriptOnDocumentEnd(
      const CefString& script,
      const std::vector<CefString>& script_rules) override {}
  void RemoveJavaScriptOnDocumentEnd() override {}
  void SetDrawRect(int x, int y, int width, int height) override {}
  void SetDrawMode(int mode) override {}
  void CreateWebPrintDocumentAdapter(const CefString& jobName,
                                     void** webPrintDocumentAdapter) override {}
  void SetOverscrollMode(int mode) override {}
  bool Discard() override { return true; }
  bool Restore() override { return true; }
  void SetBrowserZoomLevel(double zoomFactor) override {}
  int GetTopControlsOffset() override { return 0; }
  int GetShrinkViewportHeight() override { return 0; }
  void SetPrintBackground(bool enable) override {}
  bool GetPrintBackground() override { return 0; }
  void SetScrollable(bool enable, int scrollType) override {}
  void StartCamera() override {}
  void StopCamera() override {}
  void CloseCamera() override {}
  CefString GetLastJavascriptProxyCallingFrameUrl() override { return ""; }
  void SetNWebId(int nWebId) override {}
  bool GetPendingSizeStatus() override { return true; }
  void PrecompileJavaScript(
      const std::string& url,
      const std::string& script,
      CefRefPtr<CefCacheOptions> cacheOptions,
      CefRefPtr<CefPrecompileCallback> callback) override {}
  void SetWakeLockHandler(int32_t windowId,
                          CefRefPtr<CefSetLockCallback> callback) override {}
  CefRefPtr<CefDownloadItem> GetDownloadItem(uint32 item_id) override {
    return nullptr;
  }
  void NotifyNeedsReload(bool needs_reload) override {}
  bool NeedsReload() override { return true; }
  bool TerminateRenderProcess() override { return true; }
  void RegisterNativeJSProxy(const CefString& object_name,
                             const std::vector<CefString>& method_list,
                             const int32_t object_id,
                             bool is_async,
                             const CefString& permission) override {}
  void SendTouchpadFlingEvent(const CefMouseEvent& event,
                              double vx,
                              double vy) override {}
  void SetFitContentMode(int mode) override {}
  void UpdateDrawRect() override {}
  void OnTextSelected(bool flag) override {}
  float GetPageScaleFactor() override { return 1.0; }
  bool WebPageSnapshot(const char* id,
                       int width,
                       int height,
                       cef_web_snapshot_callback_t callback) override {
    return true;
  }
  void AdvanceFocusForIME(int focusType) override {}
  void ScrollToWithAnime(float x, float y, int32_t duration) override {}
  void ScrollByWithAnime(float delta_x,
                         float delta_y,
                         int32_t duration) override {}
  void GetScrollOffset(float* offset_x, float* offset_y) override {}
  void GetOverScrollOffset(float* offset_x, float* offset_y) override {}
  void OnSafeInsetsChange(int left, int top, int right, int bottom) override {}
  void NotifyForNextTouchEvent() override {}
  void SetGrantFileAccessDirs(const std::vector<CefString>& dir_list) override {
  }
  void SetAutofillCallback(CefRefPtr<CefWebMessageReceiver> callback) override {
  }
  void FillAutofillData(CefRefPtr<CefValue> message) override {}
  void ScrollFocusedEditableNodeIntoView() override {}
  void ProcessAutofillCancel(const std::string& fillContent) override {}
  void AutoFillWithIMFEvent(bool is_username,
                            bool is_other_account,
                            bool is_new_password,
                            const std::string& content) override {}
  void CreateToPDF(const CefPdfPrintSettings& settings,
                   CefRefPtr<CefPdfValueCallback> callback) override {}

  void SetPopupWindow(cef_native_window_t window) {}
  void UpdateBrowserSettings(const CefBrowserSettings& browser_settings);
  CefBrowserSettings settings_;
  std::unique_ptr<CefBrowserContentsDelegate> contents_delegate_;
};

CefRefPtr<MockCefBrowser> MockCefBrowser::Create() {
  auto browser = new MockCefBrowser();
  return browser;
}

CefRefPtr<CefBrowserHost> MockCefBrowser::GetHost() {
  return this;
}
CefRefPtr<CefBrowser> MockCefBrowser::GetBrowser() {
  return this;
}
void MockCefBrowser::SetWebPreferences(
    const CefBrowserSettings& browser_settings) {
  UpdateBrowserSettings(browser_settings);
}
void MockCefBrowser::UpdateBrowserSettings(
    const CefBrowserSettings& browser_settings) {
  cef_string_set(browser_settings.standard_font_family.str,
                 browser_settings.standard_font_family.length,
                 &(settings_.standard_font_family),
                 true);
  cef_string_set(browser_settings.fixed_font_family.str,
                 browser_settings.fixed_font_family.length,
                 &(settings_.fixed_font_family),
                 true);
  cef_string_set(browser_settings.serif_font_family.str,
                 browser_settings.serif_font_family.length,
                 &(settings_.serif_font_family),
                 true);
  cef_string_set(browser_settings.sans_serif_font_family.str,
                 browser_settings.sans_serif_font_family.length,
                 &(settings_.sans_serif_font_family),
                 true);
  cef_string_set(browser_settings.cursive_font_family.str,
                 browser_settings.cursive_font_family.length,
                 &(settings_.cursive_font_family),
                 true);
  cef_string_set(browser_settings.fantasy_font_family.str,
                 browser_settings.fantasy_font_family.length,
                 &(settings_.fantasy_font_family),
                 true);

  settings_.default_font_size = browser_settings.default_font_size;
  settings_.default_fixed_font_size = browser_settings.default_fixed_font_size;
  settings_.minimum_font_size = browser_settings.minimum_font_size;
  settings_.minimum_logical_font_size =
      browser_settings.minimum_logical_font_size;
  cef_string_set(browser_settings.default_encoding.str,
                 browser_settings.default_encoding.length,
                 &(settings_.default_encoding),
                 true);

  settings_.javascript = browser_settings.javascript;
  settings_.image_loading = browser_settings.image_loading;
  settings_.local_storage = browser_settings.local_storage;
  settings_.databases = browser_settings.databases;

  settings_.force_dark_mode_enabled = browser_settings.force_dark_mode_enabled;
#if defined(OHOS_DARKMODE)
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
#if defined(OHOS_INPUT_EVENTS)
  settings_.hide_vertical_scrollbars =
      browser_settings.hide_vertical_scrollbars;
  settings_.hide_horizontal_scrollbars =
      browser_settings.hide_horizontal_scrollbars;
  settings_.scroll_enabled = browser_settings.scroll_enabled;
#endif  // defined(OHOS_INPUT_EVENTS)
#if BUILDFLAG(IS_OHOS)
  settings_.native_embed_mode_enabled =
      browser_settings.native_embed_mode_enabled;
  cef_string_set(browser_settings.embed_tag.str,
                 browser_settings.embed_tag.length,
                 &(settings_.embed_tag),
                 true);
  cef_string_set(browser_settings.embed_tag_type.str,
                 browser_settings.embed_tag_type.length,
                 &(settings_.embed_tag),
                 true);
  settings_.draw_mode = browser_settings.draw_mode;
  settings_.text_autosizing_enabled = browser_settings.text_autosizing_enabled;
#endif  // BUILDFLAG(IS_OHOS)
#ifdef OHOS_SCROLLBAR
  settings_.scrollbar_color = browser_settings.scrollbar_color;
#endif  // OHOS_SCROLLBAR

#ifdef OHOS_EX_FREE_COPY
  settings_.contextmenu_customization_enabled =
      browser_settings.contextmenu_customization_enabled;
#endif

#if defined(OHOS_CLIPBOARD)
  settings_.copy_option = browser_settings.copy_option;
#endif  // defined(OHOS_CLIPBOARD)
#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  settings_.custom_video_player_enable =
      browser_settings.custom_video_player_enable;
  settings_.custom_video_player_overlay =
      browser_settings.custom_video_player_overlay;
#endif  // OHOS_CUSTOM_VIDEO_PLAYER

#if defined(OHOS_SOFTWARE_COMPOSITOR)
  settings_.record_whole_document = browser_settings.record_whole_document;
#endif

#if defined(OHOS_MULTI_WINDOW)
  settings_.supports_multiple_windows =
      browser_settings.supports_multiple_windows;
#endif  // OHOS_MULTI_WINDOW

#ifdef OHOS_NETWORK_LOAD
  settings_.universal_access_from_file_urls =
      browser_settings.universal_access_from_file_urls;
#endif
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

  browser_settings.default_font_size = kDefaultFontSize;
  browser_settings.default_fixed_font_size = kDefaultFixedFontSize;
  browser_settings.minimum_font_size = kMinimumFontSize;
  browser_settings.minimum_logical_font_size = kMinimumLogicalFontSize;
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
#if defined(OHOS_DARKMODE)
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
#if defined(OHOS_CLIPBOARD)
  browser_settings.copy_option = 0;
#endif  // defined(OHOS_CLIPBOARD)
#ifdef OHOS_SCROLLBAR
  browser_settings.scrollbar_color = 0;
#endif  // OHOS_SCROLLBAR
#ifdef OHOS_VIEWPORT
  browser_settings.viewport_meta_enabled = false;
#endif  // OHOS_VIEWPORT

#if defined(OHOS_BACKGROUND_COLOR)
  browser_settings.background_color = 0xffffffff;
#endif  // defined(OHOS_BACKGROUND_COLOR)
#ifdef OHOS_EX_FREE_COPY
  browser_settings.contextmenu_customization_enabled = false;
  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  if (command_line->HasSwitch(::switches::kEnableNwebExFreeCopy)) {
    browser_settings.contextmenu_customization_enabled = true;
  }
#endif  // OHOS_EX_FREE_COPY
#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  std::tuple<bool, bool> native_video_player_config{false, false};
  browser_settings.custom_video_player_enable =
      std::get<0>(native_video_player_config);
  browser_settings.custom_video_player_overlay =
      std::get<1>(native_video_player_config);
#endif  // OHOS_CUSTOM_VIDEO_PLAYER
#if defined(OHOS_MULTI_WINDOW)
  browser_settings.supports_multiple_windows = false;
#endif  // defined(OHOS_MULTI_WINDOW)
#if defined(OHOS_SOFTWARE_COMPOSITOR)
  browser_settings.record_whole_document = false;
#endif  // OHOS_SOFTWARE_COMPOSITOR
}

bool NWebTestVarifyBrowserSettings(CefBrowserSettings& browser_settings,
                                   NWebPreferenceDelegate* pWeb) {
  bool result = true;
  result &= (browser_settings.default_font_size == 16);
  result &= (browser_settings.default_font_size == kDefaultFontSize);
  result &=
      (browser_settings.default_fixed_font_size == kDefaultFixedFontSize);
  result &= (browser_settings.minimum_font_size == kMinimumFontSize);
  result &=
      (browser_settings.minimum_logical_font_size == kMinimumLogicalFontSize);
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

TEST(NWebPreferenceDelegateTest, PutOverlayScrollbarEnabled) {
  auto preference_delegate = std::make_shared<NWebPreferenceDelegate>();
  preference_delegate->PutOverlayScrollbarEnabled(true);
  EXPECT_FALSE(ui::IsOverlayScrollbarEnabled());
}
