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

#include "ohos_adapter/cpptoc/ark_battery_event_callback_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/ctocpp/ark_battery_info_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_battery_event_callback_battery_info_changed(
    struct _ark_battery_event_callback_t *self, ark_battery_info_t *info) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkBatteryEventCallbackCppToC::Get(self)->BatteryInfoChanged(
      ArkBatteryInfoCToCpp::Invert(info));
}

} // namespace

ArkBatteryEventCallbackCppToC::ArkBatteryEventCallbackCppToC() {
  GetStruct()->battery_info_changed =
      ark_battery_event_callback_battery_info_changed;
}

ArkBatteryEventCallbackCppToC::~ArkBatteryEventCallbackCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkBatteryEventCallbackCppToC, ArkBatteryEventCallback,
    ark_battery_event_callback_t>::kBridgeType = ARK_BATTERY_EVENT_CALLBACK;

} // namespace OHOS::ArkWeb
