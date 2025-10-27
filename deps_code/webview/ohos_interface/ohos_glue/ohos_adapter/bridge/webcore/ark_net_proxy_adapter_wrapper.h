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

#ifndef ARK_NET_PROXY_ADAPTER_WRAPPER_H
#define ARK_NET_PROXY_ADAPTER_WRAPPER_H
#pragma once

#include "net_proxy_adapter.h"
#include "ohos_adapter/include/ark_net_proxy_adapter.h"

namespace OHOS::ArkWeb {

class ArkNetProxyAdapterWrapper : public OHOS::NWeb::NetProxyAdapter {
public:
    explicit ArkNetProxyAdapterWrapper(ArkWebRefPtr<ArkNetProxyAdapter>);

    void RegNetProxyEvent(std::shared_ptr<OHOS::NWeb::NetProxyEventCallbackAdapter> eventCallback) override;

    bool StartListen() override;

    void StopListen() override;

    void GetProperty(std::string& host, uint16_t& port, std::string& pacUrl, std::string& exclusion) override;

private:
    ArkWebRefPtr<ArkNetProxyAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_NET_PROXY_ADAPTER_WRAPPER_H
