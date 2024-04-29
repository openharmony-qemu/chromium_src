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

#ifndef ARK_DATE_TIME_FORMAT_ADAPTER_CAPI_H_
#define ARK_DATE_TIME_FORMAT_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_timezone_info_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ArkWebString(ARK_WEB_CALLBACK *get_tz_id)(
      struct _ark_web_timezone_info_t *self);
} ark_web_timezone_info_t;

typedef struct _ark_timezone_event_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *timezone_changed)(
      struct _ark_timezone_event_callback_adapter_t *self,
      ark_web_timezone_info_t *info);
} ark_timezone_event_callback_adapter_t;

typedef struct _ark_date_time_format_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *reg_timezone_event)(
      struct _ark_date_time_format_adapter_t *self,
      ark_timezone_event_callback_adapter_t *eventCallback);

  bool(ARK_WEB_CALLBACK *start_listen)(
      struct _ark_date_time_format_adapter_t *self);

  void(ARK_WEB_CALLBACK *stop_listen)(
      struct _ark_date_time_format_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_timezone)(
      struct _ark_date_time_format_adapter_t *self);
} ark_date_time_format_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_DATE_TIME_FORMAT_ADAPTER_CAPI_H_
