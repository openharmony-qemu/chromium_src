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

#ifndef ARK_CERT_MANAGER_ADAPTER_CAPI_H_
#define ARK_CERT_MANAGER_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_cert_manager_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  uint32_t(ARK_WEB_CALLBACK *get_cert_max_size)(
      struct _ark_cert_manager_adapter_t *self);

  uint32_t(ARK_WEB_CALLBACK *get_app_cert_max_size)(
      struct _ark_cert_manager_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_sytem_root_cert_data)(
      struct _ark_cert_manager_adapter_t *self, uint32_t certCount,
      uint8_t *certData);

  uint32_t(ARK_WEB_CALLBACK *get_sytem_root_cert_sum)(
      struct _ark_cert_manager_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_user_root_cert_data)(
      struct _ark_cert_manager_adapter_t *self, uint32_t certCount,
      uint8_t *certData);

  uint32_t(ARK_WEB_CALLBACK *get_user_root_cert_sum)(
      struct _ark_cert_manager_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_app_cert)(
      struct _ark_cert_manager_adapter_t *self, uint8_t *uriData,
      uint8_t *certData, uint32_t *len);

  int32_t(ARK_WEB_CALLBACK *sign)(struct _ark_cert_manager_adapter_t *self,
                                  const uint8_t *uri, const uint8_t *certData,
                                  uint32_t certDataLen, uint8_t *signData,
                                  uint32_t signDataLen);

  int32_t(ARK_WEB_CALLBACK *get_cert_data_by_subject)(
      struct _ark_cert_manager_adapter_t *self, const char *subjectName,
      uint8_t *certData, int32_t certType);

  int(ARK_WEB_CALLBACK *verify_cert_from_net_ssl)(
      struct _ark_cert_manager_adapter_t *self, uint8_t *certData,
      uint32_t certSize);

  bool(ARK_WEB_CALLBACK *get_trust_anchors_for_host_name)(
      struct _ark_cert_manager_adapter_t *self, const ArkWebString *hostname,
      ArkWebStringVector *certs);

  bool(ARK_WEB_CALLBACK *get_pin_set_for_host_name)(
      struct _ark_cert_manager_adapter_t *self, const ArkWebString *hostname,
      ArkWebStringVector *pins);
} ark_cert_manager_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_CERT_MANAGER_ADAPTER_CAPI_H_
