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

#include "audio_system_manager_adapter_impl.h"

namespace OHOS::NWeb {
AudioSystemManagerAdapterImpl& AudioSystemManagerAdapterImpl::GetInstance()
{
    static AudioSystemManagerAdapterImpl instance;
    return instance;
}

bool AudioSystemManagerAdapterImpl::HasAudioOutputDevices()
{
    return false;
}

bool AudioSystemManagerAdapterImpl::HasAudioInputDevices()
{
    return false;
}

int32_t AudioSystemManagerAdapterImpl::RequestAudioFocus(const std::shared_ptr<AudioInterruptAdapter> audioInterrupt)
{
    return -1;
}

int32_t AudioSystemManagerAdapterImpl::AbandonAudioFocus(const std::shared_ptr<AudioInterruptAdapter> audioInterrupt)
{
    return -1;
}

int32_t AudioSystemManagerAdapterImpl::SetAudioManagerInterruptCallback(
    std::shared_ptr<AudioManagerCallbackAdapter> callback)
{
    return -1;
}

int32_t AudioSystemManagerAdapterImpl::UnsetAudioManagerInterruptCallback()
{
    return -1;
}

std::vector<std::shared_ptr<AudioDeviceDescAdapter>> AudioSystemManagerAdapterImpl::GetDevices(AdapterDeviceFlag flag)
{
    return std::vector<std::shared_ptr<AudioDeviceDescAdapter>>();
}

int32_t AudioSystemManagerAdapterImpl::SelectAudioDeviceById(int32_t deviceId, bool isInput)
{
    return -1;
}

std::shared_ptr<AudioDeviceDescAdapter> AudioSystemManagerAdapterImpl::GetDefaultOutputDevice()
{
    return nullptr;
}

int32_t AudioSystemManagerAdapterImpl::SetDeviceChangeCallback(
    std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> callback)
{
    return -1;
}

int32_t AudioSystemManagerAdapterImpl::UnsetDeviceChangeCallback()
{
    return -1;
}

std::shared_ptr<AudioDeviceDescAdapter> AudioSystemManagerAdapterImpl::GetDefaultInputDevice()
{
    return nullptr;
}
} // namespace OHOS::NWeb
