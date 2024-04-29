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

#ifndef ARK_MEDIA_AVSESSION_ADAPTER_CAPI_H_
#define ARK_MEDIA_AVSESSION_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_media_avsession_metadata_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *set_title)(
      struct _ark_media_avsession_metadata_adapter_t *self,
      const ArkWebString *title);

  ArkWebString(ARK_WEB_CALLBACK *get_title)(
      struct _ark_media_avsession_metadata_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_artist)(
      struct _ark_media_avsession_metadata_adapter_t *self,
      const ArkWebString *artist);

  ArkWebString(ARK_WEB_CALLBACK *get_artist)(
      struct _ark_media_avsession_metadata_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_album)(
      struct _ark_media_avsession_metadata_adapter_t *self,
      const ArkWebString *album);

  ArkWebString(ARK_WEB_CALLBACK *get_album)(
      struct _ark_media_avsession_metadata_adapter_t *self);
} ark_media_avsession_metadata_adapter_t;

typedef struct _ark_media_avsession_position_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *set_duration)(
      struct _ark_media_avsession_position_adapter_t *self, int64_t duration);

  int64_t(ARK_WEB_CALLBACK *get_duration)(
      struct _ark_media_avsession_position_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_elapsed_time)(
      struct _ark_media_avsession_position_adapter_t *self,
      int64_t elapsedTime);

  int64_t(ARK_WEB_CALLBACK *get_elapsed_time)(
      struct _ark_media_avsession_position_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_update_time)(
      struct _ark_media_avsession_position_adapter_t *self, int64_t updateTime);

  int64_t(ARK_WEB_CALLBACK *get_update_time)(
      struct _ark_media_avsession_position_adapter_t *self);
} ark_media_avsession_position_adapter_t;

typedef struct _ark_media_avsession_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *play)(
      struct _ark_media_avsession_callback_adapter_t *self);

  void(ARK_WEB_CALLBACK *pause)(
      struct _ark_media_avsession_callback_adapter_t *self);

  void(ARK_WEB_CALLBACK *stop)(
      struct _ark_media_avsession_callback_adapter_t *self);

  void(ARK_WEB_CALLBACK *seek_to)(
      struct _ark_media_avsession_callback_adapter_t *self, int64_t millisTime);
} ark_media_avsession_callback_adapter_t;

typedef struct _ark_media_avsession_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *create_avsession)(
      struct _ark_media_avsession_adapter_t *self, int32_t type);

  void(ARK_WEB_CALLBACK *destroy_avsession)(
      struct _ark_media_avsession_adapter_t *self);

  bool(ARK_WEB_CALLBACK *regist_callback)(
      struct _ark_media_avsession_adapter_t *self,
      ark_media_avsession_callback_adapter_t *callbackAdapter);

  bool(ARK_WEB_CALLBACK *is_activated)(
      struct _ark_media_avsession_adapter_t *self);

  bool(ARK_WEB_CALLBACK *activate)(struct _ark_media_avsession_adapter_t *self);

  void(ARK_WEB_CALLBACK *de_activate)(
      struct _ark_media_avsession_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_metadata)(
      struct _ark_media_avsession_adapter_t *self,
      ark_media_avsession_metadata_adapter_t *metadata);

  void(ARK_WEB_CALLBACK *set_playback_state)(
      struct _ark_media_avsession_adapter_t *self, int32_t state);

  void(ARK_WEB_CALLBACK *set_playback_position)(
      struct _ark_media_avsession_adapter_t *self,
      ark_media_avsession_position_adapter_t *position);
} ark_media_avsession_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_MEDIA_AVSESSION_ADAPTER_CAPI_H_
