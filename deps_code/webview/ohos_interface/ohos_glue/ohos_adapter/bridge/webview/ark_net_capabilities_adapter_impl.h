/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_NET_CAPABILITIES_ADAPTER_IMPL_H
#define ARK_NET_CAPABILITIES_ADAPTER_IMPL_H
#pragma once

#include "net_connect_adapter.h"
#include "ohos_adapter/include/ark_net_connect_adapter.h"

namespace OHOS::ArkWeb {

class ArkNetCapabilitiesAdapterImpl : public ArkNetCapabilitiesAdapter {
public:
    ArkNetCapabilitiesAdapterImpl(std::shared_ptr<OHOS::NWeb::NetCapabilitiesAdapter>);

    int32_t GetNetId() override;

    uint32_t GetConnectType() override;

    uint32_t GetConnectSubtype() override;

private:
    std::shared_ptr<OHOS::NWeb::NetCapabilitiesAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkNetCapabilitiesAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_NET_CAPABILITIES_ADAPTER_IMPL_H