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

#ifndef ARK_WEB_WEB_STORAGE_CAPI_H_
#define ARK_WEB_WEB_STORAGE_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"
#include "ohos_nweb/capi/ark_web_long_value_callback_capi.h"
#include "ohos_nweb/capi/ark_web_web_storage_origin_vector_value_callback_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_web_storage_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ArkWebWebStorageOriginVector(ARK_WEB_CALLBACK *get_origins1)(
      struct _ark_web_web_storage_t *self);

  void(ARK_WEB_CALLBACK *get_origins2)(
      struct _ark_web_web_storage_t *self,
      ark_web_web_storage_origin_vector_value_callback_t *callback);

  long(ARK_WEB_CALLBACK *get_origin_quota1)(struct _ark_web_web_storage_t *self,
                                            const ArkWebString *origin);

  void(ARK_WEB_CALLBACK *get_origin_quota2)(
      struct _ark_web_web_storage_t *self, const ArkWebString *origin,
      ark_web_long_value_callback_t *callback);

  long(ARK_WEB_CALLBACK *get_origin_usage1)(struct _ark_web_web_storage_t *self,
                                            const ArkWebString *origin);

  void(ARK_WEB_CALLBACK *get_origin_usage2)(
      struct _ark_web_web_storage_t *self, const ArkWebString *origin,
      ark_web_long_value_callback_t *callback);

  int(ARK_WEB_CALLBACK *delete_origin)(struct _ark_web_web_storage_t *self,
                                       const ArkWebString *origin);

  void(ARK_WEB_CALLBACK *delete_all_data)(struct _ark_web_web_storage_t *self,
                                          bool incognito_mode);
} ark_web_web_storage_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_WEB_STORAGE_CAPI_H_
