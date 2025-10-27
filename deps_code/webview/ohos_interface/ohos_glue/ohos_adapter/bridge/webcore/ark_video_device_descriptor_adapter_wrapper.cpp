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

#include "ohos_adapter/bridge/ark_video_device_descriptor_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_video_control_support_adapter_wrapper.h"
#include "ohos_adapter/ctocpp/ark_format_adapter_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkVideoDeviceDescriptorAdapterWrapper::ArkVideoDeviceDescriptorAdapterWrapper(
    ArkWebRefPtr<ArkVideoDeviceDescriptorAdapter> ref)
    : ctocpp_(ref)
{}

std::string ArkVideoDeviceDescriptorAdapterWrapper::GetDisplayName()
{
    ArkWebString str = ctocpp_->GetDisplayName();
    std::string result = ArkWebStringStructToClass(str);

    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkVideoDeviceDescriptorAdapterWrapper::GetDeviceId()
{
    ArkWebString str = ctocpp_->GetDeviceId();
    std::string result = ArkWebStringStructToClass(str);

    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkVideoDeviceDescriptorAdapterWrapper::GetModelId()
{
    ArkWebString str = ctocpp_->GetModelId();
    std::string result = ArkWebStringStructToClass(str);

    ArkWebStringStructRelease(str);
    return result;
}

std::shared_ptr<NWeb::VideoControlSupportAdapter> ArkVideoDeviceDescriptorAdapterWrapper::GetControlSupport()
{
    ArkWebRefPtr<ArkVideoControlSupportAdapter> adapter = ctocpp_->GetControlSupport();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }
    return std::make_shared<ArkVideoControlSupportAdapterWrapper>(adapter);
}

NWeb::VideoTransportType ArkVideoDeviceDescriptorAdapterWrapper::GetTransportType()
{
    return (NWeb::VideoTransportType)ctocpp_->GetTransportType();
}

NWeb::VideoFacingModeAdapter ArkVideoDeviceDescriptorAdapterWrapper::GetFacingMode()
{
    return (NWeb::VideoFacingModeAdapter)ctocpp_->GetFacingMode();
}

std::vector<std::shared_ptr<NWeb::FormatAdapter>> ArkVideoDeviceDescriptorAdapterWrapper::GetSupportCaptureFormats()
{
    ArkFormatAdapterVector ark_vector = ctocpp_->GetSupportCaptureFormats();
    std::vector<std::shared_ptr<NWeb::FormatAdapter>> result = ArkFormatAdapterVectorStructToClass(ark_vector);
    ArkFormatAdapterVectorStructRelease(ark_vector);
    return result;
}

} // namespace OHOS::ArkWeb
