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

#include "ohos_adapter/bridge/ark_enterprise_device_management_adapter_impl.h"

#include "ohos_adapter/bridge/ark_edm_policy_changed_event_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkEnterpriseDeviceManagementAdapterImpl::ArkEnterpriseDeviceManagementAdapterImpl(
    NWeb::EnterpriseDeviceManagementAdapter& ref)
    : real_(ref)
{}

int32_t ArkEnterpriseDeviceManagementAdapterImpl::GetPolicies(ArkWebString& policies)
{
    std::string str;
    int32_t result = real_.GetPolicies(str);
    policies = ArkWebStringClassToStruct(str);
    return result;
}

void ArkEnterpriseDeviceManagementAdapterImpl::RegistPolicyChangeEventCallback(
    ArkWebRefPtr<ArkEdmPolicyChangedEventCallbackAdapter> eventCallback)
{
    if (CHECK_REF_PTR_IS_NULL(eventCallback)) {
        return real_.RegistPolicyChangeEventCallback(nullptr);
    }

    real_.RegistPolicyChangeEventCallback(
        std::make_shared<ArkEdmPolicyChangedEventCallbackAdapterWrapper>(eventCallback));
}

bool ArkEnterpriseDeviceManagementAdapterImpl::StartObservePolicyChange()
{
    return real_.StartObservePolicyChange();
}

bool ArkEnterpriseDeviceManagementAdapterImpl::StopObservePolicyChange()
{
    return real_.StopObservePolicyChange();
}

} // namespace OHOS::ArkWeb
