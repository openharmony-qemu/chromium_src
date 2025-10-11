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

#include "enterprise_device_management_adapter_impl.h"

#include "nweb_log.h"

namespace OHOS::NWeb {
// static
EnterpriseDeviceManagementAdapterImpl& EnterpriseDeviceManagementAdapterImpl::GetInstance()
{
    static EnterpriseDeviceManagementAdapterImpl instance;
    return instance;
}

void EnterpriseDeviceManagementAdapterImpl::RegistPolicyChangeEventCallback(
    std::shared_ptr<EdmPolicyChangedEventCallbackAdapter> eventCallback)
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

bool EnterpriseDeviceManagementAdapterImpl::StartObservePolicyChange()
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return false;
}

bool EnterpriseDeviceManagementAdapterImpl::StopObservePolicyChange()
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return true;
}

int32_t EnterpriseDeviceManagementAdapterImpl::GetPolicies(std::string& policies)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    WVLOG_I("Enterprise device management not supported.");
    return -1;
}

} // namespace OHOS::NWeb
