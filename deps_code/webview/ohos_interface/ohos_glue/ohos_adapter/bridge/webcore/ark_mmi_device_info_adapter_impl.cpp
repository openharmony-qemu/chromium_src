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

#include "ohos_adapter/bridge/ark_mmi_device_info_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkMMIDeviceInfoAdapterImpl::ArkMMIDeviceInfoAdapterImpl(std::shared_ptr<OHOS::NWeb::MMIDeviceInfoAdapter> ref)
    : real_(ref)
{}

int32_t ArkMMIDeviceInfoAdapterImpl::GetId()
{
    return real_->GetId();
}

int32_t ArkMMIDeviceInfoAdapterImpl::GetType()
{
    return real_->GetType();
}

int32_t ArkMMIDeviceInfoAdapterImpl::GetBus()
{
    return real_->GetBus();
}

int32_t ArkMMIDeviceInfoAdapterImpl::GetVersion()
{
    return real_->GetVersion();
}

int32_t ArkMMIDeviceInfoAdapterImpl::GetProduct()
{
    return real_->GetProduct();
}

int32_t ArkMMIDeviceInfoAdapterImpl::GetVendor()
{
    return real_->GetVendor();
}

ArkWebString ArkMMIDeviceInfoAdapterImpl::GetName()
{
    std::string str = real_->GetName();
    return ArkWebStringClassToStruct(str);
}

ArkWebString ArkMMIDeviceInfoAdapterImpl::GetPhys()
{
    std::string str = real_->GetPhys();
    return ArkWebStringClassToStruct(str);
}

ArkWebString ArkMMIDeviceInfoAdapterImpl::GetUniq()
{
    std::string str = real_->GetUniq();
    return ArkWebStringClassToStruct(str);
}

void ArkMMIDeviceInfoAdapterImpl::SetId(int32_t id)
{
    real_->SetId(id);
}

void ArkMMIDeviceInfoAdapterImpl::SetType(int32_t type)
{
    real_->SetType(type);
}

void ArkMMIDeviceInfoAdapterImpl::SetBus(int32_t bus)
{
    real_->SetBus(bus);
}

void ArkMMIDeviceInfoAdapterImpl::SetVersion(int32_t version)
{
    real_->SetVersion(version);
}

void ArkMMIDeviceInfoAdapterImpl::SetProduct(int32_t product)
{
    real_->SetProduct(product);
}

void ArkMMIDeviceInfoAdapterImpl::SetVendor(int32_t vendor)
{
    real_->SetVendor(vendor);
}

void ArkMMIDeviceInfoAdapterImpl::SetName(ArkWebString name)
{
    real_->SetName(ArkWebStringStructToClass(name));
}

void ArkMMIDeviceInfoAdapterImpl::SetPhys(ArkWebString phys)
{
    real_->SetPhys(ArkWebStringStructToClass(phys));
}

void ArkMMIDeviceInfoAdapterImpl::SetUniq(ArkWebString uniq)
{
    real_->SetUniq(ArkWebStringStructToClass(uniq));
}

} // namespace OHOS::ArkWeb
