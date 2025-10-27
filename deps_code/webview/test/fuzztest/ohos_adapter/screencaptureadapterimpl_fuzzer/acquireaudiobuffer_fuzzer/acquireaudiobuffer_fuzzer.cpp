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

#include "acquireaudiobuffer_fuzzer.h"

#include <securec.h>
#include <sys/mman.h>
#include <fuzzer/FuzzedDataProvider.h>
#include "ohos_adapter_helper.h"
#include "screen_capture_adapter_impl.h"

using namespace OHOS::NWeb;

class AudioBufferAdapterMock : public AudioBufferAdapter {
public:
    AudioBufferAdapterMock() {}

    uint8_t* GetBuffer() override;

    int32_t GetLength() override;

    int64_t GetTimestamp() override;

    OHOS::NWeb::AudioCaptureSourceTypeAdapter GetSourcetype() override;

    void SetBuffer(uint8_t* buffer) override;

    void SetLength(int32_t length) override;

    void SetTimestamp(int64_t timestamp) override;

    void SetSourcetype(OHOS::NWeb::AudioCaptureSourceTypeAdapter sourcetype) override;

private:
    uint8_t* buffer_;
    int32_t len_;
    int64_t timestamp_;
    OHOS::NWeb::AudioCaptureSourceTypeAdapter type_;
};

uint8_t* AudioBufferAdapterMock::GetBuffer()
{
    return buffer_;
}

int32_t AudioBufferAdapterMock::GetLength()
{
    return len_;
}

int64_t AudioBufferAdapterMock::GetTimestamp()
{
    return timestamp_;
}

OHOS::NWeb::AudioCaptureSourceTypeAdapter AudioBufferAdapterMock::GetSourcetype()
{
    return type_;
}

void AudioBufferAdapterMock::SetBuffer(uint8_t* buffer)
{
    buffer_ = buffer;
}

void AudioBufferAdapterMock::SetLength(int32_t length)
{
    len_ = length;
}

void AudioBufferAdapterMock::SetTimestamp(int64_t timestamp)
{
    timestamp_ = timestamp;
}

void AudioBufferAdapterMock::SetSourcetype(OHOS::NWeb::AudioCaptureSourceTypeAdapter sourcetype)
{
    type_ = sourcetype;
}

namespace OHOS {

bool ApplyAcquireAudioBufferFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }
    ScreenCaptureAdapterImpl impl;
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<AudioBufferAdapter> buffer = std::make_shared<AudioBufferAdapterMock>();
    int32_t typeID = dataProvider.ConsumeIntegralInRange<int32_t>(-1, 3);
    AudioCaptureSourceTypeAdapter type = static_cast<AudioCaptureSourceTypeAdapter>(typeID);
    impl.AcquireAudioBuffer(buffer, type);
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ApplyAcquireAudioBufferFuzzTest(data, size);
    return 0;
}