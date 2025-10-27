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

#include "ohos_adapter/bridge/ark_mmi_device_info_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMMIDeviceInfoAdapterWrapper::ArkMMIDeviceInfoAdapterWrapper(ArkWebRefPtr<ArkMMIDeviceInfoAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkMMIDeviceInfoAdapterWrapper::GetId()
{
    return ctocpp_->GetId();
}

int32_t ArkMMIDeviceInfoAdapterWrapper::GetType()
{
    return ctocpp_->GetType();
}

int32_t ArkMMIDeviceInfoAdapterWrapper::GetBus()
{
    return ctocpp_->GetBus();
}

int32_t ArkMMIDeviceInfoAdapterWrapper::GetVersion()
{
    return ctocpp_->GetVersion();
}

int32_t ArkMMIDeviceInfoAdapterWrapper::GetProduct()
{
    return ctocpp_->GetProduct();
}

int32_t ArkMMIDeviceInfoAdapterWrapper::GetVendor()
{
    return ctocpp_->GetVendor();
}

std::string ArkMMIDeviceInfoAdapterWrapper::GetName()
{
    ArkWebString str = ctocpp_->GetName();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkMMIDeviceInfoAdapterWrapper::GetPhys()
{
    ArkWebString str = ctocpp_->GetPhys();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkMMIDeviceInfoAdapterWrapper::GetUniq()
{
    ArkWebString str = ctocpp_->GetUniq();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

void ArkMMIDeviceInfoAdapterWrapper::SetId(int32_t id)
{
    ctocpp_->SetId(id);
}

void ArkMMIDeviceInfoAdapterWrapper::SetType(int32_t type)
{
    ctocpp_->SetType(type);
}

void ArkMMIDeviceInfoAdapterWrapper::SetBus(int32_t bus)
{
    ctocpp_->SetBus(bus);
}

void ArkMMIDeviceInfoAdapterWrapper::SetVersion(int32_t version)
{
    ctocpp_->SetVersion(version);
}

void ArkMMIDeviceInfoAdapterWrapper::SetProduct(int32_t product)
{
    ctocpp_->SetProduct(product);
}

void ArkMMIDeviceInfoAdapterWrapper::SetVendor(int32_t vendor)
{
    ctocpp_->SetVendor(vendor);
}

void ArkMMIDeviceInfoAdapterWrapper::SetName(std::string name)
{
    ArkWebString str = ArkWebStringClassToStruct(name);
    ctocpp_->SetName(str);
    ArkWebStringStructRelease(str);
}

void ArkMMIDeviceInfoAdapterWrapper::SetPhys(std::string phys)
{
    ArkWebString str = ArkWebStringClassToStruct(phys);
    ctocpp_->SetPhys(str);
    ArkWebStringStructRelease(str);
}

void ArkMMIDeviceInfoAdapterWrapper::SetUniq(std::string uniq)
{
    ArkWebString str = ArkWebStringClassToStruct(uniq);
    ctocpp_->SetUniq(str);
    ArkWebStringStructRelease(str);
}

} // namespace OHOS::ArkWeb
