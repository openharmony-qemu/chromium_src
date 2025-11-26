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
#include <ohaudio/native_audiocapturer.h>
#include <ohaudio/native_audiostream_base.h>
#include <ohaudio/native_audiostreambuilder.h>
#include <ohaudio/native_audio_common.h>
#include "arkweb/ohos_adapter_ndk/ndk_callback_wrapper/callback_shared_wrapper.h"

namespace OHOS::NWeb {

typedef struct {
    std::shared_ptr<AudioCapturerReadCallbackAdapter> callback;
    uint8_t *buffer;
    size_t length;
} UserDataCallBack;

class AudioCapturerAdapterImpl : public AudioCapturerAdapter {
public:
    AudioCapturerAdapterImpl() = default;

    ~AudioCapturerAdapterImpl() override;

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

    static int32_t GetAudioSamplingRate(AudioAdapterSamplingRate samplingRate);

    static OH_AudioStream_EncodingType GetAudioEncodingType(AudioAdapterEncodingType encodingType);

    static OH_AudioStream_SampleFormat GetAudioSampleFormat(AudioAdapterSampleFormat sampleFormat);

    static int32_t GetAudioChannel(AudioAdapterChannel channel);

    static OH_AudioStream_SourceType GetAudioSourceType(AudioAdapterSourceType SourceType);

    static int32_t OnReadData(OH_AudioCapturer* capturer, void* userData, void* buffer, int32_t length);

    static int32_t OnInterruptEvent(OH_AudioCapturer* capturer, void* userData, OH_AudioInterrupt_ForceType type,
                                    OH_AudioInterrupt_Hint hint);
    static std::shared_mutex& GetAdapterMutex();
private:
    OH_AudioCapturer* audio_capturer_ = nullptr;
    size_t callback_index_ = 0;
    static CallbackSharedWrapper<UserDataCallBack> callback_wrapper_;
    static std::shared_mutex adapterMutex_;
};
}  // namespace OHOS::NWeb

#endif // AUDIO_CAPTURER_ADAPTER_IMPL_H
