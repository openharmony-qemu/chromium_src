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

#include "onaudiobufferavailable_fuzzer.h"

#include <securec.h>
#include <sys/mman.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "ohos_adapter_helper.h"
#include "screen_capture_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {

bool ApplyOnAudioBufferAvailableFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }
    FuzzedDataProvider provider(data, size);
    size_t callCount = provider.ConsumeIntegral<uint8_t>() % 10;
    for (size_t i = 0; i < callCount; ++i) {
        std::shared_ptr<ScreenCaptureCallbackAdapter> cb;
        OHScreenCaptureCallback adapter(cb);
        bool isReady = provider.ConsumeBool();
        OHOS::Media::AudioCaptureSourceType sourceType = static_cast<OHOS::Media::AudioCaptureSourceType>(0);
        adapter.OnAudioBufferAvailable(isReady, sourceType);
    }
    std::shared_ptr<ScreenCaptureCallbackAdapter> cb;
    OHScreenCaptureCallback adapter(cb);
    bool isReady = provider.ConsumeBool();
    OHOS::Media::AudioCaptureSourceType sourceType = static_cast<OHOS::Media::AudioCaptureSourceType>(0);
    adapter.OnAudioBufferAvailable(isReady, sourceType);
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ApplyOnAudioBufferAvailableFuzzTest(data, size);
    return 0;
}
