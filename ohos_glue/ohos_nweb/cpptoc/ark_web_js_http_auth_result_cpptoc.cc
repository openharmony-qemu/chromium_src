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

#include "ohos_nweb/cpptoc/ark_web_js_http_auth_result_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_web_js_http_auth_result_cancel(
    struct _ark_web_js_http_auth_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebJsHttpAuthResultCppToC::Get(self)->Cancel();
}

bool ARK_WEB_CALLBACK ark_web_js_http_auth_result_confirm(
    struct _ark_web_js_http_auth_result_t *self, const ArkWebString *user_name,
    const ArkWebString *pwd) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(user_name, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(pwd, false);

  // Execute
  return ArkWebJsHttpAuthResultCppToC::Get(self)->Confirm(*user_name, *pwd);
}

bool ARK_WEB_CALLBACK ark_web_js_http_auth_result_is_http_auth_info_saved(
    struct _ark_web_js_http_auth_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebJsHttpAuthResultCppToC::Get(self)->IsHttpAuthInfoSaved();
}

} // namespace

ArkWebJsHttpAuthResultCppToC::ArkWebJsHttpAuthResultCppToC() {
  GetStruct()->cancel = ark_web_js_http_auth_result_cancel;
  GetStruct()->confirm = ark_web_js_http_auth_result_confirm;
  GetStruct()->is_http_auth_info_saved =
      ark_web_js_http_auth_result_is_http_auth_info_saved;
}

ArkWebJsHttpAuthResultCppToC::~ArkWebJsHttpAuthResultCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebJsHttpAuthResultCppToC, ArkWebJsHttpAuthResult,
                           ark_web_js_http_auth_result_t>::kBridgeType =
        ARK_WEB_JS_HTTP_AUTH_RESULT;

} // namespace OHOS::ArkWeb
