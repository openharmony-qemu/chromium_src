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

#ifndef ARK_DISPLAY_MANAGER_ADAPTER_CAPI_H_
#define ARK_DISPLAY_MANAGER_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_display_listener_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_create)(
      struct _ark_display_listener_adapter_t *self, uint64_t displayId);

  void(ARK_WEB_CALLBACK *on_destroy)(
      struct _ark_display_listener_adapter_t *self, uint64_t displayId);

  void(ARK_WEB_CALLBACK *on_change)(
      struct _ark_display_listener_adapter_t *self, uint64_t displayId);
} ark_display_listener_adapter_t;

typedef struct _ark_display_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  uint64_t(ARK_WEB_CALLBACK *get_id)(struct _ark_display_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_width)(struct _ark_display_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_height)(struct _ark_display_adapter_t *self);

  float(ARK_WEB_CALLBACK *get_virtual_pixel_ratio)(
      struct _ark_display_adapter_t *self);

  uint32_t(ARK_WEB_CALLBACK *get_rotation)(struct _ark_display_adapter_t *self);

  uint32_t(ARK_WEB_CALLBACK *get_orientation)(
      struct _ark_display_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_dpi)(struct _ark_display_adapter_t *self);

  uint32_t (ARK_WEB_CALLBACK *get_display_orientation)(struct _ark_display_adapter_t* self);
} ark_display_adapter_t;

typedef struct _ark_display_manager_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  uint64_t(ARK_WEB_CALLBACK *get_default_display_id)(
      struct _ark_display_manager_adapter_t *self);

  ark_display_adapter_t *(ARK_WEB_CALLBACK *get_default_display)(
      struct _ark_display_manager_adapter_t *self);

  uint32_t(ARK_WEB_CALLBACK *register_display_listener)(
      struct _ark_display_manager_adapter_t *self,
      ark_display_listener_adapter_t *listener);

  bool(ARK_WEB_CALLBACK *unregister_display_listener)(
      struct _ark_display_manager_adapter_t *self, uint32_t id);

  bool(ARK_WEB_CALLBACK *is_default_portrait)(
      struct _ark_display_manager_adapter_t *self);
} ark_display_manager_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_DISPLAY_MANAGER_ADAPTER_CAPI_H_
