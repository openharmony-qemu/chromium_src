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

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "cef/ohos_cef_ext/libcef/browser/net_service/cookie_manager_impl_ext.h"
#include "content/public/browser/browser_context.h"
#endif

namespace renderer_prefs {

#if BUILDFLAG(ARKWEB_COPY_OPTION)
void SetCopyOptionToWeb(const CefBrowserSettings& cef,
                        blink::web_pref::WebPreferences& web) {
  const uint32_t copy_option_2 = 2;
  const uint32_t copy_option_3 = 3;
  switch (cef.copy_option) {
    case 0:
      web.copy_option = blink::mojom::CopyOptionMode::NONE;
      break;
    case 1:
      web.copy_option = blink::mojom::CopyOptionMode::IN_APP;
      break;
    case copy_option_2:
      web.copy_option = blink::mojom::CopyOptionMode::LOCAL_DEVICE;
      break;
    case copy_option_3:
      web.copy_option = blink::mojom::CopyOptionMode::CROSS_DEVICE;
      break;
    default:
      web.copy_option = blink::mojom::CopyOptionMode::CROSS_DEVICE;
  }
}
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

void SetCefPrefsExt(const CefBrowserSettings& cef,
                    blink::web_pref::WebPreferences& web) {
#if BUILDFLAG(ARKWEB_PINCH_SMOOTH)
  web.pinch_smooth_mode = cef.pinch_smooth_mode;
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  web.supports_multiple_windows = cef.supports_multiple_windows;
#endif
#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  web.delay_for_background_tab_freezing = cef.delay_for_background_tab_freezing;
#endif
}

void SetCefPrefsSetStateExt(const CefBrowserSettings& cef,
                            blink::web_pref::WebPreferences& web) {
#if BUILDFLAG(IS_ARKWEB)
  /* ohos webview begin*/
  SET_STATE(cef.image_loading, web.images_enabled);
  SET_STATE(cef.force_dark_mode_enabled, web.force_dark_mode_enabled);
#if BUILDFLAG(ARKWEB_DARKMODE)
  if (cef.dark_prefer_color_scheme_enabled == STATE_ENABLED) {
    web.preferred_color_scheme = blink::mojom::PreferredColorScheme::kDark;
  } else {
    web.preferred_color_scheme = blink::mojom::PreferredColorScheme::kLight;
  }
#endif
  SET_STATE(cef.loads_images_automatically, web.loads_images_automatically);
  SET_STATE(cef.allow_running_insecure_content,
            web.allow_running_insecure_content);
  SET_STATE(cef.strict_mixed_content_checking,
            web.strict_mixed_content_checking);
  SET_STATE(cef.allow_mixed_content_upgrades, web.allow_mixed_content_upgrades);
  SET_STATE(cef.initialize_at_minimum_page_scale,
            web.initialize_at_minimum_page_scale);
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  SET_STATE(cef.hide_vertical_scrollbars, web.hide_vertical_scrollbars);
  SET_STATE(cef.hide_horizontal_scrollbars, web.hide_horizontal_scrollbars);
  web.scroll_enabled = cef.scroll_enabled;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  web.scrollbar_color = cef.scrollbar_color;
#endif  // ARKWEB_SCROLLBAR
  if (cef.viewport_meta_enabled.has_value()) {
    web.viewport_meta_enabled = cef.viewport_meta_enabled.value();
  }
  SET_STATE(cef.text_autosizing_enabled, web.text_autosizing_enabled);
  /* ohos webview end */
#endif  // BUILDFLAG(IS_ARKWEB)
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  SET_STATE(cef.native_embed_mode_enabled, web.native_embed_mode_enabled);
  SET_STATE(cef.intrinsic_size_enabled, web.intrinsic_size_enabled);
  SET_STATE(cef.css_display_change_enabled, web.css_display_change_enabled);
  web.embed_tag = CefString(&cef.embed_tag);
  web.embed_tag_type = CefString(&cef.embed_tag_type);
#endif
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  web.draw_mode = cef.draw_mode;
#endif
#if BUILDFLAG(ARKWEB_FIT_CONTENT)
  web.force_zero_layout_height = cef.force_zero_layout_height;
#endif
#if BUILDFLAG(ARKWEB_CSS_FONT)
  web.font_weight_scale = cef.font_weight_scale;
#endif
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  web.record_whole_document = cef.record_whole_document;
#endif
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  SET_STATE(cef.universal_access_from_file_urls,
            web.allow_universal_access_from_file_urls);
#endif  // BUILDFLAG(ARKWEB_NETWORK_LOAD)
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  web.usage_scenario = cef.usage_scenario;
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  web.contextmenu_customization_enabled = cef.contextmenu_customization_enabled;
#endif
#if BUILDFLAG(ARKWEB_MEDIA)
  if (!(base::ohos::IsPcDevice() || base::ohos::IsPcMode()) &&
      cef.viewport_meta_enabled.has_value()) {
    web.viewport_meta_enabled = cef.viewport_meta_enabled.value();
  }
  web.autoplay_policy =
      cef.user_gesture_required
          ? blink::mojom::AutoplayPolicy::kDocumentUserActivationRequired
          : blink::mojom::AutoplayPolicy::kNoUserGestureRequired;
#endif
#if BUILDFLAG(ARKWEB_COPY_OPTION)
  SetCopyOptionToWeb(cef, web);
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)
#if BUILDFLAG(ARKWEB_FOCUS)
  web.gesture_focus_mode = cef.gesture_focus_mode;
#endif
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  web.custom_video_player_enable = cef.custom_video_player_enable;
  web.custom_video_player_overlay = cef.custom_video_player_overlay;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  web.error_page_enabled = cef.error_page_enabled;
#endif
}

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
void SetJsDefaultContent(content::WebContents* web_contents,
                         blink::web_pref::WebPreferences& web) {
  const base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (web_contents && command_line && command_line->HasSwitch(switches::kEnableNwebExExceptionList)) {
    bool javascript_enabled = web.javascript_enabled;

    // Update content setting default value
    ContentSetting setting = javascript_enabled ? CONTENT_SETTING_ALLOW : CONTENT_SETTING_BLOCK;
    content::BrowserContext* browser_context = web_contents->GetBrowserContext();
    HostContentSettingsMap* host_content_settings_map =
      HostContentSettingsMapFactory::GetForProfile(browser_context);
    if (host_content_settings_map) {
      host_content_settings_map->SetDefaultContentSetting(ContentSettingsType::JAVASCRIPT, setting);
      LOG(INFO) << "JAVASCRIPT SetDefaultContentSetting, setting = " << int(setting);
    }

    // Update cookie manager host content settings map
    bool is_off_the_record = browser_context ? browser_context->IsOffTheRecord() : false;
    CefRefPtr<CefCookieManagerImplExt> cookie_manager =
      CefCookieManagerImplExt::GetInstance(is_off_the_record);
    if (cookie_manager) {
      cookie_manager->UpdateHostContentSettingsMap();
    }
  }
}
#endif

}  // namespace renderer_prefs
