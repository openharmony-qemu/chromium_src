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

#ifndef ARK_MEDIA_CODEC_DECODER_ADAPTER_CAPI_H_
#define ARK_MEDIA_CODEC_DECODER_ADAPTER_CAPI_H_
#pragma once

#include "ohos_adapter/capi/ark_media_codec_adapter_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_decoder_format_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_width)(
      struct _ark_decoder_format_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_height)(
      struct _ark_decoder_format_adapter_t *self);

  double(ARK_WEB_CALLBACK *get_frame_rate)(
      struct _ark_decoder_format_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_width)(struct _ark_decoder_format_adapter_t *self,
                                    int32_t width);

  void(ARK_WEB_CALLBACK *set_height)(struct _ark_decoder_format_adapter_t *self,
                                     int32_t height);

  void(ARK_WEB_CALLBACK *set_frame_rate)(
      struct _ark_decoder_format_adapter_t *self, double frameRate);
} ark_decoder_format_adapter_t;

typedef struct _ark_decoder_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_error)(struct _ark_decoder_callback_adapter_t *self,
                                   int32_t errorType, int32_t errorCode);

  void(ARK_WEB_CALLBACK *on_stream_changed)(
      struct _ark_decoder_callback_adapter_t *self, int32_t width,
      int32_t height, double frameRate);

  void(ARK_WEB_CALLBACK *on_need_input_data)(
      struct _ark_decoder_callback_adapter_t *self, uint32_t index,
      ark_ohos_buffer_adapter_t *buffer);

  void(ARK_WEB_CALLBACK *on_need_output_data)(
      struct _ark_decoder_callback_adapter_t *self, uint32_t index,
      ark_buffer_info_adapter_t *info, uint32_t flag);
} ark_decoder_callback_adapter_t;

typedef struct _ark_media_codec_decoder_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *create_video_decoder_by_mime)(
      struct _ark_media_codec_decoder_adapter_t *self,
      const ArkWebString *mimetype);

  int32_t(ARK_WEB_CALLBACK *create_video_decoder_by_name)(
      struct _ark_media_codec_decoder_adapter_t *self,
      const ArkWebString *name);

  int32_t(ARK_WEB_CALLBACK *configure_decoder)(
      struct _ark_media_codec_decoder_adapter_t *self,
      ark_decoder_format_adapter_t *format);

  int32_t(ARK_WEB_CALLBACK *set_parameter_decoder)(
      struct _ark_media_codec_decoder_adapter_t *self,
      ark_decoder_format_adapter_t *format);

  int32_t(ARK_WEB_CALLBACK *set_output_surface)(
      struct _ark_media_codec_decoder_adapter_t *self, void *window);

  int32_t(ARK_WEB_CALLBACK *prepare_decoder)(
      struct _ark_media_codec_decoder_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *start_decoder)(
      struct _ark_media_codec_decoder_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *stop_decoder)(
      struct _ark_media_codec_decoder_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *flush_decoder)(
      struct _ark_media_codec_decoder_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *reset_decoder)(
      struct _ark_media_codec_decoder_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *release_decoder)(
      struct _ark_media_codec_decoder_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *queue_input_buffer_dec)(
      struct _ark_media_codec_decoder_adapter_t *self, uint32_t index,
      int64_t presentationTimeUs, int32_t size, int32_t offset, uint32_t flag);

  int32_t(ARK_WEB_CALLBACK *get_output_format_dec)(
      struct _ark_media_codec_decoder_adapter_t *self,
      ark_decoder_format_adapter_t *format);

  int32_t(ARK_WEB_CALLBACK *release_output_buffer_dec)(
      struct _ark_media_codec_decoder_adapter_t *self, uint32_t index,
      bool isRender);

  int32_t(ARK_WEB_CALLBACK *set_callback_dec)(
      struct _ark_media_codec_decoder_adapter_t *self,
      ark_decoder_callback_adapter_t *callback);
} ark_media_codec_decoder_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_MEDIA_CODEC_DECODER_ADAPTER_CAPI_H_
