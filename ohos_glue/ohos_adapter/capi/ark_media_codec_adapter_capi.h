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

#ifndef ARK_MEDIA_CODEC_ADAPTER_CAPI_H_
#define ARK_MEDIA_CODEC_ADAPTER_CAPI_H_
#pragma once

#include "ohos_adapter/capi/ark_graphic_adapter_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_capability_data_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_max_width)(
      struct _ark_capability_data_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_max_height)(
      struct _ark_capability_data_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_maxframe_rate)(
      struct _ark_capability_data_adapter_t *self);
} ark_capability_data_adapter_t;

typedef struct _ark_codec_format_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_width)(
      struct _ark_codec_format_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_height)(
      struct _ark_codec_format_adapter_t *self);
} ark_codec_format_adapter_t;

typedef struct _ark_buffer_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int64_t(ARK_WEB_CALLBACK *get_presentation_time_us)(
      struct _ark_buffer_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_size)(struct _ark_buffer_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_offset)(
      struct _ark_buffer_info_adapter_t *self);
} ark_buffer_info_adapter_t;

typedef struct _ark_ohos_buffer_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  uint8_t *(ARK_WEB_CALLBACK *get_addr)(
      struct _ark_ohos_buffer_adapter_t *self);

  uint32_t(ARK_WEB_CALLBACK *get_buffer_size)(
      struct _ark_ohos_buffer_adapter_t *self);
} ark_ohos_buffer_adapter_t;

typedef struct _ark_codec_config_para_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_width)(
      struct _ark_codec_config_para_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_height)(
      struct _ark_codec_config_para_adapter_t *self);

  int64_t(ARK_WEB_CALLBACK *get_bit_rate)(
      struct _ark_codec_config_para_adapter_t *self);

  double(ARK_WEB_CALLBACK *get_frame_rate)(
      struct _ark_codec_config_para_adapter_t *self);
} ark_codec_config_para_adapter_t;

typedef struct _ark_codec_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_error)(struct _ark_codec_callback_adapter_t *self,
                                   int32_t errorType, int32_t errorCode);

  void(ARK_WEB_CALLBACK *on_stream_changed)(
      struct _ark_codec_callback_adapter_t *self,
      ark_codec_format_adapter_t *format);

  void(ARK_WEB_CALLBACK *on_need_input_data)(
      struct _ark_codec_callback_adapter_t *self, uint32_t index,
      ark_ohos_buffer_adapter_t *buffer);

  void(ARK_WEB_CALLBACK *on_need_output_data)(
      struct _ark_codec_callback_adapter_t *self, uint32_t index,
      ark_buffer_info_adapter_t *info, int32_t flag,
      ark_ohos_buffer_adapter_t *buffer);
} ark_codec_callback_adapter_t;

typedef struct _ark_media_codec_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *create_video_codec_by_mime)(
      struct _ark_media_codec_adapter_t *self, const ArkWebString mimetype);

  int32_t(ARK_WEB_CALLBACK *create_video_codec_by_name)(
      struct _ark_media_codec_adapter_t *self, const ArkWebString name);

  int32_t(ARK_WEB_CALLBACK *set_codec_callback)(
      struct _ark_media_codec_adapter_t *self,
      ark_codec_callback_adapter_t *callback);

  int32_t(ARK_WEB_CALLBACK *configure)(struct _ark_media_codec_adapter_t *self,
                                       ark_codec_config_para_adapter_t *config);

  int32_t(ARK_WEB_CALLBACK *prepare)(struct _ark_media_codec_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *start)(struct _ark_media_codec_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *stop)(struct _ark_media_codec_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *reset)(struct _ark_media_codec_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *release)(struct _ark_media_codec_adapter_t *self);

  ark_producer_surface_adapter_t *(ARK_WEB_CALLBACK *create_input_surface)(
      struct _ark_media_codec_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *release_output_buffer)(
      struct _ark_media_codec_adapter_t *self, uint32_t index, bool isRender);

  int32_t(ARK_WEB_CALLBACK *request_key_frame_soon)(
      struct _ark_media_codec_adapter_t *self);
} ark_media_codec_adapter_t;

typedef struct _ark_media_codec_list_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ark_capability_data_adapter_t *(ARK_WEB_CALLBACK *get_codec_capability)(
      struct _ark_media_codec_list_adapter_t *self, const ArkWebString mime,
      const bool isCodec);
} ark_media_codec_list_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_MEDIA_CODEC_ADAPTER_CAPI_H_
