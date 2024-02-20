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

#ifndef ARK_WEB_SELECT_POPUP_MENU_PARAM_CAPI_H_
#define ARK_WEB_SELECT_POPUP_MENU_PARAM_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "ohos_nweb/include/ark_web_nweb_structs.h"
#include "ohos_nweb/include/ark_web_select_popup_menu_item_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_select_popup_menu_param_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ArkWebSelectPopupMenuItemVector(ARK_WEB_CALLBACK *get_menu_items)(
      struct _ark_web_select_popup_menu_param_t *self);

  int(ARK_WEB_CALLBACK *get_item_height)(
      struct _ark_web_select_popup_menu_param_t *self);

  int(ARK_WEB_CALLBACK *get_selected_item)(
      struct _ark_web_select_popup_menu_param_t *self);

  double(ARK_WEB_CALLBACK *get_item_font_size)(
      struct _ark_web_select_popup_menu_param_t *self);

  bool(ARK_WEB_CALLBACK *get_is_right_aligned)(
      struct _ark_web_select_popup_menu_param_t *self);

  ArkWebSelectMenuBound(ARK_WEB_CALLBACK *get_select_menu_bound)(
      struct _ark_web_select_popup_menu_param_t *self);

  bool(ARK_WEB_CALLBACK *get_is_allow_multiple_selection)(
      struct _ark_web_select_popup_menu_param_t *self);
} ark_web_select_popup_menu_param_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_SELECT_POPUP_MENU_PARAM_CAPI_H_
