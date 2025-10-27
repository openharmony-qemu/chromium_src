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

#include "power_mgr_client_adapter_impl.h"

using namespace OHOS::PowerMgr;
using namespace OHOS::NWeb;

namespace {
OHOS::PowerMgr::RunningLockType ConvertRunningLockType(
    RunningLockAdapterType type)
{
    OHOS::PowerMgr::RunningLockType rawType =
        OHOS::PowerMgr::RunningLockType::RUNNINGLOCK_BUTT;
    switch (type) {
        case RunningLockAdapterType::SCREEN:
            rawType = OHOS::PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN;
            break;
        case RunningLockAdapterType::BACKGROUND:
            rawType = OHOS::PowerMgr::RunningLockType::RUNNINGLOCK_BACKGROUND;
            break;
        case RunningLockAdapterType::PROXIMITY_SCREEN_CONTROL:
            rawType = OHOS::PowerMgr::RunningLockType::RUNNINGLOCK_PROXIMITY_SCREEN_CONTROL;
            break;
        default:
            break;
    }
    return rawType;
}
}

namespace OHOS::NWeb {
RunningLockAdapterImpl::RunningLockAdapterImpl(
    std::shared_ptr<OHOS::PowerMgr::RunningLock> lock)
    : lock_(lock) {}

bool RunningLockAdapterImpl::IsUsed()
{
    if (lock_ != nullptr) {
        return lock_->IsUsed();
    }
    return false;
}

int32_t RunningLockAdapterImpl::Lock(uint32_t timeOutMs)
{
    if (lock_ != nullptr) {
        return lock_->Lock(timeOutMs);
    }
    return -1;
}

int32_t RunningLockAdapterImpl::UnLock()
{
    if (lock_ != nullptr) {
        return lock_->UnLock();
    }
    return -1;
}

std::shared_ptr<RunningLockAdapter> PowerMgrClientAdapterImpl::CreateRunningLock(
    const std::string& name, RunningLockAdapterType type)
{
    RunningLockType rawType = ConvertRunningLockType(type);
    std::shared_ptr<RunningLock> lock = PowerMgrClient::GetInstance().CreateRunningLock(
        name, rawType);
    return std::make_shared<RunningLockAdapterImpl>(lock);
}
}