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

#include "ohos_adapter/bridge/ark_power_mgr_client_adapter_impl.h"

#include "ohos_adapter/bridge/ark_running_lock_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkPowerMgrClientAdapterImpl::ArkPowerMgrClientAdapterImpl(std::shared_ptr<OHOS::NWeb::PowerMgrClientAdapter> ref)
    : real_(ref)
{}

ArkWebRefPtr<ArkRunningLockAdapter> ArkPowerMgrClientAdapterImpl::CreateRunningLock(
    const ArkWebString& name, uint32_t type)
{
    std::string sstr = ArkWebStringStructToClass(name);

    std::shared_ptr<OHOS::NWeb::RunningLockAdapter> adapter =
        real_->CreateRunningLock(sstr, (OHOS::NWeb::RunningLockAdapterType)type);

    if (CHECK_SHARED_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return new ArkRunningLockAdapterImpl(adapter);
}

} // namespace OHOS::ArkWeb
