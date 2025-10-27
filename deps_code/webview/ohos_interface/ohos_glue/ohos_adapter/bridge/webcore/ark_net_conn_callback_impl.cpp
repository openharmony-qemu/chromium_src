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

#include "ohos_adapter/bridge/ark_net_capabilities_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_net_conn_callback_impl.h"
#include "ohos_adapter/bridge/ark_net_connection_properties_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNetConnCallbackImpl::ArkNetConnCallbackImpl(std::shared_ptr<OHOS::NWeb::NetConnCallback> ref) : real_(ref) {}

int32_t ArkNetConnCallbackImpl::NetAvailable()
{
    return real_->NetAvailable();
}

int32_t ArkNetConnCallbackImpl::NetCapabilitiesChange(const uint32_t& netConnectType, const uint32_t& netConnectSubtype)
{
    return real_->NetCapabilitiesChange(
        (NWeb::NetConnectType)netConnectType, (NWeb::NetConnectSubtype)netConnectSubtype);
}

int32_t ArkNetConnCallbackImpl::NetConnectionPropertiesChange()
{
    return real_->NetConnectionPropertiesChange();
}

int32_t ArkNetConnCallbackImpl::NetUnavailable()
{
    return real_->NetUnavailable();
}

int32_t ArkNetConnCallbackImpl::OnNetCapabilitiesChanged(
    const ArkWebRefPtr<ArkNetCapabilitiesAdapter> capabilites)
{
    if (CHECK_REF_PTR_IS_NULL(capabilites)) {
        return real_->OnNetCapabilitiesChanged(nullptr);
    }

    return real_->OnNetCapabilitiesChanged(
        std::make_shared<ArkNetCapabilitiesAdapterWrapper>(capabilites));
}

int32_t ArkNetConnCallbackImpl::OnNetConnectionPropertiesChanged(
    const ArkWebRefPtr<ArkNetConnectionPropertiesAdapter> properties)
{
    if (CHECK_REF_PTR_IS_NULL(properties)) {
        return real_->OnNetConnectionPropertiesChanged(nullptr);
    }

    return real_->OnNetConnectionPropertiesChanged(
        std::make_shared<ArkNetConnectionPropertiesAdapterWrapper>(properties));
}

} // namespace OHOS::ArkWeb
