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

#ifndef ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_WRAPPER_H
#define ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_WRAPPER_H
#pragma once

#include "audio_system_manager_adapter.h"
#include "ohos_adapter/include/ark_audio_system_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioSystemManagerAdapterWrapper : public OHOS::NWeb::AudioSystemManagerAdapter {
public:
    explicit ArkAudioSystemManagerAdapterWrapper(ArkWebRefPtr<ArkAudioSystemManagerAdapter>);

    bool HasAudioOutputDevices() override;

    bool HasAudioInputDevices() override;

    int32_t RequestAudioFocus(const std::shared_ptr<NWeb::AudioInterruptAdapter> audioInterrupt) override;

    int32_t AbandonAudioFocus(const std::shared_ptr<NWeb::AudioInterruptAdapter> audioInterrupt) override;

    int32_t SetAudioManagerInterruptCallback(std::shared_ptr<NWeb::AudioManagerCallbackAdapter> callback) override;

    int32_t UnsetAudioManagerInterruptCallback() override;

    std::vector<std::shared_ptr<NWeb::AudioDeviceDescAdapter>> GetDevices(NWeb::AdapterDeviceFlag flag) override;

    int32_t SelectAudioDeviceById(int32_t deviceId, bool isInput) override;

    int32_t SetDeviceChangeCallback(std::shared_ptr<NWeb::AudioManagerDeviceChangeCallbackAdapter> callback) override;

    int32_t UnsetDeviceChangeCallback() override;

    std::shared_ptr<NWeb::AudioDeviceDescAdapter> GetDefaultOutputDevice() override;

    std::shared_ptr<NWeb::AudioDeviceDescAdapter> GetDefaultInputDevice() override;

    bool SetLanguage(const std::string& language) override;

private:
    ArkWebRefPtr<ArkAudioSystemManagerAdapter> ctocpp_;
};
} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_RENDERER_ADAPTER_WRAPPER_H
