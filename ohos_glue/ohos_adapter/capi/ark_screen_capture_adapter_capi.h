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

#ifndef ARK_SCREEN_CAPTURE_ADAPTER_CAPI_H_
#define ARK_SCREEN_CAPTURE_ADAPTER_CAPI_H_
#pragma once

#include "ohos_adapter/capi/ark_graphic_adapter_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_audio_capture_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_audio_sample_rate)(
      struct _ark_audio_capture_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_audio_channels)(
      struct _ark_audio_capture_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_audio_source)(
      struct _ark_audio_capture_info_adapter_t *self);
} ark_audio_capture_info_adapter_t;

typedef struct _ark_audio_enc_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_audio_bitrate)(
      struct _ark_audio_enc_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_audio_codecformat)(
      struct _ark_audio_enc_info_adapter_t *self);
} ark_audio_enc_info_adapter_t;

typedef struct _ark_audio_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ark_audio_capture_info_adapter_t *(ARK_WEB_CALLBACK *get_mic_cap_info)(
      struct _ark_audio_info_adapter_t *self);

  ark_audio_capture_info_adapter_t *(ARK_WEB_CALLBACK *get_inner_cap_info)(
      struct _ark_audio_info_adapter_t *self);

  ark_audio_enc_info_adapter_t *(ARK_WEB_CALLBACK *get_audio_enc_info)(
      struct _ark_audio_info_adapter_t *self);
} ark_audio_info_adapter_t;

typedef struct _ark_video_capture_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  uint64_t(ARK_WEB_CALLBACK *get_display_id)(
      struct _ark_video_capture_info_adapter_t *self);

  ArkWebInt32List(ARK_WEB_CALLBACK *get_task_ids)(
      struct _ark_video_capture_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_video_frame_width)(
      struct _ark_video_capture_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_video_frame_height)(
      struct _ark_video_capture_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_video_source_type)(
      struct _ark_video_capture_info_adapter_t *self);
} ark_video_capture_info_adapter_t;

typedef struct _ark_video_enc_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_video_codec_format)(
      struct _ark_video_enc_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_video_bitrate)(
      struct _ark_video_enc_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_video_frame_rate)(
      struct _ark_video_enc_info_adapter_t *self);
} ark_video_enc_info_adapter_t;

typedef struct _ark_video_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ark_video_capture_info_adapter_t *(ARK_WEB_CALLBACK *get_video_cap_info)(
      struct _ark_video_info_adapter_t *self);

  ark_video_enc_info_adapter_t *(ARK_WEB_CALLBACK *get_video_enc_info)(
      struct _ark_video_info_adapter_t *self);
} ark_video_info_adapter_t;

typedef struct _ark_recorder_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ArkWebString(ARK_WEB_CALLBACK *get_url)(
      struct _ark_recorder_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_file_format)(
      struct _ark_recorder_info_adapter_t *self);
} ark_recorder_info_adapter_t;

typedef struct _ark_screen_capture_config_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_capture_mode)(
      struct _ark_screen_capture_config_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_data_type)(
      struct _ark_screen_capture_config_adapter_t *self);

  ark_audio_info_adapter_t *(ARK_WEB_CALLBACK *get_audio_info)(
      struct _ark_screen_capture_config_adapter_t *self);

  ark_video_info_adapter_t *(ARK_WEB_CALLBACK *get_video_info)(
      struct _ark_screen_capture_config_adapter_t *self);

  ark_recorder_info_adapter_t *(ARK_WEB_CALLBACK *get_recorder_info)(
      struct _ark_screen_capture_config_adapter_t *self);
} ark_screen_capture_config_adapter_t;

typedef struct _ark_screen_capture_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_error)(
      struct _ark_screen_capture_callback_adapter_t *self, int32_t errorCode);

  void(ARK_WEB_CALLBACK *on_audio_buffer_available)(
      struct _ark_screen_capture_callback_adapter_t *self, bool isReady,
      int32_t type);

  void(ARK_WEB_CALLBACK *on_video_buffer_available)(
      struct _ark_screen_capture_callback_adapter_t *self, bool isReady);
} ark_screen_capture_callback_adapter_t;

typedef struct _ark_screen_capture_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *init)(struct _ark_screen_capture_adapter_t *self,
                                  ark_screen_capture_config_adapter_t *config);

  int32_t(ARK_WEB_CALLBACK *set_microphone_enable)(
      struct _ark_screen_capture_adapter_t *self, bool enable);

  int32_t(ARK_WEB_CALLBACK *start_capture)(
      struct _ark_screen_capture_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *stop_capture)(
      struct _ark_screen_capture_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *set_capture_callback)(
      struct _ark_screen_capture_adapter_t *self,
      ark_screen_capture_callback_adapter_t *callback);

  ark_surface_buffer_adapter_t *(ARK_WEB_CALLBACK *acquire_video_buffer)(
      struct _ark_screen_capture_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *release_video_buffer)(
      struct _ark_screen_capture_adapter_t *self);
} ark_screen_capture_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_SCREEN_CAPTURE_ADAPTER_CAPI_H_
