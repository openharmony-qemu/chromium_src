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

#include "ohos_adapter/cpptoc/ark_timezone_event_callback_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/ctocpp/ark_web_timezone_info_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_timezone_event_callback_adapter_timezone_changed(
    struct _ark_timezone_event_callback_adapter_t *self,
    ark_web_timezone_info_t *info) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkTimezoneEventCallbackAdapterCppToC::Get(self)->TimezoneChanged(
      ArkWebTimezoneInfoCToCpp::Invert(info));
}

} // namespace

ArkTimezoneEventCallbackAdapterCppToC::ArkTimezoneEventCallbackAdapterCppToC() {
  GetStruct()->timezone_changed =
      ark_timezone_event_callback_adapter_timezone_changed;
}

ArkTimezoneEventCallbackAdapterCppToC::
    ~ArkTimezoneEventCallbackAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkTimezoneEventCallbackAdapterCppToC, ArkTimezoneEventCallbackAdapter,
    ark_timezone_event_callback_adapter_t>::kBridgeType =
    ARK_TIMEZONE_EVENT_CALLBACK_ADAPTER;

} // namespace OHOS::ArkWeb
