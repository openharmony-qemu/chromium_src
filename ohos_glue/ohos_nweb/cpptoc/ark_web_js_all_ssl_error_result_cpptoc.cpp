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

#include "ohos_nweb/cpptoc/ark_web_js_all_ssl_error_result_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_web_js_all_ssl_error_result_handle_cancel(
    struct _ark_web_js_all_ssl_error_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebJsAllSslErrorResultCppToC::Get(self)->HandleCancel();
}

void ARK_WEB_CALLBACK ark_web_js_all_ssl_error_result_handle_confirm(
    struct _ark_web_js_all_ssl_error_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebJsAllSslErrorResultCppToC::Get(self)->HandleConfirm();
}

} // namespace

ArkWebJsAllSslErrorResultCppToC::ArkWebJsAllSslErrorResultCppToC() {
  GetStruct()->handle_cancel = ark_web_js_all_ssl_error_result_handle_cancel;
  GetStruct()->handle_confirm = ark_web_js_all_ssl_error_result_handle_confirm;
}

ArkWebJsAllSslErrorResultCppToC::~ArkWebJsAllSslErrorResultCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebJsAllSslErrorResultCppToC, ArkWebJsAllSslErrorResult,
    ark_web_js_all_ssl_error_result_t>::kBridgeType =
    ARK_WEB_JS_ALL_SSL_ERROR_RESULT;

} // namespace OHOS::ArkWeb
