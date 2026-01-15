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
#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_WEB_PREFERENCES_WEB_PREFERENCES_MOJOM_TRAITS_H_
#error "must be in include form THIRD_PARTY_BLINK_PUBLIC_COMMON_WEB_PREFERENCES_WEB_PREFERENCES_MOJOM_TRAITS_H_"
#endif

#include "third_party/blink/public/common/web_preferences/web_preferences_mojom_traits.h"

namespace mojo {

static bool WebPreferenceMojomBoolExt(blink::mojom::WebPreferencesDataView data,
                                              blink::web_pref::WebPreferences* out) {
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  return !data.ReadEmbedTag(&out->embed_tag) ||
    !data.ReadEmbedTagType(&out->embed_tag_type);
#endif
}

static void WebPreferenceMojomExt(blink::mojom::WebPreferencesDataView data,
                                  blink::web_pref::WebPreferences* out) {
#if BUILDFLAG(ARKWEB_AI)
  out->image_analyzer_enabled = data.image_analyzer_enabled();
  out->arkweb_agent_enabled = data.arkweb_agent_enabled();
  out->agent_need_highlight = data.agent_need_highlight();
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  out->hide_vertical_scrollbars = data.hide_vertical_scrollbars();
  out->hide_horizontal_scrollbars = data.hide_horizontal_scrollbars();
  out->scroll_enabled = data.scroll_enabled();
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_ZOOM)
  out->text_zoom_factor = data.text_zoom_factor();
#endif
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  out->scrollbar_color = data.scrollbar_color();
#endif  // ARKWEB_SCROLLBAR
#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  out->cast_enabled = data.cast_enabled();
#endif  // BUILDFLAG(ARKWEB_MEDIA_CAST)
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  out->font_scale_factor = data.font_scale_factor();
  out->font_weight_adjustment = data.font_weight_adjustment();
  out->text_size_contrast_factor = data.text_size_contrast_factor();
  out->device_scale_adjustment = data.device_scale_adjustment();
  out->support_deprecated_target_density_dpi =
      data.support_deprecated_target_density_dpi();
  out->wide_viewport_quirk = data.wide_viewport_quirk();
  out->use_wide_viewport = data.use_wide_viewport();
  out->force_zero_layout_height = data.force_zero_layout_height();
  out->viewport_meta_merge_content_quirk =
      data.viewport_meta_merge_content_quirk();
  out->viewport_meta_non_user_scalable_quirk =
      data.viewport_meta_non_user_scalable_quirk();
  out->viewport_meta_zero_values_quirk = data.viewport_meta_zero_values_quirk();
  out->ignore_main_frame_overflow_hidden_quirk =
      data.ignore_main_frame_overflow_hidden_quirk();
  out->report_screen_size_in_physical_pixels_quirk =
      data.report_screen_size_in_physical_pixels_quirk();
  out->reuse_global_for_unowned_main_frame =
      data.reuse_global_for_unowned_main_frame();
  out->css_hex_alpha_color_enabled = data.css_hex_alpha_color_enabled();
  out->disable_accelerated_small_canvases =
      data.disable_accelerated_small_canvases();
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  out->native_embed_mode_enabled = data.native_embed_mode_enabled();
  out->intrinsic_size_enabled = data.intrinsic_size_enabled();
  out->css_display_change_enabled = data.css_display_change_enabled();
#endif
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  out->draw_mode = data.draw_mode();
#endif  // BUILDFLAG(ARKWEB_SYNC_RENDER)
#if BUILDFLAG(ARKWEB_CSS_FONT)
  out->font_weight_scale = data.font_weight_scale();
#endif
#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  out->delay_for_background_tab_freezing =
      data.delay_for_background_tab_freezing();
#endif  // BUILDFLAG(ARKWEB_ACTIVE_POLICY)
#if BUILDFLAG(ARKWEB_PINCH_SMOOTH)
  out->pinch_smooth_mode = data.pinch_smooth_mode();
#endif
#if BUILDFLAG(ARKWEB_COPY_OPTION)
  out->copy_option = data.copy_option();
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)
#if BUILDFLAG(ARKWEB_FOCUS)
  out->gesture_focus_mode = data.gesture_focus_mode();
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  out->contextmenu_customization_enabled =
      data.contextmenu_customization_enabled();
#endif
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  out->custom_video_player_enable = data.custom_video_player_enable();
  out->custom_video_player_overlay = data.custom_video_player_overlay();
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  out->usage_scenario = data.usage_scenario();
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
  out->force_enable_zoom = data.force_enable_zoom();
#endif
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  out->video_assistant_enabled = data.video_assistant_enabled();
  out->custom_media_player_enabled = data.custom_media_player_enabled();
#endif // ARKWEB_VIDEO_ASSISTANT
#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  out->error_page_enabled = data.error_page_enabled();
#endif
#if BUILDFLAG(ARKWEB_BFCACHE)
  out->media_resume_from_bfcache_page = data.media_resume_from_bfcache_page();
#endif // BUILDFLAG(ARKWEB_BFCACHE)
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  out->clipboard_site_permission_enabled = data.clipboard_site_permission_enabled();
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  out->is_autofill_enabled = data.is_autofill_enabled();
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
}

} // namespace mojo