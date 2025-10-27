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

#include "audio_render_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "audio_renderer_adapter_impl.h"
#include "audio_system_manager_adapter_impl.h"

using namespace OHOS::NWeb;

class AudioRendererCallbackAdapterMock : public AudioRendererCallbackAdapter {
public:
    AudioRendererCallbackAdapterMock() = default;
    void OnSuspend() {}
    void OnResume() {}
};

// AudioOutputChangeCallbackAdapter

class AudioOutputChangeCallbackAdapterMock : public AudioOutputChangeCallbackAdapter {
public:
    AudioOutputChangeCallbackAdapterMock() = default;
};
namespace OHOS {

bool AudioSystemFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }

    FuzzedDataProvider fdp(data, size);
    std::shared_ptr<AudioOutputChangeCallbackAdapter> outcallback =
        std::make_shared<AudioOutputChangeCallbackAdapterMock>();
    std::shared_ptr<AudioOutputChangeCallbackImpl> outcallbackImpl =
        std::make_shared<AudioOutputChangeCallbackImpl>(outcallback);

    std::shared_ptr<AudioRendererCallbackAdapter> callback =
        std::make_shared<AudioRendererCallbackAdapterMock>();
    std::shared_ptr<AudioRendererCallbackImpl> adapter =
        std::make_shared<AudioRendererCallbackImpl>(callback);
    std::shared_ptr<AudioRendererAdapterImpl> renderAdapter =
        std::make_shared<AudioRendererAdapterImpl>();

    auto rawValue = fdp.ConsumeIntegralInRange<int32_t>(0,7);
    InterruptEvent event;
    event.hintType = static_cast<InterruptHint>(rawValue);
    adapter->OnInterrupt(event);

    auto value = fdp.ConsumeIntegralInRange<int32_t>(-1,4);
    auto concurrencyMode = static_cast<AudioAdapterConcurrencyMode>(value);
    renderAdapter->SetAudioOutputChangeCallback(outcallback);
    renderAdapter->GetAudioAudioStrategy(concurrencyMode);
    renderAdapter->Flush();

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AudioSystemFuzzTest(data, size);
    return 0;
}
