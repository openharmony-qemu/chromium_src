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

#include "ohos_adapter/cpptoc/ark_location_callback_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/ctocpp/ark_location_info_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_location_callback_adapter_on_location_report(
    struct _ark_location_callback_adapter_t *self,
    ark_location_info_t *location) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkLocationCallbackAdapterCppToC::Get(self)->OnLocationReport(
      ArkLocationInfoCToCpp::Invert(location));
}

void ARK_WEB_CALLBACK ark_location_callback_adapter_on_locating_status_change(
    struct _ark_location_callback_adapter_t *self, const int status) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkLocationCallbackAdapterCppToC::Get(self)->OnLocatingStatusChange(status);
}

void ARK_WEB_CALLBACK ark_location_callback_adapter_on_error_report(
    struct _ark_location_callback_adapter_t *self, const int errorCode) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkLocationCallbackAdapterCppToC::Get(self)->OnErrorReport(errorCode);
}

} // namespace

ArkLocationCallbackAdapterCppToC::ArkLocationCallbackAdapterCppToC() {
  GetStruct()->on_location_report =
      ark_location_callback_adapter_on_location_report;
  GetStruct()->on_locating_status_change =
      ark_location_callback_adapter_on_locating_status_change;
  GetStruct()->on_error_report = ark_location_callback_adapter_on_error_report;
}

ArkLocationCallbackAdapterCppToC::~ArkLocationCallbackAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkLocationCallbackAdapterCppToC, ArkLocationCallbackAdapter,
    ark_location_callback_adapter_t>::kBridgeType =
    ARK_LOCATION_CALLBACK_ADAPTER;

} // namespace OHOS::ArkWeb
