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

#ifndef ARK_MMI_ADAPTER_CAPI_H_
#define ARK_MMI_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_mmidevice_info_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_id)(struct _ark_mmidevice_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_type)(
      struct _ark_mmidevice_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_bus)(
      struct _ark_mmidevice_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_version)(
      struct _ark_mmidevice_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_product)(
      struct _ark_mmidevice_info_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_vendor)(
      struct _ark_mmidevice_info_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_name)(
      struct _ark_mmidevice_info_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_phys)(
      struct _ark_mmidevice_info_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_uniq)(
      struct _ark_mmidevice_info_adapter_t *self);

  void(ARK_WEB_CALLBACK *set_id)(struct _ark_mmidevice_info_adapter_t *self,
                                 int32_t id);

  void(ARK_WEB_CALLBACK *set_type)(struct _ark_mmidevice_info_adapter_t *self,
                                   int32_t type);

  void(ARK_WEB_CALLBACK *set_bus)(struct _ark_mmidevice_info_adapter_t *self,
                                  int32_t bus);

  void(ARK_WEB_CALLBACK *set_version)(
      struct _ark_mmidevice_info_adapter_t *self, int32_t version);

  void(ARK_WEB_CALLBACK *set_product)(
      struct _ark_mmidevice_info_adapter_t *self, int32_t product);

  void(ARK_WEB_CALLBACK *set_vendor)(struct _ark_mmidevice_info_adapter_t *self,
                                     int32_t vendor);

  void(ARK_WEB_CALLBACK *set_name)(struct _ark_mmidevice_info_adapter_t *self,
                                   ArkWebString name);

  void(ARK_WEB_CALLBACK *set_phys)(struct _ark_mmidevice_info_adapter_t *self,
                                   ArkWebString phys);

  void(ARK_WEB_CALLBACK *set_uniq)(struct _ark_mmidevice_info_adapter_t *self,
                                   ArkWebString uniq);
} ark_mmidevice_info_adapter_t;

typedef struct _ark_mmilistener_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_device_added)(
      struct _ark_mmilistener_adapter_t *self, int32_t deviceId,
      const ArkWebString *type);

  void(ARK_WEB_CALLBACK *on_device_removed)(
      struct _ark_mmilistener_adapter_t *self, int32_t deviceId,
      const ArkWebString *type);
} ark_mmilistener_adapter_t;

typedef struct _ark_mmiinput_listener_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_input_event)(
      struct _ark_mmiinput_listener_adapter_t *self, int32_t keyCode,
      int32_t keyAction);
} ark_mmiinput_listener_adapter_t;

typedef struct _ark_mmiadapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  char *(ARK_WEB_CALLBACK *key_code_to_string)(struct _ark_mmiadapter_t *self,
                                               int32_t keyCode);

  int32_t(ARK_WEB_CALLBACK *register_mmiinput_listener)(
      struct _ark_mmiadapter_t *self,
      ark_mmiinput_listener_adapter_t *eventCallback);

  void(ARK_WEB_CALLBACK *unregister_mmiinput_listener)(
      struct _ark_mmiadapter_t *self, int32_t monitorId);

  int32_t(ARK_WEB_CALLBACK *register_dev_listener)(
      struct _ark_mmiadapter_t *self, ArkWebString type,
      ark_mmilistener_adapter_t *listener);

  int32_t(ARK_WEB_CALLBACK *unregister_dev_listener)(
      struct _ark_mmiadapter_t *self, ArkWebString type);

  int32_t(ARK_WEB_CALLBACK *get_keyboard_type)(struct _ark_mmiadapter_t *self,
                                               int32_t deviceId, int32_t *type);

  int32_t(ARK_WEB_CALLBACK *get_device_ids)(struct _ark_mmiadapter_t *self,
                                            ArkWebInt32Vector *ids);

  int32_t(ARK_WEB_CALLBACK *get_device_info)(
      struct _ark_mmiadapter_t *self, int32_t deviceId,
      ark_mmidevice_info_adapter_t *info);
} ark_mmiadapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_MMI_ADAPTER_CAPI_H_
