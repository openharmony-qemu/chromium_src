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

#include "ohos_adapter/ctocpp/ark_battery_info_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
double ArkBatteryInfoCToCpp::GetLevel() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_battery_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_level, 0);

  // Execute
  return _struct->get_level(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkBatteryInfoCToCpp::IsCharging() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_battery_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_charging, false);

  // Execute
  return _struct->is_charging(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkBatteryInfoCToCpp::DisChargingTime() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_battery_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, dis_charging_time, 0);

  // Execute
  return _struct->dis_charging_time(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkBatteryInfoCToCpp::ChargingTime() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_battery_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, charging_time, 0);

  // Execute
  return _struct->charging_time(_struct);
}

ArkBatteryInfoCToCpp::ArkBatteryInfoCToCpp() {
}

ArkBatteryInfoCToCpp::~ArkBatteryInfoCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<ArkBatteryInfoCToCpp, ArkBatteryInfo,
                                        ark_battery_info_t>::kBridgeType =
    ARK_BATTERY_INFO;

} // namespace OHOS::ArkWeb
