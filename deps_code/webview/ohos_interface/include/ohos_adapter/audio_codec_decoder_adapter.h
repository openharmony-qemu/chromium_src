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

#ifndef AUDIO_CODEC_DECODER_ADAPTER_H
#define AUDIO_CODEC_DECODER_ADAPTER_H

#include <vector>
#include <memory>
#include <cstdint>
#include <string>
#include "media_codec_adapter.h"
#include "audio_cenc_info_adapter.h"

namespace OHOS::NWeb {

enum class AudioDecoderAdapterCode : int32_t {
    DECODER_OK = 0,
    DECODER_ERROR = 1,
    DECODER_RETRY = 2
};

class AudioDecoderFormatAdapter {
public:
    AudioDecoderFormatAdapter() = default;

    virtual ~AudioDecoderFormatAdapter() = default;

    virtual int32_t GetSampleRate() = 0;

    virtual int32_t GetChannelCount() = 0;

    virtual int64_t GetBitRate() = 0;

    virtual int32_t GetMaxInputSize() = 0;

    virtual bool GetAACIsAdts() = 0;

    virtual int32_t GetAudioSampleFormat() = 0;

    virtual int32_t GetIdentificationHeader() = 0;

    virtual int32_t GetSetupHeader() = 0;

    virtual uint8_t* GetCodecConfig() = 0;

    virtual uint32_t GetCodecConfigSize() = 0;

    virtual void SetSampleRate(int32_t sampleRate) = 0;

    virtual void SetChannelCount(int32_t channelCount) = 0;

    virtual void SetBitRate(int64_t bitRate) = 0;

    virtual void SetMaxInputSize(int32_t maxInputSize) = 0;

    virtual void SetAACIsAdts(bool isAdts) = 0;

    virtual void SetAudioSampleFormat(int32_t audioSampleFormat) = 0;

    virtual void SetIdentificationHeader(int32_t idHeader) = 0;

    virtual void SetSetupHeader(int32_t setupHeader) = 0;

    virtual void SetCodecConfig(uint8_t* codecConfig) = 0;

    virtual void SetCodecConfigSize(uint32_t size) = 0;
};

class AudioDecoderCallbackAdapter {
public:
    AudioDecoderCallbackAdapter() = default;

    virtual ~AudioDecoderCallbackAdapter() = default;

    virtual void OnError(int32_t errorCode) = 0;

    virtual void OnOutputFormatChanged() = 0;

    virtual void OnInputBufferAvailable(uint32_t index) = 0;

    virtual void OnOutputBufferAvailable(
        uint32_t index, uint8_t *bufferData, int32_t size, int64_t pts, int32_t offset, uint32_t flags) = 0;
};

class AudioCodecDecoderAdapter {
public:
    AudioCodecDecoderAdapter() = default;

    virtual ~AudioCodecDecoderAdapter() = default;

    virtual AudioDecoderAdapterCode CreateAudioDecoderByMime(const std::string& mimetype) = 0;

    virtual AudioDecoderAdapterCode CreateAudioDecoderByName(const std::string& name) = 0;

    virtual AudioDecoderAdapterCode ConfigureDecoder(const std::shared_ptr<AudioDecoderFormatAdapter> format) = 0;

    virtual AudioDecoderAdapterCode SetParameterDecoder(const std::shared_ptr<AudioDecoderFormatAdapter> format) = 0;

    virtual AudioDecoderAdapterCode PrepareDecoder() = 0;

    virtual AudioDecoderAdapterCode StartDecoder() = 0;

    virtual AudioDecoderAdapterCode StopDecoder() = 0;

    virtual AudioDecoderAdapterCode FlushDecoder() = 0;

    virtual AudioDecoderAdapterCode ResetDecoder() = 0;

    virtual AudioDecoderAdapterCode ReleaseDecoder() = 0;

    virtual AudioDecoderAdapterCode QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs, uint8_t *bufferData,
        int32_t bufferSize, std::shared_ptr<AudioCencInfoAdapter> cencInfo, bool isEncrypted, BufferFlag flag) = 0;

    virtual AudioDecoderAdapterCode GetOutputFormatDec(std::shared_ptr<AudioDecoderFormatAdapter> format) = 0;

    virtual AudioDecoderAdapterCode ReleaseOutputBufferDec(uint32_t index) = 0;

    virtual AudioDecoderAdapterCode SetCallbackDec(const std::shared_ptr<AudioDecoderCallbackAdapter> callback) = 0;

    virtual AudioDecoderAdapterCode SetDecryptionConfig(void *session, bool secureAudio) = 0;
};

} // namespace OHOS::NWeb

#endif // AUDIO_CODEC_DECODER_ADAPTER_H
