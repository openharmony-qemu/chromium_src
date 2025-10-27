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

#include "ohos_adapter/bridge/ark_battery_mgr_client_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_battery_event_callback_impl.h"
#include "ohos_adapter/bridge/ark_battery_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkBatteryMgrClientAdapterWrapper::ArkBatteryMgrClientAdapterWrapper(ArkWebRefPtr<ArkBatteryMgrClientAdapter> ref)
    : ctocpp_(ref)
{}

void ArkBatteryMgrClientAdapterWrapper::RegBatteryEvent(
    const std::shared_ptr<OHOS::NWeb::WebBatteryEventCallback> eventCallback)
{
    if (CHECK_SHARED_PTR_IS_NULL(eventCallback)) {
        return ctocpp_->RegBatteryEvent(nullptr);
    }

    ctocpp_->RegBatteryEvent(new ArkBatteryEventCallbackImpl(eventCallback));
}

bool ArkBatteryMgrClientAdapterWrapper::StartListen()
{
    return ctocpp_->StartListen();
}

void ArkBatteryMgrClientAdapterWrapper::StopListen()
{
    ctocpp_->StopListen();
}

std::shared_ptr<OHOS::NWeb::WebBatteryInfo> ArkBatteryMgrClientAdapterWrapper::RequestBatteryInfo()
{
    ArkWebRefPtr<ArkBatteryInfo> ark_info = ctocpp_->RequestBatteryInfo();
    if (CHECK_REF_PTR_IS_NULL(ark_info)) {
        return nullptr;
    }

    return std::make_shared<ArkBatteryInfoWrapper>(ark_info);
}

} // namespace OHOS::ArkWeb
