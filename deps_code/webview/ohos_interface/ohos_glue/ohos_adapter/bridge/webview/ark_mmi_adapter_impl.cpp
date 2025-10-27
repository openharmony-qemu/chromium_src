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

#include "ohos_adapter/bridge/ark_mmi_adapter_impl.h"

#include "ohos_adapter/bridge/ark_mmi_device_info_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_mmi_input_listener_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_mmi_listener_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMMIAdapterImpl::ArkMMIAdapterImpl(std::shared_ptr<OHOS::NWeb::MMIAdapter> ref) : real_(ref) {}

char* ArkMMIAdapterImpl::KeyCodeToString(int32_t keyCode)
{
    return real_->KeyCodeToString(keyCode);
}

int32_t ArkMMIAdapterImpl::RegisterMMIInputListener(ArkWebRefPtr<ArkMMIInputListenerAdapter> eventCallback)
{
    if (CHECK_REF_PTR_IS_NULL(eventCallback)) {
        return real_->RegisterMMIInputListener(nullptr);
    }

    return real_->RegisterMMIInputListener(std::make_shared<ArkMMIInputListenerAdapterWrapper>(eventCallback));
}

void ArkMMIAdapterImpl::UnregisterMMIInputListener(int32_t monitorId)
{
    real_->UnregisterMMIInputListener(monitorId);
}

int32_t ArkMMIAdapterImpl::RegisterDevListener(ArkWebString type, ArkWebRefPtr<ArkMMIListenerAdapter> listener)
{
    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->RegisterDevListener(ArkWebStringStructToClass(type), nullptr);
    }

    return real_->RegisterDevListener(
        ArkWebStringStructToClass(type), std::make_shared<ArkMMIListenerAdapterWrapper>(listener));
}

int32_t ArkMMIAdapterImpl::UnregisterDevListener(ArkWebString type)
{
    return real_->UnregisterDevListener(ArkWebStringStructToClass(type));
}

int32_t ArkMMIAdapterImpl::GetKeyboardType(int32_t deviceId, int32_t& type)
{
    return real_->GetKeyboardType(deviceId, type);
}

int32_t ArkMMIAdapterImpl::GetDeviceIds(ArkWebInt32Vector& ids)
{
    std::vector<int32_t> vec;
    int result = real_->GetDeviceIds(vec);
    ids = ArkWebBasicVectorClassToStruct<int32_t, ArkWebInt32Vector>(vec);
    return result;
}

int32_t ArkMMIAdapterImpl::GetDeviceInfo(int32_t deviceId, ArkWebRefPtr<ArkMMIDeviceInfoAdapter> info)
{
    if (CHECK_REF_PTR_IS_NULL(info)) {
        return real_->GetDeviceInfo(deviceId, nullptr);
    }
    return real_->GetDeviceInfo(deviceId, std::make_shared<ArkMMIDeviceInfoAdapterWrapper>(info));
}

int32_t ArkMMIAdapterImpl::GetMaxTouchPoints()
{
    return real_->GetMaxTouchPoints();
}

} // namespace OHOS::ArkWeb
