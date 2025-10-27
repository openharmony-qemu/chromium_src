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
    return "";
}

std::string VideoDeviceDescriptorAdapterImpl::GetDeviceId()
{
    return "";
}

std::string VideoDeviceDescriptorAdapterImpl::GetModelId()
{
    return "";
}

std::shared_ptr<VideoControlSupportAdapter> VideoDeviceDescriptorAdapterImpl::GetControlSupport()
{
    return nullptr;
}

VideoTransportType VideoDeviceDescriptorAdapterImpl::GetTransportType()
{
    return VideoTransportType::VIDEO_TRANS_TYPE_OTHER;
}

VideoFacingModeAdapter VideoDeviceDescriptorAdapterImpl::GetFacingMode()
{
    return VideoFacingModeAdapter::NUM_FACING_MODES;
}

std::vector<std::shared_ptr<FormatAdapter>> VideoDeviceDescriptorAdapterImpl::GetSupportCaptureFormats()
{
    return std::vector<std::shared_ptr<FormatAdapter>>();
}

void VideoDeviceDescriptorAdapterImpl::SetDisplayName(std::string name)
{
}

void VideoDeviceDescriptorAdapterImpl::SetDeviceId(std::string deviceId)
{
}

void VideoDeviceDescriptorAdapterImpl::SetModelId(std::string modelId)
{
}

void VideoDeviceDescriptorAdapterImpl::SetControlSupport(std::shared_ptr<VideoControlSupportAdapter> support)
{
}

void VideoDeviceDescriptorAdapterImpl::SetTransportType(VideoTransportType type)
{
}

void VideoDeviceDescriptorAdapterImpl::SetFacingMode(VideoFacingModeAdapter facing)
{
}

void VideoDeviceDescriptorAdapterImpl::SetSupportCaptureFormats(std::vector<std::shared_ptr<FormatAdapter>> formats)
{
}

} // namespace OHOS::NWeb
