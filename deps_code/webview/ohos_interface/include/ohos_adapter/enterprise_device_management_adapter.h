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

#ifndef ENTERPRISE_DEVICE_MANAGEMENT_ADAPTER_H
#define ENTERPRISE_DEVICE_MANAGEMENT_ADAPTER_H

#include <memory>
#include <string>

namespace OHOS::NWeb {

class EdmPolicyChangedEventCallbackAdapter {
public:
    EdmPolicyChangedEventCallbackAdapter() = default;
    virtual ~EdmPolicyChangedEventCallbackAdapter() = default;

    virtual void Changed() = 0;
};

class EnterpriseDeviceManagementAdapter {
public:
    EnterpriseDeviceManagementAdapter() = default;
    virtual ~EnterpriseDeviceManagementAdapter() = default;
    virtual int32_t GetPolicies(std::string& policies) = 0;

    virtual void RegistPolicyChangeEventCallback(
        std::shared_ptr<EdmPolicyChangedEventCallbackAdapter> eventCallback) = 0;
    virtual bool StartObservePolicyChange() = 0;
    virtual bool StopObservePolicyChange() = 0;
};

} // namespace OHOS::NWeb

#endif // ENTERPRISE_DEVICE_MANAGEMENT_ADAPTER_H
