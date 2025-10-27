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

#ifndef ARK_AUDIO_DECODER_FORMAT_ADAPTER_WRAPPER_H
#define ARK_AUDIO_DECODER_FORMAT_ADAPTER_WRAPPER_H
#pragma once

#include "audio_codec_decoder_adapter.h"
#include "ohos_adapter/include/ark_audio_codec_decoder_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioDecoderFormatAdapterWrapper : public NWeb::AudioDecoderFormatAdapter {
public:
    explicit ArkAudioDecoderFormatAdapterWrapper(ArkWebRefPtr<ArkAudioDecoderFormatAdapter>);

    int32_t GetSampleRate() override;

    int32_t GetChannelCount() override;

    int64_t GetBitRate() override;

    int32_t GetMaxInputSize() override;

    bool GetAACIsAdts() override;

    int32_t GetAudioSampleFormat() override;

    int32_t GetIdentificationHeader() override;

    int32_t GetSetupHeader() override;

    uint8_t* GetCodecConfig() override;

    uint32_t GetCodecConfigSize() override;

    void SetSampleRate(int32_t sampleRate) override;

    void SetChannelCount(int32_t channelCount) override;

    void SetBitRate(int64_t bitRate) override;

    void SetMaxInputSize(int32_t maxInputSize) override;

    void SetAACIsAdts(bool isAdts) override;

    void SetAudioSampleFormat(int32_t audioSampleFormat) override;

    void SetIdentificationHeader(int32_t idHeader) override;

    void SetSetupHeader(int32_t setupHeader) override;

    void SetCodecConfig(uint8_t* codecConfig) override;

    void SetCodecConfigSize(uint32_t size) override;

private:
    ArkWebRefPtr<ArkAudioDecoderFormatAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_DECODER_FORMAT_ADAPTER_WRAPPER_H
