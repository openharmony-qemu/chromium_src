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

#include "ohos_adapter/bridge/ark_running_lock_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkRunningLockAdapterImpl::ArkRunningLockAdapterImpl(std::shared_ptr<OHOS::NWeb::RunningLockAdapter> ref) : real_(ref)
{}

bool ArkRunningLockAdapterImpl::IsUsed()
{
    return real_->IsUsed();
}

int32_t ArkRunningLockAdapterImpl::Lock(uint32_t timeOutMs)
{
    return real_->Lock(timeOutMs);
}

int32_t ArkRunningLockAdapterImpl::UnLock()
{
    return real_->UnLock();
}

} // namespace OHOS::ArkWeb
