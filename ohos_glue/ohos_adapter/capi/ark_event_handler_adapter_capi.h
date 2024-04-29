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

#ifndef ARK_EVENT_HANDLER_ADAPTER_CAPI_H_
#define ARK_EVENT_HANDLER_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_event_handler_fdlistener_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_readable)(
      struct _ark_event_handler_fdlistener_adapter_t *self,
      int32_t fileDescriptor);
} ark_event_handler_fdlistener_adapter_t;

typedef struct _ark_event_handler_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *add_file_descriptor_listener)(
      struct _ark_event_handler_adapter_t *self, int32_t fileDescriptor,
      uint32_t events, ark_event_handler_fdlistener_adapter_t *listener);

  void(ARK_WEB_CALLBACK *remove_file_descriptor_listener)(
      struct _ark_event_handler_adapter_t *self, int32_t fileDescriptor);
} ark_event_handler_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_EVENT_HANDLER_ADAPTER_CAPI_H_
