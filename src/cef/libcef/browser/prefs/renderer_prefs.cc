// Copyright (c) 2010 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "cef/libcef/browser/prefs/renderer_prefs.h"

#include "base/command_line.h"
#include "cef/libcef/common/cef_switches.h"
#include "content/public/common/content_switches.h"
#include "third_party/blink/public/common/web_preferences/web_preferences.h"
#include "cef/ohos_cef_ext/libcef/browser/prefs/renderer_prefs_for_include1.cc"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace renderer_prefs {

void SetDefaultPrefs(blink::web_pref::WebPreferences& web) {
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  web.javascript_enabled =
      !command_line->HasSwitch(switches::kDisableJavascript);
  web.allow_scripts_to_close_windows =
      !command_line->HasSwitch(switches::kDisableJavascriptCloseWindows);
  web.javascript_can_access_clipboard =
      !command_line->HasSwitch(switches::kDisableJavascriptAccessClipboard);
  web.allow_universal_access_from_file_urls =
      command_line->HasSwitch(switches::kAllowUniversalAccessFromFileUrls);
  web.shrinks_standalone_images_to_fit =
      command_line->HasSwitch(switches::kImageShrinkStandaloneToFit);
  web.text_areas_are_resizable =
      !command_line->HasSwitch(switches::kDisableTextAreaResize);

#if BUILDFLAG(IS_ARKWEB)
  SetDefaultPrefsExt(web);
#endif
}

// Helper macro for setting a WebPreferences variable based on the value of a
// CefBrowserSettings variable.
#define SET_STATE(cef_var, web_var)   \
  if (cef_var == STATE_ENABLED)       \
    web_var = true;                   \
  else if (cef_var == STATE_DISABLED) \
    web_var = false;

}  // namespace renderer_prefs
#include "cef/ohos_cef_ext/libcef/browser/prefs/renderer_prefs_for_include2.cc"
namespace renderer_prefs {

void SetCefPrefs(const CefBrowserSettings& cef,
                 blink::web_pref::WebPreferences& web) {
  if (cef.standard_font_family.length > 0) {
    web.standard_font_family_map[blink::web_pref::kCommonScript] =
        CefString(&cef.standard_font_family);
  }
  if (cef.fixed_font_family.length > 0) {
    web.fixed_font_family_map[blink::web_pref::kCommonScript] =
        CefString(&cef.fixed_font_family);
  }
  if (cef.serif_font_family.length > 0) {
    web.serif_font_family_map[blink::web_pref::kCommonScript] =
        CefString(&cef.serif_font_family);
  }
  if (cef.sans_serif_font_family.length > 0) {
    web.sans_serif_font_family_map[blink::web_pref::kCommonScript] =
        CefString(&cef.sans_serif_font_family);
  }
  if (cef.cursive_font_family.length > 0) {
    web.cursive_font_family_map[blink::web_pref::kCommonScript] =
        CefString(&cef.cursive_font_family);
  }
  if (cef.fantasy_font_family.length > 0) {
    web.fantasy_font_family_map[blink::web_pref::kCommonScript] =
        CefString(&cef.fantasy_font_family);
  }
#if BUILDFLAG(IS_ARKWEB)
  SetCefPrefsExt(cef, web);
#endif

  if (cef.default_font_size > 0) {
    web.default_font_size = cef.default_font_size;
  }
  if (cef.default_fixed_font_size > 0) {
    web.default_fixed_font_size = cef.default_fixed_font_size;
  }
  if (cef.minimum_font_size > 0) {
    web.minimum_font_size = cef.minimum_font_size;
  }
  if (cef.minimum_logical_font_size > 0) {
    web.minimum_logical_font_size = cef.minimum_logical_font_size;
  }

  if (cef.default_encoding.length > 0) {
    web.default_encoding = CefString(&cef.default_encoding);
  }

  SET_STATE(cef.remote_fonts, web.remote_fonts_enabled);
  SET_STATE(cef.javascript, web.javascript_enabled);
  SET_STATE(cef.javascript_close_windows, web.allow_scripts_to_close_windows);
  SET_STATE(cef.javascript_access_clipboard,
            web.javascript_can_access_clipboard);
  SET_STATE(cef.javascript_dom_paste, web.dom_paste_enabled);
#if !BUILDFLAG(IS_ARKWEB)
  SET_STATE(cef.image_loading, web.loads_images_automatically);
#endif
  SET_STATE(cef.image_shrink_standalone_to_fit,
            web.shrinks_standalone_images_to_fit);
  SET_STATE(cef.text_area_resize, web.text_areas_are_resizable);
  SET_STATE(cef.tab_to_links, web.tabs_to_links);
  SET_STATE(cef.local_storage, web.local_storage_enabled);

  // Never explicitly enable GPU-related functions in this method because the
  // GPU blacklist is not being checked here.
  if (cef.webgl == STATE_DISABLED) {
    web.webgl1_enabled = false;
    web.webgl2_enabled = false;
  }

#if BUILDFLAG(IS_ARKWEB)
  SetCefPrefsSetStateExt(cef, web);
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  web.border_radius_top_left = cef.border_radius_top_left;
  web.border_radius_top_right = cef.border_radius_top_right;
  web.border_radius_bottom_left = cef.border_radius_bottom_left;
  web.border_radius_bottom_right = cef.border_radius_bottom_right;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_MENU)
  web.touch_handle_exist = cef.touch_handle_exist;
  web.viewport_scale = cef.viewport_scale;
#endif  // BUILDFLAG(ARKWEB_MENU)
#endif
}

}  // namespace renderer_prefs
