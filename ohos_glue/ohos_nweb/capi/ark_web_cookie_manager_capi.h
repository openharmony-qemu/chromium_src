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

#ifndef ARK_WEB_COOKIE_MANAGER_CAPI_H_
#define ARK_WEB_COOKIE_MANAGER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "ohos_nweb/capi/ark_web_bool_value_callback_capi.h"
#include "ohos_nweb/capi/ark_web_long_value_callback_capi.h"
#include "ohos_nweb/capi/ark_web_string_value_callback_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_cookie_manager_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *store1)(struct _ark_web_cookie_manager_t *self);

  void(ARK_WEB_CALLBACK *store2)(struct _ark_web_cookie_manager_t *self,
                                 ark_web_bool_value_callback_t *callback);

  int(ARK_WEB_CALLBACK *set_cookie1)(struct _ark_web_cookie_manager_t *self,
                                     const ArkWebString *url,
                                     const ArkWebString *value,
                                     bool incognito_mode);

  void(ARK_WEB_CALLBACK *set_cookie2)(struct _ark_web_cookie_manager_t *self,
                                      const ArkWebString *url,
                                      const ArkWebString *value,
                                      ark_web_bool_value_callback_t *callback);

  bool(ARK_WEB_CALLBACK *exist_cookies1)(struct _ark_web_cookie_manager_t *self,
                                         bool incognito_mode);

  void(ARK_WEB_CALLBACK *exist_cookies2)(
      struct _ark_web_cookie_manager_t *self,
      ark_web_bool_value_callback_t *callback);

  ArkWebString(ARK_WEB_CALLBACK *return_cookie1)(
      struct _ark_web_cookie_manager_t *self, const ArkWebString *url,
      bool *is_valid, bool incognito_mode);

  void(ARK_WEB_CALLBACK *return_cookie2)(
      struct _ark_web_cookie_manager_t *self, const ArkWebString *url,
      ark_web_string_value_callback_t *callback);

  void(ARK_WEB_CALLBACK *config_cookie)(
      struct _ark_web_cookie_manager_t *self, const ArkWebString *url,
      const ArkWebString *value, ark_web_long_value_callback_t *callback);

  void(ARK_WEB_CALLBACK *delete_session_cookies)(
      struct _ark_web_cookie_manager_t *self,
      ark_web_bool_value_callback_t *callback);

  void(ARK_WEB_CALLBACK *delete_cookie_entirely)(
      struct _ark_web_cookie_manager_t *self,
      ark_web_bool_value_callback_t *callback, bool incognito_mode);

  bool(ARK_WEB_CALLBACK *is_accept_cookie_allowed)(
      struct _ark_web_cookie_manager_t *self);

  void(ARK_WEB_CALLBACK *put_accept_cookie_enabled)(
      struct _ark_web_cookie_manager_t *self, bool accept);

  bool(ARK_WEB_CALLBACK *is_third_party_cookie_allowed)(
      struct _ark_web_cookie_manager_t *self);

  bool(ARK_WEB_CALLBACK *is_file_urlscheme_cookies_allowed)(
      struct _ark_web_cookie_manager_t *self);

  void(ARK_WEB_CALLBACK *put_accept_third_party_cookie_enabled)(
      struct _ark_web_cookie_manager_t *self, bool accept);

  void(ARK_WEB_CALLBACK *put_accept_file_urlscheme_cookies_enabled)(
      struct _ark_web_cookie_manager_t *self, bool allow);
} ark_web_cookie_manager_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_COOKIE_MANAGER_CAPI_H_
