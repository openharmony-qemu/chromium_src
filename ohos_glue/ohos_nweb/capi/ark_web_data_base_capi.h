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

#ifndef ARK_WEB_DATA_BASE_CAPI_H_
#define ARK_WEB_DATA_BASE_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_data_base_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *clear_all_permission)(
      struct _ark_web_data_base_t *self, int type, bool incognito);

  void(ARK_WEB_CALLBACK *get_http_auth_credentials)(
      struct _ark_web_data_base_t *self, const ArkWebString *host,
      const ArkWebString *realm, ArkWebString *user_name, char *password,
      uint32_t password_size);

  void(ARK_WEB_CALLBACK *save_http_auth_credentials)(
      struct _ark_web_data_base_t *self, const ArkWebString *host,
      const ArkWebString *realm, const ArkWebString *user_name,
      const char *password);

  bool(ARK_WEB_CALLBACK *exist_http_auth_credentials)(
      struct _ark_web_data_base_t *self);

  void(ARK_WEB_CALLBACK *delete_http_auth_credentials)(
      struct _ark_web_data_base_t *self);

  ArkWebStringVector(ARK_WEB_CALLBACK *get_origins_by_permission)(
      struct _ark_web_data_base_t *self, int type, bool incognito);

  bool(ARK_WEB_CALLBACK *get_permission_by_origin)(
      struct _ark_web_data_base_t *self, const ArkWebString *origin, int type,
      bool *result, bool incognito);

  int(ARK_WEB_CALLBACK *set_permission_by_origin)(
      struct _ark_web_data_base_t *self, const ArkWebString *origin, int type,
      bool result, bool incognito);

  bool(ARK_WEB_CALLBACK *exist_permission_by_origin)(
      struct _ark_web_data_base_t *self, const ArkWebString *origin, int type,
      bool incognito);

  int(ARK_WEB_CALLBACK *clear_permission_by_origin)(
      struct _ark_web_data_base_t *self, const ArkWebString *origin, int type,
      bool incognito);
} ark_web_data_base_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_DATA_BASE_CAPI_H_
