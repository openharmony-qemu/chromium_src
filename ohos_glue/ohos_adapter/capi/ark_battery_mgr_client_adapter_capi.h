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

#ifndef ARK_BATTERY_MGR_CLIENT_ADAPTER_CAPI_H_
#define ARK_BATTERY_MGR_CLIENT_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_battery_info_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  double(ARK_WEB_CALLBACK *get_level)(struct _ark_battery_info_t *self);

  bool(ARK_WEB_CALLBACK *is_charging)(struct _ark_battery_info_t *self);

  int(ARK_WEB_CALLBACK *dis_charging_time)(struct _ark_battery_info_t *self);

  int(ARK_WEB_CALLBACK *charging_time)(struct _ark_battery_info_t *self);
} ark_battery_info_t;

typedef struct _ark_battery_event_callback_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *battery_info_changed)(
      struct _ark_battery_event_callback_t *self, ark_battery_info_t *info);
} ark_battery_event_callback_t;

typedef struct _ark_battery_mgr_client_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *reg_battery_event)(
      struct _ark_battery_mgr_client_adapter_t *self,
      ark_battery_event_callback_t *eventCallback);

  bool(ARK_WEB_CALLBACK *start_listen)(
      struct _ark_battery_mgr_client_adapter_t *self);

  void(ARK_WEB_CALLBACK *stop_listen)(
      struct _ark_battery_mgr_client_adapter_t *self);

  ark_battery_info_t *(ARK_WEB_CALLBACK *request_battery_info)(
      struct _ark_battery_mgr_client_adapter_t *self);
} ark_battery_mgr_client_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_BATTERY_MGR_CLIENT_ADAPTER_CAPI_H_
