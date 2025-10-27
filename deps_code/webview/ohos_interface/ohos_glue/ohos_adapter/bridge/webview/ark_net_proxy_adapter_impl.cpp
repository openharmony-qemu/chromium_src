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

#include "ohos_adapter/bridge/ark_net_proxy_adapter_impl.h"

#include "ohos_adapter/bridge/ark_net_proxy_event_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNetProxyAdapterImpl::ArkNetProxyAdapterImpl(NWeb::NetProxyAdapter& ref) : real_(ref) {}

void ArkNetProxyAdapterImpl::RegNetProxyEvent(ArkWebRefPtr<ArkNetProxyEventCallbackAdapter> eventCallback)
{
    if (CHECK_REF_PTR_IS_NULL(eventCallback)) {
        return real_.RegNetProxyEvent(nullptr);
    }

    real_.RegNetProxyEvent(std::make_shared<ArkNetProxyEventCallbackAdapterWrapper>(eventCallback));
}

bool ArkNetProxyAdapterImpl::StartListen()
{
    return real_.StartListen();
}

void ArkNetProxyAdapterImpl::StopListen()
{
    return real_.StopListen();
}

void ArkNetProxyAdapterImpl::GetProperty(
    ArkWebString& host, uint16_t& port, ArkWebString& pacUrl, ArkWebString& exclusion)
{
    std::string s_host;
    std::string s_pacUrl;
    std::string s_exclusion;

    real_.GetProperty(s_host, port, s_pacUrl, s_exclusion);
    host = ArkWebStringClassToStruct(s_host);
    pacUrl = ArkWebStringClassToStruct(s_pacUrl);
    exclusion = ArkWebStringClassToStruct(s_exclusion);
}

} // namespace OHOS::ArkWeb
