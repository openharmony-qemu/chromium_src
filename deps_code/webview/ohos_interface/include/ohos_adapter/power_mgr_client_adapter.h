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

#ifndef POWER_MGR_CLIENT_ADAPTER_H
#define POWER_MGR_CLIENT_ADAPTER_H

#include <memory>

namespace OHOS::NWeb {

enum class RunningLockAdapterType : uint32_t { SCREEN = 0, BACKGROUND, PROXIMITY_SCREEN_CONTROL, BUTT };

class RunningLockAdapter {
public:
    RunningLockAdapter() = default;

    virtual ~RunningLockAdapter() = default;

    virtual bool IsUsed() = 0;

    virtual int32_t Lock(uint32_t timeOutMs = 0) = 0;

    virtual int32_t UnLock() = 0;
};

class PowerMgrClientAdapter {
public:
    PowerMgrClientAdapter() = default;

    virtual ~PowerMgrClientAdapter() = default;

    virtual std::shared_ptr<RunningLockAdapter> CreateRunningLock(
        const std::string& name, RunningLockAdapterType type) = 0;
};

} // namespace OHOS::NWeb

#endif // POWER_MGR_CLIENT_ADAPTER_H