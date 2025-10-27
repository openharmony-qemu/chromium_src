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

#include "ohos_adapter/bridge/ark_vsync_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkVSyncAdapterImpl::ArkVSyncAdapterImpl(NWeb::VSyncAdapter& ref) : real_(ref) {}

uint32_t ArkVSyncAdapterImpl::RequestVsync(void* data, void* cb)
{
    return (uint32_t)real_.RequestVsync(data, reinterpret_cast<NWeb::NWebVSyncCb>(cb));
}

int64_t ArkVSyncAdapterImpl::GetVSyncPeriod()
{
    return real_.GetVSyncPeriod();
}

void ArkVSyncAdapterImpl::SetFrameRateLinkerEnable(bool enabled)
{
    return real_.SetFrameRateLinkerEnable(enabled);
}

void ArkVSyncAdapterImpl::SetFramePreferredRate(int32_t preferredRate)
{
    return real_.SetFramePreferredRate(preferredRate);
}

void ArkVSyncAdapterImpl::SetOnVsyncCallback(OnVsyncCallback callback)
{
    return real_.SetOnVsyncCallback(callback);
}

void ArkVSyncAdapterImpl::SetOnVsyncEndCallback(OnVsyncCallback onVsyncEndCallback)
{
    return real_.SetOnVsyncEndCallback(onVsyncEndCallback);
}

void ArkVSyncAdapterImpl::SetScene(const ArkWebString& sceneName, uint32_t state)
{
    std::string sceneNameStr = ArkWebStringStructToClass(sceneName);
    return real_.SetScene(sceneNameStr, state);
}

void ArkVSyncAdapterImpl::SetDVSyncSwitch(bool dvsyncSwitch)
{
    return real_.SetDVSyncSwitch(dvsyncSwitch);
}
} // namespace OHOS::ArkWeb