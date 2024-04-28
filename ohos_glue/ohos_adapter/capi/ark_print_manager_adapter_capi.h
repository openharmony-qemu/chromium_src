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

#ifndef ARK_PRINT_MANAGER_ADAPTER_CAPI_H_
#define ARK_PRINT_MANAGER_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"
#include "ohos_adapter/include/ark_web_adapter_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_print_write_result_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *write_result_callback)(
      struct _ark_print_write_result_callback_adapter_t *self,
      ArkWebString jobId, uint32_t code);
} ark_print_write_result_callback_adapter_t;

typedef struct _ark_print_document_adapter_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_start_layout_write)(
      struct _ark_print_document_adapter_adapter_t *self,
      const ArkWebString *jobId, const ArkPrintAttributesAdapter *oldAttrs,
      const ArkPrintAttributesAdapter *newAttrs, uint32_t fd,
      ark_print_write_result_callback_adapter_t *callback);

  void(ARK_WEB_CALLBACK *on_job_state_changed)(
      struct _ark_print_document_adapter_adapter_t *self,
      const ArkWebString *jobId, uint32_t state);
} ark_print_document_adapter_adapter_t;

typedef struct _ark_print_manager_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *start_print)(
      struct _ark_print_manager_adapter_t *self,
      const ArkWebStringVector *fileList, const ArkWebUint32Vector *fdList,
      ArkWebString *taskId);

  int32_t(ARK_WEB_CALLBACK *print1)(
      struct _ark_print_manager_adapter_t *self,
      const ArkWebString *printJobName,
      ark_print_document_adapter_adapter_t *listener,
      const ArkPrintAttributesAdapter *printAttributes);

  int32_t(ARK_WEB_CALLBACK *print2)(
      struct _ark_print_manager_adapter_t *self,
      const ArkWebString *printJobName,
      ark_print_document_adapter_adapter_t *listener,
      const ArkPrintAttributesAdapter *printAttributes, void *contextToken);
} ark_print_manager_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_PRINT_MANAGER_ADAPTER_CAPI_H_
