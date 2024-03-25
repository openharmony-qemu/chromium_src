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

#ifndef ARK_WEB_DATE_TIME_CHOOSER_CAPI_H_
#define ARK_WEB_DATE_TIME_CHOOSER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "ohos_nweb/include/ark_web_nweb_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_date_time_chooser_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int(ARK_WEB_CALLBACK *get_type)(struct _ark_web_date_time_chooser_t *self);

  double(ARK_WEB_CALLBACK *get_step)(struct _ark_web_date_time_chooser_t *self);

  ArkWebDateTime(ARK_WEB_CALLBACK *get_minimum)(
      struct _ark_web_date_time_chooser_t *self);

  ArkWebDateTime(ARK_WEB_CALLBACK *get_maximum)(
      struct _ark_web_date_time_chooser_t *self);

  ArkWebDateTime(ARK_WEB_CALLBACK *get_dialog_value)(
      struct _ark_web_date_time_chooser_t *self);

  bool(ARK_WEB_CALLBACK *get_has_selected)(
      struct _ark_web_date_time_chooser_t *self);

  size_t(ARK_WEB_CALLBACK *get_suggestion_index)(
      struct _ark_web_date_time_chooser_t *self);
} ark_web_date_time_chooser_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_DATE_TIME_CHOOSER_CAPI_H_
