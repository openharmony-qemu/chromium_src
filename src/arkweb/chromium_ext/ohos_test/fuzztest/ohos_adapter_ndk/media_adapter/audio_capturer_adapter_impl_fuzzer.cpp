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

#define private public
#include "arkweb/ohos_adapter_ndk/audio_capturer_adapter/audio_capturer_adapter_impl.h"
#undef private
#include <fuzzer/FuzzedDataProvider.h>
#include <inputmethod/inputmethod_controller_capi.h>
 
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "base/logging.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;
namespace OHOS::NWeb {

class MockAudioCapturerOptionsAdapter : public AudioCapturerOptionsAdapter {
public:
    MockAudioCapturerOptionsAdapter() = default;

    AudioAdapterSamplingRate GetSamplingRate() override
    {
        return samplingRate;
    }

    AudioAdapterEncodingType GetEncoding() override
    {
        return encoding;
    }

    AudioAdapterSampleFormat GetSampleFormat() override
    {
        return format;
    }

    AudioAdapterChannel GetChannels() override
    {
        return channels;
    }

    AudioAdapterSourceType GetSourceType() override
    {
        return sourceType;
    }

    int32_t GetCapturerFlags() override
    {
        return capturerFlags;
    }

    AudioAdapterSamplingRate samplingRate;
    AudioAdapterEncodingType encoding;
    AudioAdapterSampleFormat format;
    AudioAdapterChannel channels;
    AudioAdapterSourceType sourceType;
    int32_t capturerFlags;
};

class MockAudioCapturerReadCallbackAdapter : public AudioCapturerReadCallbackAdapter {
public:
    MockAudioCapturerReadCallbackAdapter() = default;

    void OnReadData(size_t length) override {}
};

class MockBufferDescAdapter : public BufferDescAdapter {
public:
    MockBufferDescAdapter() = default;

    uint8_t* GetBuffer() override {
      return nullptr;
    }

    size_t GetBufLength() override {
      return 64;
    }

    size_t GetDataLength() override {
      return 1024;
    }

    void SetBuffer(uint8_t* buffer) override {}

    void SetBufLength(size_t bufLength) override {}

    void SetDataLength(size_t dataLength) override {}
};

bool AudioCapturerFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    std::shared_ptr<MockAudioCapturerOptionsAdapter> capturerOptions =
        std::make_shared<MockAudioCapturerOptionsAdapter>();
    capturerOptions->samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    FuzzedDataProvider fdp(data, size);

    int32_t rawValue = fdp.ConsumeIntegralInRange<int32_t>(-1, 9);
    auto encode = static_cast<AudioAdapterEncodingType>(rawValue);
    capturerOptions->encoding = encode;

    rawValue = fdp.ConsumeIntegralInRange<int32_t>(-1, 9);
    auto sample = static_cast<AudioAdapterSampleFormat>(rawValue);
    capturerOptions->format = sample;

    rawValue = fdp.ConsumeIntegralInRange<int32_t>(-1, 9);
    auto channel = static_cast<AudioAdapterChannel>(rawValue);
    capturerOptions->channels = channel;

    rawValue = fdp.ConsumeIntegralInRange<int32_t>(-1, 9);
    auto source = static_cast<AudioAdapterSourceType>(rawValue);
    capturerOptions->sourceType = source;
    capturerOptions->capturerFlags = 0;
    std::string cachePath(reinterpret_cast<const char*>(data), size);
    AudioCapturerAdapterImpl adapter;
    adapter.Create(capturerOptions, cachePath);

    auto callback = std::make_shared<MockAudioCapturerReadCallbackAdapter>();
    adapter.SetCapturerReadCallback(callback);
    std::shared_ptr<BufferDescAdapter> bufferDesc = std::make_shared<MockBufferDescAdapter>();
    adapter.GetBufferDesc(bufferDesc);
    adapter.Enqueue(bufferDesc);
    adapter.GetAudioTime();
    uint32_t frameCount = 0;
    adapter.GetFrameCount(frameCount);

    adapter.GetAudioSamplingRate(AudioAdapterSamplingRate::SAMPLE_RATE_32000);

    rawValue = fdp.ConsumeIntegralInRange<int32_t>(-1, 9);
    encode = static_cast<AudioAdapterEncodingType>(rawValue);

    rawValue = fdp.ConsumeIntegralInRange<int32_t>(-1, 9);
    sample = static_cast<AudioAdapterSampleFormat>(rawValue);

    rawValue = fdp.ConsumeIntegralInRange<int32_t>(-1, 9);
    channel = static_cast<AudioAdapterChannel>(rawValue);

    rawValue = fdp.ConsumeIntegralInRange<int32_t>(-1, 9);
    source = static_cast<AudioAdapterSourceType>(rawValue);

    adapter.GetAudioEncodingType(encode);
    adapter.GetAudioSampleFormat(sample);
    adapter.GetAudioChannel(channel);
    adapter.GetAudioSourceType(source);
    adapter.Start();
    adapter.Stop();
    adapter.Release();
    return true;
}

bool AudioCapturerNullFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    std::shared_ptr<MockAudioCapturerOptionsAdapter> capturerOptions =
        std::make_shared<MockAudioCapturerOptionsAdapter>();

    capturerOptions->samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    capturerOptions->encoding = AudioAdapterEncodingType::ENCODING_PCM;
    capturerOptions->format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    capturerOptions->channels = AudioAdapterChannel::STEREO;
    capturerOptions->sourceType = AudioAdapterSourceType::SOURCE_TYPE_MIC;
    capturerOptions->capturerFlags = 0;
    FuzzedDataProvider fdp(data, size);
    auto testStr = fdp.ConsumeRandomLengthString(32);

    AudioCapturerAdapterImpl adapter;
    adapter.Create(nullptr, testStr);

    auto callback = std::make_shared<MockAudioCapturerReadCallbackAdapter>();
    adapter.SetCapturerReadCallback(callback);
    std::shared_ptr<BufferDescAdapter> bufferDesc = std::make_shared<MockBufferDescAdapter>();
    adapter.GetBufferDesc(bufferDesc);
    adapter.Enqueue(bufferDesc);
    adapter.GetAudioTime();
    uint32_t frameCount = 0;
    adapter.GetFrameCount(frameCount);
    adapter.GetAudioSamplingRate(AudioAdapterSamplingRate::SAMPLE_RATE_32000);
    adapter.GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_INVALID);
    adapter.GetAudioSampleFormat(AudioAdapterSampleFormat::INVALID_WIDTH);
    adapter.GetAudioChannel(AudioAdapterChannel::CHANNEL_4);
    adapter.GetAudioSourceType(AudioAdapterSourceType::SOURCE_TYPE_ULTRASONIC);
    adapter.Start();
    adapter.Stop();
    adapter.Release();
    return true;
}

} // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if (data == nullptr || size == 0) {
    return 0;
  }
  AudioCapturerFuzzTest(data, size);
  AudioCapturerNullFuzzTest(data, size);
  return 0;
}