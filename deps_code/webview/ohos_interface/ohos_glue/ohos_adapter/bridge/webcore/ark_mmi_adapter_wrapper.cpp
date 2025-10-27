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

#include "ohos_adapter/bridge/ark_mmi_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_mmi_device_info_adapter_impl.h"
#include "ohos_adapter/bridge/ark_mmi_input_listener_adapter_impl.h"
#include "ohos_adapter/bridge/ark_mmi_listener_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMMIAdapterWrapper::ArkMMIAdapterWrapper(ArkWebRefPtr<ArkMMIAdapter> ref) : ctocpp_(ref) {}

char* ArkMMIAdapterWrapper::KeyCodeToString(int32_t keyCode)
{
    return ctocpp_->KeyCodeToString(keyCode);
}

int32_t ArkMMIAdapterWrapper::RegisterMMIInputListener(std::shared_ptr<NWeb::MMIInputListenerAdapter> eventCallback)
{
    if (CHECK_SHARED_PTR_IS_NULL(eventCallback)) {
        return ctocpp_->RegisterMMIInputListener(nullptr);
    }

    return ctocpp_->RegisterMMIInputListener(new ArkMMIInputListenerAdapterImpl(eventCallback));
}

void ArkMMIAdapterWrapper::UnregisterMMIInputListener(int32_t monitorId)
{
    return ctocpp_->UnregisterMMIInputListener(monitorId);
}

int32_t ArkMMIAdapterWrapper::RegisterDevListener(std::string type, std::shared_ptr<NWeb::MMIListenerAdapter> listener)
{
    ArkWebString str = ArkWebStringClassToStruct(type);
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        result = ctocpp_->RegisterDevListener(str, nullptr);
    } else {
        result = ctocpp_->RegisterDevListener(str, new ArkMMIListenerAdapterImpl(listener));
    }
    ArkWebStringStructRelease(str);
    return result;
}

int32_t ArkMMIAdapterWrapper::UnregisterDevListener(std::string type)
{
    ArkWebString str = ArkWebStringClassToStruct(type);
    int result = ctocpp_->UnregisterDevListener(str);
    ArkWebStringStructRelease(str);
    return result;
}

int32_t ArkMMIAdapterWrapper::GetKeyboardType(int32_t deviceId, int32_t& type)
{
    return ctocpp_->GetKeyboardType(deviceId, type);
}

int32_t ArkMMIAdapterWrapper::GetDeviceIds(std::vector<int32_t>& ids)
{
    ArkWebInt32Vector vec;
    int result = ctocpp_->GetDeviceIds(vec);
    ids = ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>(vec);
    ArkWebBasicVectorStructRelease(vec);
    return result;
}

int32_t ArkMMIAdapterWrapper::GetDeviceInfo(int32_t deviceId, std::shared_ptr<NWeb::MMIDeviceInfoAdapter> info)
{
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return ctocpp_->GetDeviceInfo(deviceId, nullptr);
    }
    return ctocpp_->GetDeviceInfo(deviceId, new ArkMMIDeviceInfoAdapterImpl(info));
}

int32_t ArkMMIAdapterWrapper::GetMaxTouchPoints()
{
    return ctocpp_->GetMaxTouchPoints();
}

} // namespace OHOS::ArkWeb
