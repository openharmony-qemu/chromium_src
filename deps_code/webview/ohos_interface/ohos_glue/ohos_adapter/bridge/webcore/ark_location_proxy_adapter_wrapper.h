/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_LOCATION_PROXY_ADAPTER_WRAPPER_H
#define ARK_LOCATION_PROXY_ADAPTER_WRAPPER_H
#pragma once

#include "location_adapter.h"
#include "ohos_adapter/include/ark_location_adapter.h"

namespace OHOS::ArkWeb {
class ArkLocationProxyAdapterWrapper : public OHOS::NWeb::LocationProxyAdapter {
public:
    explicit ArkLocationProxyAdapterWrapper(ArkWebRefPtr<ArkLocationProxyAdapter>);

    int32_t StartLocating(std::shared_ptr<OHOS::NWeb::LocationRequestConfig> requestConfig,
        std::shared_ptr<OHOS::NWeb::LocationCallbackAdapter> callback) override;

    bool StopLocating(int32_t callbackId) override;

    bool EnableAbility(bool isEnabled) override;

    bool IsLocationEnabled() override;

private:
    ArkWebRefPtr<ArkLocationProxyAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif
