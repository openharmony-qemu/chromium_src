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

#ifndef ARK_IMF_ADAPTER_CAPI_H_
#define ARK_IMF_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_imfcursor_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  double(ARK_WEB_CALLBACK *get_left)(
      struct _ark_imfcursor_info_adapter_t *self);

  double(ARK_WEB_CALLBACK *get_top)(struct _ark_imfcursor_info_adapter_t *self);

  double(ARK_WEB_CALLBACK *get_width)(
      struct _ark_imfcursor_info_adapter_t *self);

  double(ARK_WEB_CALLBACK *get_height)(
      struct _ark_imfcursor_info_adapter_t *self);
} ark_imfcursor_info_adapter_t;

typedef struct _ark_imfinput_attribute_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_input_pattern)(
      struct _ark_imfinput_attribute_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_enter_key_type)(
      struct _ark_imfinput_attribute_adapter_t *self);
} ark_imfinput_attribute_adapter_t;

typedef struct _ark_imfselection_range_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_start)(
      struct _ark_imfselection_range_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_end)(
      struct _ark_imfselection_range_adapter_t *self);
} ark_imfselection_range_adapter_t;

typedef struct _ark_imftext_config_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ark_imfinput_attribute_adapter_t *(ARK_WEB_CALLBACK *get_input_attribute)(
      struct _ark_imftext_config_adapter_t *self);

  ark_imfcursor_info_adapter_t *(ARK_WEB_CALLBACK *get_cursor_info)(
      struct _ark_imftext_config_adapter_t *self);

  ark_imfselection_range_adapter_t *(ARK_WEB_CALLBACK *get_selection_range)(
      struct _ark_imftext_config_adapter_t *self);

  uint32_t(ARK_WEB_CALLBACK *get_window_id)(
      struct _ark_imftext_config_adapter_t *self);
} ark_imftext_config_adapter_t;

typedef struct _ark_imfadapter_function_key_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_enter_key_type)(
      struct _ark_imfadapter_function_key_adapter_t *self);
} ark_imfadapter_function_key_adapter_t;

typedef struct _ark_imftext_listener_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *insert_text)(
      struct _ark_imftext_listener_adapter_t *self,
      const ArkWebU16String *text);

  void(ARK_WEB_CALLBACK *delete_forward)(
      struct _ark_imftext_listener_adapter_t *self, int32_t length);

  void(ARK_WEB_CALLBACK *delete_backward)(
      struct _ark_imftext_listener_adapter_t *self, int32_t length);

  void(ARK_WEB_CALLBACK *send_key_event_from_input_method)(
      struct _ark_imftext_listener_adapter_t *self);

  void(ARK_WEB_CALLBACK *send_keyboard_status)(
      struct _ark_imftext_listener_adapter_t *self,
      const int32_t *keyboardStatus);

  void(ARK_WEB_CALLBACK *send_function_key)(
      struct _ark_imftext_listener_adapter_t *self,
      ark_imfadapter_function_key_adapter_t *functionKey);

  void(ARK_WEB_CALLBACK *set_keyboard_status)(
      struct _ark_imftext_listener_adapter_t *self, bool status);

  void(ARK_WEB_CALLBACK *move_cursor)(
      struct _ark_imftext_listener_adapter_t *self, const uint32_t direction);

  void(ARK_WEB_CALLBACK *handle_set_selection)(
      struct _ark_imftext_listener_adapter_t *self, int32_t start, int32_t end);

  void(ARK_WEB_CALLBACK *handle_extend_action)(
      struct _ark_imftext_listener_adapter_t *self, int32_t action);

  void(ARK_WEB_CALLBACK *handle_select)(
      struct _ark_imftext_listener_adapter_t *self, int32_t keyCode,
      int32_t cursorMoveSkip);

  int32_t(ARK_WEB_CALLBACK *get_text_index_at_cursor)(
      struct _ark_imftext_listener_adapter_t *self);

  ArkWebU16String(ARK_WEB_CALLBACK *get_left_text_of_cursor)(
      struct _ark_imftext_listener_adapter_t *self, int32_t number);

  ArkWebU16String(ARK_WEB_CALLBACK *get_right_text_of_cursor)(
      struct _ark_imftext_listener_adapter_t *self, int32_t number);

  int32_t(ARK_WEB_CALLBACK *set_preview_text)(
      struct _ark_imftext_listener_adapter_t *self, const ArkWebU16String *text,
      int32_t start, int32_t end);

  void(ARK_WEB_CALLBACK *finish_text_preview)(
      struct _ark_imftext_listener_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_need_under_line)(
      struct _ark_imftext_listener_adapter_t *self, bool isNeedUnderline);
} ark_imftext_listener_adapter_t;

typedef struct _ark_imfadapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *attach1)(struct _ark_imfadapter_t *self,
                                  ark_imftext_listener_adapter_t *listener,
                                  bool isShowKeyboard);

  bool(ARK_WEB_CALLBACK *attach2)(struct _ark_imfadapter_t *self,
                                  ark_imftext_listener_adapter_t *listener,
                                  bool isShowKeyboard,
                                  ark_imftext_config_adapter_t *config);

  void(ARK_WEB_CALLBACK *show_current_input)(struct _ark_imfadapter_t *self,
                                             const int32_t *inputType);

  void(ARK_WEB_CALLBACK *hide_text_input)(struct _ark_imfadapter_t *self);

  void(ARK_WEB_CALLBACK *close)(struct _ark_imfadapter_t *self);

  void(ARK_WEB_CALLBACK *on_cursor_update)(
      struct _ark_imfadapter_t *self, ark_imfcursor_info_adapter_t *cursorInfo);

  void(ARK_WEB_CALLBACK *on_selection_change)(struct _ark_imfadapter_t *self,
                                              ArkWebU16String *text, int start,
                                              int end);
} ark_imfadapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_IMF_ADAPTER_CAPI_H_
