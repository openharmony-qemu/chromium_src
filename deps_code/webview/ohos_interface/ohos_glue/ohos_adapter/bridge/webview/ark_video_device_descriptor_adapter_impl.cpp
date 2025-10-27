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

#include "ohos_adapter/bridge/ark_video_device_descriptor_adapter_impl.h"

#include "ohos_adapter/bridge/ark_video_control_support_adapter_impl.h"
#include "ohos_adapter/cpptoc/ark_format_adapter_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkVideoDeviceDescriptorAdapterImpl::ArkVideoDeviceDescriptorAdapterImpl(
    std::shared_ptr<OHOS::NWeb::VideoDeviceDescriptorAdapter> ref)
    : real_(ref)
{}

ArkWebString ArkVideoDeviceDescriptorAdapterImpl::GetDisplayName()
{
    std::string str = real_->GetDisplayName();
    return ArkWebStringClassToStruct(str);
}

ArkWebString ArkVideoDeviceDescriptorAdapterImpl::GetDeviceId()
{
    std::string str = real_->GetDeviceId();
    return ArkWebStringClassToStruct(str);
}

ArkWebString ArkVideoDeviceDescriptorAdapterImpl::GetModelId()
{
    std::string str = real_->GetModelId();
    return ArkWebStringClassToStruct(str);
}

ArkWebRefPtr<ArkVideoControlSupportAdapter> ArkVideoDeviceDescriptorAdapterImpl::GetControlSupport()
{
    std::shared_ptr<NWeb::VideoControlSupportAdapter> adapter = real_->GetControlSupport();
    if (CHECK_SHARED_PTR_IS_NULL(adapter)) {
        return nullptr;
    }
    return new ArkVideoControlSupportAdapterImpl(adapter);
}

int32_t ArkVideoDeviceDescriptorAdapterImpl::GetTransportType()
{
    return (int32_t)real_->GetTransportType();
}

int32_t ArkVideoDeviceDescriptorAdapterImpl::GetFacingMode()
{
    return (int32_t)real_->GetFacingMode();
}

ArkFormatAdapterVector ArkVideoDeviceDescriptorAdapterImpl::GetSupportCaptureFormats()
{
    std::vector<std::shared_ptr<NWeb::FormatAdapter>> format = real_->GetSupportCaptureFormats();
    ArkFormatAdapterVector result = ArkFormatAdapterVectorClassToStruct(format);
    return result;
}

} // namespace OHOS::ArkWeb
