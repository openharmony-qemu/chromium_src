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

#ifndef ARK_WEB_URL_RESOURCE_RESPONSE_CAPI_H_
#define ARK_WEB_URL_RESOURCE_RESPONSE_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"
#include "ohos_nweb/capi/ark_web_resource_ready_callback_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_url_resource_response_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ArkWebString(ARK_WEB_CALLBACK *response_data)(
      struct _ark_web_url_resource_response_t *self);

  void(ARK_WEB_CALLBACK *put_response_data)(
      struct _ark_web_url_resource_response_t *self,
      const ArkWebString *input_stream);

  ArkWebString(ARK_WEB_CALLBACK *response_status)(
      struct _ark_web_url_resource_response_t *self);

  const ArkWebStringMap(ARK_WEB_CALLBACK *response_headers)(
      struct _ark_web_url_resource_response_t *self);

  void(ARK_WEB_CALLBACK *put_response_headers)(
      struct _ark_web_url_resource_response_t *self,
      const ArkWebStringMap *response_headers);

  int32_t(ARK_WEB_CALLBACK *response_data_type)(
      struct _ark_web_url_resource_response_t *self);

  ArkWebString(ARK_WEB_CALLBACK *response_mime_type)(
      struct _ark_web_url_resource_response_t *self);

  void(ARK_WEB_CALLBACK *put_response_mime_type)(
      struct _ark_web_url_resource_response_t *self,
      const ArkWebString *mime_type);

  ArkWebString(ARK_WEB_CALLBACK *response_encoding)(
      struct _ark_web_url_resource_response_t *self);

  void(ARK_WEB_CALLBACK *put_response_encoding)(
      struct _ark_web_url_resource_response_t *self,
      const ArkWebString *encoding);

  bool(ARK_WEB_CALLBACK *response_data_status)(
      struct _ark_web_url_resource_response_t *self);

  void(ARK_WEB_CALLBACK *put_response_data_status)(
      struct _ark_web_url_resource_response_t *self, bool is_data_ready);

  int(ARK_WEB_CALLBACK *response_status_code)(
      struct _ark_web_url_resource_response_t *self);

  ArkWebString(ARK_WEB_CALLBACK *response_resource_url)(
      struct _ark_web_url_resource_response_t *self);

  void(ARK_WEB_CALLBACK *put_response_resource_url)(
      struct _ark_web_url_resource_response_t *self, const ArkWebString *url);

  int(ARK_WEB_CALLBACK *response_file_handle)(
      struct _ark_web_url_resource_response_t *self);

  void(ARK_WEB_CALLBACK *put_response_file_handle)(
      struct _ark_web_url_resource_response_t *self, int fd);

  bool(ARK_WEB_CALLBACK *response_is_file_handle)(
      struct _ark_web_url_resource_response_t *self);

  void(ARK_WEB_CALLBACK *put_response_state_and_statuscode)(
      struct _ark_web_url_resource_response_t *self, int status_code,
      const ArkWebString *reason_phrase);

  void(ARK_WEB_CALLBACK *put_response_ready_callback)(
      struct _ark_web_url_resource_response_t *self,
      ark_web_resource_ready_callback_t *callback);
} ark_web_url_resource_response_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_URL_RESOURCE_RESPONSE_CAPI_H_
