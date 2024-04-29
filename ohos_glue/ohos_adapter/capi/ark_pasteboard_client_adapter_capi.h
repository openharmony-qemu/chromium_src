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

#ifndef ARK_PASTEBOARD_CLIENT_ADAPTER_CAPI_H_
#define ARK_PASTEBOARD_CLIENT_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"
#include "ohos_adapter/include/ark_paste_record_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_clip_board_image_data_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_color_type)(
      struct _ark_clip_board_image_data_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_alpha_type)(
      struct _ark_clip_board_image_data_adapter_t *self);

  uint32_t *(ARK_WEB_CALLBACK *get_data)(
      struct _ark_clip_board_image_data_adapter_t *self);

  size_t(ARK_WEB_CALLBACK *get_data_size)(
      struct _ark_clip_board_image_data_adapter_t *self);

  size_t(ARK_WEB_CALLBACK *get_row_bytes)(
      struct _ark_clip_board_image_data_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_width)(
      struct _ark_clip_board_image_data_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_height)(
      struct _ark_clip_board_image_data_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_color_type)(
      struct _ark_clip_board_image_data_adapter_t *self, int32_t color);

  void(ARK_WEB_CALLBACK *set_alpha_type)(
      struct _ark_clip_board_image_data_adapter_t *self, int32_t alpha);

  void(ARK_WEB_CALLBACK *set_data)(
      struct _ark_clip_board_image_data_adapter_t *self, uint32_t *data);

  void(ARK_WEB_CALLBACK *set_data_size)(
      struct _ark_clip_board_image_data_adapter_t *self, size_t size);

  void(ARK_WEB_CALLBACK *set_row_bytes)(
      struct _ark_clip_board_image_data_adapter_t *self, size_t rowBytes);

  void(ARK_WEB_CALLBACK *set_width)(
      struct _ark_clip_board_image_data_adapter_t *self, int32_t width);

  void(ARK_WEB_CALLBACK *set_height)(
      struct _ark_clip_board_image_data_adapter_t *self, int32_t height);
} ark_clip_board_image_data_adapter_t;

typedef struct _ark_paste_board_observer_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_paste_board_changed)(
      struct _ark_paste_board_observer_adapter_t *self);
} ark_paste_board_observer_adapter_t;

typedef struct _ark_paste_board_client_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *get_paste_data)(
      struct _ark_paste_board_client_adapter_t *self,
      ArkPasteRecordVector *data);

  void(ARK_WEB_CALLBACK *set_paste_data)(
      struct _ark_paste_board_client_adapter_t *self,
      ArkPasteRecordVector *data, int32_t copyOption);

  bool(ARK_WEB_CALLBACK *has_paste_data)(
      struct _ark_paste_board_client_adapter_t *self);

  void(ARK_WEB_CALLBACK *clear)(struct _ark_paste_board_client_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *open_remote_uri)(
      struct _ark_paste_board_client_adapter_t *self, const ArkWebString *path);

  bool(ARK_WEB_CALLBACK *is_local_paste)(
      struct _ark_paste_board_client_adapter_t *self);

  uint32_t(ARK_WEB_CALLBACK *get_token_id)(
      struct _ark_paste_board_client_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *add_pasteboard_changed_observer)(
      struct _ark_paste_board_client_adapter_t *self,
      ark_paste_board_observer_adapter_t *callback);

  void(ARK_WEB_CALLBACK *remove_pasteboard_changed_observer)(
      struct _ark_paste_board_client_adapter_t *self, int32_t callbackId);
} ark_paste_board_client_adapter_t;

typedef struct _ark_paste_data_record_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *set_html_text)(
      struct _ark_paste_data_record_adapter_t *self, void *htmlText);

  bool(ARK_WEB_CALLBACK *set_plain_text)(
      struct _ark_paste_data_record_adapter_t *self, void *plainText);

  bool(ARK_WEB_CALLBACK *set_img_data)(
      struct _ark_paste_data_record_adapter_t *self,
      ark_clip_board_image_data_adapter_t *imageData);

  ArkWebString(ARK_WEB_CALLBACK *get_mime_type)(
      struct _ark_paste_data_record_adapter_t *self);

  void(ARK_WEB_CALLBACK *get_html_text)(
      struct _ark_paste_data_record_adapter_t *self, void *data);

  void(ARK_WEB_CALLBACK *get_plain_text)(
      struct _ark_paste_data_record_adapter_t *self, void *data);

  bool(ARK_WEB_CALLBACK *get_img_data)(
      struct _ark_paste_data_record_adapter_t *self,
      ark_clip_board_image_data_adapter_t *imageData);

  bool(ARK_WEB_CALLBACK *set_uri)(struct _ark_paste_data_record_adapter_t *self,
                                  const ArkWebString *uriString);

  bool(ARK_WEB_CALLBACK *set_custom_data)(
      struct _ark_paste_data_record_adapter_t *self, void *data);

  void(ARK_WEB_CALLBACK *get_uri)(struct _ark_paste_data_record_adapter_t *self,
                                  void *data);

  void(ARK_WEB_CALLBACK *get_custom_data)(
      struct _ark_paste_data_record_adapter_t *self, void *data);
} ark_paste_data_record_adapter_t;

ARK_WEB_EXPORT ark_paste_data_record_adapter_t *
ark_paste_data_record_adapter_new_record1(const ArkWebString *mimeType);

ARK_WEB_EXPORT ark_paste_data_record_adapter_t *
ark_paste_data_record_adapter_new_record2(const ArkWebString *mimeType,
                                          void *htmlText, void *plainText);

typedef struct _ark_paste_data_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *add_html_record)(
      struct _ark_paste_data_adapter_t *self, const ArkWebString *html);

  void(ARK_WEB_CALLBACK *add_text_record)(
      struct _ark_paste_data_adapter_t *self, const ArkWebString *text);

  ArkWebStringVector(ARK_WEB_CALLBACK *get_mime_types)(
      struct _ark_paste_data_adapter_t *self);

  void(ARK_WEB_CALLBACK *get_primary_html)(
      struct _ark_paste_data_adapter_t *self, void *data);

  void(ARK_WEB_CALLBACK *get_primary_text)(
      struct _ark_paste_data_adapter_t *self, void *data);

  void(ARK_WEB_CALLBACK *get_primary_mime_type)(
      struct _ark_paste_data_adapter_t *self, void *data);

  ark_paste_data_record_adapter_t *(ARK_WEB_CALLBACK *get_record_at)(
      struct _ark_paste_data_adapter_t *self, size_t index);

  size_t(ARK_WEB_CALLBACK *get_record_count)(
      struct _ark_paste_data_adapter_t *self);

  ArkPasteRecordVector(ARK_WEB_CALLBACK *all_records)(
      struct _ark_paste_data_adapter_t *self);
} ark_paste_data_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_PASTEBOARD_CLIENT_ADAPTER_CAPI_H_
