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

#ifndef ARK_BATTERY_INFO_CTOCPP_H_
#define ARK_BATTERY_INFO_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_battery_mgr_client_adapter_capi.h"
#include "ohos_adapter/include/ark_battery_mgr_client_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkBatteryInfoCToCpp
    : public ArkWebCToCppRefCounted<ArkBatteryInfoCToCpp, ArkBatteryInfo,
                                    ark_battery_info_t> {
public:
  ArkBatteryInfoCToCpp();
  virtual ~ArkBatteryInfoCToCpp();

  // ArkBatteryInfo methods.
  double GetLevel() override;

  bool IsCharging() override;

  int DisChargingTime() override;

  int ChargingTime() override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_BATTERY_INFO_CTOCPP_H_
