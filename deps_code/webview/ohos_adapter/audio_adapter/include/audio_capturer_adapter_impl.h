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

#ifndef AUDIO_CAPTURER_ADAPTER_IMPL_H
#define AUDIO_CAPTURER_ADAPTER_IMPL_H

#include "audio_capturer_adapter.h"

#include <unordered_set>
#if defined(NWEB_AUDIO_ENABLE)
#include "audio_capturer.h"
#endif

namespace OHOS::NWeb {
#if defined(NWEB_AUDIO_ENABLE)
using namespace OHOS::AudioStandard;

class AudioCapturerReadCallbackImpl : public AudioCapturerReadCallback {
public:
    AudioCapturerReadCallbackImpl(std::shared_ptr<AudioCapturerReadCallbackAdapter> cb);

    ~AudioCapturerReadCallbackImpl() override = default;

    void OnReadData(size_t length) override;

private:
    std::shared_ptr<AudioCapturerReadCallbackAdapter> cb_ = nullptr;
};
#endif

class AudioCapturerAdapterImpl : public AudioCapturerAdapter {
public:
    AudioCapturerAdapterImpl() = default;

    ~AudioCapturerAdapterImpl() override = default;

    int32_t Create(const std::shared_ptr<AudioCapturerOptionsAdapter> capturerOptions,
        std::string cachePath = std::string()) override;

    bool Start() override;

    bool Stop() override;

    bool Release() override;

    int32_t SetCapturerReadCallback(
        std::shared_ptr<AudioCapturerReadCallbackAdapter> callbck) override;

    int32_t GetBufferDesc(std::shared_ptr<BufferDescAdapter> bufferDesc) override;

    int32_t Enqueue(const std::shared_ptr<BufferDescAdapter> bufferDesc) override;

    int32_t GetFrameCount(uint32_t &frameCount) override;

    int64_t GetAudioTime() override;

#if defined(NWEB_AUDIO_ENABLE)
    static AudioSamplingRate GetAudioSamplingRate(AudioAdapterSamplingRate samplingRate);

    static AudioEncodingType GetAudioEncodingType(AudioAdapterEncodingType encodingType);

    static AudioSampleFormat GetAudioSampleFormat(AudioAdapterSampleFormat sampleFormat);

    static AudioChannel GetAudioChannel(AudioAdapterChannel channel);

    static SourceType GetAudioSourceType(AudioAdapterSourceType SourceType);

private:
    std::unique_ptr<AudioCapturer> audio_capturer_;
#endif
};
}  // namespace OHOS::NWeb

#endif // AUDIO_CAPTURER_ADAPTER_IMPL_H
