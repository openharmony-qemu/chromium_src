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

#ifndef ARK_AUDIO_CAPTURER_ADAPTER_CAPI_H_
#define ARK_AUDIO_CAPTURER_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_audio_capturer_options_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_sampling_rate)(
      struct _ark_audio_capturer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_encoding)(
      struct _ark_audio_capturer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_sample_format)(
      struct _ark_audio_capturer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_channels)(
      struct _ark_audio_capturer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_source_type)(
      struct _ark_audio_capturer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_capturer_flags)(
      struct _ark_audio_capturer_options_adapter_t *self);
} ark_audio_capturer_options_adapter_t;

typedef struct _ark_buffer_desc_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  uint8_t *(ARK_WEB_CALLBACK *get_buffer)(
      struct _ark_buffer_desc_adapter_t *self);

  size_t(ARK_WEB_CALLBACK *get_buf_length)(
      struct _ark_buffer_desc_adapter_t *self);

  size_t(ARK_WEB_CALLBACK *get_data_length)(
      struct _ark_buffer_desc_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_buffer)(struct _ark_buffer_desc_adapter_t *self,
                                     uint8_t *buffer);

  void(ARK_WEB_CALLBACK *set_buf_length)(
      struct _ark_buffer_desc_adapter_t *self, size_t bufLength);

  void(ARK_WEB_CALLBACK *set_data_length)(
      struct _ark_buffer_desc_adapter_t *self, size_t dataLength);
} ark_buffer_desc_adapter_t;

typedef struct _ark_audio_capturer_read_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_read_data)(
      struct _ark_audio_capturer_read_callback_adapter_t *self, size_t length);
} ark_audio_capturer_read_callback_adapter_t;

typedef struct _ark_audio_capturer_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *create)(
      struct _ark_audio_capturer_adapter_t *self,
      ark_audio_capturer_options_adapter_t *capturerOptions,
      ArkWebString *cachePath);

  bool(ARK_WEB_CALLBACK *start)(struct _ark_audio_capturer_adapter_t *self);

  bool(ARK_WEB_CALLBACK *stop)(struct _ark_audio_capturer_adapter_t *self);

  bool(ARK_WEB_CALLBACK *release2)(struct _ark_audio_capturer_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *set_capturer_read_callback)(
      struct _ark_audio_capturer_adapter_t *self,
      ark_audio_capturer_read_callback_adapter_t *callbck);

  int32_t(ARK_WEB_CALLBACK *get_buffer_desc)(
      struct _ark_audio_capturer_adapter_t *self,
      ark_buffer_desc_adapter_t *buffferDesc);

  int32_t(ARK_WEB_CALLBACK *enqueue)(struct _ark_audio_capturer_adapter_t *self,
                                     ark_buffer_desc_adapter_t *bufferDesc);

  int32_t(ARK_WEB_CALLBACK *get_frame_count)(
      struct _ark_audio_capturer_adapter_t *self, uint32_t *frameCount);

  int64_t(ARK_WEB_CALLBACK *get_audio_time)(
      struct _ark_audio_capturer_adapter_t *self);
} ark_audio_capturer_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_AUDIO_CAPTURER_ADAPTER_CAPI_H_
