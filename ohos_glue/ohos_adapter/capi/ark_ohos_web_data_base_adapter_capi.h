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

#ifndef ARK_OHOS_WEB_DATA_BASE_ADAPTER_CAPI_H_
#define ARK_OHOS_WEB_DATA_BASE_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_ohos_web_data_base_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *exist_http_auth_credentials)(
      struct _ark_ohos_web_data_base_adapter_t *self);

  void(ARK_WEB_CALLBACK *delete_http_auth_credentials)(
      struct _ark_ohos_web_data_base_adapter_t *self);

  void(ARK_WEB_CALLBACK *save_http_auth_credentials)(
      struct _ark_ohos_web_data_base_adapter_t *self, const ArkWebString *host,
      const ArkWebString *realm, const ArkWebString *username,
      const char *password);

  void(ARK_WEB_CALLBACK *get_http_auth_credentials)(
      struct _ark_ohos_web_data_base_adapter_t *self, const ArkWebString *host,
      const ArkWebString *realm, ArkWebString *username, char *password,
      uint32_t passwordSize);
} ark_ohos_web_data_base_adapter_t;

typedef struct _ark_ohos_web_permission_data_base_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *exist_permission_by_origin)(
      struct _ark_ohos_web_permission_data_base_adapter_t *self,
      const ArkWebString *origin, const int32_t *key);

  bool(ARK_WEB_CALLBACK *get_permission_result_by_origin)(
      struct _ark_ohos_web_permission_data_base_adapter_t *self,
      const ArkWebString *origin, const int32_t *key, bool *result);

  void(ARK_WEB_CALLBACK *set_permission_by_origin)(
      struct _ark_ohos_web_permission_data_base_adapter_t *self,
      const ArkWebString *origin, const int32_t *key, bool result);

  void(ARK_WEB_CALLBACK *clear_permission_by_origin)(
      struct _ark_ohos_web_permission_data_base_adapter_t *self,
      const ArkWebString *origin, const int32_t *key);

  void(ARK_WEB_CALLBACK *clear_all_permission)(
      struct _ark_ohos_web_permission_data_base_adapter_t *self,
      const int32_t *key);

  void(ARK_WEB_CALLBACK *get_origins_by_permission)(
      struct _ark_ohos_web_permission_data_base_adapter_t *self,
      const int32_t *key, ArkWebStringVector *origins);
} ark_ohos_web_permission_data_base_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_OHOS_WEB_DATA_BASE_ADAPTER_CAPI_H_
