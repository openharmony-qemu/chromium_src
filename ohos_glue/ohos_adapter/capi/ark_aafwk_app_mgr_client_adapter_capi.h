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

#ifndef ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_CAPI_H_
#define ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_CAPI_H_
#pragma once

#include "base/include/ark_web_types.h"
#include "ohos_adapter/capi/ark_aafwk_render_scheduler_host_adapter_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_aafwk_app_mgr_client_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int(ARK_WEB_CALLBACK *start_render_process)(
      struct _ark_aafwk_app_mgr_client_adapter_t *self,
      const ArkWebString *renderParam, int32_t ipcFd, int32_t sharedFd,
      int32_t crashFd, pid_t *renderPid);

  void(ARK_WEB_CALLBACK *attach_render_process)(
      struct _ark_aafwk_app_mgr_client_adapter_t *self,
      ark_aafwk_render_scheduler_host_adapter_t *adapter);

  int(ARK_WEB_CALLBACK *get_render_process_termination_status)(
      struct _ark_aafwk_app_mgr_client_adapter_t *self, pid_t renderPid,
      int *status);
} ark_aafwk_app_mgr_client_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_CAPI_H_
