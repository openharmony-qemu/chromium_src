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

#ifndef ARK_LOCATION_ADAPTER_CAPI_H_
#define ARK_LOCATION_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_location_request_config_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *set_scenario)(
      struct _ark_location_request_config_t *self, int32_t scenario);

  void(ARK_WEB_CALLBACK *set_fix_number)(
      struct _ark_location_request_config_t *self, int32_t number);

  void(ARK_WEB_CALLBACK *set_max_accuracy)(
      struct _ark_location_request_config_t *self, int32_t maxAccuary);

  void(ARK_WEB_CALLBACK *set_distance_interval)(
      struct _ark_location_request_config_t *self, int32_t disInterval);

  void(ARK_WEB_CALLBACK *set_time_interval)(
      struct _ark_location_request_config_t *self, int32_t timeInterval);

  void(ARK_WEB_CALLBACK *set_priority)(
      struct _ark_location_request_config_t *self, int32_t priority);
} ark_location_request_config_t;

typedef struct _ark_location_info_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  double(ARK_WEB_CALLBACK *get_latitude)(struct _ark_location_info_t *self);

  double(ARK_WEB_CALLBACK *get_longitude)(struct _ark_location_info_t *self);

  double(ARK_WEB_CALLBACK *get_altitude)(struct _ark_location_info_t *self);

  float(ARK_WEB_CALLBACK *get_accuracy)(struct _ark_location_info_t *self);

  float(ARK_WEB_CALLBACK *get_speed)(struct _ark_location_info_t *self);

  double(ARK_WEB_CALLBACK *get_direction)(struct _ark_location_info_t *self);

  int64_t(ARK_WEB_CALLBACK *get_time_stamp)(struct _ark_location_info_t *self);

  int64_t(ARK_WEB_CALLBACK *get_time_since_boot)(
      struct _ark_location_info_t *self);

  ArkWebStringVector(ARK_WEB_CALLBACK *get_additions)(
      struct _ark_location_info_t *self);
} ark_location_info_t;

typedef struct _ark_location_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_location_report)(
      struct _ark_location_callback_adapter_t *self,
      ark_location_info_t *location);

  void(ARK_WEB_CALLBACK *on_locating_status_change)(
      struct _ark_location_callback_adapter_t *self, const int status);

  void(ARK_WEB_CALLBACK *on_error_report)(
      struct _ark_location_callback_adapter_t *self, const int errorCode);
} ark_location_callback_adapter_t;

typedef struct _ark_location_proxy_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *start_locating)(
      struct _ark_location_proxy_adapter_t *self,
      ark_location_request_config_t *requestConfig,
      ark_location_callback_adapter_t *callback);

  bool(ARK_WEB_CALLBACK *stop_locating)(
      struct _ark_location_proxy_adapter_t *self, int32_t callbackId);

  bool(ARK_WEB_CALLBACK *enable_ability)(
      struct _ark_location_proxy_adapter_t *self, bool isEnabled);

  bool(ARK_WEB_CALLBACK *is_location_enabled)(
      struct _ark_location_proxy_adapter_t *self);
} ark_location_proxy_adapter_t;

typedef struct _ark_location_instance_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ark_location_proxy_adapter_t *(
      ARK_WEB_CALLBACK *create_location_proxy_adapter)(
      struct _ark_location_instance_t *self);

  ark_location_request_config_t *(
      ARK_WEB_CALLBACK *create_location_request_config)(
      struct _ark_location_instance_t *self);
} ark_location_instance_t;

ARK_WEB_EXPORT ark_location_instance_t *
ark_location_instance_get_instance(void);

#ifdef __cplusplus
}
#endif

#endif // ARK_LOCATION_ADAPTER_CAPI_H_
