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

#include "ohos_adapter/bridge/ark_net_proxy_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_net_proxy_event_callback_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNetProxyAdapterWrapper::ArkNetProxyAdapterWrapper(ArkWebRefPtr<ArkNetProxyAdapter> ref) : ctocpp_(ref) {}

void ArkNetProxyAdapterWrapper::RegNetProxyEvent(
    std::shared_ptr<OHOS::NWeb::NetProxyEventCallbackAdapter> eventCallback)
{
    if (!ctocpp_) {
        return;
    }
    if (CHECK_SHARED_PTR_IS_NULL(eventCallback)) {
        return ctocpp_->RegNetProxyEvent(nullptr);
    }

    ctocpp_->RegNetProxyEvent(new ArkNetProxyEventCallbackAdapterImpl(eventCallback));
}

bool ArkNetProxyAdapterWrapper::StartListen()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->StartListen();
}

void ArkNetProxyAdapterWrapper::StopListen()
{
    if (!ctocpp_) {
        return;
    }
    return ctocpp_->StopListen();
}

void ArkNetProxyAdapterWrapper::GetProperty(
    std::string& host, uint16_t& port, std::string& pacUrl, std::string& exclusion)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString ark_host;
    ArkWebString ark_pacUrl;
    ArkWebString ark_exclusion;
    ctocpp_->GetProperty(ark_host, port, ark_pacUrl, ark_exclusion);
    host = ArkWebStringStructToClass(ark_host);
    pacUrl = ArkWebStringStructToClass(ark_pacUrl);
    exclusion = ArkWebStringStructToClass(ark_exclusion);
    ArkWebStringStructRelease(ark_host);
    ArkWebStringStructRelease(ark_pacUrl);
    ArkWebStringStructRelease(ark_exclusion);
}

} // namespace OHOS::ArkWeb
