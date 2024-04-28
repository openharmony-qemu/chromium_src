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

#ifndef ARK_SYSTEM_PROPERTIES_ADAPTER_CAPI_H_
#define ARK_SYSTEM_PROPERTIES_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"
#include "ohos_adapter/include/ark_frame_rate_setting_adapter_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_system_properties_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *get_resource_use_hap_path_enable)(
      struct _ark_system_properties_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_device_info_product_model)(
      struct _ark_system_properties_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_device_info_brand)(
      struct _ark_system_properties_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_device_info_major_version)(
      struct _ark_system_properties_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_product_device_type)(
      struct _ark_system_properties_adapter_t *self);

  bool(ARK_WEB_CALLBACK *get_web_optimization_value)(
      struct _ark_system_properties_adapter_t *self);

  bool(ARK_WEB_CALLBACK *is_advanced_security_mode)(
      struct _ark_system_properties_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_user_agent_osname)(
      struct _ark_system_properties_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_software_major_version)(
      struct _ark_system_properties_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_software_senior_version)(
      struct _ark_system_properties_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_netlog_mode)(
      struct _ark_system_properties_adapter_t *self);

  bool(ARK_WEB_CALLBACK *get_trace_debug_enable)(
      struct _ark_system_properties_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_site_isolation_mode)(
      struct _ark_system_properties_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_flow_buf_max_fd)(
      struct _ark_system_properties_adapter_t *self);

  bool(ARK_WEB_CALLBACK *get_oopgpuenable)(
      struct _ark_system_properties_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_oopgpudisable)(
      struct _ark_system_properties_adapter_t *self);

  void(ARK_WEB_CALLBACK *attach_sys_prop_observer)(
      struct _ark_system_properties_adapter_t *self, int32_t key,
      void *observer);

  void(ARK_WEB_CALLBACK *detach_sys_prop_observer)(
      struct _ark_system_properties_adapter_t *self, int32_t key,
      void *observer);

  bool(ARK_WEB_CALLBACK *get_bool_parameter)(
      struct _ark_system_properties_adapter_t *self, ArkWebString key,
      bool defaultValue);

  ArkFrameRateSettingAdapterVector(ARK_WEB_CALLBACK *get_ltpoconfig)(
      struct _ark_system_properties_adapter_t *self,
      const ArkWebString *settingName);
} ark_system_properties_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_SYSTEM_PROPERTIES_ADAPTER_CAPI_H_
