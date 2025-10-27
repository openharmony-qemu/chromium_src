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

#include "audio_capturer_adapter_impl.h"

#include <unordered_map>

#include "application_context.h"
#include "audio_errors.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

constexpr int64_t NANOSECONDS_PER_SECOND = 1000000000;

const std::unordered_map<AudioAdapterSamplingRate, AudioSamplingRate> SAMPLING_RATE_MAP = {
    {AudioAdapterSamplingRate::SAMPLE_RATE_8000, AudioSamplingRate::SAMPLE_RATE_8000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_11025, AudioSamplingRate::SAMPLE_RATE_11025},
    {AudioAdapterSamplingRate::SAMPLE_RATE_12000, AudioSamplingRate::SAMPLE_RATE_12000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_16000, AudioSamplingRate::SAMPLE_RATE_16000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_22050, AudioSamplingRate::SAMPLE_RATE_22050},
    {AudioAdapterSamplingRate::SAMPLE_RATE_24000, AudioSamplingRate::SAMPLE_RATE_24000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_32000, AudioSamplingRate::SAMPLE_RATE_32000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_44100, AudioSamplingRate::SAMPLE_RATE_44100},
    {AudioAdapterSamplingRate::SAMPLE_RATE_48000, AudioSamplingRate::SAMPLE_RATE_48000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_64000, AudioSamplingRate::SAMPLE_RATE_64000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_96000, AudioSamplingRate::SAMPLE_RATE_96000},
};

const std::unordered_map<AudioAdapterEncodingType, AudioEncodingType> ENCODING_TYPE_MAP = {
    {AudioAdapterEncodingType::ENCODING_PCM, AudioEncodingType::ENCODING_PCM},
    {AudioAdapterEncodingType::ENCODING_INVALID, AudioEncodingType::ENCODING_INVALID}
};

const std::unordered_map<AudioAdapterSampleFormat, AudioSampleFormat> SAMPLE_FORMAT_MAP = {
    {AudioAdapterSampleFormat::SAMPLE_U8, AudioSampleFormat::SAMPLE_U8},
    {AudioAdapterSampleFormat::SAMPLE_S16LE, AudioSampleFormat::SAMPLE_S16LE},
    {AudioAdapterSampleFormat::SAMPLE_S24LE, AudioSampleFormat::SAMPLE_S24LE},
    {AudioAdapterSampleFormat::SAMPLE_S32LE, AudioSampleFormat::SAMPLE_S32LE},
    {AudioAdapterSampleFormat::SAMPLE_F32LE, AudioSampleFormat::SAMPLE_F32LE},
};

const std::unordered_map<AudioAdapterChannel, AudioChannel> AUDIO_CHANNEL_MAP = {
    {AudioAdapterChannel::MONO, AudioChannel::MONO},
    {AudioAdapterChannel::STEREO, AudioChannel::STEREO},
    {AudioAdapterChannel::CHANNEL_3, AudioChannel::CHANNEL_3},
    {AudioAdapterChannel::CHANNEL_4, AudioChannel::CHANNEL_4},
    {AudioAdapterChannel::CHANNEL_5, AudioChannel::CHANNEL_5},
    {AudioAdapterChannel::CHANNEL_6, AudioChannel::CHANNEL_6},
    {AudioAdapterChannel::CHANNEL_7, AudioChannel::CHANNEL_7},
    {AudioAdapterChannel::CHANNEL_8, AudioChannel::CHANNEL_8},
};

const std::unordered_map<AudioAdapterSourceType, SourceType> SOURCE_TYPE_MAP = {
    {AudioAdapterSourceType::SOURCE_TYPE_INVALID, SourceType::SOURCE_TYPE_INVALID},
    {AudioAdapterSourceType::SOURCE_TYPE_MIC, SourceType::SOURCE_TYPE_MIC},
    {AudioAdapterSourceType::SOURCE_TYPE_VOICE_RECOGNITION, SourceType::SOURCE_TYPE_VOICE_RECOGNITION},
    {AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION, SourceType::SOURCE_TYPE_VOICE_COMMUNICATION},
    {AudioAdapterSourceType::SOURCE_TYPE_ULTRASONIC, SourceType::SOURCE_TYPE_ULTRASONIC},
};

AudioCapturerReadCallbackImpl::AudioCapturerReadCallbackImpl(
    std::shared_ptr<AudioCapturerReadCallbackAdapter> cb) : cb_(cb) {};

void AudioCapturerReadCallbackImpl::OnReadData(size_t length)
{
    if (!cb_) {
        WVLOG_E("Get AudioCapturerReadCallbackAdapter failed.");
        return;
    }
    cb_->OnReadData(length);
}

int32_t AudioCapturerAdapterImpl::Create(
    const std::shared_ptr<AudioCapturerOptionsAdapter> capturerOptions,
    std::string cachePath)
{
    std::string audioCachePath = cachePath;
    if (audioCachePath.empty()) {
        std::shared_ptr<AbilityRuntime::ApplicationContext> context =
            AbilityRuntime::ApplicationContext::GetApplicationContext();
        if (!context) {
            WVLOG_E("application context get failed");
            return AUDIO_ERROR;
        }
        audioCachePath = context->GetCacheDir();
        if (audioCachePath.empty()) {
            WVLOG_E("application cache path get failed");
            return AUDIO_ERROR;
        }
    }

    if (!capturerOptions) {
        WVLOG_E("capturerOptions is nullptr");
        return AUDIO_ERROR;
    }

    AudioCapturerOptions options;
    options.streamInfo.samplingRate = GetAudioSamplingRate(capturerOptions->GetSamplingRate());
    options.streamInfo.encoding = GetAudioEncodingType(capturerOptions->GetEncoding());
    options.streamInfo.format = GetAudioSampleFormat(capturerOptions->GetSampleFormat());
    options.streamInfo.channels = GetAudioChannel(capturerOptions->GetChannels());
    options.capturerInfo.sourceType = GetAudioSourceType(capturerOptions->GetSourceType());
    options.capturerInfo.capturerFlags = capturerOptions->GetCapturerFlags();

    audio_capturer_ = AudioCapturer::Create(options, audioCachePath);
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer create failed");
        return AUDIO_NULL_ERROR;
    }
    audio_capturer_->SetCaptureMode(AudioCaptureMode::CAPTURE_MODE_CALLBACK);
    return AUDIO_OK;
}

bool AudioCapturerAdapterImpl::Start()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return false;
    }
    return audio_capturer_->Start();
}

bool AudioCapturerAdapterImpl::Stop()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return false;
    }
    return audio_capturer_->Stop();
}

bool AudioCapturerAdapterImpl::Release()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return false;
    }
    return audio_capturer_->Release();
}

int32_t AudioCapturerAdapterImpl::SetCapturerReadCallback(
    std::shared_ptr<AudioCapturerReadCallbackAdapter> callback)
{
    if (callback == nullptr) {
        WVLOG_E("audio capturer callback is nullptr");
        return AUDIO_NULL_ERROR;
    }

    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    auto capturerReadCallback = std::make_shared<AudioCapturerReadCallbackImpl>(callback);
    if (capturerReadCallback == nullptr) {
        WVLOG_E("audio capturerReadCallback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_capturer_->SetCapturerReadCallback(capturerReadCallback);
}

int32_t AudioCapturerAdapterImpl::GetBufferDesc(std::shared_ptr<BufferDescAdapter> bufferDesc)
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }

    if (!bufferDesc) {
        WVLOG_E("bufferDesc is nullptr");
        return AUDIO_NULL_ERROR;
    }

    BufferDesc bufDesc;
    audio_capturer_->GetBufferDesc(bufDesc);
    bufferDesc->SetBuffer(bufDesc.buffer);
    bufferDesc->SetBufLength(bufDesc.bufLength);
    bufferDesc->SetDataLength(bufDesc.dataLength);
    return 0;
}

int32_t AudioCapturerAdapterImpl::Enqueue(const std::shared_ptr<BufferDescAdapter> bufferDesc)
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }
 
    if (!bufferDesc) {
        WVLOG_E("bufferDesc is nullptr");
        return AUDIO_NULL_ERROR;
    }

    BufferDesc bufDesc;
    bufDesc.buffer = bufferDesc->GetBuffer();
    bufDesc.bufLength = bufferDesc->GetBufLength();
    bufDesc.dataLength = bufferDesc->GetDataLength();
    return audio_capturer_->Enqueue(bufDesc);
}

int32_t AudioCapturerAdapterImpl::GetFrameCount(uint32_t &frameCount)
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_capturer_->GetFrameCount(frameCount);
}

int64_t AudioCapturerAdapterImpl::GetAudioTime()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    Timestamp timeStamp;
    audio_capturer_->GetAudioTime(timeStamp, Timestamp::Timestampbase::MONOTONIC);
    return timeStamp.time.tv_sec * NANOSECONDS_PER_SECOND + timeStamp.time.tv_nsec;
}

AudioSamplingRate AudioCapturerAdapterImpl::GetAudioSamplingRate(AudioAdapterSamplingRate samplingRate)
{
    auto item = SAMPLING_RATE_MAP.find(samplingRate);
    if (item == SAMPLING_RATE_MAP.end()) {
        WVLOG_E("audio sampling rate not found");
        return AudioSamplingRate::SAMPLE_RATE_44100;
    }
    return item->second;
}

AudioEncodingType AudioCapturerAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType encodingType)
{
    auto item = ENCODING_TYPE_MAP.find(encodingType);
    if (item == ENCODING_TYPE_MAP.end()) {
        WVLOG_E("audio encoding type not found");
        return AudioEncodingType::ENCODING_INVALID;
    }
    return item->second;
}

AudioSampleFormat AudioCapturerAdapterImpl::GetAudioSampleFormat(AudioAdapterSampleFormat sampleFormat)
{
    auto item = SAMPLE_FORMAT_MAP.find(sampleFormat);
    if (item == SAMPLE_FORMAT_MAP.end()) {
        WVLOG_E("audio sample format not found");
        return AudioSampleFormat::INVALID_WIDTH;
    }
    return item->second;
}

AudioChannel AudioCapturerAdapterImpl::GetAudioChannel(AudioAdapterChannel channel)
{
    auto item = AUDIO_CHANNEL_MAP.find(channel);
    if (item == AUDIO_CHANNEL_MAP.end()) {
        WVLOG_E("audio channel not found");
        return AudioChannel::STEREO;
    }
    return item->second;
}

SourceType AudioCapturerAdapterImpl::GetAudioSourceType(AudioAdapterSourceType sourceType)
{
    auto item = SOURCE_TYPE_MAP.find(sourceType);
    if (item == SOURCE_TYPE_MAP.end()) {
        WVLOG_E("audio source type not found");
        return SourceType::SOURCE_TYPE_VOICE_RECOGNITION;
    }
    return item->second;
}
}  // namespace OHOS::NWeb
