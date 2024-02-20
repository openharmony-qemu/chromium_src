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

#ifndef ARK_WEB_CONTEXT_MENU_PARAMS_CAPI_H_
#define ARK_WEB_CONTEXT_MENU_PARAMS_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_context_menu_params_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_xcoord)(
      struct _ark_web_context_menu_params_t *self);

  int32_t(ARK_WEB_CALLBACK *get_ycoord)(
      struct _ark_web_context_menu_params_t *self);

  bool(ARK_WEB_CALLBACK *is_editable)(
      struct _ark_web_context_menu_params_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_link_url)(
      struct _ark_web_context_menu_params_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_page_url)(
      struct _ark_web_context_menu_params_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_source_url)(
      struct _ark_web_context_menu_params_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_title_text)(
      struct _ark_web_context_menu_params_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_selection_text)(
      struct _ark_web_context_menu_params_t *self);

  int(ARK_WEB_CALLBACK *get_media_type)(
      struct _ark_web_context_menu_params_t *self);

  int(ARK_WEB_CALLBACK *get_source_type)(
      struct _ark_web_context_menu_params_t *self);

  int(ARK_WEB_CALLBACK *get_input_field_type)(
      struct _ark_web_context_menu_params_t *self);

  bool(ARK_WEB_CALLBACK *has_image_contents)(
      struct _ark_web_context_menu_params_t *self);

  int32_t(ARK_WEB_CALLBACK *get_edit_state_flags)(
      struct _ark_web_context_menu_params_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_unfiltered_link_url)(
      struct _ark_web_context_menu_params_t *self);

  int32_t(ARK_WEB_CALLBACK *get_context_menu_type_flags)(
      struct _ark_web_context_menu_params_t *self);
} ark_web_context_menu_params_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_CONTEXT_MENU_PARAMS_CAPI_H_
