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

#include "ohos_adapter/bridge/ark_net_proxy_event_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNetProxyEventCallbackAdapterWrapper::ArkNetProxyEventCallbackAdapterWrapper(
    ArkWebRefPtr<ArkNetProxyEventCallbackAdapter> ref)
    : ctocpp_(ref)
{}

void ArkNetProxyEventCallbackAdapterWrapper::Changed(const std::string& host, const uint16_t& port,
    const std::string& pacUrl, const std::vector<std::string>& exclusionList)
{
    ArkWebString ark_host = ArkWebStringClassToStruct(host);
    ArkWebString ark_pacUrl = ArkWebStringClassToStruct(pacUrl);
    ArkWebStringVector ark_exclusionList = ArkWebStringVectorClassToStruct(exclusionList);
    ctocpp_->Changed(ark_host, port, ark_pacUrl, ark_exclusionList);
    ArkWebStringStructRelease(ark_host);
    ArkWebStringStructRelease(ark_pacUrl);
    ArkWebStringVectorStructRelease(ark_exclusionList);
}

} // namespace OHOS::ArkWeb
