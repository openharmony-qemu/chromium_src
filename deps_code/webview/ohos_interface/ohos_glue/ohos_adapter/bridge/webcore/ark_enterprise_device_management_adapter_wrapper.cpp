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

#include "ohos_adapter/bridge/ark_enterprise_device_management_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_edm_policy_changed_event_callback_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkEnterpriseDeviceManagementAdapterWrapper::ArkEnterpriseDeviceManagementAdapterWrapper(
    ArkWebRefPtr<ArkEnterpriseDeviceManagementAdapter> ref)
    : ctocpp_(ref)
{}

int32_t ArkEnterpriseDeviceManagementAdapterWrapper::GetPolicies(std::string& policies)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString str = ArkWebStringClassToStruct(policies);
    int32_t result = ctocpp_->GetPolicies(str);
    policies = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

void ArkEnterpriseDeviceManagementAdapterWrapper::RegistPolicyChangeEventCallback(
    std::shared_ptr<NWeb::EdmPolicyChangedEventCallbackAdapter> eventCallback)
{
    if (!ctocpp_) {
        return;
    }
    if (CHECK_SHARED_PTR_IS_NULL(eventCallback)) {
        return ctocpp_->RegistPolicyChangeEventCallback(nullptr);
    }

    ctocpp_->RegistPolicyChangeEventCallback(new ArkEdmPolicyChangedEventCallbackAdapterImpl(eventCallback));
}

bool ArkEnterpriseDeviceManagementAdapterWrapper::StartObservePolicyChange()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->StartObservePolicyChange();
}

bool ArkEnterpriseDeviceManagementAdapterWrapper::StopObservePolicyChange()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->StopObservePolicyChange();
}

} // namespace OHOS::ArkWeb
