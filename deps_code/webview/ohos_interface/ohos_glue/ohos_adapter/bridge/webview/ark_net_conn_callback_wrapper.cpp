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

#include "ohos_adapter/bridge/ark_net_capabilities_adapter_impl.h"
#include "ohos_adapter/bridge/ark_net_conn_callback_wrapper.h"
#include "ohos_adapter/bridge/ark_net_connection_properties_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNetConnCallbackWrapper::ArkNetConnCallbackWrapper(ArkWebRefPtr<ArkNetConnCallback> ref) : ctocpp_(ref) {}

int32_t ArkNetConnCallbackWrapper::NetAvailable()
{
    return ctocpp_->NetAvailable();
}

int32_t ArkNetConnCallbackWrapper::NetCapabilitiesChange(
    const OHOS::NWeb::NetConnectType& netConnectType, const OHOS::NWeb::NetConnectSubtype& netConnectSubtype)
{
    return ctocpp_->NetCapabilitiesChange((uint32_t)netConnectType, (uint32_t)netConnectSubtype);
}

int32_t ArkNetConnCallbackWrapper::NetConnectionPropertiesChange()
{
    return ctocpp_->NetConnectionPropertiesChange();
}

int32_t ArkNetConnCallbackWrapper::NetUnavailable()
{
    return ctocpp_->NetUnavailable();
}

int32_t ArkNetConnCallbackWrapper::OnNetCapabilitiesChanged(
    const std::shared_ptr<NWeb::NetCapabilitiesAdapter> capabilities)
{
    if (CHECK_SHARED_PTR_IS_NULL(capabilities)) {
        return ctocpp_->OnNetCapabilitiesChanged(nullptr);
    }

    return ctocpp_->OnNetCapabilitiesChanged(new ArkNetCapabilitiesAdapterImpl(capabilities));
}

int32_t ArkNetConnCallbackWrapper::OnNetConnectionPropertiesChanged(
    const std::shared_ptr<NWeb::NetConnectionPropertiesAdapter> properties)
{
    if (CHECK_SHARED_PTR_IS_NULL(properties)) {
        return ctocpp_->OnNetConnectionPropertiesChanged(nullptr);
    }

    return ctocpp_->OnNetConnectionPropertiesChanged(new ArkNetConnectionPropertiesAdapterImpl(properties));
}

} // namespace OHOS::ArkWeb
