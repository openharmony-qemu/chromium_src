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

#ifndef ARK_HITRACE_ADAPTER_CAPI_H_
#define ARK_HITRACE_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_hi_trace_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *start_trace)(struct _ark_hi_trace_adapter_t *self,
                                      const ArkWebString *value, float limit);

  void(ARK_WEB_CALLBACK *finish_trace)(struct _ark_hi_trace_adapter_t *self);

  void(ARK_WEB_CALLBACK *start_async_trace)(
      struct _ark_hi_trace_adapter_t *self, const ArkWebString *value,
      int32_t taskId, float limit);

  void(ARK_WEB_CALLBACK *finish_async_trace)(
      struct _ark_hi_trace_adapter_t *self, const ArkWebString *value,
      int32_t taskId);

  void(ARK_WEB_CALLBACK *count_trace)(struct _ark_hi_trace_adapter_t *self,
                                      const ArkWebString *name, int64_t count);

  bool(ARK_WEB_CALLBACK *is_hi_trace_enable)(
      struct _ark_hi_trace_adapter_t *self);
} ark_hi_trace_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_HITRACE_ADAPTER_CAPI_H_
