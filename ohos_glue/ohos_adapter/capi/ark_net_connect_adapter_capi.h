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

#ifndef ARK_NET_CONNECT_ADAPTER_CAPI_H_
#define ARK_NET_CONNECT_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_net_conn_callback_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *net_available)(
      struct _ark_net_conn_callback_t *self);

  int32_t(ARK_WEB_CALLBACK *net_capabilities_change)(
      struct _ark_net_conn_callback_t *self, const uint32_t *netConnectType,
      const uint32_t *netConnectSubtype);

  int32_t(ARK_WEB_CALLBACK *net_connection_properties_change)(
      struct _ark_net_conn_callback_t *self);

  int32_t(ARK_WEB_CALLBACK *net_unavailable)(
      struct _ark_net_conn_callback_t *self);
} ark_net_conn_callback_t;

typedef struct _ark_net_connect_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *register_net_conn_callback)(
      struct _ark_net_connect_adapter_t *self, ark_net_conn_callback_t *cb);

  int32_t(ARK_WEB_CALLBACK *unregister_net_conn_callback)(
      struct _ark_net_connect_adapter_t *self, int32_t id);

  int32_t(ARK_WEB_CALLBACK *get_default_net_connect)(
      struct _ark_net_connect_adapter_t *self, uint32_t *type,
      uint32_t *netConnectSubtype);
} ark_net_connect_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_NET_CONNECT_ADAPTER_CAPI_H_
