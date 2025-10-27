/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_AUDIO_DECODER_CALLBACK_ADAPTER_WRAPPER_H
#define ARK_AUDIO_DECODER_CALLBACK_ADAPTER_WRAPPER_H
#pragma once

#include "audio_codec_decoder_adapter.h"
#include "ohos_adapter/include/ark_audio_codec_decoder_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioDecoderCallbackAdapterWrapper : public NWeb::AudioDecoderCallbackAdapter {
public:
    explicit ArkAudioDecoderCallbackAdapterWrapper(ArkWebRefPtr<ArkAudioDecoderCallbackAdapter>);

    void OnError(int32_t errorCode) override;

    void OnOutputFormatChanged() override;

    void OnInputBufferAvailable(uint32_t index) override;

    void OnOutputBufferAvailable(
        uint32_t index, uint8_t *bufferData, int32_t size, int64_t pts, int32_t offset, uint32_t flags) override;

private:
    ArkWebRefPtr<ArkAudioDecoderCallbackAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_DECODER_CALLBACK_ADAPTER_WRAPPER_H