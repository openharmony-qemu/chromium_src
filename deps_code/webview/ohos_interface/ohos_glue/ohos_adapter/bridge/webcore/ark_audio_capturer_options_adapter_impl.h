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

#ifndef ARK_AUDIO_CAPTURER_OPTIONS_ADAPTER_IMPL_H
#define ARK_AUDIO_CAPTURER_OPTIONS_ADAPTER_IMPL_H
#pragma once

#include "audio_capturer_adapter.h"
#include "ohos_adapter/include/ark_audio_capturer_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioCapturerOptionsAdapterImpl : public ArkAudioCapturerOptionsAdapter {
public:
    explicit ArkAudioCapturerOptionsAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioCapturerOptionsAdapter>);

    int32_t GetSamplingRate() override;

    int32_t GetEncoding() override;

    int32_t GetSampleFormat() override;

    int32_t GetChannels() override;

    int32_t GetSourceType() override;

    int32_t GetCapturerFlags() override;

private:
    std::shared_ptr<OHOS::NWeb::AudioCapturerOptionsAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkAudioCapturerOptionsAdapterImpl);
};
} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CAPTURER_OPTIONS_ADAPTER_IMPL_H
