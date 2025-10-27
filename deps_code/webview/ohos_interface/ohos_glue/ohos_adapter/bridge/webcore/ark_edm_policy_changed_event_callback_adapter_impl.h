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

#ifndef ARK_EDM_POLICY_CHANGED_EVENT_CALLBACK_ADAPTER_IMPL_H
#define ARK_EDM_POLICY_CHANGED_EVENT_CALLBACK_ADAPTER_IMPL_H
#pragma once

#include "enterprise_device_management_adapter.h"
#include "ohos_adapter/include/ark_enterprise_device_management_adapter.h"

namespace OHOS::ArkWeb {

class ArkEdmPolicyChangedEventCallbackAdapterImpl : public ArkEdmPolicyChangedEventCallbackAdapter {
public:
    explicit ArkEdmPolicyChangedEventCallbackAdapterImpl(
        std::shared_ptr<OHOS::NWeb::EdmPolicyChangedEventCallbackAdapter>);

    void Changed() override;

private:
    std::shared_ptr<OHOS::NWeb::EdmPolicyChangedEventCallbackAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkEdmPolicyChangedEventCallbackAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_EDM_POLICY_CHANGED_EVENT_CALLBACK_ADAPTER_IMPL_H
