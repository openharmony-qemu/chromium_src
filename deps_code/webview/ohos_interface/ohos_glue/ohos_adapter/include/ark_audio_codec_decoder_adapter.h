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

#ifndef ARK_AUDIO_CODEC_DECODER_ADAPTER_H
#define ARK_AUDIO_CODEC_DECODER_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"
#include "ohos_adapter/include/ark_audio_cenc_info_adapter.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkAudioDecoderFormatAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetSampleRate() = 0;

    /*--ark web()--*/
    virtual int32_t GetChannelCount() = 0;

    /*--ark web()--*/
    virtual int64_t GetBitRate() = 0;

    /*--ark web()--*/
    virtual int32_t GetMaxInputSize() = 0;

    /*--ark web()--*/
    virtual bool GetAACIsAdts() = 0;

    /*--ark web()--*/
    virtual int32_t GetAudioSampleFormat() = 0;

    /*--ark web()--*/
    virtual int32_t GetIdentificationHeader() = 0;

    /*--ark web()--*/
    virtual int32_t GetSetupHeader() = 0;

    /*--ark web()--*/
    virtual uint8_t* GetCodecConfig() = 0;

    /*--ark web()--*/
    virtual uint32_t GetCodecConfigSize() = 0;

    /*--ark web()--*/
    virtual void SetSampleRate(int32_t sampleRate) = 0;

    /*--ark web()--*/
    virtual void SetChannelCount(int32_t channelCount) = 0;

    /*--ark web()--*/
    virtual void SetBitRate(int64_t bitRate) = 0;

    /*--ark web()--*/
    virtual void SetMaxInputSize(int32_t maxInputSize) = 0;

    /*--ark web()--*/
    virtual void SetAACIsAdts(bool isAdts) = 0;

    /*--ark web()--*/
    virtual void SetAudioSampleFormat(int32_t audioSampleFormat) = 0;

    /*--ark web()--*/
    virtual void SetIdentificationHeader(int32_t data) = 0;

    /*--ark web()--*/
    virtual void SetSetupHeader(int32_t data) = 0;

    /*--ark web()--*/
    virtual void SetCodecConfig(uint8_t* codecConfig) = 0;

    /*--ark web()--*/
    virtual void SetCodecConfigSize(uint32_t size) = 0;
};

/*--ark web(source=webcore)--*/
class ArkAudioDecoderCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnError(int32_t errorCode) = 0;

    /*--ark web()--*/
    virtual void OnOutputFormatChanged() = 0;

    /*--ark web()--*/
    virtual void OnInputBufferAvailable(uint32_t index) = 0;

    /*--ark web()--*/
    virtual void OnOutputBufferAvailable(
        uint32_t index, uint8_t *bufferData, int32_t size, int64_t pts, int32_t offset, uint32_t flags) = 0;
};

/*--ark web(source=webview)--*/
class ArkAudioCodecDecoderAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t CreateAudioDecoderByMime(const ArkWebString& mimetype) = 0;

    /*--ark web()--*/
    virtual int32_t CreateAudioDecoderByName(const ArkWebString& name) = 0;

    /*--ark web()--*/
    virtual int32_t ConfigureDecoder(const ArkWebRefPtr<ArkAudioDecoderFormatAdapter> format) = 0;

    /*--ark web()--*/
    virtual int32_t SetParameterDecoder(const ArkWebRefPtr<ArkAudioDecoderFormatAdapter> format) = 0;

    /*--ark web()--*/
    virtual int32_t PrepareDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t StartDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t StopDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t FlushDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t ResetDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs, uint8_t *bufferData,
                                        int32_t bufferSize, const ArkWebRefPtr<ArkAudioCencInfoAdapter> cencInfo,
                                        bool isEncrypted, uint32_t flag) = 0;

    /*--ark web()--*/
    virtual int32_t GetOutputFormatDec(ArkWebRefPtr<ArkAudioDecoderFormatAdapter> format) = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseOutputBufferDec(uint32_t index) = 0;

    /*--ark web()--*/
    virtual int32_t SetCallbackDec(const ArkWebRefPtr<ArkAudioDecoderCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual int32_t SetDecryptionConfig(void *session, bool secureAudio) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CODEC_DECODER_ADAPTER_H
