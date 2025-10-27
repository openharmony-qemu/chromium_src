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

#ifndef ARK_AUDIO_CODEC_DECODER_ADAPTER_WRAPPER_H
#define ARK_AUDIO_CODEC_DECODER_ADAPTER_WRAPPER_H
#pragma once

#include "audio_codec_decoder_adapter.h"
#include "audio_cenc_info_adapter.h"
#include "ohos_adapter/include/ark_audio_codec_decoder_adapter.h"
#include "ohos_adapter/include/ark_audio_cenc_info_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioCodecDecoderAdapterWrapper : public OHOS::NWeb::AudioCodecDecoderAdapter {
public:
    explicit ArkAudioCodecDecoderAdapterWrapper(ArkWebRefPtr<ArkAudioCodecDecoderAdapter>);

    OHOS::NWeb::AudioDecoderAdapterCode CreateAudioDecoderByMime(const std::string& mimetype) override;

    OHOS::NWeb::AudioDecoderAdapterCode CreateAudioDecoderByName(const std::string& name) override;

    OHOS::NWeb::AudioDecoderAdapterCode ConfigureDecoder(
        const std::shared_ptr<OHOS::NWeb::AudioDecoderFormatAdapter> format) override;

    OHOS::NWeb::AudioDecoderAdapterCode SetParameterDecoder(
        const std::shared_ptr<OHOS::NWeb::AudioDecoderFormatAdapter> format) override;

    OHOS::NWeb::AudioDecoderAdapterCode PrepareDecoder() override;

    OHOS::NWeb::AudioDecoderAdapterCode StartDecoder() override;

    OHOS::NWeb::AudioDecoderAdapterCode StopDecoder() override;

    OHOS::NWeb::AudioDecoderAdapterCode FlushDecoder() override;

    OHOS::NWeb::AudioDecoderAdapterCode ResetDecoder() override;

    OHOS::NWeb::AudioDecoderAdapterCode ReleaseDecoder() override;

    OHOS::NWeb::AudioDecoderAdapterCode QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs,
        uint8_t *bufferData, int32_t bufferSize, const std::shared_ptr<OHOS::NWeb::AudioCencInfoAdapter> cencInfo,
        bool isEncrypted, NWeb::BufferFlag flag) override;

    OHOS::NWeb::AudioDecoderAdapterCode GetOutputFormatDec(
        std::shared_ptr<OHOS::NWeb::AudioDecoderFormatAdapter> format) override;

    OHOS::NWeb::AudioDecoderAdapterCode ReleaseOutputBufferDec(uint32_t index) override;

    OHOS::NWeb::AudioDecoderAdapterCode SetCallbackDec(
        const std::shared_ptr<OHOS::NWeb::AudioDecoderCallbackAdapter> callback) override;

    OHOS::NWeb::AudioDecoderAdapterCode SetDecryptionConfig(void *session, bool secureAudio) override;

private:
    ArkWebRefPtr<ArkAudioCodecDecoderAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CODEC_DECODER_ADAPTER_WRAPPER_H
