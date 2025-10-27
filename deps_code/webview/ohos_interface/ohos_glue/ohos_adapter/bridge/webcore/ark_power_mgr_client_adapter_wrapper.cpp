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

#include "ohos_adapter/bridge/ark_power_mgr_client_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_running_lock_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkPowerMgrClientAdapterWrapper::ArkPowerMgrClientAdapterWrapper(ArkWebRefPtr<ArkPowerMgrClientAdapter> ref)
    : ctocpp_(ref)
{}

std::shared_ptr<OHOS::NWeb::RunningLockAdapter> ArkPowerMgrClientAdapterWrapper::CreateRunningLock(
    const std::string& name, OHOS::NWeb::RunningLockAdapterType type)
{
    ArkWebString ark_name = ArkWebStringClassToStruct(name);
    uint32_t ark_type = (uint32_t)type;

    ArkWebRefPtr<ArkRunningLockAdapter> ark_running_lock = ctocpp_->CreateRunningLock(ark_name, ark_type);

    ArkWebStringStructRelease(ark_name);

    if (CHECK_REF_PTR_IS_NULL(ark_running_lock)) {
        return nullptr;
    }

    return std::make_shared<ArkRunningLockAdapterWrapper>(ark_running_lock);
}

} // namespace OHOS::ArkWeb
