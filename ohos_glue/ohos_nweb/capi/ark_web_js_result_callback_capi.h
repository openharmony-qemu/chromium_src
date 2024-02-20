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

#ifndef ARK_WEB_JS_RESULT_CALLBACK_CAPI_H_
#define ARK_WEB_JS_RESULT_CALLBACK_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"
#include "ohos_nweb/include/ark_web_value.h"
#include "ohos_nweb/include/ark_web_value_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_js_result_callback_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ArkWebValue(ARK_WEB_CALLBACK *get_java_script_result)(
      struct _ark_web_js_result_callback_t *self, ArkWebValueVector args,
      const ArkWebString *method, const ArkWebString *object_name,
      int32_t routing_id, int32_t object_id);

  bool(ARK_WEB_CALLBACK *has_java_script_object_methods)(
      struct _ark_web_js_result_callback_t *self, int32_t object_id,
      const ArkWebString *method_name);

  ArkWebValue(ARK_WEB_CALLBACK *get_java_script_object_methods)(
      struct _ark_web_js_result_callback_t *self, int32_t object_id);

  void(ARK_WEB_CALLBACK *remove_java_script_object_holder)(
      struct _ark_web_js_result_callback_t *self, int32_t holder,
      int32_t object_id);

  void(ARK_WEB_CALLBACK *remove_transient_java_script_object)(
      struct _ark_web_js_result_callback_t *self);
} ark_web_js_result_callback_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_JS_RESULT_CALLBACK_CAPI_H_
