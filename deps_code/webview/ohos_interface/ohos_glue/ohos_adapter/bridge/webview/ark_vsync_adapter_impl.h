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

#ifndef ARK_GRAPHIC_ADAPTER_IMPL_H
#define ARK_GRAPHIC_ADAPTER_IMPL_H
#pragma once

#include "graphic_adapter.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

class ArkVSyncAdapterImpl : public ArkVSyncAdapter {
public:
    explicit ArkVSyncAdapterImpl(NWeb::VSyncAdapter&);

    uint32_t RequestVsync(void* data, void* cb) override;

    int64_t GetVSyncPeriod() override;

    void SetFrameRateLinkerEnable(bool enabled) override;

    void SetFramePreferredRate(int32_t preferredRate) override;

    void SetOnVsyncCallback(OnVsyncCallback callback) override;

    void SetOnVsyncEndCallback(OnVsyncCallback onVsyncEndCallback) override;

    void SetScene(const ArkWebString& sceneName, uint32_t state) override;

    void SetDVSyncSwitch(bool dvsyncSwitch) override;
private:
    NWeb::VSyncAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkVSyncAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // Ark_GRAPHIC_ADAPTER_H
