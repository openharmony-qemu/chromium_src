/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_PREFERENCE_DELEGATE_H
#define NWEB_PREFERENCE_DELEGATE_H

#include <tuple>

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "cef/include/cef_browser.h"
#if BUILDFLAG(ARKWEB_JSPROXY)
#include "ohos_nweb/include/nweb.h"
#endif
#include "ohos_nweb/include/nweb_preference.h"

namespace OHOS::NWeb {
class NWebPreferenceDelegate : public NWebPreference {
 public:
  NWebPreferenceDelegate();
  ~NWebPreferenceDelegate();
  void OnDestroy();
  void SetBrowser(CefRefPtr<CefBrowser> browser);
  void WebPreferencesChanged();
  void ComputeBrowserSettings(CefBrowserSettings& browser_settings);

  void SetBrowserSettingsToNetHelpers();

  /* set methods */
  void PutEnableContentAccess(bool flag) override;
  void PutEnableRawFileAccess(bool flag) override;
  void PutEnableRawFileAccessFromFileURLs(bool flag) override;
  void PutEnableUniversalAccessFromFileURLs(bool flag) override;
  void PutLoadImageFromNetworkDisabled(bool flag) override;
  void PutCursiveFontFamilyName(const std::string& font) override;
  void PutDatabaseAllowed(bool flag) override;
  void PutDefaultFixedFontSize(int size) override;
  void PutDefaultFontSize(int size) override;
  void PutDefaultTextEncodingFormat(const std::string& encoding) override;
  void PutDomStorageEnabled(bool flag) override;
  void PutFantasyFontFamilyName(const std::string& font) override;
  void PutFixedFontFamilyName(const std::string& font) override;

  void PutIsCreateWindowsByJavaScriptAllowed(bool flag) override;
  void PutJavaScriptEnabled(bool flag) override;
  void PutImageLoadingAllowed(bool flag) override;
  void PutFontSizeLowerLimit(int size) override;
  void PutLoadWithOverviewMode(bool flag) override;
  void PutLogicalFontSizeLowerLimit(int size) override;
  void PutSansSerifFontFamilyName(const std::string& font) override;
  void PutSerifFontFamilyName(const std::string& font) override;
  void PutStandardFontFamilyName(const std::string& font) override;
  void PutUserAgent(const std::string& ua) override;
#if BUILDFLAG(ARKWEB_USERAGENT)
  bool HasSetUserAgent() { return has_set_user_agent_; }
#endif
  void PutZoomingForTextFactor(int textZoom) override;
  void PutGeolocationAllowed(bool flag) override;
  void PutAccessModeForSecureOriginLoadFromInsecure(AccessMode mode) override;
  void PutZoomingFunctionEnabled(bool flag) override;
  void PutBlockNetwork(bool flag) override;
  void PutCacheMode(NWebPreference::CacheModeFlag flag) override;
  void PutWebDebuggingAccess(bool flag) override;
  void PutMediaPlayGestureAccess(bool flag) override;
  void PutPinchSmoothMode(bool flag) override;
  void PutMultiWindowAccess(bool flag) override;
  /* get methods*/
  bool EnableContentAccess() override;
  bool EnableRawFileAccess() override;
  bool EnableRawFileAccessFromFileURLs() override;
  bool EnableUniversalAccessFromFileURLs() override;
  bool IsLoadImageFromNetworkDisabled() override;
  std::string CursiveFontFamilyName() override;
  bool IsDataBaseEnabled() override;
  int DefaultFixedFontSize() override;
  int DefaultFontSize() override;
  std::string DefaultTextEncodingFormat() override;
  std::string DefaultUserAgent() override;
  bool IsDomStorageEnabled() override;
  std::string FantasyFontFamilyName() override;
  std::string FixedFontFamilyName() override;

  bool IsCreateWindowsByJavaScriptAllowed() override;
  bool IsJavaScriptAllowed() override;
  bool IsImageLoadingAllowed() override;
  int FontSizeLowerLimit() override;
  int LogicalFontSizeLowerLimit() override;
  bool IsLoadWithOverviewMode() override;
  std::string SansSerifFontFamilyName() override;
  std::string SerifFontFamilyName() override;
  std::string StandardFontFamilyName() override;
  std::string UserAgent() override;
  int ZoomingForTextFactor() override;
  bool GeolocationAllowed() override;
  NWebPreference::AccessMode AccessModeForSecureOriginLoadFromInsecure()
      override;
  bool ZoomingfunctionEnabled() override;
  bool IsNetworkBlocked() override;
  NWebPreference::CacheModeFlag CacheMode() override;
  bool IsWebDebuggingAccess() override;
  bool GetMediaPlayGestureAccess() override;
  bool IsMultiWindowAccess() override;

  bool RunningInsecureContentAllowed();
  bool UseStricMixedContentCheckingAllowed();
  bool MixedContentAutoupgradesAllowed();
  bool GetPinchSmoothMode() override;

  void PutHasInternetPermission(bool flag);
#if BUILDFLAG(ARKWEB_ZOOM)
  bool IsZoomControlAccess();
  void PutZoomControlAccess(bool zoomControlAccess) override;
#endif
  int ForceDarkModeEnabled() override;
  void PutDarkSchemeEnabled(int darkScheme) override;
  int DarkSchemeEnabled() override;
  void PutForceDarkModeEnabled(int forceDark) override;
  uint32_t GetScrollBarColor() override;
#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  void SetBackgroundColor(int32_t color);
  int32_t GetBackgroundColor() const;
#endif  // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  void PutScrollBarColor(uint32_t colorValue) override;
#if BUILDFLAG(IS_OHOS)
  void SetDrawMode(int mode);
  int GetDrawMode() const;
  void PutTextAutosizingEnabled(bool enable) override;
  bool IsTextAutosizingEnabled() const;
  void SetFitContent(bool value);
  bool IsFitContent() const;
#endif
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  void SetBorderRadiusFromWeb(double borderRadiusTopLeft,
                              double borderRadiusTopRight,
                              double borderRadiusBottomLeft,
                              double borderRadiusBottomRight);
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void SetEnableAutoFill(bool enable) override;
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)

#if BUILDFLAG(ARKWEB_MENU)
  void SetTouchHandleExistState(bool touchHandleExist);
  void SetViewportScaleState(bool viewportScale);
#endif  // BUILDFLAG(ARKWEB_MENU)

#if BUILDFLAG(ARKWEB_CSS_FONT)
  void SetFontWeightScale(float size) override;
  float GetFontWeightScale() const;
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  float GetVirtualPixelRatio() { return virtual_pixel_ratio_; }
  void SetVirtualPixelRatio(float ratio) { virtual_pixel_ratio_ = ratio; }
#endif

#if BUILDFLAG(ARKWEB_PRINT)
  void PutPrintToken(void* token) { token_ = token; }
  void* GetPrintToken() { return token_; }
#endif

#if BUILDFLAG(ARKWEB_AI)
  void PutImageAnalyzerEnabled(bool enabled) override;
  bool GetImageAnalyzerEnabled() override;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool IsHorizontalScrollBarAccess() override;
  bool IsVerticalScrollBarAccess() override;
  void PutHorizontalScrollBarAccess(bool flag) override;
  void PutVerticalScrollBarAccess(bool flag) override;
  void PutOverscrollMode(int overScrollMode) override;
  int GetOverscrollMode() override;
  void SetScrollable(bool enable) override;
  void SetScrollable(bool enable, int32_t scrollType) override;
  bool GetScrollable() override;
  void SetBlurOnKeyboardHideMode(int enable) override;
  int GetBlurEnable();
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition) override;
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetNativeEmbedMode(bool flag) override;
  bool GetNativeEmbedMode() override;
  void RegisterNativeEmbedRule(const std::string& tag,
                               const std::string& type) override;
  void SetIntrinsicSizeEnable(bool enable) override;
  bool GetIntrinsicSizeEnable();
  void SetCssDisplayChangeEnabled(bool enable) override;
  bool GetCssDisplayChangeEnabled();
  bool IsEnableCustomVideoPlayer();
#endif  // BUILDFLAG(ARKWEB_SAME_LAYER)

#if BUILDFLAG(ARKWEB_VIEWPORT)
  void SetViewportEnable(bool enable) override;
  std::optional<bool> GetViewportEnable();
#endif  // BUILDFLAG(ARKWEB_VIEWPORT)

#if BUILDFLAG(ARKWEB_COPY_OPTION)
  void PutCopyOptionMode(CopyOptionMode copyOption) override;
  CopyOptionMode GetCopyOptionMode() override;
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_FOCUS)
  void SetGestureFocusMode(int32_t mode) override;
  int32_t GetGestureFocusMode() const;
#endif  // BUILDFLAG(ARKWEB_FOCUS)

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  void PutAudioExclusive(bool audioExclusive);
  bool GetAudioExclusive();
  void PutAudioResumeInterval(int32_t resumeInterval);
  int32_t GetAudioResumeInterval();
  void PutAudioSessionType(int32_t audioSessionType);
  int32_t GetAudioSessionType();
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

  void SetNativeVideoPlayerConfig(bool enable, bool shouldOverlay) override;
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  void PutOverlayScrollbarEnabled(bool enable) override;
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  void PutJavaScriptOnDocumentStart(const ScriptItems& scriptItems);
  void PutJavaScriptOnDocumentStartByOrder(
      const ScriptItems& scriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& scriptItemsByOrder);
  ScriptItems GetJavaScriptOnDocumentStart();
  ScriptRegexItems GetJavaScriptRegexItemsOnDocumentStart();
  ScriptItemsByOrder GetJavaScriptOnDocumentStartByOrder();
  void PutJavaScriptOnDocumentEnd(const ScriptItems& scriptItems);
  void PutJavaScriptOnDocumentEndByOrder(
      const ScriptItems& scriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& scriptItemsByOrder);
  ScriptItems GetJavaScriptOnDocumentEnd();
  ScriptRegexItems GetJavaScriptRegexItemsOnDocumentEnd();
  ScriptItemsByOrder GetJavaScriptOnDocumentEndByOrder();
  void PutJavaScriptOnHeadReady(const ScriptItems& scriptItems);
  void PutJavaScriptOnHeadReadyByOrder(
      const ScriptItems& scriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& scriptItemsByOrder);
  ScriptItems GetJavaScriptOnHeadReady();
  ScriptRegexItems GetJavaScriptRegexItemsOnHeadReady();
  ScriptItemsByOrder GetJavaScriptOnHeadReadyByOrder();
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  void SetWholePageDrawing();
  bool GetWholeWebPageDrawing();
#endif

#if BUILDFLAG(IS_OHOS)
  std::string GetSurfaceId() override;
  void SetSurfaceId(const std::string& surfaceId) override;
#endif

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
  void EnableMixedContentAutoUpgrades(bool enable);
  bool IsMixedContentAutoUpgradesEnabled();
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  void SetUsageScenario(int32_t usage_scenario);
  int32_t GetUsageScenario();
#endif

#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
  void EnableMediaNetworkTrafficPrompt(bool enable);
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT

#if BUILDFLAG(ARKWEB_BFCACHE)
  int GetCacheSize();
  int GetTimeToLive();
  void PutBackForwardCacheOptions(int size, int time_to_live);
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  void SetDelayDurationForBackgroundTabFreezing(
      int64_t delay_for_background_tab_freezing);
  int64_t GetDelayDurationForBackgroundTabFreezing();
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  CefRefPtr<CefWebMessageReceiver> GetAutofillCallback();
  void SetAutofillCallback(CefRefPtr<CefWebMessageReceiver> callback);

  std::shared_ptr<NWebVaultPlainTextCallback> GetVaultPlainTextCallback();
  void PutVaultPlainTextCallback(std::shared_ptr<NWebVaultPlainTextCallback> callback);
#endif

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  void PutWebMediaAVSessionEnabled(bool enable) override;
#endif  // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  bool ErrorPageEnabled();
  void PutErrorPageEnabled(bool enable);
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  int64_t GetPreferenceHash();
  uint32_t GetRotationType();
  bool SetRotationType(uint32_t type);
#endif

 private:
  CefRefPtr<CefBrowser> browser_ = nullptr;

  /* Web preferences begin*/
  std::string standard_font_family_name_{"sans-serif"};
  std::string fixed_font_family_name_{"monospace"};
  std::string serif_font_family_name_{"serif"};
  std::string sans_serif_font_family_name_{"sans-serif"};
  std::string cursive_font_family_name_{"cursive"};
  std::string fantasy_font_family_name_{"fantasy"};
  int default_font_size_{16};
  int default_fixed_font_size_{13};
  int font_size_lower_limit_{8};
  int logical_font_size_lower_limit_{8};
  std::string default_encoding_{"UTF-8"};
  bool images_enabled_{true};
  bool javascript_allowed_{false};
  bool image_loading_allowed_{true};
  bool dom_storage_{false};
  bool databases_enabled_{false};
  bool content_access_{true};
  bool raw_file_access_{false};
  bool universal_access_from_file_urls_{false};
  bool raw_file_access_from_file_urls_{false};
#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  bool error_page_enabled_{false};
#endif
#if BUILDFLAG(ARKWEB_DARKMODE)
  bool force_dark_mode_enabled_{false};
  bool dark_prefer_color_scheme_enabled_{false};
#endif
  bool is_need_gesture_access_{true};
  bool pinch_smooth_mode_{false};
  bool multiWindow_access_{false};
  /* Web preferences end*/
  bool create_windows_by_javascript_allowed_{false};
  std::string user_agent_{""};
#if BUILDFLAG(ARKWEB_USERAGENT)
  bool has_set_user_agent_{false};
#endif
  int zooming_for_text_factor_{100};
  bool geolocation_allowed_{true};
  AccessMode access_mode_{AccessMode::NEVER_ALLOW};
  bool zooming_function_enabled_{true};
  bool zoom_control_access_{true};
  bool is_network_blocked_;
  bool has_internet_permission_;
  bool overload_mode_enabled_{true};
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  uint32_t scrollbar_color_{0};
#endif  // ARKWEB_SCROLLBAR
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  double border_radius_top_left_{0.0};
  double border_radius_top_right_{0.0};
  double border_radius_bottom_left_{0.0};
  double border_radius_bottom_right_{0.0};
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER
#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  bool is_autofill_enabled_{true};
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
#if BUILDFLAG(ARKWEB_MENU)
  bool touch_handle_exist_{false};
  bool viewport_scale_{false};
#endif  // BUILDFLAG(ARKWEB_MENU)
#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  float virtual_pixel_ratio_ = 2.0;
#endif
#if BUILDFLAG(ARKWEB_PRINT)
  raw_ptr<void> token_ = nullptr;
#endif
#if BUILDFLAG(ARKWEB_AI)
  bool image_analyzer_enabled_{true};
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool horizontal_scrollBar_access_{true};
  bool vertical_scrollBar_access_{true};
  int overscroll_mode_{0};
  bool scroll_enabled_{true};
  bool setting_scroll_enabled_{true};
  int blur_enabled_;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_VIEWPORT)
  std::optional<bool> viewport_enabled_;
#endif  // BUILDFLAG(ARKWEB_VIEWPORT)
#if BUILDFLAG(IS_OHOS)
  int draw_mode_{0};
  bool text_autosizing_enabled_{true};
  std::string surface_id_{""};
  bool fit_content_{false};
#endif
#if BUILDFLAG(ARKWEB_CSS_FONT)
  float font_weight_scale_ = 1.0f;
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  bool enable_embed_mode_{false};
  bool enable_intrinsic_size_{false};
  bool enable_css_display_change_{false};
  std::string embed_tag_{"embed"};
  std::string embed_tag_type_{"native/"};
#endif  // BUILDFLAG(ARKWEB_SAME_LAYER)

#if BUILDFLAG(ARKWEB_COPY_OPTION)
  CopyOptionMode copy_option_{CopyOptionMode::CROSS_DEVICE};
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_FOCUS)
  int32_t gesture_focus_mode_{0};
#endif

  CacheModeFlag cache_mode_flag_{CacheModeFlag::USE_DEFAULT};
#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  int32_t background_color_{0xffffffff};
#endif  // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  std::tuple<bool, bool> native_video_player_config_{false, false};
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  bool audio_exclusive_{true};
  int32_t resume_interval_{0};
  int32_t audio_session_type_{5};
#endif
#if BUILDFLAG(ARKWEB_JSPROXY)
  ScriptItems script_items_start_{};
  ScriptItems script_items_end_{};
  ScriptItems script_items_head_ready_{};
  ScriptRegexItems script_regex_items_start_{};
  ScriptRegexItems script_regex_items_end_{};
  ScriptRegexItems script_regex_items_head_ready_{};
  ScriptItemsByOrder script_items_start_by_order_;
  ScriptItemsByOrder script_items_end_by_order_;
  ScriptItemsByOrder script_items_head_ready_by_order_;
#endif
  bool record_whole_document_{false};
#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
  bool enable_mixed_content_auto_upgrades_{false};
#endif
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  int32_t usage_scenario_{1};
#endif
#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
  bool enable_media_network_traffic_prompt_ = false;
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT

#if BUILDFLAG(ARKWEB_BFCACHE)
  int size_ = -1;
  int time_to_live_ = -1;
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  int64_t delay_for_background_tab_freezing_ = -1;
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  CefRefPtr<CefWebMessageReceiver> autofill_callback_ = nullptr;
  std::shared_ptr<NWebVaultPlainTextCallback> vault_plain_text_callback_ = nullptr;
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  bool pref_hash_cached_ = false;
  int64_t pref_hash_ = 0;
  uint32_t rotationType_ = 0;
#endif
};
}  // namespace OHOS::NWeb

#endif
