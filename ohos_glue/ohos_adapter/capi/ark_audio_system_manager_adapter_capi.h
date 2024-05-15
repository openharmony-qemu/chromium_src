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

#ifndef ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_CAPI_H_
#define ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"
#include "ohos_adapter/include/ark_audio_device_desc_adapter_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_audio_interrupt_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_stream_usage)(
      struct _ark_audio_interrupt_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_content_type)(
      struct _ark_audio_interrupt_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_stream_type)(
      struct _ark_audio_interrupt_adapter_t *self);

  uint32_t(ARK_WEB_CALLBACK *get_session_id)(
      struct _ark_audio_interrupt_adapter_t *self);

  bool(ARK_WEB_CALLBACK *get_pause_when_ducked)(
      struct _ark_audio_interrupt_adapter_t *self);
} ark_audio_interrupt_adapter_t;

typedef struct _ark_audio_device_desc_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *get_device_id)(
      struct _ark_audio_device_desc_adapter_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_device_name)(
      struct _ark_audio_device_desc_adapter_t *self);
} ark_audio_device_desc_adapter_t;

typedef struct _ark_audio_manager_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_suspend)(
      struct _ark_audio_manager_callback_adapter_t *self);

  void(ARK_WEB_CALLBACK *on_resume)(
      struct _ark_audio_manager_callback_adapter_t *self);
} ark_audio_manager_callback_adapter_t;

typedef struct _ark_audio_manager_device_change_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_device_change)(
      struct _ark_audio_manager_device_change_callback_adapter_t *self);
} ark_audio_manager_device_change_callback_adapter_t;

typedef struct _ark_audio_system_manager_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *has_audio_output_devices)(
      struct _ark_audio_system_manager_adapter_t *self);

  bool(ARK_WEB_CALLBACK *has_audio_input_devices)(
      struct _ark_audio_system_manager_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *request_audio_focus)(
      struct _ark_audio_system_manager_adapter_t *self,
      ark_audio_interrupt_adapter_t *audioInterrupt);

  int32_t(ARK_WEB_CALLBACK *abandon_audio_focus)(
      struct _ark_audio_system_manager_adapter_t *self,
      ark_audio_interrupt_adapter_t *audioInterrupt);

  int32_t(ARK_WEB_CALLBACK *set_audio_manager_interrupt_callback)(
      struct _ark_audio_system_manager_adapter_t *self,
      ark_audio_manager_callback_adapter_t *callback);

  int32_t(ARK_WEB_CALLBACK *unset_audio_manager_interrupt_callback)(
      struct _ark_audio_system_manager_adapter_t *self);

  ArkAudioDeviceDescAdapterVector(ARK_WEB_CALLBACK *get_devices)(
      struct _ark_audio_system_manager_adapter_t *self, int32_t flag);

  int32_t(ARK_WEB_CALLBACK *select_audio_device_by_id)(
      struct _ark_audio_system_manager_adapter_t *self, int32_t deviceId,
      bool isInput);

  int32_t(ARK_WEB_CALLBACK *set_device_change_callback)(
      struct _ark_audio_system_manager_adapter_t *self,
      ark_audio_manager_device_change_callback_adapter_t *callback);

  int32_t(ARK_WEB_CALLBACK *unset_device_change_callback)(
      struct _ark_audio_system_manager_adapter_t *self);

  ark_audio_device_desc_adapter_t *(
      ARK_WEB_CALLBACK *get_default_output_device)(
      struct _ark_audio_system_manager_adapter_t *self);

  ark_audio_device_desc_adapter_t *(ARK_WEB_CALLBACK *get_default_input_device)(
      struct _ark_audio_system_manager_adapter_t *self);
      
  bool(ARK_WEB_CALLBACK* set_language)(
      struct _ark_audio_system_manager_adapter_t* self, ArkWebString language);
} ark_audio_system_manager_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_CAPI_H_
