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
#include <mutex>
#include <set>
#include <unordered_map>
#include "audio_capturer_adapter_impl.h"
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "arkweb/ohos_adapter_ndk/arkts_adapter/arkts_hilog_adapter.h"

namespace OHOS::NWeb {

static std::unordered_map<OH_AudioCapturer*, AudioCapturerAdapterImpl*> captures_;
static std::shared_mutex capturesSetMutex_;
std::shared_mutex AudioCapturerAdapterImpl::adapterMutex_;
CallbackSharedWrapper<UserDataCallBack> AudioCapturerAdapterImpl::callback_wrapper_;

const std::unordered_map<AudioAdapterSamplingRate, int32_t> SAMPLING_RATE_MAP = {
    {AudioAdapterSamplingRate::SAMPLE_RATE_8000, 8000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_11025, 11025},
    {AudioAdapterSamplingRate::SAMPLE_RATE_12000, 12000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_16000, 16000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_22050, 22050},
    {AudioAdapterSamplingRate::SAMPLE_RATE_24000, 24000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_32000, 32000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_44100, 44100},
    {AudioAdapterSamplingRate::SAMPLE_RATE_48000, 48000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_64000, 64000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_96000, 96000},
};

const std::unordered_map<AudioAdapterEncodingType, OH_AudioStream_EncodingType> ENCODING_TYPE_MAP = {
    {AudioAdapterEncodingType::ENCODING_PCM, AUDIOSTREAM_ENCODING_TYPE_RAW},
};

const std::unordered_map<AudioAdapterSampleFormat, OH_AudioStream_SampleFormat> SAMPLE_FORMAT_MAP = {
    {AudioAdapterSampleFormat::SAMPLE_U8, AUDIOSTREAM_SAMPLE_U8},
    {AudioAdapterSampleFormat::SAMPLE_S16LE, AUDIOSTREAM_SAMPLE_S16LE},
    {AudioAdapterSampleFormat::SAMPLE_S24LE, AUDIOSTREAM_SAMPLE_S24LE},
    {AudioAdapterSampleFormat::SAMPLE_S32LE, AUDIOSTREAM_SAMPLE_S32LE},
};

const std::unordered_map<AudioAdapterChannel, int32_t> AUDIO_CHANNEL_MAP = {
    {AudioAdapterChannel::MONO, 1},
    {AudioAdapterChannel::STEREO, 2},
    {AudioAdapterChannel::CHANNEL_3, 3},
    {AudioAdapterChannel::CHANNEL_4, 4},
    {AudioAdapterChannel::CHANNEL_5, 5},
    {AudioAdapterChannel::CHANNEL_6, 6},
    {AudioAdapterChannel::CHANNEL_7, 7},
    {AudioAdapterChannel::CHANNEL_8, 8},
};

const std::unordered_map<AudioAdapterSourceType, OH_AudioStream_SourceType> SOURCE_TYPE_MAP = {
    {AudioAdapterSourceType::SOURCE_TYPE_INVALID, AUDIOSTREAM_SOURCE_TYPE_INVALID},
    {AudioAdapterSourceType::SOURCE_TYPE_MIC, AUDIOSTREAM_SOURCE_TYPE_MIC},
    {AudioAdapterSourceType::SOURCE_TYPE_VOICE_RECOGNITION, AUDIOSTREAM_SOURCE_TYPE_VOICE_RECOGNITION},
    {AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION, AUDIOSTREAM_SOURCE_TYPE_VOICE_COMMUNICATION},
};

namespace {
const int32_t DEFAULT_SAMPLING_RATE = 44100;
const OH_AudioStream_EncodingType DEFAULT_ENCODETYPE = AUDIOSTREAM_ENCODING_TYPE_RAW;
const OH_AudioStream_SampleFormat DEFAULT_SAMPLE_FORMAT = AUDIOSTREAM_SAMPLE_U8;
const int32_t DEFAULT_AUDIO_CHANNEL = 2;
const OH_AudioStream_SourceType DEFAULT_SourceType = AUDIOSTREAM_SOURCE_TYPE_VOICE_RECOGNITION;
} // namespace

static AudioCapturerAdapterImpl* FindAudioCapturerAdapter(OH_AudioCapturer* capturer) {
    std::shared_lock<std::shared_mutex> lock(capturesSetMutex_);
    auto it = captures_.find(capturer);
    if (it == captures_.end()) {
        WVLOG_E("AudioCapturerAdapterImpl cannot find capture, return");
        return nullptr;
    }
    return it->second;
}

AudioCapturerAdapterImpl::~AudioCapturerAdapterImpl() {
    std::unique_lock<std::shared_mutex> lock(adapterMutex_);
    if (audio_capturer_ != nullptr) {
        Release();
    }
    if (callback_index_ > 0) {
        callback_wrapper_.Clear(callback_index_);
        callback_index_ = 0;
    }
}

int32_t AudioCapturerAdapterImpl::OnReadData(OH_AudioCapturer* capturer, void* userData, void* buffer, int32_t length)
{
    AudioCapturerAdapterImpl* adapter = FindAudioCapturerAdapter(capturer);
    if (!adapter) {
        WVLOG_E("AudioCapturerAdapterImpl::OnReadData adapter is null");
        return -1;
    }
    if (userData == nullptr) {
        return -1;
    }

    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<UserDataCallBack> userDataCallback = callback_wrapper_.GetCallback(callback_index);
    if (!userDataCallback) {
        WVLOG_E("AudioCapturerAdapterImpl userDataCallBack is nullptr");
        return -1;
    }
    userDataCallback->buffer = static_cast<uint8_t*>(buffer);
    userDataCallback->length = length;
    if (userDataCallback->callback == nullptr) {
        return -1;
    }
    userDataCallback->callback->OnReadData(length);
    return 0;
}

int32_t AudioCapturerAdapterImpl::OnInterruptEvent(OH_AudioCapturer* capturer, void* userData,
                                                   OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint)
{
    std::shared_lock<std::shared_mutex> lock(AudioCapturerAdapterImpl::GetAdapterMutex());
    AudioCapturerAdapterImpl* adapter = FindAudioCapturerAdapter(capturer);
    if (!adapter) {
        WVLOG_E("AudioCapturerAdapterImpl::OnInterruptEvent adapter is null");
        return -1;
    }
    WVLOG_I("AudioCapturerAdapterImpl::OnInterruptEvent %{public}d", static_cast<int>(hint));
    if (hint == OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_RESUME && adapter != nullptr) {
        WVLOG_I("AudioCapturerAdapterImpl::OnInterruptEvent RESUME restart capture");
        adapter->Start();
    }
    return 0;
}

std::shared_mutex& AudioCapturerAdapterImpl::GetAdapterMutex() {
    return adapterMutex_;
}

int32_t AudioCapturerAdapterImpl::Create(
    const std::shared_ptr<AudioCapturerOptionsAdapter> capturerOptions,
    std::string cachePath)
{
    (void) cachePath;

    ALOG_D("AudioCapturerAdapterImpl::Create start, test debug log printed by %{public}s", "arkts");
    ALOG_I("AudioCapturerAdapterImpl::Create start, test info log printed by %{public}s", "arkts");
    ALOG_W("AudioCapturerAdapterImpl::Create start, test warn log printed by %{public}s", "arkts");
    ALOG_E("AudioCapturerAdapterImpl::Create start, test error log printed by %{public}s", "arkts");

    if (!capturerOptions) {
        WVLOG_E("capturerOptions is nullptr");
        return AUDIO_ERROR;
    }

    OH_AudioStreamBuilder* builder;
    auto ret = OH_AudioStreamBuilder_Create(&builder, AUDIOSTREAM_TYPE_CAPTURER);
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("create audio stream capturer builder failed");
        return AUDIO_NULL_ERROR;
    }

    ret = OH_AudioStreamBuilder_SetSamplingRate(builder, GetAudioSamplingRate(capturerOptions->GetSamplingRate()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio caputure builder set samptlerate failed");
        OH_AudioStreamBuilder_Destroy(builder);
        return AUDIO_ERROR;
    }

    ret = OH_AudioStreamBuilder_SetEncodingType(builder, GetAudioEncodingType(capturerOptions->GetEncoding()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio caputure builder set encoding type failed");
        OH_AudioStreamBuilder_Destroy(builder);
        return AUDIO_ERROR;
    }

    ret = OH_AudioStreamBuilder_SetSampleFormat(builder, GetAudioSampleFormat(capturerOptions->GetSampleFormat()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio caputure builder set smaple format failed");
        OH_AudioStreamBuilder_Destroy(builder);
        return AUDIO_ERROR;
    }

    ret = OH_AudioStreamBuilder_SetChannelCount(builder, GetAudioChannel(capturerOptions->GetChannels()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio caputure builder set channel count failed");
        OH_AudioStreamBuilder_Destroy(builder);
        return AUDIO_ERROR;
    }

    ret = OH_AudioStreamBuilder_SetCapturerInfo(builder, GetAudioSourceType(capturerOptions->GetSourceType()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio caputure builder set capturer infor failed");
        OH_AudioStreamBuilder_Destroy(builder);
        return AUDIO_ERROR;
    }

    if (capturerOptions->GetCapturerFlags() != 0) {
        OH_AudioStream_LatencyMode latencyMode = AUDIOSTREAM_LATENCY_MODE_FAST;
        ret = OH_AudioStreamBuilder_SetLatencyMode(builder, latencyMode);
        if (ret != AUDIOSTREAM_SUCCESS) {
            // continue when set latencyMode fail
            WVLOG_E("audio caputure builder set capturer infor failed");
        }
    }

    OH_AudioCapturer_Callbacks callbacks;
    callbacks.OH_AudioCapturer_OnReadData = OnReadData;
    callbacks.OH_AudioCapturer_OnInterruptEvent = OnInterruptEvent;
    callbacks.OH_AudioCapturer_OnStreamEvent = nullptr;
    callbacks.OH_AudioCapturer_OnError = nullptr;

    if (callback_index_ > 0) {
        callback_wrapper_.Clear(callback_index_);
        callback_index_ = 0;
    }
    std::shared_ptr<UserDataCallBack> userDataCallBack = std::make_shared<UserDataCallBack>();
    callback_index_ = callback_wrapper_.AddCallback(userDataCallBack);
    OH_AudioStreamBuilder_SetCapturerCallback(builder, callbacks, reinterpret_cast<void*>(callback_index_));

    ret = OH_AudioStreamBuilder_GenerateCapturer(builder, &audio_capturer_);
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("create audio stream capturer failed");
        OH_AudioStreamBuilder_Destroy(builder);
        return AUDIO_ERROR;
    }
    OH_AudioStreamBuilder_Destroy(builder);
    return AUDIO_OK;
}

bool AudioCapturerAdapterImpl::Start()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return false;
    }
    auto ret = OH_AudioCapturer_Start(audio_capturer_);
    {
        std::unique_lock<std::shared_mutex> lock(capturesSetMutex_);
        if (ret == AUDIOSTREAM_SUCCESS) {
            captures_.insert({audio_capturer_, this});
        }
    }
    return ret == AUDIOSTREAM_SUCCESS;
}

bool AudioCapturerAdapterImpl::Stop()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock(capturesSetMutex_);
        captures_.erase(audio_capturer_);
    }
    auto ret = OH_AudioCapturer_Stop(audio_capturer_);
    return ret == AUDIOSTREAM_SUCCESS;
}

bool AudioCapturerAdapterImpl::Release()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock(capturesSetMutex_);
        captures_.erase(audio_capturer_);
    }
    auto ret = OH_AudioCapturer_Release(audio_capturer_);
    if (ret == AUDIOSTREAM_SUCCESS) {
        audio_capturer_ = nullptr;
        return true;
    }
    return false;
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

    auto userDataCallBack = callback_wrapper_.GetCallback(callback_index_);
    if (userDataCallBack == nullptr) {
        WVLOG_E("userDataCallBack is nullptr");
        return AUDIO_NULL_ERROR;
    }

    userDataCallBack->callback = callback;
    return AUDIO_OK;
}

int32_t AudioCapturerAdapterImpl::GetBufferDesc(std::shared_ptr<BufferDescAdapter> bufferDesc)
{
    auto userDataCallBack = callback_wrapper_.GetCallback(callback_index_);
    if (!bufferDesc || !userDataCallBack || userDataCallBack->buffer == nullptr) {
        WVLOG_E("bufferDesc is nullptr");
        return AUDIO_NULL_ERROR;
    }

    WVLOG_D("GetBufferDesc buffer size: %{public}zu", userDataCallBack->length);
    bufferDesc->SetBuffer(userDataCallBack->buffer);
    bufferDesc->SetBufLength(userDataCallBack->length);
    bufferDesc->SetDataLength(userDataCallBack->length);
    return AUDIO_OK;
}

int32_t AudioCapturerAdapterImpl::Enqueue(const std::shared_ptr<BufferDescAdapter> bufferDesc)
{
    if (audio_capturer_ == nullptr) {
        return AUDIO_NULL_ERROR;
    }
    // No need to do enqueue when using NDK interface
    WVLOG_D("[adapter mock] AudioCapturerAdapterImpl::Enqueue");
    return AUDIO_OK;
}

int32_t AudioCapturerAdapterImpl::GetFrameCount(uint32_t &frameCount)
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    int32_t frameCountValue;
    auto ret = OH_AudioCapturer_GetFrameSizeInCallback(audio_capturer_, &frameCountValue);
    if (ret != AUDIOSTREAM_SUCCESS) {
        return AUDIO_ERROR;
    }
    frameCount = static_cast<uint32_t>(frameCountValue);
    return AUDIO_OK;
}

int64_t AudioCapturerAdapterImpl::GetAudioTime()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }

    int64_t framePosition;
    int64_t timestamp;
    auto ret = OH_AudioCapturer_GetTimestamp(audio_capturer_, CLOCK_MONOTONIC, &framePosition, &timestamp);
    if (ret != AUDIOSTREAM_SUCCESS) {
        return AUDIO_ERROR;
    }
    return timestamp;
}

int32_t AudioCapturerAdapterImpl::GetAudioSamplingRate(AudioAdapterSamplingRate samplingRate)
{
    auto item = SAMPLING_RATE_MAP.find(samplingRate);
    if (item == SAMPLING_RATE_MAP.end()) {
        WVLOG_E("audio sampling rate not found");
        return DEFAULT_SAMPLING_RATE;
    }
    return item->second;
}

OH_AudioStream_EncodingType AudioCapturerAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType encodingType)
{
    auto item = ENCODING_TYPE_MAP.find(encodingType);
    if (item == ENCODING_TYPE_MAP.end()) {
        WVLOG_E("audio encoding type not found");
        return DEFAULT_ENCODETYPE;
    }
    return item->second;
}

OH_AudioStream_SampleFormat AudioCapturerAdapterImpl::GetAudioSampleFormat(AudioAdapterSampleFormat sampleFormat)
{
    auto item = SAMPLE_FORMAT_MAP.find(sampleFormat);
    if (item == SAMPLE_FORMAT_MAP.end()) {
        WVLOG_E("audio sample format not found");
        return DEFAULT_SAMPLE_FORMAT;
    }
    return item->second;
}

int32_t AudioCapturerAdapterImpl::GetAudioChannel(AudioAdapterChannel channel)
{
    auto item = AUDIO_CHANNEL_MAP.find(channel);
    if (item == AUDIO_CHANNEL_MAP.end()) {
        WVLOG_E("audio channel not found");
        return DEFAULT_AUDIO_CHANNEL;
    }
    return item->second;
}

OH_AudioStream_SourceType AudioCapturerAdapterImpl::GetAudioSourceType(AudioAdapterSourceType sourceType)
{
    auto item = SOURCE_TYPE_MAP.find(sourceType);
    if (item == SOURCE_TYPE_MAP.end()) {
        WVLOG_E("audio source type not found");
        return DEFAULT_SourceType;
    }
    return item->second;
}
}  // namespace OHOS::NWeb
