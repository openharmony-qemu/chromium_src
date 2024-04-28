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

#ifndef ARK_AUDIO_RENDERER_ADAPTER_CAPI_H_
#define ARK_AUDIO_RENDERER_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_audio_renderer_options_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_sampling_rate)(
      struct _ark_audio_renderer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_encoding_type)(
      struct _ark_audio_renderer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_sample_format)(
      struct _ark_audio_renderer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_channel)(
      struct _ark_audio_renderer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_content_type)(
      struct _ark_audio_renderer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_stream_usage)(
      struct _ark_audio_renderer_options_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_render_flags)(
      struct _ark_audio_renderer_options_adapter_t *self);
} ark_audio_renderer_options_adapter_t;

typedef struct _ark_audio_renderer_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_suspend)(
      struct _ark_audio_renderer_callback_adapter_t *self);

  void(ARK_WEB_CALLBACK *on_resume)(
      struct _ark_audio_renderer_callback_adapter_t *self);
} ark_audio_renderer_callback_adapter_t;

typedef struct _ark_audio_renderer_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *create)(
      struct _ark_audio_renderer_adapter_t *self,
      ark_audio_renderer_options_adapter_t *options, ArkWebString *str);

  bool(ARK_WEB_CALLBACK *start)(struct _ark_audio_renderer_adapter_t *self);

  bool(ARK_WEB_CALLBACK *pause)(struct _ark_audio_renderer_adapter_t *self);

  bool(ARK_WEB_CALLBACK *stop)(struct _ark_audio_renderer_adapter_t *self);

  bool(ARK_WEB_CALLBACK *release2)(struct _ark_audio_renderer_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *write)(struct _ark_audio_renderer_adapter_t *self,
                                   uint8_t *buffer, size_t bufferSize);

  int32_t(ARK_WEB_CALLBACK *get_latency)(
      struct _ark_audio_renderer_adapter_t *self, uint64_t *latency);

  int32_t(ARK_WEB_CALLBACK *set_volume)(
      struct _ark_audio_renderer_adapter_t *self, float volume);

  float(ARK_WEB_CALLBACK *get_volume)(
      struct _ark_audio_renderer_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *set_audio_renderer_callback)(
      struct _ark_audio_renderer_adapter_t *self,
      ark_audio_renderer_callback_adapter_t *callback);

  void(ARK_WEB_CALLBACK *set_interrupt_mode)(
      struct _ark_audio_renderer_adapter_t *self, bool audioExclusive);

  bool(ARK_WEB_CALLBACK *is_renderer_state_running)(
      struct _ark_audio_renderer_adapter_t *self);
} ark_audio_renderer_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_AUDIO_RENDERER_ADAPTER_CAPI_H_
