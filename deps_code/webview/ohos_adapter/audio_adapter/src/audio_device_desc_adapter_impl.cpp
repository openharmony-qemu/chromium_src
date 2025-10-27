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

#include "audio_device_desc_adapter_impl.h"

namespace OHOS::NWeb {

int32_t AudioDeviceDescAdapterImpl::GetDeviceId()
{
    return device_id_;
}

std::string AudioDeviceDescAdapterImpl::GetDeviceName()
{
    return device_name_;
}

void AudioDeviceDescAdapterImpl::SetDeviceId(int32_t id)
{
    device_id_ = id;
}

void AudioDeviceDescAdapterImpl::SetDeviceName(std::string name)
{
    device_name_ = name;
}

} // namespace OHOS::NWeb
