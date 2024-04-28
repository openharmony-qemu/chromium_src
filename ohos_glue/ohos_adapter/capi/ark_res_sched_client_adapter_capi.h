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

#ifndef ARK_RES_SCHED_CLIENT_ADAPTER_CAPI_H_
#define ARK_RES_SCHED_CLIENT_ADAPTER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_res_sched_client_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;
} ark_res_sched_client_adapter_t;

ARK_WEB_EXPORT bool
ark_res_sched_client_adapter_report_key_thread(int32_t statusAdapter, pid_t pid,
                                               pid_t tid, int32_t roleAdapter);

ARK_WEB_EXPORT bool ark_res_sched_client_adapter_report_window_status(
    int32_t statusAdapter, pid_t pid, uint32_t windowId, int32_t nwebId);

ARK_WEB_EXPORT bool
ark_res_sched_client_adapter_report_scene(int32_t statusAdapter,
                                          int32_t sceneAdapter, int32_t nwebId);

ARK_WEB_EXPORT bool
ark_res_sched_client_adapter_report_audio_data(int32_t statusAdapter, pid_t pid,
                                               pid_t tid);

ARK_WEB_EXPORT void
ark_res_sched_client_adapter_report_window_id(int32_t windowId, int32_t nwebId);

ARK_WEB_EXPORT void
ark_res_sched_client_adapter_report_nweb_init(int32_t statusAdapter,
                                              int32_t nweb_id);

ARK_WEB_EXPORT void
ark_res_sched_client_adapter_report_render_process_status(int32_t statusAdapter,
                                                          pid_t pid);

ARK_WEB_EXPORT bool
ark_res_sched_client_adapter_report_screen_capture(int32_t statusAdapter,
                                                   pid_t pid);

ARK_WEB_EXPORT bool
ark_res_sched_client_adapter_report_video_playing(int32_t statusAdapter,
                                                  pid_t pid);

ARK_WEB_EXPORT void
ark_res_sched_client_adapter_report_process_in_use(pid_t pid);

ARK_WEB_EXPORT void
ark_res_sched_client_adapter_report_site_isolation_mode(bool mode);

#ifdef __cplusplus
}
#endif

#endif // ARK_RES_SCHED_CLIENT_ADAPTER_CAPI_H_
