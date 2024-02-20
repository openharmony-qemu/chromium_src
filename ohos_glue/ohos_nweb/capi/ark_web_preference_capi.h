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

#ifndef ARK_WEB_PREFERENCE_CAPI_H_
#define ARK_WEB_PREFERENCE_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_preference_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ArkWebString(ARK_WEB_CALLBACK *user_agent)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_user_agent)(struct _ark_web_preference_t *self,
                                         const ArkWebString *ua);

  ArkWebString(ARK_WEB_CALLBACK *default_user_agent)(
      struct _ark_web_preference_t *self);

  int(ARK_WEB_CALLBACK *cache_mode)(struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_cache_mode)(struct _ark_web_preference_t *self,
                                         int flag);

  bool(ARK_WEB_CALLBACK *is_network_blocked)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_block_network)(struct _ark_web_preference_t *self,
                                            bool flag);

  uint32_t(ARK_WEB_CALLBACK *get_scroll_bar_color)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_scroll_bar_color)(
      struct _ark_web_preference_t *self, uint32_t color_value);

  int(ARK_WEB_CALLBACK *get_overscroll_mode)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_overscroll_mode)(
      struct _ark_web_preference_t *self, int over_scroll_mode);

  int(ARK_WEB_CALLBACK *default_font_size)(struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_default_font_size)(
      struct _ark_web_preference_t *self, int size);

  bool(ARK_WEB_CALLBACK *get_pinch_smooth_mode)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_pinch_smooth_mode)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *is_data_base_enabled)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_database_allowed)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *is_multi_window_access)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_multi_window_access)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *is_java_script_allowed)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_java_script_enabled)(
      struct _ark_web_preference_t *self, bool flag);

  int(ARK_WEB_CALLBACK *dark_scheme_enabled)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_dark_scheme_enabled)(
      struct _ark_web_preference_t *self, int dark_scheme);

  bool(ARK_WEB_CALLBACK *is_dom_storage_enabled)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_dom_storage_enabled)(
      struct _ark_web_preference_t *self, bool flag);

  int(ARK_WEB_CALLBACK *font_size_lower_limit)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_font_size_lower_limit)(
      struct _ark_web_preference_t *self, int size);

  bool(ARK_WEB_CALLBACK *geo_location_allowed)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_geo_location_allowed)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *is_web_debugging_access)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_web_debugging_access)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *enable_content_access)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_enable_content_access)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *enable_raw_file_access)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_enable_raw_file_access)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *is_image_loading_allowed)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_image_loading_allowed)(
      struct _ark_web_preference_t *self, bool flag);

  ArkWebString(ARK_WEB_CALLBACK *fixed_font_family_name)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_fixed_font_family_name)(
      struct _ark_web_preference_t *self, const ArkWebString *font);

  ArkWebString(ARK_WEB_CALLBACK *serif_font_family_name)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_serif_font_family_name)(
      struct _ark_web_preference_t *self, const ArkWebString *font);

  int(ARK_WEB_CALLBACK *zooming_for_text_factor)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_zooming_for_text_factor)(
      struct _ark_web_preference_t *self, int text_zoom);

  int(ARK_WEB_CALLBACK *force_dark_mode_enabled)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_force_dark_mode_enabled)(
      struct _ark_web_preference_t *self, int force_dark);

  bool(ARK_WEB_CALLBACK *is_load_with_overview_mode)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_load_with_overview_mode)(
      struct _ark_web_preference_t *self, bool flag);

  int(ARK_WEB_CALLBACK *default_fixed_font_size)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_default_fixed_font_size)(
      struct _ark_web_preference_t *self, int size);

  ArkWebString(ARK_WEB_CALLBACK *cursive_font_family_name)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_cursive_font_family_name)(
      struct _ark_web_preference_t *self, const ArkWebString *font);

  ArkWebString(ARK_WEB_CALLBACK *fantasy_font_family_name)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_fantasy_font_family_name)(
      struct _ark_web_preference_t *self, const ArkWebString *font);

  bool(ARK_WEB_CALLBACK *zoomingfunction_enabled)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_zooming_function_enabled)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *get_media_play_gesture_access)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_media_play_gesture_access)(
      struct _ark_web_preference_t *self, bool flag);

  ArkWebString(ARK_WEB_CALLBACK *standard_font_family_name)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_standard_font_family_name)(
      struct _ark_web_preference_t *self, const ArkWebString *font);

  ArkWebString(ARK_WEB_CALLBACK *sans_serif_font_family_name)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_sans_serif_font_family_name)(
      struct _ark_web_preference_t *self, const ArkWebString *font);

  bool(ARK_WEB_CALLBACK *is_vertical_scroll_bar_access)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_vertical_scroll_bar_access)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *is_horizontal_scroll_bar_access)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_horizontal_scroll_bar_access)(
      struct _ark_web_preference_t *self, bool flag);

  int(ARK_WEB_CALLBACK *logical_font_size_lower_limit)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_logical_font_size_lower_limit)(
      struct _ark_web_preference_t *self, int size);

  ArkWebString(ARK_WEB_CALLBACK *default_text_encoding_format)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_default_text_encoding_format)(
      struct _ark_web_preference_t *self, const ArkWebString *encoding);

  bool(ARK_WEB_CALLBACK *is_load_image_from_network_disabled)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_load_image_from_network_disabled)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *enable_raw_file_access_from_file_urls)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_enable_raw_file_access_from_file_urls)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *enable_universal_access_from_file_urls)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_enable_universal_access_from_file_urls)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *is_create_windows_by_java_script_allowed)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_is_create_windows_by_java_script_allowed)(
      struct _ark_web_preference_t *self, bool flag);

  int(ARK_WEB_CALLBACK *access_mode_for_secure_origin_load_from_insecure)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_access_mode_for_secure_origin_load_from_insecure)(
      struct _ark_web_preference_t *self, int mode);

  int(ARK_WEB_CALLBACK *get_copy_option_mode)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *put_copy_option_mode)(
      struct _ark_web_preference_t *self, int copyOption);

  void(ARK_WEB_CALLBACK *set_native_embed_mode)(
      struct _ark_web_preference_t *self, bool flag);

  bool(ARK_WEB_CALLBACK *get_native_embed_mode)(
      struct _ark_web_preference_t *self);

  void(ARK_WEB_CALLBACK *set_scrollable)(struct _ark_web_preference_t *self,
                                         bool enable);

  bool(ARK_WEB_CALLBACK *get_scrollable)(struct _ark_web_preference_t *self);
} ark_web_preference_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_PREFERENCE_CAPI_H_
