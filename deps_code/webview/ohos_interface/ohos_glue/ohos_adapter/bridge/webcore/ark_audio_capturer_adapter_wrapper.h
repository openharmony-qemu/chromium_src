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

#ifndef ARK_AUDIO_CAPTURE_ADAPTER_WRAPPER_H
#define ARK_AUDIO_CAPTURE_ADAPTER_WRAPPER_H
#pragma once

#include "audio_capturer_adapter.h"
#include "ohos_adapter/include/ark_audio_capturer_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioCapturerAdapterWrapper : public OHOS::NWeb::AudioCapturerAdapter {
public:
    explicit ArkAudioCapturerAdapterWrapper(ArkWebRefPtr<ArkAudioCapturerAdapter>);

    int32_t Create(const std::shared_ptr<NWeb::AudioCapturerOptionsAdapter> capturerOptions,
        std::string cachePath = std::string()) override;

    bool Start() override;

    bool Stop() override;

    bool Release() override;

    int32_t SetCapturerReadCallback(std::shared_ptr<NWeb::AudioCapturerReadCallbackAdapter> callbck) override;

    int32_t GetBufferDesc(std::shared_ptr<NWeb::BufferDescAdapter> bufferDesc) override;

    int32_t Enqueue(const std::shared_ptr<NWeb::BufferDescAdapter> bufferDesc) override;

    int32_t GetFrameCount(uint32_t& frameCount) override;

    int64_t GetAudioTime() override;

private:
    ArkWebRefPtr<ArkAudioCapturerAdapter> ctocpp_;
};
} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CAPTURE_ADAPTER_WRAPPER_H
