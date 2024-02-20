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

#ifndef ARK_WEB_NWEB_CREATE_INFO_CAPI_H_
#define ARK_WEB_NWEB_CREATE_INFO_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "ohos_nweb/capi/ark_web_engine_init_args_capi.h"
#include "ohos_nweb/capi/ark_web_output_frame_callback_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_nweb_create_info_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  uint32_t(ARK_WEB_CALLBACK *get_width)(
      struct _ark_web_nweb_create_info_t *self);

  uint32_t(ARK_WEB_CALLBACK *get_height)(
      struct _ark_web_nweb_create_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_incognito_mode)(
      struct _ark_web_nweb_create_info_t *self);

  void *(ARK_WEB_CALLBACK *get_producer_surface)(
      struct _ark_web_nweb_create_info_t *self);

  void *(ARK_WEB_CALLBACK *get_enhance_surface_info)(
      struct _ark_web_nweb_create_info_t *self);

  ark_web_engine_init_args_t *(ARK_WEB_CALLBACK *get_engine_init_args)(
      struct _ark_web_nweb_create_info_t *self);

  ark_web_output_frame_callback_t *(
      ARK_WEB_CALLBACK *get_output_frame_callback)(
      struct _ark_web_nweb_create_info_t *self);
} ark_web_nweb_create_info_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_NWEB_CREATE_INFO_CAPI_H_
