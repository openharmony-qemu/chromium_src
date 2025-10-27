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

#include "video_device_descriptor_adapter_impl.h"

namespace OHOS::NWeb {

std::string VideoDeviceDescriptorAdapterImpl::GetDisplayName()
{
    return display_name_;
}

std::string VideoDeviceDescriptorAdapterImpl::GetDeviceId()
{
    return device_id_;
}

std::string VideoDeviceDescriptorAdapterImpl::GetModelId()
{
    return model_id_;
}

std::shared_ptr<VideoControlSupportAdapter> VideoDeviceDescriptorAdapterImpl::GetControlSupport()
{
    return control_support_;
}

VideoTransportType VideoDeviceDescriptorAdapterImpl::GetTransportType()
{
    return transport_type_;
}

VideoFacingModeAdapter VideoDeviceDescriptorAdapterImpl::GetFacingMode()
{
    return facing_;
}

std::vector<std::shared_ptr<FormatAdapter>> VideoDeviceDescriptorAdapterImpl::GetSupportCaptureFormats()
{
    return support_formats_;
}

void VideoDeviceDescriptorAdapterImpl::SetDisplayName(std::string name)
{
    display_name_ = name;
}

void VideoDeviceDescriptorAdapterImpl::SetDeviceId(std::string deviceId)
{
    device_id_ = deviceId;
}

void VideoDeviceDescriptorAdapterImpl::SetModelId(std::string modelId)
{
    model_id_ = modelId;
}

void VideoDeviceDescriptorAdapterImpl::SetControlSupport(std::shared_ptr<VideoControlSupportAdapter> support)
{
    control_support_ = support;
}

void VideoDeviceDescriptorAdapterImpl::SetTransportType(VideoTransportType type)
{
    transport_type_ = type;
}

void VideoDeviceDescriptorAdapterImpl::SetFacingMode(VideoFacingModeAdapter facing)
{
    facing_ = facing;
}

void VideoDeviceDescriptorAdapterImpl::SetSupportCaptureFormats(std::vector<std::shared_ptr<FormatAdapter>> formats)
{
    support_formats_ = formats;
}

} // namespace OHOS::NWeb
