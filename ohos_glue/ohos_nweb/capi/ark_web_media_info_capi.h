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

#ifndef ARK_WEB_MEDIA_INFO_CAPI_H_
#define ARK_WEB_MEDIA_INFO_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "ohos_nweb/capi/ark_web_native_media_player_surface_info_capi.h"
#include "ohos_nweb/include/ark_web_media_source_info_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_media_info_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int(ARK_WEB_CALLBACK *get_preload)(struct _ark_web_media_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_muted)(struct _ark_web_media_info_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_embed_id)(
      struct _ark_web_media_info_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_poster_url)(
      struct _ark_web_media_info_t *self);

  int(ARK_WEB_CALLBACK *get_media_type)(struct _ark_web_media_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_controls_shown)(
      struct _ark_web_media_info_t *self);

  ArkWebStringVector(ARK_WEB_CALLBACK *get_controls)(
      struct _ark_web_media_info_t *self);

  ArkWebStringMap(ARK_WEB_CALLBACK *get_headers)(
      struct _ark_web_media_info_t *self);

  ArkWebStringMap(ARK_WEB_CALLBACK *get_attributes)(
      struct _ark_web_media_info_t *self);

  ArkWebMediaSourceInfoVector(ARK_WEB_CALLBACK *get_source_infos)(
      struct _ark_web_media_info_t *self);

  ark_web_native_media_player_surface_info_t *(
      ARK_WEB_CALLBACK *get_surface_info)(struct _ark_web_media_info_t *self);
} ark_web_media_info_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_MEDIA_INFO_CAPI_H_
