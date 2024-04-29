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

#ifndef ARK_POWER_MGR_CLIENT_ADAPTER_CAPI_H_
#define ARK_POWER_MGR_CLIENT_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_running_lock_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *is_used)(struct _ark_running_lock_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *lock)(struct _ark_running_lock_adapter_t *self,
                                  uint32_t timeOutMs);

  int32_t(ARK_WEB_CALLBACK *un_lock)(struct _ark_running_lock_adapter_t *self);
} ark_running_lock_adapter_t;

typedef struct _ark_power_mgr_client_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ark_running_lock_adapter_t *(ARK_WEB_CALLBACK *create_running_lock)(
      struct _ark_power_mgr_client_adapter_t *self, const ArkWebString *name,
      uint32_t type);
} ark_power_mgr_client_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_POWER_MGR_CLIENT_ADAPTER_CAPI_H_
