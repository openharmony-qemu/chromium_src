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

#ifndef ARK_WEB_ENGINE_CAPI_H_
#define ARK_WEB_ENGINE_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "ohos_nweb/capi/ark_web_cookie_manager_capi.h"
#include "ohos_nweb/capi/ark_web_data_base_capi.h"
#include "ohos_nweb/capi/ark_web_download_manager_capi.h"
#include "ohos_nweb/capi/ark_web_engine_init_args_capi.h"
#include "ohos_nweb/capi/ark_web_engine_prefetch_args_capi.h"
#include "ohos_nweb/capi/ark_web_nweb_capi.h"
#include "ohos_nweb/capi/ark_web_nweb_create_info_capi.h"
#include "ohos_nweb/capi/ark_web_web_storage_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_engine_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ark_web_nweb_t *(ARK_WEB_CALLBACK *create_nweb)(
      struct _ark_web_engine_t *self, ark_web_nweb_create_info_t *create_info);

  ark_web_nweb_t *(ARK_WEB_CALLBACK *get_nweb)(struct _ark_web_engine_t *self,
                                               int32_t nweb_id);

  ark_web_data_base_t *(ARK_WEB_CALLBACK *get_data_base)(
      struct _ark_web_engine_t *self);

  ark_web_web_storage_t *(ARK_WEB_CALLBACK *get_web_storage)(
      struct _ark_web_engine_t *self);

  ark_web_cookie_manager_t *(ARK_WEB_CALLBACK *get_cookie_manager)(
      struct _ark_web_engine_t *self);

  ark_web_download_manager_t *(ARK_WEB_CALLBACK *get_download_manager)(
      struct _ark_web_engine_t *self);

  void(ARK_WEB_CALLBACK *set_web_tag)(struct _ark_web_engine_t *self,
                                      int32_t nweb_id, const char *web_tag);

  void(ARK_WEB_CALLBACK *initialize_web_engine)(
      struct _ark_web_engine_t *self, ark_web_engine_init_args_t *init_args);

  void(ARK_WEB_CALLBACK *prepare_for_page_load)(struct _ark_web_engine_t *self,
                                                const ArkWebString *url,
                                                bool preconnectable,
                                                int32_t num_sockets);

  void(ARK_WEB_CALLBACK *set_web_debugging_access)(
      struct _ark_web_engine_t *self, bool isEnableDebug);

  void(ARK_WEB_CALLBACK *add_intelligent_tracking_prevention_bypassing_list)(
      struct _ark_web_engine_t *self, const ArkWebStringVector *hosts);

  void(ARK_WEB_CALLBACK *remove_intelligent_tracking_prevention_bypassing_list)(
      struct _ark_web_engine_t *self, const ArkWebStringVector *hosts);

  void(ARK_WEB_CALLBACK *clear_intelligent_tracking_prevention_bypassing_list)(
      struct _ark_web_engine_t *self);

  void(ARK_WEB_CALLBACK *pause_all_timers)(struct _ark_web_engine_t *self);

  void(ARK_WEB_CALLBACK *resume_all_timers)(struct _ark_web_engine_t *self);

  void(ARK_WEB_CALLBACK *prefetch_resource)(
      struct _ark_web_engine_t *self, ark_web_engine_prefetch_args_t **pre_args,
      const ArkWebStringMap *additional_http_headers,
      const ArkWebString *cache_key, const uint32_t *cache_valid_time);
} ark_web_engine_t;

ARK_WEB_EXPORT ark_web_engine_t *ark_web_engine_get_instance(void);

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_ENGINE_CAPI_H_
