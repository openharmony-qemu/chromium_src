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

#ifndef ARK_NET_PROXY_ADAPTER_IMPL_H
#define ARK_NET_PROXY_ADAPTER_IMPL_H
#pragma once

#include "net_proxy_adapter.h"
#include "ohos_adapter/include/ark_net_proxy_adapter.h"

namespace OHOS::ArkWeb {

class ArkNetProxyAdapterImpl : public ArkNetProxyAdapter {
public:
    explicit ArkNetProxyAdapterImpl(NWeb::NetProxyAdapter&);

    void RegNetProxyEvent(ArkWebRefPtr<ArkNetProxyEventCallbackAdapter> eventCallback) override;

    bool StartListen() override;

    void StopListen() override;

    void GetProperty(ArkWebString& host, uint16_t& port, ArkWebString& pacUrl, ArkWebString& exclusion) override;

private:
    NWeb::NetProxyAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkNetProxyAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_NET_PROXY_ADAPTER_IMPL_H
