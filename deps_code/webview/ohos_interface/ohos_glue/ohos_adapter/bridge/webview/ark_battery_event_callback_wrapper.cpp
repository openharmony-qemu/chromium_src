/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_battery_event_callback_wrapper.h"

#include "ohos_adapter/bridge/ark_battery_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkBatteryEventCallbackWrapper::ArkBatteryEventCallbackWrapper(ArkWebRefPtr<ArkBatteryEventCallback> ref)
    : ctocpp_ { ref }
{}

void ArkBatteryEventCallbackWrapper::BatteryInfoChanged(std::shared_ptr<OHOS::NWeb::WebBatteryInfo> info)
{
    ctocpp_->BatteryInfoChanged(new ArkBatteryInfoImpl(info));
}

} // namespace OHOS::ArkWeb
