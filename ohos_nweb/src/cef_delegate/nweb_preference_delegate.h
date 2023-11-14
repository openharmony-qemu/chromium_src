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

#include "cef/include/cef_browser.h"
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
  void PutCursiveFontFamilyName(std::string font) override;
  void PutDatabaseAllowed(bool flag) override;
  void PutDefaultFixedFontSize(int size) override;
  void PutDefaultFontSize(int size) override;
  void PutDefaultTextEncodingFormat(std::string encoding) override;
  void PutDomStorageEnabled(bool flag) override;
  void PutFantasyFontFamilyName(std::string font) override;
  void PutFixedFontFamilyName(std::string font) override;
  void PutForceDarkModeEnabled(int forceDark) override;
  void PutDarkSchemeEnabled(int darkScheme) override;
  void PutIsCreateWindowsByJavaScriptAllowed(bool flag) override;
  void PutJavaScriptEnabled(bool flag) override;
  void PutImageLoadingAllowed(bool flag) override;
  void PutFontSizeLowerLimit(int size) override;
  void PutLoadWithOverviewMode(bool flag) override;
  void PutLogicalFontSizeLowerLimit(int size) override;
  void PutSansSerifFontFamilyName(std::string font) override;
  void PutSerifFontFamilyName(std::string font) override;
  void PutStandardFontFamilyName(std::string font) override;
  void PutUserAgent(std::string ua) override;
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
  void PutHorizontalScrollBarAccess(bool flag) override;
  void PutVerticalScrollBarAccess(bool flag) override;
  void PutScrollBarColor(uint32_t colorValue) override;
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
  int ForceDarkModeEnabled() override;
  int DarkSchemeEnabled() override;
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
  NWebPreference::AccessMode AccessModeForSecureOriginLoadFromInsecure() override;
  bool ZoomingfunctionEnabled() override;
  bool IsNetworkBlocked() override;
  NWebPreference::CacheModeFlag CacheMode() override;
  bool IsWebDebuggingAccess() override;
  bool GetMediaPlayGestureAccess() override;
  bool IsMultiWindowAccess() override;
  bool IsHorizontalScrollBarAccess() override;
  bool IsVerticalScrollBarAccess() override;
  uint32_t GetScrollBarColor() override;

  bool RunningInsecureContentAllowed();
  bool UseStricMixedContentCheckingAllowed();
  bool MixedContentAutoupgradesAllowed();
  bool GetPinchSmoothMode() override;

  void PutHasInternetPermission(bool flag);
  void SetBackgroundColor(int32_t color);
  int32_t GetBackgroundColor() const;
  void SetEnableBlankTargetPopupIntercept(bool enable);
  bool IsBlankTargetPopupInterceptEnabled();
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
  bool force_dark_mode_enabled_{false};
  bool dark_prefer_color_scheme_enabled_{false};
  bool is_need_gesture_access_{true};
  bool pinch_smooth_mode_{false};
  bool multiWindow_access_{false};
  bool horizontal_scrollBar_access_{true};
  bool vertical_scrollBar_access_{true};
  /* Web preferences end*/
  bool create_windows_by_javascript_allowed_{false};
  std::string user_agent_{""};
  int zooming_for_text_factor_{100};
  bool geolocation_allowed_{true};
  AccessMode access_mode_{AccessMode::NEVER_ALLOW};
  bool zooming_function_enabled_{true};
  bool is_network_blocked_;
  bool has_internet_permission_;
  bool overload_mode_enabled_{true};
  uint32_t scrollbar_color_{0};
  CacheModeFlag cache_mode_flag_{CacheModeFlag::USE_DEFAULT};
  int32_t background_color_{0xffffffff};
  bool enable_blank_target_popup_intercept_{true};
};
}  // namespace OHOS::NWeb

#endif
