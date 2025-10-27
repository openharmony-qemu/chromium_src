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

#ifndef ARK_AUDIO_CODEC_DECODER_ADAPTER_IMPL_H
#define ARK_AUDIO_CODEC_DECODER_ADAPTER_IMPL_H
#pragma once

#include "audio_codec_decoder_adapter.h"
#include "ohos_adapter/include/ark_audio_codec_decoder_adapter.h"
#include "audio_cenc_info_adapter.h"
#include "ohos_adapter/include/ark_audio_cenc_info_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioCodecDecoderAdapterImpl : public ArkAudioCodecDecoderAdapter {
public:
    explicit ArkAudioCodecDecoderAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioCodecDecoderAdapter>);

    int32_t CreateAudioDecoderByMime(const ArkWebString& mimetype) override;

    int32_t CreateAudioDecoderByName(const ArkWebString& name) override;

    int32_t ConfigureDecoder(const ArkWebRefPtr<ArkAudioDecoderFormatAdapter> format) override;

    int32_t SetParameterDecoder(const ArkWebRefPtr<ArkAudioDecoderFormatAdapter> format) override;

    int32_t PrepareDecoder() override;

    int32_t StartDecoder() override;

    int32_t StopDecoder() override;

    int32_t FlushDecoder() override;

    int32_t ResetDecoder() override;

    int32_t ReleaseDecoder() override;

    int32_t QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs, uint8_t *bufferData, int32_t bufferSize,
        const ArkWebRefPtr<ArkAudioCencInfoAdapter> cencInfo, bool isEncrypted, uint32_t flag) override;

    int32_t GetOutputFormatDec(ArkWebRefPtr<ArkAudioDecoderFormatAdapter> format) override;

    int32_t ReleaseOutputBufferDec(uint32_t index) override;

    int32_t SetCallbackDec(const ArkWebRefPtr<ArkAudioDecoderCallbackAdapter> callback) override;

    int32_t SetDecryptionConfig(void *session, bool secureAudio) override;

private:
    std::shared_ptr<OHOS::NWeb::AudioCodecDecoderAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkAudioCodecDecoderAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CODEC_DECODER_ADAPTER_IMPL_H
