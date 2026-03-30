// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/public/common/web_preferences/web_preferences.h"

#include "arkweb/build/features/features.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/mojom/v8_cache_options.mojom.h"
#include "third_party/blink/public/mojom/webpreferences/web_preferences.mojom.h"
#include "ui/base/ui_base_switches_util.h"

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#endif

namespace blink {

namespace web_pref {

using blink::mojom::EffectiveConnectionType;

// "Zyyy" is the ISO 15924 script code for undetermined script aka Common.
const char kCommonScript[] = "Zyyy";
#if BUILDFLAG(ARKWEB_SAME_LAYER)
const char kObjectTag[] = "object";
const char kEmbedTag[] = "embed";
const char kNativeType[] = "native/";
#endif

WebPreferences::WebPreferences()
#if BUILDFLAG(ARKWEB_COPY_OPTION) || BUILDFLAG(ARKWEB_INPUT_EVENTS) || BUILDFLAG(ARKWEB_SAME_LAYER) || \
    BUILDFLAG(ARKWEB_CSS_FONT) || BUILDFLAG(ARKWEB_ZOOM) || BUILDFLAG(ARKWEB_EXT_FREE_COPY) || \
    BUILDFLAG(ARKWEB_SCROLLBAR)
    :
#if BUILDFLAG(ARKWEB_COPY_OPTION)
      copy_option(mojom::CopyOptionMode::CROSS_DEVICE),
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
      scroll_enabled(true),
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
      native_embed_mode_enabled(false),
      intrinsic_size_enabled(false),
      css_display_change_enabled(false),
      embed_tag(kEmbedTag),
      embed_tag_type(kNativeType),
#endif

#if BUILDFLAG(ARKWEB_CSS_FONT)
      font_weight_scale(1.0f),
#endif

#if BUILDFLAG(ARKWEB_ZOOM)
      text_zoom_factor(1.0f),
#endif
#if defined(ARKWEB_EXT_FREE_COPY)
      contextmenu_customization_enabled(false),
#endif  // (ARKWEB_EXT_FREE_COPY)
#if BUILDFLAG(ARKWEB_SCROLLBAR)
      scrollbar_color(0),
#endif  // ARKWEB_SCROLLBAR
#if BUILDFLAG(ARKWEB_MEDIA_CAST)
      cast_enabled(false)
#endif
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION) || BUILDFLAG(ARKWEB_INPUT_EVENTS) || BUILDFLAG(ARKWEB_SAME_LAYER) || \
          // BUILDFLAG(ARKWEB_CSS_FONT) || BUILDFLAG(ARKWEB_ZOOM) || BUILDFLAG(ARKWEB_EXT_FREE_COPY) || \
          // BUILDFLAG(ARKWEB_SCROLLBAR)
    {
  standard_font_family_map[web_pref::kCommonScript] = u"Times New Roman";
#if BUILDFLAG(IS_MAC)
  fixed_font_family_map[web_pref::kCommonScript] = u"Menlo";
#else
  fixed_font_family_map[web_pref::kCommonScript] = u"Courier New";
#endif
  serif_font_family_map[web_pref::kCommonScript] = u"Times New Roman";
  sans_serif_font_family_map[web_pref::kCommonScript] = u"Arial";
  cursive_font_family_map[web_pref::kCommonScript] = u"Script";
  fantasy_font_family_map[web_pref::kCommonScript] = u"Impact";
  // Latin Modern Math is an open source font available in LaTeX distributions,
  // and consequently other installable system packages. It provides the default
  // "Computer Modern" style that math people are used to and contains an
  // OpenType MATH table for math layout. It is thus a good default choice which
  // may be refined via resource files for the Chrome profile, in order to take
  // into account platform-specific availability of math fonts.
  math_font_family_map[web_pref::kCommonScript] = u"Latin Modern Math";
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  // To match platform convention like Windows, context menu should be shown
  // on mouse up instead of mouse down for pc device.
  context_menu_on_mouse_up = base::ohos::IsPcDevice();
#endif
}

WebPreferences::WebPreferences(const WebPreferences& other) = default;

WebPreferences::WebPreferences(WebPreferences&& other) = default;

WebPreferences::~WebPreferences() = default;

WebPreferences& WebPreferences::operator=(const WebPreferences& other) =
    default;

WebPreferences& WebPreferences::operator=(WebPreferences&& other) = default;

}  // namespace web_pref

}  // namespace blink
