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

static inline void setForInclude(const struct_type* src,
                                 struct_type* target,
                                 bool copy) {
#if BUILDFLAG(ARKWEB_COPY_OPTION)
  target->copy_option = src->copy_option;
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(IS_OHOS)
  /* ohos webview begin */
  target->force_dark_mode_enabled = src->force_dark_mode_enabled;
  target->javascript_can_open_windows_automatically =
      src->javascript_can_open_windows_automatically;
  target->loads_images_automatically = src->loads_images_automatically;
  target->text_size_percent = src->text_size_percent;
  target->allow_running_insecure_content = src->allow_running_insecure_content;
  target->strict_mixed_content_checking = src->strict_mixed_content_checking;
  target->allow_mixed_content_upgrades = src->allow_mixed_content_upgrades;
  target->geolocation_enabled = src->geolocation_enabled;
  target->supports_double_tap_zoom = src->supports_double_tap_zoom;
  target->supports_multi_touch_zoom = src->supports_multi_touch_zoom;
  target->initialize_at_minimum_page_scale =
      src->initialize_at_minimum_page_scale;
  target->viewport_meta_enabled = src->viewport_meta_enabled;
  target->user_gesture_required = src->user_gesture_required;
  target->pinch_smooth_mode = src->pinch_smooth_mode;
  target->hide_vertical_scrollbars = src->hide_vertical_scrollbars;
  target->hide_horizontal_scrollbars = src->hide_horizontal_scrollbars;
  target->contextmenu_customization_enabled =
      src->contextmenu_customization_enabled;
  target->scrollbar_color = src->scrollbar_color;
  target->is_safe_browsing_enable = src->is_safe_browsing_enable;
  target->scroll_enabled = src->scroll_enabled;
  target->draw_mode = src->draw_mode;
  target->force_zero_layout_height = src->force_zero_layout_height;
  /* ohos webview end */
#endif  // BUILDFLAG(IS_OHOS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  target->native_embed_mode_enabled = src->native_embed_mode_enabled;
  target->intrinsic_size_enabled = src->intrinsic_size_enabled;
  target->css_display_change_enabled = src->css_display_change_enabled;
  cef_string_set(src->embed_tag.str, src->embed_tag.length, &target->embed_tag,
                 copy);
  cef_string_set(src->embed_tag.str, src->embed_tag.length,
                 &target->embed_tag_type, copy);
#endif  // BUILDFLAG(ARKWEB_SAME_LAYER)

#if BUILDFLAG(ARKWEB_CSS_FONT)
  target->font_weight_scale = src->font_weight_scale;
#endif

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  target->incognito_mode = src->incognito_mode;
#endif
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  target->custom_video_player_enable = src->custom_video_player_enable;
  target->custom_video_player_overlay = src->custom_video_player_overlay;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  target->supports_multiple_windows = src->supports_multiple_windows;
#endif  // ARKWEB_MULTI_WINDOW

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  target->record_whole_document = src->record_whole_document;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  target->usage_scenario = src->usage_scenario;
#endif

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  target->delay_for_background_tab_freezing =
      src->delay_for_background_tab_freezing;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  target->universal_access_from_file_urls =
      src->universal_access_from_file_urls;
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  target->shared_render_process_token = src->shared_render_process_token;
#endif
#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  target->error_page_enabled = src->error_page_enabled;
#endif
}
