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
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "cef/include/cef_client.h"
#include "cef/include/cef_display_handler.h"
#include "cef/include/cef_command_line.h"
#include "cef/libcef/browser/browser_contents_delegate.h"
#include "cef/libcef/browser/browser_info.h"
#include "cef/include/cef_devtools_message_handler_delegate.h"
#include "gtest/gtest.h"
#include "nweb.h"

#define DEFAULT_FONT_SIZE 16
#define DEFAULT_FIXED_FONT_SIZE 33
#define MINIMUM_FONT_SIZE 8
#define MINIMUM_LOGICAL_FONT_SIZE 8
#define SETTINGS_STRING_SET(src, target) \
  cef_string_set(src.str, src.length, &target, true)

using namespace testing;
using namespace OHOS::NWeb;

class MockCefBrowser : public CefBrowser, public CefBrowserHost {
 public:
  static CefRefPtr<MockCefBrowser> Create();

  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
  bool IsIframe() override { return false; }

  bool IsValid() override {return false;}
  CefRefPtr<ArkWebBrowserExt> AsArkWebBrowser() { return nullptr; }
  CefRefPtr<CefBrowserHostBase> AsCefBrowserHostBase() {return nullptr;}
  CefRefPtr<ArkWebBrowserHostExt> GetHost() { return nullptr; }
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
  CefBrowserSettings settings_;
  std::unique_ptr<CefBrowserContentsDelegate> contents_delegate_;
};

CefRefPtr<MockCefBrowser> MockCefBrowser::Create() {
  auto browser = new MockCefBrowser();
  return browser;
}

void MockCefBrowser::UpdateBrowserSettings(
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
