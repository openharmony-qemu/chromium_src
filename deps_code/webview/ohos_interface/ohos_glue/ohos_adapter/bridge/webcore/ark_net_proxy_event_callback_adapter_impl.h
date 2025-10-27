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
#ifndef ARK_NET_PROXY_EVENT_CALLBACK_ADAPTER_IMPL_H
#define ARK_NET_PROXY_EVENT_CALLBACK_ADAPTER_IMPL_H
#pragma once

#include "net_proxy_adapter.h"
#include "ohos_adapter/include/ark_net_proxy_adapter.h"

namespace OHOS::ArkWeb {

class ArkNetProxyEventCallbackAdapterImpl : public ArkNetProxyEventCallbackAdapter {
public:
    explicit ArkNetProxyEventCallbackAdapterImpl(std::shared_ptr<OHOS::NWeb::NetProxyEventCallbackAdapter>);

    void Changed(const ArkWebString& host, const uint16_t& port, const ArkWebString& pacUrl,
        const ArkWebStringVector& exclusionList) override;

private:
    std::shared_ptr<OHOS::NWeb::NetProxyEventCallbackAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkNetProxyEventCallbackAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_NET_PROXY_EVENT_CALLBACK_ADAPTER_IMPL_H
