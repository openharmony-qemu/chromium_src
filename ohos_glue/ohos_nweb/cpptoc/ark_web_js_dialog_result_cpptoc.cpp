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

#include "ohos_nweb/cpptoc/ark_web_js_dialog_result_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK
ark_web_js_dialog_result_cancel(struct _ark_web_js_dialog_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebJsDialogResultCppToC::Get(self)->Cancel();
}

void ARK_WEB_CALLBACK
ark_web_js_dialog_result_confirm1(struct _ark_web_js_dialog_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebJsDialogResultCppToC::Get(self)->Confirm();
}

void ARK_WEB_CALLBACK ark_web_js_dialog_result_confirm2(
    struct _ark_web_js_dialog_result_t *self, const ArkWebString *message) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(message, );

  // Execute
  ArkWebJsDialogResultCppToC::Get(self)->Confirm(*message);
}

} // namespace

ArkWebJsDialogResultCppToC::ArkWebJsDialogResultCppToC() {
  GetStruct()->cancel = ark_web_js_dialog_result_cancel;
  GetStruct()->confirm1 = ark_web_js_dialog_result_confirm1;
  GetStruct()->confirm2 = ark_web_js_dialog_result_confirm2;
}

ArkWebJsDialogResultCppToC::~ArkWebJsDialogResultCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebJsDialogResultCppToC, ArkWebJsDialogResult,
                           ark_web_js_dialog_result_t>::kBridgeType =
        ARK_WEB_JS_DIALOG_RESULT;

} // namespace OHOS::ArkWeb
