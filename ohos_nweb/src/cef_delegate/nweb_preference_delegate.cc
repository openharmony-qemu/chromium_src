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

#include "ohos_nweb/src/cef_delegate/nweb_preference_delegate.h"

#include "base/logging.h"
#include "cef/include/cef_command_line.h"
#include "cef/include/internal/cef_string.h"
#include "cef/include/internal/cef_string_types.h"
#include "cef/include/internal/cef_string_wrappers.h"
#include "cef/include/internal/cef_types.h"
#include "cef/include/internal/cef_types_wrappers.h"
#include "cef/libcef/browser/net_service/net_helpers.h"

#include "base/feature_list.h"
#include "content/public/common/content_switches.h"
#include "net/base/load_flags.h"
#include "ohos_adapter_helper.h"
#include "ohos_nweb/src/cef_delegate/nweb_application.h"

namespace OHOS::NWeb {

constexpr int fontMinSize = 1;
constexpr int fontMaxSize = 72;

int ConvertCacheMode(NWebPreference::CacheModeFlag flag) {
  switch (flag) {
    case NWebPreference::CacheModeFlag::USE_CACHE_ELSE_NETWORK:
      return net::LOAD_SKIP_CACHE_VALIDATION;
    case NWebPreference::CacheModeFlag::USE_NO_CACHE:
      return net::LOAD_BYPASS_CACHE;
    case NWebPreference::CacheModeFlag::USE_CACHE_ONLY:
      return net::LOAD_ONLY_FROM_CACHE | net::LOAD_SKIP_CACHE_VALIDATION;
    default:
      return 0;
  }
}

const std::string ACCESS_LOCATION = "ohos.permission.INTERNET";
NWebPreferenceDelegate::NWebPreferenceDelegate() {
  has_internet_permission_ = OhosAdapterHelper::GetInstance()
                                 .GetAccessTokenAdapterInstance()
                                 .VerifyAccessToken(ACCESS_LOCATION);
  is_network_blocked_ = !has_internet_permission_;
  SetBrowserSettingsToNetHelpers();
}

NWebPreferenceDelegate::~NWebPreferenceDelegate() {}

void NWebPreferenceDelegate::OnDestroy() {
  browser_ = nullptr;
}

void NWebPreferenceDelegate::SetBrowser(CefRefPtr<CefBrowser> browser) {
  this->browser_ = browser;
}

void NWebPreferenceDelegate::WebPreferencesChanged() {
  if (!browser_) {
    LOG(ERROR) << "update web preferences failed, browser is null";
    return;
  }

  CefBrowserSettings browser_settings;
  ComputeBrowserSettings(browser_settings);
  browser_->GetHost()->SetWebPreferences(browser_settings);
}

void NWebPreferenceDelegate::ComputeBrowserSettings(
    CefBrowserSettings& browser_settings) {
  CefString str = CefString(StandardFontFamilyName());
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.standard_font_family), true);

  str = CefString(FixedFontFamilyName());
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.fixed_font_family), true);

  str = CefString(SerifFontFamilyName());
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.serif_font_family), true);

  str = CefString(SansSerifFontFamilyName());
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.sans_serif_font_family), true);

  str = CefString(CursiveFontFamilyName());
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.cursive_font_family), true);

  str = CefString(FantasyFontFamilyName());
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.fantasy_font_family), true);

  browser_settings.default_font_size = DefaultFontSize();
  browser_settings.default_fixed_font_size = DefaultFixedFontSize();
  browser_settings.minimum_font_size = FontSizeLowerLimit();
  browser_settings.minimum_logical_font_size = LogicalFontSizeLowerLimit();
  browser_settings.initialize_at_minimum_page_scale =
      IsLoadWithOverviewMode() ? STATE_ENABLED : STATE_DISABLED;

  str = CefString(DefaultTextEncodingFormat());
  cef_string_set(str.c_str(), str.length(),
                 &(browser_settings.default_encoding), true);

  browser_settings.javascript =
      IsJavaScriptAllowed() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.loads_images_automatically =
      IsImageLoadingAllowed() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.image_loading =
      !IsLoadImageFromNetworkDisabled() ? STATE_ENABLED : STATE_DISABLED;

  browser_settings.local_storage =
      IsDomStorageEnabled() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.databases =
      IsDataBaseEnabled() ? STATE_ENABLED : STATE_DISABLED;
 // TODO(ohos): Fix allow_universal_access_from_file_urls and allow_file_access_from_file_urls
  //browser_settings.universal_access_from_file_urls =
  //    EnableUniversalAccessFromFileURLs() ? STATE_ENABLED : STATE_DISABLED;
  //browser_settings.file_access_from_file_urls =
  //    EnableRawFileAccessFromFileURLs() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.force_dark_mode_enabled =
      ForceDarkModeEnabled() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.dark_prefer_color_scheme_enabled =
      DarkSchemeEnabled() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.javascript_can_open_windows_automatically =
      IsCreateWindowsByJavaScriptAllowed();
  browser_settings.text_size_percent = ZoomingForTextFactor();
  browser_settings.allow_running_insecure_content = STATE_ENABLED;
    //   RunningInsecureContentAllowed() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.strict_mixed_content_checking = STATE_DISABLED;
    //   UseStricMixedContentCheckingAllowed() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.allow_mixed_content_upgrades = STATE_ENABLED;
    //   MixedContentAutoupgradesAllowed() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.geolocation_enabled = GeolocationAllowed();
  browser_settings.supports_double_tap_zoom = ZoomingfunctionEnabled();
  browser_settings.supports_multi_touch_zoom = ZoomingfunctionEnabled();
  browser_settings.user_gesture_required = GetMediaPlayGestureAccess();
  browser_settings.pinch_smooth_mode = GetPinchSmoothMode();
  browser_settings.hide_horizontal_scrollbars =
      !IsHorizontalScrollBarAccess() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.hide_vertical_scrollbars =
      !IsVerticalScrollBarAccess() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.viewport_meta_enabled = true;
  browser_settings.contextmenu_customization_enabled = false;
  browser_settings.scrollbar_color = GetScrollBarColor();
  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  if (command_line->HasSwitch(::switches::kForBrowser)) {
    browser_settings.contextmenu_customization_enabled = true;
  }
  browser_settings.background_color = GetBackgroundColor();
}

void NWebPreferenceDelegate::SetBrowserSettingsToNetHelpers() {
  net_service::NetHelpers::allow_file_access = EnableRawFileAccess();
  net_service::NetHelpers::allow_content_access = EnableContentAccess();
  net_service::NetHelpers::is_network_blocked = IsNetworkBlocked();
  net_service::NetHelpers::cache_mode = ConvertCacheMode(CacheMode());
  return;
}

void NWebPreferenceDelegate::PutMultiWindowAccess(bool flag) {
  multiWindow_access_ = flag;
}

void NWebPreferenceDelegate::PutHorizontalScrollBarAccess(bool flag) {
  horizontal_scrollBar_access_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutVerticalScrollBarAccess(bool flag) {
  vertical_scrollBar_access_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutEnableContentAccess(bool flag) {
  content_access_ = flag;
  SetBrowserSettingsToNetHelpers();
}

void NWebPreferenceDelegate::PutEnableRawFileAccess(bool flag) {
  raw_file_access_ = flag;
  if (!browser_.get()) {
    LOG(ERROR) << "browser is null";
    return;
  }

  browser_->GetHost()->SetFileAccess(flag);
}

void NWebPreferenceDelegate::PutEnableRawFileAccessFromFileURLs(bool flag) {
  raw_file_access_from_file_urls_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutEnableUniversalAccessFromFileURLs(bool flag) {
  universal_access_from_file_urls_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutLoadImageFromNetworkDisabled(bool flag) {
  images_enabled_ = !flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutCursiveFontFamilyName(std::string font) {
  cursive_font_family_name_ = font;
  WebPreferencesChanged();
}
void NWebPreferenceDelegate::PutDatabaseAllowed(bool flag) {
  databases_enabled_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutDefaultFixedFontSize(int size) {
  default_fixed_font_size_ = std::clamp(size, fontMinSize, fontMaxSize);
  WebPreferencesChanged();
}
void NWebPreferenceDelegate::PutDefaultFontSize(int size) {
  default_font_size_ = std::clamp(size, fontMinSize, fontMaxSize);
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutDefaultTextEncodingFormat(
    std::string encoding) {
  default_encoding_ = encoding;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutDomStorageEnabled(bool flag) {
  dom_storage_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutFantasyFontFamilyName(std::string font) {
  fantasy_font_family_name_ = font;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutFixedFontFamilyName(std::string font) {
  fixed_font_family_name_ = font;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutForceDarkModeEnabled(int forceDark) {
  force_dark_mode_enabled_ = forceDark;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutDarkSchemeEnabled(int darkScheme) {
  dark_prefer_color_scheme_enabled_ = darkScheme;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutIsCreateWindowsByJavaScriptAllowed(bool flag) {
  create_windows_by_javascript_allowed_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutJavaScriptEnabled(bool flag) {
  javascript_allowed_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutImageLoadingAllowed(bool flag) {
  image_loading_allowed_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutFontSizeLowerLimit(int size) {
  font_size_lower_limit_ = std::clamp(size, fontMinSize, fontMaxSize);
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutLogicalFontSizeLowerLimit(int size) {
  logical_font_size_lower_limit_ = std::clamp(size, fontMinSize, fontMaxSize);
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutLoadWithOverviewMode(bool flag) {
  overload_mode_enabled_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutSansSerifFontFamilyName(std::string font) {
  sans_serif_font_family_name_ = font;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutSerifFontFamilyName(std::string font) {
  serif_font_family_name_ = font;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutStandardFontFamilyName(std::string font) {
  standard_font_family_name_ = font;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutUserAgent(std::string ua) {
  std::string old_user_agent = user_agent_;
  if (ua.empty() || ua.length() == 0) {
    user_agent_ = DefaultUserAgent();
  } else {
    user_agent_ = ua;
  }
  if (!browser_) {
    return;
  }
  if (old_user_agent != user_agent_) {
    browser_->GetHost()->PutUserAgent(ua);
  }
}

void NWebPreferenceDelegate::PutZoomingForTextFactor(int textZoom) {
  if (zooming_for_text_factor_ != textZoom) {
    zooming_for_text_factor_ = textZoom;
    WebPreferencesChanged();
  }
}

void NWebPreferenceDelegate::PutGeolocationAllowed(bool flag) {
  geolocation_allowed_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutAccessModeForSecureOriginLoadFromInsecure(
    AccessMode mode) {
  if (access_mode_ != mode) {
    access_mode_ = mode;
    WebPreferencesChanged();
  }
}

void NWebPreferenceDelegate::PutZoomingFunctionEnabled(bool flag) {
  if (zooming_function_enabled_ != flag) {
    zooming_function_enabled_ = flag;
    WebPreferencesChanged();
  }
}

void NWebPreferenceDelegate::PutBlockNetwork(bool flag) {
  if (!flag && !has_internet_permission_) {
    LOG(ERROR) << "Put network-blocked false failed, because app missing "
                  "INTERNET permission";
  }
  is_network_blocked_ = flag;
  if (!browser_.get()) {
    LOG(ERROR) << "browser is null";
    return;
  }

  browser_->GetHost()->SetBlockNetwork(flag);
}

void NWebPreferenceDelegate::PutCacheMode(CacheModeFlag flag) {
  cache_mode_flag_ = flag;
  if (!browser_.get()) {
    LOG(ERROR) << "browser is null";
    return;
  }

  browser_->GetHost()->SetCacheMode(ConvertCacheMode(flag));
}

void NWebPreferenceDelegate::PutWebDebuggingAccess(bool flag) {
  if (!browser_.get()) {
    LOG(ERROR) << "put web debugging access failed, browser is null";
    return;
  }

  browser_->GetHost()->SetWebDebuggingAccess(flag);
}

void NWebPreferenceDelegate::PutMediaPlayGestureAccess(bool flag) {
  is_need_gesture_access_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutPinchSmoothMode(bool flag) {
  pinch_smooth_mode_ = flag;
  LOG(INFO) << "Put Pinch Smooth Mode:" << pinch_smooth_mode_;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutScrollBarColor(uint32_t colorValue) {
  scrollbar_color_ = colorValue;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutOverscrollMode(int mode) {
  overscroll_mode_ = mode;
  if (!browser_) {
    LOG(ERROR) << "PutOverscrollMode failed, browser is null";
    return;
  }
  browser_->GetHost()->SetOverscrollMode(mode);
}

bool NWebPreferenceDelegate::EnableContentAccess() {
  return content_access_;
}
bool NWebPreferenceDelegate::EnableRawFileAccess() {
  return raw_file_access_;
}

bool NWebPreferenceDelegate::EnableRawFileAccessFromFileURLs() {
  return raw_file_access_from_file_urls_;
}

bool NWebPreferenceDelegate::EnableUniversalAccessFromFileURLs() {
  return universal_access_from_file_urls_;
}

bool NWebPreferenceDelegate::IsLoadImageFromNetworkDisabled() {
  return !images_enabled_;
}

std::string NWebPreferenceDelegate::CursiveFontFamilyName() {
  return cursive_font_family_name_;
}

bool NWebPreferenceDelegate::IsDataBaseEnabled() {
  return databases_enabled_;
}

int NWebPreferenceDelegate::DefaultFixedFontSize() {
  return default_fixed_font_size_;
}

int NWebPreferenceDelegate::DefaultFontSize() {
  return default_font_size_;
}

std::string NWebPreferenceDelegate::DefaultTextEncodingFormat() {
  return default_encoding_;
}

std::string NWebPreferenceDelegate::DefaultUserAgent() {
  if (!browser_) {
    LOG(ERROR) << "get DefaultUserAgent failed, browser is null";
    return "";
  }
  std::string ua = browser_->GetHost()->DefaultUserAgent();
  return ua;
}

bool NWebPreferenceDelegate::IsDomStorageEnabled() {
  return dom_storage_;
}

std::string NWebPreferenceDelegate::FantasyFontFamilyName() {
  return fantasy_font_family_name_;
}

std::string NWebPreferenceDelegate::FixedFontFamilyName() {
  return fixed_font_family_name_;
}

int NWebPreferenceDelegate::ForceDarkModeEnabled() {
  return force_dark_mode_enabled_;
}

int NWebPreferenceDelegate::DarkSchemeEnabled() {
  return dark_prefer_color_scheme_enabled_;
}

bool NWebPreferenceDelegate::IsCreateWindowsByJavaScriptAllowed() {
  return create_windows_by_javascript_allowed_;
}

bool NWebPreferenceDelegate::IsJavaScriptAllowed() {
  return javascript_allowed_;
}

bool NWebPreferenceDelegate::IsImageLoadingAllowed() {
  return image_loading_allowed_;
}

int NWebPreferenceDelegate::FontSizeLowerLimit() {
  return font_size_lower_limit_;
}

int NWebPreferenceDelegate::LogicalFontSizeLowerLimit() {
  return logical_font_size_lower_limit_;
}

bool NWebPreferenceDelegate::IsLoadWithOverviewMode() {
  return overload_mode_enabled_;
}

std::string NWebPreferenceDelegate::SansSerifFontFamilyName() {
  return sans_serif_font_family_name_;
}

std::string NWebPreferenceDelegate::SerifFontFamilyName() {
  return serif_font_family_name_;
}

std::string NWebPreferenceDelegate::StandardFontFamilyName() {
  return standard_font_family_name_;
}

std::string NWebPreferenceDelegate::UserAgent() {
  if (user_agent_.empty()) {
    user_agent_ = DefaultUserAgent();
  }
  return user_agent_;
}

int NWebPreferenceDelegate::ZoomingForTextFactor() {
  return zooming_for_text_factor_;
}

bool NWebPreferenceDelegate::GeolocationAllowed() {
  return geolocation_allowed_;
}

NWebPreference::AccessMode
NWebPreferenceDelegate::AccessModeForSecureOriginLoadFromInsecure() {
  return access_mode_;
}

bool NWebPreferenceDelegate::RunningInsecureContentAllowed() {
  return access_mode_ == AccessMode::ALWAYS_ALLOW;
}

bool NWebPreferenceDelegate::UseStricMixedContentCheckingAllowed() {
  return access_mode_ == AccessMode::NEVER_ALLOW;
}

const base::Feature webview_mixed_content_autoupgrades{
    "WebViewMixedContentAutoupgrades", base::FEATURE_DISABLED_BY_DEFAULT};

bool NWebPreferenceDelegate::MixedContentAutoupgradesAllowed() {
  if (base::FeatureList::IsEnabled(webview_mixed_content_autoupgrades)) {
    return access_mode_ == AccessMode::COMPATIBILITY_MODE;
  }
  return false;
}

bool NWebPreferenceDelegate::ZoomingfunctionEnabled() {
  return zooming_function_enabled_;
}

bool NWebPreferenceDelegate::IsNetworkBlocked() {
  return is_network_blocked_;
}

NWebPreference::CacheModeFlag NWebPreferenceDelegate::CacheMode() {
  return cache_mode_flag_;
}

void NWebPreferenceDelegate::PutHasInternetPermission(bool flag) {
  has_internet_permission_ = flag;
}

bool NWebPreferenceDelegate::IsWebDebuggingAccess() {
  if (!browser_.get()) {
    LOG(ERROR) << "get web debugggin access failed, browser is null";
    return false;
  }

  return browser_->GetHost()->GetWebDebuggingAccess();
}

bool NWebPreferenceDelegate::GetMediaPlayGestureAccess() {
  return is_need_gesture_access_;
}

bool NWebPreferenceDelegate::GetPinchSmoothMode() {
  return pinch_smooth_mode_;
}

bool NWebPreferenceDelegate::IsMultiWindowAccess() {
  return multiWindow_access_;
}

bool NWebPreferenceDelegate::IsHorizontalScrollBarAccess() {
  return horizontal_scrollBar_access_;
}

bool NWebPreferenceDelegate::IsVerticalScrollBarAccess() {
  return vertical_scrollBar_access_;
}

uint32_t NWebPreferenceDelegate::GetScrollBarColor() {
  return scrollbar_color_;
}

void NWebPreferenceDelegate::SetBackgroundColor(int32_t color) {
  background_color_ = color;
}

int32_t NWebPreferenceDelegate::GetBackgroundColor() const {
  return background_color_;
}

int NWebPreferenceDelegate::GetOverscrollMode() {
  return overscroll_mode_;
}

void NWebPreferenceDelegate::SetEnableBlankTargetPopupIntercept(bool enable) {
  enable_blank_target_popup_intercept_ = enable;
}

bool NWebPreferenceDelegate::IsBlankTargetPopupInterceptEnabled() {
  return enable_blank_target_popup_intercept_;
}
}  // namespace OHOS::NWeb
