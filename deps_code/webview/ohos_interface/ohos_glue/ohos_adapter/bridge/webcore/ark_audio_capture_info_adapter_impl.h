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

#ifndef ARK_AUDIO_CAPTURE_INFO_ADAPTER_IMPL_H
#define ARK_AUDIO_CAPTURE_INFO_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_screen_capture_adapter.h"
#include "screen_capture_adapter.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkAudioCaptureInfoAdapterImpl : public ArkAudioCaptureInfoAdapter {
public:
    explicit ArkAudioCaptureInfoAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioCaptureInfoAdapter>);

    int32_t GetAudioSampleRate() override;

    int32_t GetAudioChannels() override;

    int32_t GetAudioSource() override;

private:
    std::shared_ptr<OHOS::NWeb::AudioCaptureInfoAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkAudioCaptureInfoAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CAPTURE_INFO_ADAPTER_IMPL_H
