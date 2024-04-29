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

#include "ohos_nweb/cpptoc/ark_web_js_ssl_select_cert_result_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_web_js_ssl_select_cert_result_cancel(
    struct _ark_web_js_ssl_select_cert_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebJsSslSelectCertResultCppToC::Get(self)->Cancel();
}

void ARK_WEB_CALLBACK ark_web_js_ssl_select_cert_result_ignore(
    struct _ark_web_js_ssl_select_cert_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebJsSslSelectCertResultCppToC::Get(self)->Ignore();
}

void ARK_WEB_CALLBACK ark_web_js_ssl_select_cert_result_confirm(
    struct _ark_web_js_ssl_select_cert_result_t *self,
    const ArkWebString *private_key_file, const ArkWebString *cert_chain_file) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(private_key_file, );

  ARK_WEB_CPPTOC_CHECK_PARAM(cert_chain_file, );

  // Execute
  ArkWebJsSslSelectCertResultCppToC::Get(self)->Confirm(*private_key_file,
                                                        *cert_chain_file);
}

} // namespace

ArkWebJsSslSelectCertResultCppToC::ArkWebJsSslSelectCertResultCppToC() {
  GetStruct()->cancel = ark_web_js_ssl_select_cert_result_cancel;
  GetStruct()->ignore = ark_web_js_ssl_select_cert_result_ignore;
  GetStruct()->confirm = ark_web_js_ssl_select_cert_result_confirm;
}

ArkWebJsSslSelectCertResultCppToC::~ArkWebJsSslSelectCertResultCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebJsSslSelectCertResultCppToC, ArkWebJsSslSelectCertResult,
    ark_web_js_ssl_select_cert_result_t>::kBridgeType =
    ARK_WEB_JS_SSL_SELECT_CERT_RESULT;

} // namespace OHOS::ArkWeb
