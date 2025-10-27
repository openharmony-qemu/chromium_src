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

#include "ohos_adapter/bridge/ark_running_lock_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkRunningLockAdapterWrapper::ArkRunningLockAdapterWrapper(ArkWebRefPtr<ArkRunningLockAdapter> ref) : ctocpp_(ref) {}

bool ArkRunningLockAdapterWrapper::IsUsed()
{
    return ctocpp_->IsUsed();
}

int32_t ArkRunningLockAdapterWrapper::Lock(uint32_t timeOutMs)
{
    return ctocpp_->Lock(timeOutMs);
}

int32_t ArkRunningLockAdapterWrapper::UnLock()
{
    return ctocpp_->UnLock();
}

} // namespace OHOS::ArkWeb
