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

#ifndef ARK_AUDIO_MANAGER_DEVICE_CHANGE_CALLBACK_ADAPTER_WRAPPER_H
#define ARK_AUDIO_MANAGER_DEVICE_CHANGE_CALLBACK_ADAPTER_WRAPPER_H
#pragma once

#include "audio_system_manager_adapter.h"
#include "ohos_adapter/include/ark_audio_system_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioManagerDeviceChangeCallbackAdapterWrapper : public OHOS::NWeb::AudioManagerDeviceChangeCallbackAdapter {
public:
    explicit ArkAudioManagerDeviceChangeCallbackAdapterWrapper(
        ArkWebRefPtr<ArkAudioManagerDeviceChangeCallbackAdapter>);

    void OnDeviceChange() override;

private:
    ArkWebRefPtr<ArkAudioManagerDeviceChangeCallbackAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_MANAGER_DEVICE_CHANGE_CALLBACK_ADAPTER_WRAPPER_H
