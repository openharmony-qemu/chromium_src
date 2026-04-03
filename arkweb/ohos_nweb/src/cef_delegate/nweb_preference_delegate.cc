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

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include <sstream>
#include <iostream>
#endif

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#if BUILDFLAG(ARKWEB_PERFORMANCE_NETWORK_TRACE)
#include "base/trace_event/trace_event.h"
#endif
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/command_line.h"
#include "base/feature_list.h"
#include "cef/include/cef_command_line.h"
#include "cef/include/internal/cef_string.h"
#include "cef/include/internal/cef_string_types.h"
#include "cef/include/internal/cef_string_wrappers.h"
#include "cef/include/internal/cef_types.h"
#include "cef/include/internal/cef_types_wrappers.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#include "content/public/common/content_switches.h"
#include "net/base/load_flags.h"
#include "ohos_nweb/src/cef_delegate/nweb_application.h"
#include "arkweb/chromium_ext/base/feature_list_utils.h"

#if BUILDFLAG(ARKWEB_CLIPBOARD)
#include "nweb_impl.h"
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

namespace OHOS::NWeb {

constexpr int fontMinSize = 1;
constexpr int fontMaxSize = 72;

enum class WebScrollType : int32_t {
  UNKNOWN = -1,
  EVENT = 0,
};

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
    LOG(DEBUG) << "update web preferences failed, browser is null";
    return;
  }

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  pref_hash_cached_ = false;
  pref_hash_ = 0;
#endif

  CefBrowserSettings browser_settings;
  ComputeBrowserSettings(browser_settings);
  if (!(browser_->GetHost())) {
    LOG(ERROR) << "host is null";
    return;
  }
  browser_->GetHost()->SetWebPreferences(browser_settings);
}

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void NWebPreferenceDelegate::SetBypassVsyncCondition(int32_t condition) {
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "host or browser_ is null";
    return;
  }
  LOG(INFO) << "NWebPreferenceDelegate::SetBypassVsyncCondition condition:"
            << condition;
  browser_->GetHost()->SetBypassVsyncCondition(condition);
}
#endif

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
  browser_settings.universal_access_from_file_urls =
      EnableUniversalAccessFromFileURLs() ? STATE_ENABLED : STATE_DISABLED;
  // browser_settings.file_access_from_file_urls =
  //    EnableRawFileAccessFromFileURLs() ? STATE_ENABLED : STATE_DISABLED;
#if BUILDFLAG(ARKWEB_DARKMODE)
  browser_settings.force_dark_mode_enabled =
      ForceDarkModeEnabled() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.dark_prefer_color_scheme_enabled =
      DarkSchemeEnabled() ? STATE_ENABLED : STATE_DISABLED;
#endif
#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  browser_settings.error_page_enabled = ErrorPageEnabled();
#endif
  browser_settings.javascript_can_open_windows_automatically =
      IsCreateWindowsByJavaScriptAllowed();
  browser_settings.text_size_percent = ZoomingForTextFactor();
  browser_settings.allow_running_insecure_content =
      RunningInsecureContentAllowed() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.strict_mixed_content_checking =
      UseStricMixedContentCheckingAllowed() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.allow_mixed_content_upgrades =
      MixedContentAutoupgradesAllowed() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.geolocation_enabled = GeolocationAllowed();
  browser_settings.supports_double_tap_zoom = ZoomingfunctionEnabled();
  browser_settings.supports_multi_touch_zoom = ZoomingfunctionEnabled();
#if BUILDFLAG(ARKWEB_ZOOM)
  browser_settings.zoom_control_access = IsZoomControlAccess();
#endif
  browser_settings.user_gesture_required = GetMediaPlayGestureAccess();
  browser_settings.pinch_smooth_mode = GetPinchSmoothMode();
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  browser_settings.border_radius_top_left = border_radius_top_left_;
  browser_settings.border_radius_top_right = border_radius_top_right_;
  browser_settings.border_radius_bottom_left = border_radius_bottom_left_;
  browser_settings.border_radius_bottom_right = border_radius_bottom_right_;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER
#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  browser_settings.is_autofill_enabled = is_autofill_enabled_;
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  browser_settings.is_drag_enabled = is_drag_enabled_;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
#if BUILDFLAG(ARKWEB_MENU)
  browser_settings.touch_handle_exist = touch_handle_exist_;
  browser_settings.viewport_scale = viewport_scale_;
#endif  // BUILDFLAG(ARKWEB_MENU)
#if BUILDFLAG(ARKWEB_AI)
  browser_settings.image_analyzer_enabled =
      GetImageAnalyzerEnabled() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.arkweb_agent_enabled =
      GetArkwebAgentEnabled() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.agent_need_highlight =
      GetAgentNeedHighlight() ? STATE_ENABLED : STATE_DISABLED;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  browser_settings.hide_horizontal_scrollbars =
      !IsHorizontalScrollBarAccess() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.hide_vertical_scrollbars =
      !IsVerticalScrollBarAccess() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.scroll_enabled = setting_scroll_enabled_;
  browser_settings.blur_enabled = GetBlurEnable();
  browser_settings.scrollbar_layout_policy =
      scrollbar_layout_policy_;
  browser_settings.is_system_rtl_enabled = is_system_rtl_enabled_;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  browser_settings.native_embed_mode_enabled =
      GetNativeEmbedMode() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.intrinsic_size_enabled =
      GetIntrinsicSizeEnable() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.css_display_change_enabled =
      GetCssDisplayChangeEnabled() ? STATE_ENABLED : STATE_DISABLED;
  str = CefString(embed_tag_);
  cef_string_set(str.c_str(), str.length(), &(browser_settings.embed_tag),
                 true);
  str = CefString(embed_tag_type_);
  cef_string_set(str.c_str(), str.length(), &(browser_settings.embed_tag_type),
                 true);
#endif  //BUILDFLAG(ARKWEB_SAME_LAYER)
#if BUILDFLAG(IS_OHOS)
  browser_settings.draw_mode = GetDrawMode();
  browser_settings.text_autosizing_enabled =
      IsTextAutosizingEnabled() ? STATE_ENABLED : STATE_DISABLED;
  browser_settings.force_zero_layout_height = IsFitContent();
#endif  // BUILDFLAG(IS_OHOS)
#if BUILDFLAG(ARKWEB_CSS_FONT)
  browser_settings.font_weight_scale = GetFontWeightScale();
#endif  // BUILDFLAG(ARKWEB_CSS_FONT)
#if BUILDFLAG(ARKWEB_COPY_OPTION)
  browser_settings.copy_option = static_cast<int>(GetCopyOptionMode());
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  browser_settings.clipboard_site_permission_enabled = OHOS::NWeb::NWebImpl::IsClipboardSitePermissionEnabled();
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  browser_settings.scrollbar_color = GetScrollBarColor();
#endif  // ARKWEB_SCROLLBAR
#if BUILDFLAG(ARKWEB_VIEWPORT)
  browser_settings.viewport_meta_enabled = GetViewportEnable();
#endif  // BUILDFLAG(ARKWEB_VIEWPORT)
#if BUILDFLAG(ARKWEB_FOCUS)
  browser_settings.gesture_focus_mode = GetGestureFocusMode();
#endif

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  browser_settings.background_color = GetBackgroundColor();
#endif  // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  browser_settings.contextmenu_customization_enabled = false;
  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  if (command_line->HasSwitch(::switches::kEnableNwebExFreeCopy)) {
    bool is_win =
        (UserAgent().find("Windows NT") >= 0) &&
        (UserAgent().find("Win64") >= 0 || UserAgent().find("WOW64") >= 0);
    browser_settings.contextmenu_customization_enabled = true;
  }
#endif  // BUILDFLAG(ARKWEB_EXT_FREE_COPY)
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  browser_settings.custom_video_player_enable =
      std::get<0>(native_video_player_config_);
  browser_settings.custom_video_player_overlay =
      std::get<1>(native_video_player_config_);
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  browser_settings.supports_multiple_windows = IsMultiWindowAccess();
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  browser_settings.record_whole_document = GetWholeWebPageDrawing();
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  browser_settings.usage_scenario = GetUsageScenario();
#endif

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  browser_settings.delay_for_background_tab_freezing =
      GetDelayDurationForBackgroundTabFreezing();
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  browser_settings.cast_enabled =
      GetCastEnabled() ? STATE_ENABLED : STATE_DISABLED;
#endif  // ARKWEB_MEDIA_CAST

#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
  browser_settings.enable_media_network_traffic_prompt =
      enable_media_network_traffic_prompt_;
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT
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
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutEnableContentAccess(bool flag) {
  content_access_ = flag;
  SetBrowserSettingsToNetHelpers();
}

void NWebPreferenceDelegate::PutEnableRawFileAccess(bool flag) {
  raw_file_access_ = flag;
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "browser or host is null";
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

void NWebPreferenceDelegate::PutCursiveFontFamilyName(const std::string& font) {
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
    const std::string& encoding) {
  default_encoding_ = encoding;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutDomStorageEnabled(bool flag) {
  dom_storage_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutFantasyFontFamilyName(const std::string& font) {
  fantasy_font_family_name_ = font;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutFixedFontFamilyName(const std::string& font) {
  fixed_font_family_name_ = font;
  WebPreferencesChanged();
}

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
void NWebPreferenceDelegate::SetBorderRadiusFromWeb(
    double borderRadiusTopLeft,
    double borderRadiusTopRight,
    double borderRadiusBottomLeft,
    double borderRadiusBottomRight)
{
  if ((border_radius_top_left_ != borderRadiusTopLeft) ||
      (border_radius_top_right_ != borderRadiusTopRight) ||
      (border_radius_bottom_left_ != borderRadiusBottomLeft) ||
      (border_radius_bottom_right_ != borderRadiusBottomRight)) {
    border_radius_top_left_ = borderRadiusTopLeft;
    border_radius_top_right_ = borderRadiusTopRight;
    border_radius_bottom_left_ = borderRadiusBottomLeft;
    border_radius_bottom_right_ = borderRadiusBottomRight;
    WebPreferencesChanged();
  }
}
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
void NWebPreferenceDelegate::SetEnableAutoFill(bool enable) {
  is_autofill_enabled_ = enable;
  WebPreferencesChanged();
}
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void NWebPreferenceDelegate::SetEnableDrag(bool enable) {
  is_drag_enabled_ = enable;
  WebPreferencesChanged();
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_MENU)
void NWebPreferenceDelegate::SetTouchHandleExistState(bool touchHandleExist) {
  touch_handle_exist_ = touchHandleExist;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::SetViewportScaleState(bool viewportScale) {
  viewport_scale_ = viewportScale;
  WebPreferencesChanged();
}
#endif  // BUILDFLAG(ARKWEB_MENU)

void NWebPreferenceDelegate::PutForceDarkModeEnabled(int forceDark) {
  int nweb_id = -1;
  if (browser_.get() && browser_->GetHost()) {
    nweb_id = browser_->GetHost()->GetNWebId();
  }
  LOG(INFO) << "NWebPreferenceDelegate::PutForceDarkModeEnabled nweb_id: " << nweb_id
            << ", forceDark: " << forceDark;
  force_dark_mode_enabled_ = forceDark;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutDarkSchemeEnabled(int darkScheme) {
  int nweb_id = -1;
  if (browser_.get() && browser_->GetHost()) {
    nweb_id = browser_->GetHost()->GetNWebId();
  }
  LOG(INFO) << "NWebPreferenceDelegate::PutDarkSchemeEnabled nweb_id: " << nweb_id
            << ", darkScheme: " << darkScheme;
  dark_prefer_color_scheme_enabled_ = darkScheme;
  WebPreferencesChanged();
}
// #endif

void NWebPreferenceDelegate::PutIsCreateWindowsByJavaScriptAllowed(bool flag) {
  create_windows_by_javascript_allowed_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutJavaScriptEnabled(bool flag) {
  LOG(INFO) << "Put JavaScript Enabled:" << flag;
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

void NWebPreferenceDelegate::PutSansSerifFontFamilyName(
    const std::string& font) {
  sans_serif_font_family_name_ = font;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutSerifFontFamilyName(const std::string& font) {
  serif_font_family_name_ = font;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutStandardFontFamilyName(
    const std::string& font) {
  standard_font_family_name_ = font;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutUserAgent(const std::string& ua) {
  std::string old_user_agent = user_agent_;
  has_set_user_agent_ = true;
  if (ua.empty() || ua.length() == 0) {
    user_agent_ = DefaultUserAgent();
  } else {
    user_agent_ = ua;
  }
  if (!browser_ || !(browser_->GetHost())) {
    return;
  }
  if (old_user_agent != user_agent_) {
    browser_->GetHost()->PutUserAgent(ua, has_set_user_agent_);
  }

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
      ::switches::kEnableNwebEx)) {
    browser_->GetHost()->CancelAllPrerendering();
  }
#endif
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

#if BUILDFLAG(ARKWEB_ZOOM)
void NWebPreferenceDelegate::PutZoomControlAccess(bool zoomControlAccess) {
  LOG(INFO) << "set zoomControlAccess:" << zoomControlAccess << " zoom_control_access_:" << zoom_control_access_;
  if (zoom_control_access_ != zoomControlAccess) {
    zoom_control_access_ = zoomControlAccess;
    WebPreferencesChanged();
  }
}
#endif

void NWebPreferenceDelegate::PutBlockNetwork(bool flag) {
  if (!flag && !has_internet_permission_) {
    LOG(ERROR) << "Put network-blocked false failed, because app missing "
                  "INTERNET permission";
  }
  is_network_blocked_ = flag;
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "browser or host is null";
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
#if BUILDFLAG(ARKWEB_PERFORMANCE_NETWORK_TRACE)
  TRACE_EVENT1("base", "NWebPreferenceDelegate::PutCacheMode", "flag", flag);
#endif
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  net_service::NetHelpers::no_use_cache = (CacheMode() == USE_NO_CACHE);
#endif
  if (!(browser_->GetHost())) {
    LOG(ERROR) << "host is null";
    return;
  }
  browser_->GetHost()->SetCacheMode(ConvertCacheMode(flag));
}

void NWebPreferenceDelegate::PutWebDebuggingAccess(bool flag) {
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "put web debugging access failed, browser or host is null";
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

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
bool NWebPreferenceDelegate::ErrorPageEnabled() {
  return error_page_enabled_;
}
#endif

int NWebPreferenceDelegate::DefaultFontSize() {
  return default_font_size_;
}

std::string NWebPreferenceDelegate::DefaultTextEncodingFormat() {
  return default_encoding_;
}

std::string NWebPreferenceDelegate::DefaultUserAgent() {
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "get DefaultUserAgent failed, browser or host is null";
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
// #endif

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
    "WebViewMixedContentAutoupgrades", base::FEATURE_DISABLED_BY_DEFAULT,
    base::internal::FeatureMacroHandshake::kSecret};

bool NWebPreferenceDelegate::MixedContentAutoupgradesAllowed() {
#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
  if (enable_mixed_content_auto_upgrades_) {
    return access_mode_ == AccessMode::COMPATIBILITY_MODE;
  }
#endif
  if (base::FeatureList::IsEnabled(webview_mixed_content_autoupgrades)) {
    return access_mode_ == AccessMode::COMPATIBILITY_MODE;
  }
  return false;
}

bool NWebPreferenceDelegate::ZoomingfunctionEnabled() {
  return zooming_function_enabled_;
}

#if BUILDFLAG(ARKWEB_ZOOM)
bool NWebPreferenceDelegate::IsZoomControlAccess() {
  return zoom_control_access_;
}
#endif

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
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "get web debugggin access failed, browser or host is null";
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

#if BUILDFLAG(ARKWEB_AI)
void NWebPreferenceDelegate::PutImageAnalyzerEnabled(bool enabled) {
  image_analyzer_enabled_ = enabled;
  WebPreferencesChanged();
}

bool NWebPreferenceDelegate::GetImageAnalyzerEnabled() {
  return image_analyzer_enabled_;
}

void NWebPreferenceDelegate::PutArkwebAgentEnabled(bool enabled) {
  arkweb_agent_enabled_ = enabled;
  WebPreferencesChanged();
}

bool NWebPreferenceDelegate::GetArkwebAgentEnabled() {
  return arkweb_agent_enabled_;
}

void NWebPreferenceDelegate::PutAgentNeedHighlight(bool enabled) {
  agent_need_highlight_ = enabled;
  WebPreferencesChanged();
}

bool NWebPreferenceDelegate::GetAgentNeedHighlight() {
  return agent_need_highlight_;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
bool NWebPreferenceDelegate::IsHorizontalScrollBarAccess() {
  return horizontal_scrollBar_access_;
}

bool NWebPreferenceDelegate::IsVerticalScrollBarAccess() {
  return vertical_scrollBar_access_;
}

void NWebPreferenceDelegate::PutHorizontalScrollBarAccess(bool flag) {
  horizontal_scrollBar_access_ = flag;
  WebPreferencesChanged();
}

void NWebPreferenceDelegate::PutVerticalScrollBarAccess(bool flag) {
  vertical_scrollBar_access_ = flag;
  WebPreferencesChanged();
}

int NWebPreferenceDelegate::GetOverscrollMode() {
  return overscroll_mode_;
}

void NWebPreferenceDelegate::PutOverscrollMode(int mode) {
  overscroll_mode_ = mode;
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "PutOverscrollMode failed, browser or host is null";
    return;
  }
  browser_->GetHost()->SetOverscrollMode(mode);
}

void NWebPreferenceDelegate::SetBlurOnKeyboardHideMode(int enable) {
  blur_enabled_ = enable;
  WebPreferencesChanged();
}

int NWebPreferenceDelegate::GetBlurEnable() {
  return blur_enabled_;
}

void NWebPreferenceDelegate::SetScrollable(bool enable) {
  LOG(INFO) << "SetScrollable enable:" << enable;
  scroll_enabled_ = enable;
  WebPreferencesChanged();
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "SetScrollable failed, browser or host is null";
    return;
  }
  browser_->GetHost()->SetScrollable(
      enable, static_cast<int32_t>(WebScrollType::UNKNOWN));
}

void NWebPreferenceDelegate::SetScrollable(bool enable, int32_t scrollType) {
  LOG(INFO) << "SetScrollable enable:" << enable
            << " scrollType:" << scrollType;
  scroll_enabled_ = enable;
  setting_scroll_enabled_ = enable;
  if (scrollType == static_cast<int32_t>(WebScrollType::UNKNOWN)) {
    WebPreferencesChanged();
  } else if (scrollType == static_cast<int32_t>(WebScrollType::EVENT)) {
    setting_scroll_enabled_ = true;
    WebPreferencesChanged();
  }
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "SetScrollable failed, browser or host is null";
    return;
  }
  browser_->GetHost()->SetScrollable(enable, scrollType);
}

bool NWebPreferenceDelegate::GetScrollable() {
  return scroll_enabled_;
}

void NWebPreferenceDelegate::SetScrollbarLayoutPolicy(int policy) {
  scrollbar_layout_policy_ = policy;
  WebPreferencesChanged();
}

int NWebPreferenceDelegate::GetScrollbarLayoutPolicy() const {
  return scrollbar_layout_policy_;
}

void NWebPreferenceDelegate::SetIsSystemRtlEnable(bool enable) {
  is_system_rtl_enabled_ = enable;
  WebPreferencesChanged();
}

bool NWebPreferenceDelegate::GetIsSystemRtlEnabled() const {
  return is_system_rtl_enabled_;
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_SAME_LAYER)
void NWebPreferenceDelegate::SetNativeEmbedMode(bool flag) {
  // Native Embed is not supported on pc device.
  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  auto isEnableEmbed = command_line->HasSwitch(::switches::kEnableEmbedMode);
  enable_embed_mode_ = flag;
  if (enable_embed_mode_) {
    zooming_function_enabled_ = false;
  }
  WebPreferencesChanged();
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "SetNativeEmbedMode failed, browser or host is null";
    return;
  }
  browser_->GetHost()->SetNativeEmbedMode(enable_embed_mode_);
}

bool NWebPreferenceDelegate::GetNativeEmbedMode() {
  return enable_embed_mode_;
}

void NWebPreferenceDelegate::SetIntrinsicSizeEnable(bool enable) {
  enable_intrinsic_size_ = enable;
  WebPreferencesChanged();
}

bool NWebPreferenceDelegate::GetIntrinsicSizeEnable() {
  return enable_intrinsic_size_;
}

void NWebPreferenceDelegate::SetCssDisplayChangeEnabled(bool enable) {
  enable_css_display_change_ = enable;
  WebPreferencesChanged();
}

bool NWebPreferenceDelegate::GetCssDisplayChangeEnabled() {
  return enable_css_display_change_;
}

void NWebPreferenceDelegate::RegisterNativeEmbedRule(const std::string& tag,
                                                     const std::string& type) {
  embed_tag_ = tag;
  embed_tag_type_ = type;
  WebPreferencesChanged();
}

bool NWebPreferenceDelegate::IsEnableCustomVideoPlayer() {
  return std::get<0>(native_video_player_config_);
}
#endif

#if BUILDFLAG(ARKWEB_VIEWPORT)
void NWebPreferenceDelegate::SetViewportEnable(bool enable) {
  int nweb_id = -1;
  if (browser_.get() && browser_->GetHost()) {
    nweb_id = browser_->GetHost()->GetNWebId();
  }
  LOG(INFO) << "NWebPreferenceDelegate::SetViewportEnable nweb_id: " << nweb_id
            << ", metaViewport: " << enable;
  viewport_enabled_ = enable;
  WebPreferencesChanged();
}

std::optional<bool> NWebPreferenceDelegate::GetViewportEnable() {
  return viewport_enabled_;
}
#endif  // BUILDFLAG(ARKWEB_VIEWPORT)

uint32_t NWebPreferenceDelegate::GetScrollBarColor() {
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  return scrollbar_color_;
#else
  return -1;
#endif  // ARKWEB_SCROLLBAR
}

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
void NWebPreferenceDelegate::SetBackgroundColor(int32_t color) {
  background_color_ = color;
  WebPreferencesChanged();
}

int32_t NWebPreferenceDelegate::GetBackgroundColor() const {
  return background_color_;
}
#endif  // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)

void NWebPreferenceDelegate::PutScrollBarColor(uint32_t colorValue) {
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  scrollbar_color_ = colorValue;
  WebPreferencesChanged();
#endif  // ARKWEB_SCROLLBAR
}

#if BUILDFLAG(IS_OHOS)
void NWebPreferenceDelegate::SetDrawMode(int mode) {
  draw_mode_ = mode;
}
int NWebPreferenceDelegate::GetDrawMode() const {
  return draw_mode_;
}

void NWebPreferenceDelegate::PutTextAutosizingEnabled(bool flag) {
  if (text_autosizing_enabled_ == flag) {
    return;
  }
  text_autosizing_enabled_ = flag;
  WebPreferencesChanged();
}

bool NWebPreferenceDelegate::IsTextAutosizingEnabled() const {
  return text_autosizing_enabled_;
}

void NWebPreferenceDelegate::SetFitContent(bool value) {
  fit_content_ = value;
#if BUILDFLAG(ARKWEB_FIT_CONTENT)
  WebPreferencesChanged();
#endif
}

bool NWebPreferenceDelegate::IsFitContent() const {
  return fit_content_;
}
#endif

#if BUILDFLAG(ARKWEB_CSS_FONT)
void NWebPreferenceDelegate::SetFontWeightScale(float size) {
  LOG(INFO) << "set fontWeightScale = " << size;
  font_weight_scale_ = size;
  WebPreferencesChanged();
}

float NWebPreferenceDelegate::GetFontWeightScale() const {
  return font_weight_scale_;
}
#endif

#if BUILDFLAG(ARKWEB_COPY_OPTION)
void NWebPreferenceDelegate::PutCopyOptionMode(
    NWebPreference::CopyOptionMode copyOption) {
  copy_option_ = copyOption;
  WebPreferencesChanged();
}

NWebPreference::CopyOptionMode NWebPreferenceDelegate::GetCopyOptionMode() {
  return copy_option_;
}
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_FOCUS)
void NWebPreferenceDelegate::SetGestureFocusMode(int32_t mode) {
  gesture_focus_mode_ = mode;
  WebPreferencesChanged();
}

int32_t NWebPreferenceDelegate::GetGestureFocusMode() const {
  return gesture_focus_mode_;
}
#endif

void NWebPreferenceDelegate::SetNativeVideoPlayerConfig(bool enable,
                                                        bool shouldOverlay) {
  if (native_video_player_config_ == std::make_tuple(enable, shouldOverlay)) {
    return;
  }
  native_video_player_config_ = {enable, shouldOverlay};
  WebPreferencesChanged();
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "SetNativeVideoPlayerConfig failed, browser or host is null";
    return;
  }
  LOG(INFO) << "NWebPreferenceDelegate::SetNativeVideoPlayerConfig enable:"
            << enable;
  browser_->GetHost()->SetEnableCustomVideoPlayer(enable);
}

#if BUILDFLAG(ARKWEB_SCROLLBAR)
void NWebPreferenceDelegate::PutOverlayScrollbarEnabled(bool enable) {
  bool overlay_scrollbar_enable = enable;
  if (IsFitContent()) {
    LOG(DEBUG) << "Fit content and set overlayscrollbar false";
    overlay_scrollbar_enable = false;
  }
  base::FeatureListUtils::SetScrollbarEnable(overlay_scrollbar_enable);
  WebPreferencesChanged();
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
void NWebPreferenceDelegate::PutAudioExclusive(bool audioExclusive) {
  audio_exclusive_ = audioExclusive;
}

bool NWebPreferenceDelegate::GetAudioExclusive() {
  return audio_exclusive_;
}

void NWebPreferenceDelegate::PutAudioResumeInterval(int32_t resumeInterval) {
  resume_interval_ = resumeInterval;
}

int32_t NWebPreferenceDelegate::GetAudioResumeInterval() {
  return resume_interval_;
}

void NWebPreferenceDelegate::PutAudioSessionType(int32_t audioSessionType) {
  audio_session_type_ = audioSessionType;
}
 
int32_t NWebPreferenceDelegate::GetAudioSessionType() {
  return audio_session_type_;
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_JSPROXY)
void NWebPreferenceDelegate::PutJavaScriptOnDocumentStart(
    const ScriptItems& scriptItems) {
  script_items_start_ = scriptItems;
}

void NWebPreferenceDelegate::PutJavaScriptOnDocumentStartByOrder(
    const ScriptItems& scriptItems,
    const ScriptRegexItems& scriptRegexItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  script_items_start_ = scriptItems;
  script_regex_items_start_ = scriptRegexItems;
  script_items_start_by_order_ = scriptItemsByOrder;
}

ScriptItems NWebPreferenceDelegate::GetJavaScriptOnDocumentStart() {
  return script_items_start_;
}

ScriptRegexItems NWebPreferenceDelegate::GetJavaScriptRegexItemsOnDocumentStart() {
  return script_regex_items_start_;
}

ScriptItemsByOrder
NWebPreferenceDelegate::GetJavaScriptOnDocumentStartByOrder() {
  return script_items_start_by_order_;
}

void NWebPreferenceDelegate::PutJavaScriptOnDocumentEnd(
    const ScriptItems& scriptItems) {
  script_items_end_ = scriptItems;
}

void NWebPreferenceDelegate::PutJavaScriptOnDocumentEndByOrder(
    const ScriptItems& scriptItems,
    const ScriptRegexItems& scriptRegexItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  script_items_end_ = scriptItems;
  script_regex_items_end_ = scriptRegexItems;
  script_items_end_by_order_ = scriptItemsByOrder;
}

ScriptItems NWebPreferenceDelegate::GetJavaScriptOnDocumentEnd() {
  return script_items_end_;
}

ScriptRegexItems NWebPreferenceDelegate::GetJavaScriptRegexItemsOnDocumentEnd() {
  return script_regex_items_end_;
}

ScriptItemsByOrder NWebPreferenceDelegate::GetJavaScriptOnDocumentEndByOrder() {
  return script_items_end_by_order_;
}

void NWebPreferenceDelegate::PutJavaScriptOnHeadReady(
    const ScriptItems& scriptItems) {
  script_items_head_ready_ = scriptItems;
}

void NWebPreferenceDelegate::PutJavaScriptOnHeadReadyByOrder(
    const ScriptItems& scriptItems,
    const ScriptRegexItems& scriptRegexItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  script_items_head_ready_ = scriptItems;
  script_regex_items_head_ready_ = scriptRegexItems;
  script_items_head_ready_by_order_ = scriptItemsByOrder;
}

ScriptItems NWebPreferenceDelegate::GetJavaScriptOnHeadReady() {
  return script_items_head_ready_;
}

ScriptRegexItems NWebPreferenceDelegate::GetJavaScriptRegexItemsOnHeadReady() {
  return script_regex_items_head_ready_;
}

ScriptItemsByOrder NWebPreferenceDelegate::GetJavaScriptOnHeadReadyByOrder() {
  return script_items_head_ready_by_order_;
}
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
bool NWebPreferenceDelegate::GetWholeWebPageDrawing() {
  return record_whole_document_;
}

void NWebPreferenceDelegate::SetWholePageDrawing() {
  record_whole_document_ = true;
}
#endif

#if BUILDFLAG(IS_OHOS)
std::string NWebPreferenceDelegate::GetSurfaceId() {
  // LOG(DEBUG)<<"[getSurfaceId] GetSurfaceId is "<<surface_id_;
  return surface_id_;
}

void NWebPreferenceDelegate::SetSurfaceId(const std::string& surfaceId) {
  // LOG(DEBUG)<<"[getSurfaceId] SetSurfaceId is "<<surfaceId;
  surface_id_ = surfaceId;
}

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
void NWebPreferenceDelegate::EnableMixedContentAutoUpgrades(bool enable) {
  enable_mixed_content_auto_upgrades_ = enable;
}

bool NWebPreferenceDelegate::IsMixedContentAutoUpgradesEnabled() {
  return enable_mixed_content_auto_upgrades_;
}
#endif

#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
void NWebPreferenceDelegate::SetUsageScenario(int32_t usage_scenario) {
  if (usage_scenario_ == usage_scenario) {
    return;
  }
  usage_scenario_ = usage_scenario;
  WebPreferencesChanged();
}

int32_t NWebPreferenceDelegate::GetUsageScenario() {
  return usage_scenario_;
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
void NWebPreferenceDelegate::EnableMediaNetworkTrafficPrompt(bool enable) {
  if (enable_media_network_traffic_prompt_ == enable) {
    return;
  }
  enable_media_network_traffic_prompt_ = enable;
  WebPreferencesChanged();
}
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT

#if BUILDFLAG(ARKWEB_BFCACHE)
void NWebPreferenceDelegate::PutBackForwardCacheOptions(int size,
                                                        int time_to_live) {
  size_ = size;
  time_to_live_ = time_to_live;
}

int NWebPreferenceDelegate::GetCacheSize() {
  return size_;
}

int NWebPreferenceDelegate::GetTimeToLive() {
  return time_to_live_;
}
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
void NWebPreferenceDelegate::SetDelayDurationForBackgroundTabFreezing(
    int64_t delay_for_background_tab_freezing) {
  if (delay_for_background_tab_freezing_ == delay_for_background_tab_freezing) {
    return;
  }
  delay_for_background_tab_freezing_ = delay_for_background_tab_freezing;
  WebPreferencesChanged();
}

int64_t NWebPreferenceDelegate::GetDelayDurationForBackgroundTabFreezing() {
  return delay_for_background_tab_freezing_;
}
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
CefRefPtr<CefWebMessageReceiver> NWebPreferenceDelegate::GetAutofillCallback() {
  return autofill_callback_;
}

void NWebPreferenceDelegate::SetAutofillCallback(
    CefRefPtr<CefWebMessageReceiver> callback) {
  autofill_callback_ = callback;
}

std::shared_ptr<NWebVaultPlainTextCallback> NWebPreferenceDelegate::GetVaultPlainTextCallback() {
  return vault_plain_text_callback_;
}

void NWebPreferenceDelegate::PutVaultPlainTextCallback(
    std::shared_ptr<NWebVaultPlainTextCallback> callback) {
  vault_plain_text_callback_ = callback;
}

#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
void NWebPreferenceDelegate::PutWebMediaAVSessionEnabled(bool enable) {
  if (!(browser_.get()) || !(browser_->GetHost())) {
    LOG(ERROR) << "PutWebMediaAVSessionEnabled failed, browser or host is null";
    return;
  }
  LOG(INFO) << "NWebPreferenceDelegate::PutWebMediaAVSessionEnabled enable:"
            << enable;
  browser_->GetHost()->PutWebMediaAVSessionEnabled(enable);

#if BUILDFLAG(ARKWEB_NWEB_EX)
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  if (currentProcess && !currentProcess->HasSwitch(::switches::kEnableMediaAvsession)) {
    cast_enabled_ = true;
  }
#endif // ARKWEB_NWEB_EX
  cast_enabled_ = cast_enabled_ && enable;
  LOG(INFO) << "NWebPreferenceDelegate::PutWebMediaAVSessionEnabled cast_enabled_:"
            << cast_enabled_;
}
#endif  // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  bool NWebPreferenceDelegate::GetCastEnabled() {
    return cast_enabled_;
  }
#endif  // ARKWEB_MEDIA_CAST

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
void NWebPreferenceDelegate::PutErrorPageEnabled(bool enable) {
  error_page_enabled_ = enable;
  WebPreferencesChanged();
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
int64_t NWebPreferenceDelegate::GetPreferenceHash()
{
  if (pref_hash_cached_) {
    LOG(DEBUG) << "blankless NWebPreferenceDelegate::GetPreferenceHash() using cache. hash = " << pref_hash_;
    return pref_hash_;
  }
  pref_hash_cached_ = true;

  std::ostringstream str;
  str << CursiveFontFamilyName() << ", "
    << FantasyFontFamilyName() << ", "
    << FixedFontFamilyName() << ", "
    << SansSerifFontFamilyName() << ", "
    << SerifFontFamilyName() << ", "
    << StandardFontFamilyName() << ", "
    << DefaultFixedFontSize() << ", "
    << DefaultFontSize() << ", "
    << ZoomingForTextFactor() << ", "
    << ForceDarkModeEnabled() << ", "
    << FontSizeLowerLimit() << ", "
    << LogicalFontSizeLowerLimit() << ", "
    << IsLoadWithOverviewMode() << ", "
    << DarkSchemeEnabled() << ", "
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
    << IsHorizontalScrollBarAccess() << ", "
    << IsVerticalScrollBarAccess() << ", "
    << GetOverscrollMode() << ", "
    << GetScrollable() << ", "
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
    << GetNativeEmbedMode() << ", "
#endif
    << GetRotationType();
  pref_hash_ = std::hash<std::string>{}(str.str());

  LOG(DEBUG) << "blankless NWebPreferenceDelegate::GetPreferenceHash() hash = " << pref_hash_
             << ", pref_str: " << str.str();

  return pref_hash_;
}

bool NWebPreferenceDelegate::SetRotationType(uint32_t type) {
  if (rotationType_ == type) {
    return false;
  }
  rotationType_ = type;
  pref_hash_cached_ = false;
  pref_hash_ = 0;
  return true;
}

uint32_t NWebPreferenceDelegate::GetRotationType() {
  return rotationType_;
}
#endif
}  // namespace OHOS::NWeb
